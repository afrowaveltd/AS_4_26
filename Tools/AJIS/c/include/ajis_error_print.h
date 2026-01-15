#ifndef AJIS_ERROR_PRINT_H
#define AJIS_ERROR_PRINT_H

#include "ajis_error.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
   Pretty Error Printing (Rust-style-ish)

   Goals:
   - Clear header + file:line:col
   - Show the relevant source line
   - Caret points at the exact byte offset
   - Long lines are cropped around the error (like "...xxxxx..."),
     so you always see the important neighborhood.
   - Tabs are expanded for correct caret alignment.

   You can tweak behavior via macros:
     - AJIS_ENABLE_ANSI (default: 1)
     - AJIS_TAB_WIDTH (default: 4)
     - AJIS_ERROR_CONTEXT_CHARS (default: 20)  // chars on each side
   ============================================================ */

#ifndef AJIS_ENABLE_ANSI
#define AJIS_ENABLE_ANSI 1
#endif

#ifndef AJIS_TAB_WIDTH
#define AJIS_TAB_WIDTH 4
#endif

#ifndef AJIS_ERROR_CONTEXT_CHARS
#define AJIS_ERROR_CONTEXT_CHARS 20
#endif

#if AJIS_ENABLE_ANSI
  #define AJIS_C_RED_B   "\033[1;31m"
  #define AJIS_C_BLUE_B  "\033[1;34m"
  #define AJIS_C_DIM     "\033[2m"
  #define AJIS_C_RESET   "\033[0m"
#else
  #define AJIS_C_RED_B   ""
  #define AJIS_C_BLUE_B  ""
  #define AJIS_C_DIM     ""
  #define AJIS_C_RESET   ""
#endif

static inline const char* ajis_error_code_name(ajis_error_code code) {
    switch (code) {
        case AJIS_OK: return "OK";
        case AJIS_ERR_UNKNOWN: return "Unknown error";
        case AJIS_ERR_UNEXPECTED_EOF: return "Unexpected end of file";
        case AJIS_ERR_INVALID_TOKEN: return "Invalid token";
        case AJIS_ERR_INVALID_SYNTAX: return "Invalid syntax";
        case AJIS_ERR_DUPLICATE_KEY: return "Duplicate key";
        case AJIS_ERR_MISSING_COLON: return "Missing colon";
        case AJIS_ERR_MISSING_COMMA: return "Missing comma";
        case AJIS_ERR_TRAILING_COMMA: return "Trailing comma";
        case AJIS_ERR_INVALID_NUMBER: return "Invalid number";
        case AJIS_ERR_INVALID_STRING: return "Invalid string";
        case AJIS_ERR_INVALID_ESCAPE: return "Invalid escape sequence";
        case AJIS_ERR_INVALID_HEX: return "Invalid hexadecimal literal";
        case AJIS_ERR_INVALID_BINARY: return "Invalid binary literal";
        case AJIS_ERR_INVALID_CHAR: return "Invalid character";
        case AJIS_ERR_UNTERMINATED_COMMENT: return "Unterminated comment";
        case AJIS_ERR_DEPTH_LIMIT: return "Nesting depth limit exceeded";
        case AJIS_ERR_SIZE_LIMIT: return "Size limit exceeded";
        default: return "Unknown error code";
    }
}

static inline int ajis__is_linebreak(char c) {
    return (c == '\n' || c == '\r');
}

/* Compute visual column offset from [line_start, err_offset), expanding tabs. */
static inline size_t ajis__visual_col(const char* s, size_t line_start, size_t err_offset) {
    size_t col = 0;
    for (size_t i = line_start; i < err_offset; i++) {
        char c = s[i];
        if (c == '\t') {
            size_t w = AJIS_TAB_WIDTH ? (size_t)AJIS_TAB_WIDTH : 4;
            size_t next = ((col / w) + 1) * w;
            col = next;
        } else {
            /* ASCII assumed for now; for UTF-8 we treat bytes as columns. */
            col++;
        }
    }
    return col;
}

/* Print [start,end) but expand tabs to spaces so caret alignment matches. */
static inline void ajis__fwrite_expand_tabs(FILE* out, const char* s, size_t start, size_t end) {
    size_t col = 0;
    for (size_t i = start; i < end; i++) {
        char c = s[i];
        if (c == '\t') {
            size_t w = AJIS_TAB_WIDTH ? (size_t)AJIS_TAB_WIDTH : 4;
            size_t next = ((col / w) + 1) * w;
            while (col < next) {
                fputc(' ', out);
                col++;
            }
        } else {
            fputc(c, out);
            col++;
        }
    }
}

static inline void ajis_error_print_pretty(
    FILE* out,
    const char* filename,
    const char* source,
    size_t source_len,
    const ajis_error* err
) {
    if (!out || !err || err->code == AJIS_OK) return;

    /* Header */
    fprintf(out, "%sError:%s %s", AJIS_C_RED_B, AJIS_C_RESET, ajis_error_code_name(err->code));
    if (err->context && err->context[0] != '\0') {
        fprintf(out, " (%s)", err->context);
    }
    fprintf(out, "\n");

    /* Location */
    fprintf(out, "  %s-->%s %s:%u:%u\n",
        AJIS_C_BLUE_B, AJIS_C_RESET,
        filename ? filename : "<input>",
        err->location.line,
        err->location.column
    );

    /*
       Offsets can legitimately point to EOF (offset == source_len), e.g. unterminated
       block comment, dangling escape, missing closing quote/bracket at end of file.
       In that case we still want to show a snippet and caret at the end.
     */
    if (!source || source_len == 0 || err->location.offset > source_len) return;

    /* If the error points to "just past" the final newline, show the previous line. */
    size_t view_off = err->location.offset;
    if (view_off == source_len && source_len > 0 && source[source_len - 1] == '\n') {
        view_off = source_len - 1;
    }

    /* Find the logical line [line_start, line_end) containing the error offset */
    size_t line_start = view_off;
    while (line_start > 0 && !ajis__is_linebreak(source[line_start - 1])) {
        line_start--;
    }

    size_t line_end = view_off;
    while (line_end < source_len && !ajis__is_linebreak(source[line_end])) {
        line_end++;
    }

    /* If we are on CRLF and offset points at '\n', shift back to '\r' line end */
    if (line_end > line_start && source[line_end - 1] == '\r') {
        line_end--;
    }

    /* Line-number gutter width */
    int line_num_width = snprintf(NULL, 0, "%u", err->location.line);
    if (line_num_width < 2) line_num_width = 2;

    /* Visual caret column in the full line */
    size_t caret_vis_col = ajis__visual_col(source, line_start, view_off);

    /* Crop long lines around the caret (context on each side, in bytes) */
    size_t ctx = (size_t)AJIS_ERROR_CONTEXT_CHARS;
    size_t slice_start = line_start;
    size_t slice_end = line_end;
    int left_ellipsis = 0;
    int right_ellipsis = 0;

    /* Byte-based cropping, but caret alignment is computed visually. */
    size_t err_byte_in_line = view_off - line_start;

    if (line_end - line_start > (ctx * 2 + 40)) {
        size_t want_start = (err_byte_in_line > ctx) ? (view_off - ctx) : line_start;
        size_t want_end = view_off + ctx;

        if (want_start > line_start) {
            slice_start = want_start;
            left_ellipsis = 1;
        }
        if (want_end < line_end) {
            slice_end = want_end;
            right_ellipsis = 1;
        }
    }

    /* Visual caret column inside the printed slice */
    size_t slice_caret_vis_col;
    if (slice_start <= view_off) {
        slice_caret_vis_col = ajis__visual_col(source, slice_start, view_off);
    } else {
        slice_caret_vis_col = 0;
    }

    /* Print empty gutter */
    fprintf(out, " %*s %s|%s\n", line_num_width, "", AJIS_C_BLUE_B, AJIS_C_RESET);

    /* Print the source line (cropped) */
    fprintf(out, " %s%*u |%s ", AJIS_C_BLUE_B, line_num_width, err->location.line, AJIS_C_RESET);
    if (left_ellipsis) {
        fprintf(out, "%s...%s", AJIS_C_DIM, AJIS_C_RESET);
    }
    ajis__fwrite_expand_tabs(out, source, slice_start, slice_end);
    if (right_ellipsis) {
        fprintf(out, "%s...%s", AJIS_C_DIM, AJIS_C_RESET);
    }
    fprintf(out, "\n");

    /* Pointer line */
    fprintf(out, " %*s %s|%s ", line_num_width, "", AJIS_C_BLUE_B, AJIS_C_RESET);

    /* If we printed a left ellipsis, the caret must be shifted by 3 chars. */
    size_t caret_pad = slice_caret_vis_col + (left_ellipsis ? 3 : 0);

    for (size_t i = 0; i < caret_pad; i++) fputc(' ', out);

    /* Highlight length heuristic: default 1, extend to 2 for common "range" errors */
    size_t highlight_len = 1;
    if (err->context) {
        if (strstr(err->context, "separator") || strstr(err->context, "group") || strstr(err->context, "digit")) {
            highlight_len = 2;
        }
        if (strstr(err->context, "unterminated") || strstr(err->context, "newline")) {
            highlight_len = 1;
        }
    }

    fprintf(out, "%s^", AJIS_C_RED_B);
    for (size_t i = 1; i < highlight_len; i++) fputc('^', out);
    fprintf(out, "%s", AJIS_C_RESET);

    if (err->context && err->context[0] != '\0') {
        fprintf(out, " %s", err->context);
    }
    fprintf(out, "\n");

    /* Closing gutter */
    fprintf(out, " %*s %s|%s\n", line_num_width, "", AJIS_C_BLUE_B, AJIS_C_RESET);

    /* Optional extra hint: show absolute caret visual column for debugging */
    (void)caret_vis_col;
}

#ifdef __cplusplus
}
#endif

#endif /* AJIS_ERROR_PRINT_H */
