#include "../include/ajis_input.h"
#include "../include/ajis_lexer.h"
#include "../include/ajis_error_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* POSIX directory traversal (WSL/Linux/macOS) */
#include <dirent.h>
#include <sys/stat.h>

/* ---------------- Token names (optional dump) ---------------- */

static const char* tok_name(ajis_token_type t) {
    switch (t) {
        case AJIS_TOKEN_EOF:      return "EOF";
        case AJIS_TOKEN_LBRACE:   return "LBRACE";
        case AJIS_TOKEN_RBRACE:   return "RBRACE";
        case AJIS_TOKEN_LBRACKET: return "LBRACKET";
        case AJIS_TOKEN_RBRACKET: return "RBRACKET";
        case AJIS_TOKEN_COLON:    return "COLON";
        case AJIS_TOKEN_COMMA:    return "COMMA";
        case AJIS_TOKEN_STRING:   return "STRING";
        case AJIS_TOKEN_INVALID:  return "INVALID";
        case AJIS_TOKEN_TRUE:     return "TRUE";
        case AJIS_TOKEN_FALSE:    return "FALSE";
        case AJIS_TOKEN_NULL:     return "NULL";
        case AJIS_TOKEN_NUMBER:   return "NUMBER";
        default:                  return "OTHER";
    }
}

/* ---------------- Small helpers ---------------- */

static int ends_with(const char* s, const char* suffix) {
    size_t sl = strlen(s), su = strlen(suffix);
    return (sl >= su) && (memcmp(s + (sl - su), suffix, su) == 0);
}

static int contains_part(const char* s, const char* part) {
    return strstr(s, part) != NULL;
}

static int is_sep_char(char c) {
    return c == '/' || c == '\\';
}

/* Read file into memory. out_data is malloc'ed, caller frees. */
static int read_file_all(const char* path, char** out_data, size_t* out_size) {
    *out_data = NULL;
    *out_size = 0;

    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[TEST] Cannot open: %s\n", path);
        return 0;
    }

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    long len = ftell(f);
    if (len < 0) { fclose(f); return 0; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return 0; }

    char* buf = (char*)malloc((size_t)len + 1);
    if (!buf) { fclose(f); return 0; }

    size_t rd = fread(buf, 1, (size_t)len, f);
    fclose(f);

    if (rd != (size_t)len) {
        free(buf);
        fprintf(stderr, "[TEST] Short read: %s (expected %ld, got %zu)\n", path, len, rd);
        return 0;
    }

    buf[len] = '\0';
    *out_data = buf;
    *out_size = (size_t)len;
    return 1;
}

/* ---------------- Test filtering ---------------- */

typedef struct TestFilter {
    int dump;
    int show_errors;      /* --errors: show pretty error reports for expected failures */

    int run_all;          /* --all: traverse test_data */
    int only_valid;       /* --valid */
    int only_invalid;     /* --invalid */

    /* category filters */
    int f_numbers;
    int f_strings;
    int f_arrays;
    int f_objects;
    int f_complex;
    int f_canonical;

    const char* single_path; /* if set: run only this one file */
} TestFilter;

static int any_category_selected(const TestFilter* f) {
    return f->f_numbers || f->f_strings || f->f_arrays || f->f_objects || f->f_complex || f->f_canonical;
}

/* Determine expected result by path (heuristics) */
static int expect_fail_from_path(const char* path) {
    if (contains_part(path, "/invalid/") || contains_part(path, "\\invalid\\")) return 1;
    if (ends_with(path, "_invalid.ajis")) return 1;
    return 0;
}

/* Category match: checks if path contains "/numbers/" etc. */
static int matches_category(const char* path, const TestFilter* f) {
    /* If no category selected -> accept all categories */
    if (!any_category_selected(f)) return 1;

    int ok = 0;
    if (f->f_numbers   && (contains_part(path, "/numbers/")   || contains_part(path, "\\numbers\\"))) ok = 1;
    if (f->f_strings   && (contains_part(path, "/strings/")   || contains_part(path, "\\strings\\"))) ok = 1;
    if (f->f_arrays    && (contains_part(path, "/arrays/")    || contains_part(path, "\\arrays\\"))) ok = 1;
    if (f->f_objects   && (contains_part(path, "/objects/")   || contains_part(path, "\\objects\\"))) ok = 1;
    if (f->f_complex   && (contains_part(path, "/complex/")   || contains_part(path, "\\complex\\"))) ok = 1;
    if (f->f_canonical && (contains_part(path, "/canonical/") || contains_part(path, "\\canonical\\"))) ok = 1;

    return ok;
}

static int matches_validity(const char* path, const TestFilter* f) {
    if (f->only_valid)   return !expect_fail_from_path(path);
    if (f->only_invalid) return  expect_fail_from_path(path);
    return 1; /* both */
}

/* ---------------- Running one file ---------------- */

typedef struct TestStats {
    int total;
    int passed;
    int failed;
    int skipped;
} TestStats;

static int run_one_file(const char* path, int dump, int show_errors, TestStats* st) {
    int expect_fail = expect_fail_from_path(path);

    if (st) st->total++;

    char* src = NULL;
    size_t src_len = 0;
    if (!read_file_all(path, &src, &src_len)) {
    fprintf(stderr, "[TEST] Failed to read input (open/read error): %s\n", path);
    if (st) st->failed++;
    free(src);
    return 0;
}

if (src_len == 0) {
    /* Placeholder file – treat as SKIP (not a failure). */
    if (st) st->skipped++;
    printf("[SKIP] %s (empty file)\n", path);
    free(src);
    return 1; /* skip counts as non-failure */
}


    ajis_input in;
    ajis_input_init(&in, src, src_len);

    ajis_lexer lx;
    ajis_lexer_options opt;
    opt.allow_multiline_strings = 0;
    opt.allow_number_separators = 1;
    ajis_lexer_init(&lx, &in, opt);

    int saw_error = 0;
    ajis_error first_error = ajis_error_ok();

    for (;;) {
        ajis_token tok;
        ajis_error err = ajis_error_ok();

        ajis_error_code rc = ajis_lexer_next(&lx, &tok, &err);
        if (rc != AJIS_OK) {
            saw_error = 1;
            first_error = err; /* save first error for later display */
            if (dump) {
                fprintf(stderr, "\n");
                ajis_error_print_pretty(stderr, path, src, src_len, &err);
                fprintf(stderr, "\n");
            }
            break;
        }

        if (dump) {
            printf("%s: %s span(off=%zu,len=%zu)\n",
                path, tok_name(tok.type), tok.span.offset, tok.span.length);
        }

        if (tok.type == AJIS_TOKEN_EOF) break;
    }

    int ok = 0;
    if (!expect_fail) {
        ok = !saw_error;
    } else {
        ok = saw_error;
    }

    if (ok) {
        if (st) st->passed++;
        printf("[PASS] %s%s\n", path, expect_fail ? " (expected fail)" : "");
        
        /* If --errors is enabled and this was an expected failure, show the error */
        if (show_errors && expect_fail && saw_error) {
            printf("\n");
            ajis_error_print_pretty(stdout, path, src, src_len, &first_error);
            printf("\n");
        }
    } else {
        if (st) st->failed++;
        printf("[FAIL] %s%s\n", path, expect_fail ? " (expected fail)" : "");
        
        /* For unexpected failures, always show the error */
        if (saw_error) {
            printf("\n");
            ajis_error_print_pretty(stdout, path, src, src_len, &first_error);
            printf("\n");
        }
    }

    free(src);
    return ok;
}

/* ---------------- Directory traversal ---------------- */

static int is_regular_file(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISREG(st.st_mode);
}

static int is_directory(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

static void join_path(char* out, size_t out_cap, const char* a, const char* b) {
    size_t al = strlen(a);
    int need_sep = (al > 0 && !is_sep_char(a[al - 1]));
    snprintf(out, out_cap, "%s%s%s", a, need_sep ? "/" : "", b);
}

static void run_tree_recursive(const char* dir, const TestFilter* f, TestStats* st) {
    DIR* d = opendir(dir);
    if (!d) {
        fprintf(stderr, "[TEST] Cannot open dir: %s (%s)\n", dir, strerror(errno));
        return;
    }

    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        const char* name = ent->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

        /* Skip parser_invalid directory - these are parser-level errors, not lexer errors */
        if (strcmp(name, "parser_invalid") == 0) continue;

        char path[1024];
        join_path(path, sizeof(path), dir, name);

        if (is_directory(path)) {
            run_tree_recursive(path, f, st);
            continue;
        }

        if (!is_regular_file(path)) continue;
        if (!ends_with(path, ".ajis")) continue;

        if (!matches_category(path, f)) continue;
        if (!matches_validity(path, f)) continue;

        run_one_file(path, f->dump, f->show_errors, st);
    }

    closedir(d);
}

/* ---------------- CLI ---------------- */

static void usage(const char* exe) {
    fprintf(stderr,
        "AJIS lexer tests\n\n"
        "Usage:\n"
        "  %s [path] [options]\n"
        "  %s --all [options]\n\n"
        "Options:\n"
        "  --all              Run all .ajis files under tests/test_data (recursive)\n"
        "  --valid            Only valid tests\n"
        "  --invalid          Only invalid tests\n"
        "  --numbers          Only numbers category\n"
        "  --strings          Only strings category\n"
        "  --arrays           Only arrays category\n"
        "  --objects          Only objects category\n"
        "  --complex          Only complex category\n"
        "  --canonical        Only canonical category\n"
        "  --dump             Dump tokens + errors\n"
        "  --errors           Show pretty error reports for expected failures\n"
        "  -h, --help         Show help\n\n"
        "Examples:\n"
        "  %s tests/test_data/valid/numbers/n_basic_valid.ajis\n"
        "  %s --all\n"
        "  %s --all --numbers\n"
        "  %s --all --numbers --invalid\n"
        "  %s --all --complex --valid\n",
        exe, exe, exe, exe, exe, exe, exe, exe);
}

static int parse_args(int argc, char** argv, TestFilter* f) {
    memset(f, 0, sizeof(*f));

    for (int i = 1; i < argc; i++) {
        const char* a = argv[i];

        if (strcmp(a, "--dump") == 0) f->dump = 1;
        else if (strcmp(a, "--errors") == 0) f->show_errors = 1;
        else if (strcmp(a, "--all") == 0) f->run_all = 1;
        else if (strcmp(a, "--valid") == 0) f->only_valid = 1;
        else if (strcmp(a, "--invalid") == 0) f->only_invalid = 1;

        else if (strcmp(a, "--numbers") == 0) f->f_numbers = 1;
        else if (strcmp(a, "--strings") == 0) f->f_strings = 1;
        else if (strcmp(a, "--arrays") == 0) f->f_arrays = 1;
        else if (strcmp(a, "--objects") == 0) f->f_objects = 1;
        else if (strcmp(a, "--complex") == 0) f->f_complex = 1;
        else if (strcmp(a, "--canonical") == 0) f->f_canonical = 1;

        else if (strcmp(a, "-h") == 0 || strcmp(a, "--help") == 0) return 0;

        else {
            /* treat as path (single file) */
            f->single_path = a;
        }
    }

    /* sanity: --valid and --invalid together makes no sense */
    if (f->only_valid && f->only_invalid) {
        fprintf(stderr, "[TEST] Cannot use --valid and --invalid together.\n");
        return 0;
    }

    /* If a path is provided, we don't need --all */
    return 1;
}

int main(int argc, char** argv) {
    const char* default_path = "tests/test_data/valid/numbers/n_basic_valid.ajis";
    const char* root_dir     = "tests/test_data";

    TestFilter f;
    if (!parse_args(argc, argv, &f)) {
        usage(argv[0]);
        return 2;
    }

    TestStats st = {0};

    if (f.run_all) {
        printf("[TEST] Mode: ALL (recursive)\n");
        printf("[TEST] Root: %s\n", root_dir);
        if (f.only_valid) printf("[TEST] Filter: valid only\n");
        if (f.only_invalid) printf("[TEST] Filter: invalid only\n");
        if (any_category_selected(&f)) {
            printf("[TEST] Category filter enabled\n");
        }
        if (f.dump) printf("[TEST] Dump: ON\n");

        run_tree_recursive(root_dir, &f, &st);

    } else {
        const char* path = f.single_path ? f.single_path : default_path;
        printf("[TEST] Mode: SINGLE\n");
        printf("[TEST] File: %s\n", path);
        if (f.dump) printf("[TEST] Dump: ON\n");

        run_one_file(path, f.dump, f.show_errors, &st);
    }

   printf("\n[SUMMARY] total=%d passed=%d failed=%d skipped=%d\n",
       st.total, st.passed, st.failed, st.skipped);

return (st.failed == 0) ? 0 : 1;

}
