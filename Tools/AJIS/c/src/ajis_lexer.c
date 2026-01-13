#include "../include/ajis_lexer.h"

/* ---------- helpers ---------- */

static void set_err(ajis_error *err, ajis_error_code code, const ajis_input *in, const char *ctx) {
    if (!err) return;
    err->code = code;
    err->location.line = in->line;
    err->location.column = in->column;
    err->location.offset = in->offset;
    err->context = ctx;
}

static void set_tok(ajis_token *t, ajis_token_type type, size_t off, size_t len) {
    t->type = type;
    t->span.offset = off;
    t->span.length = len;
}

int ajis_input_peek_ahead(const ajis_input *in, size_t ahead) {
    if (!in || !in->data) return -1;
    size_t pos = in->offset + ahead;
    if (pos >= in->length) return -1;
    return (unsigned char)in->data[pos];
}

static int is_ws(int b) {
    return b == ' ' || b == '\t' || b == '\r' || b == '\n';
}

static int is_digit(int b) { return b >= '0' && b <= '9'; }

static int is_sep(int b) { return b == ' ' || b == '_' || b == ','; }

static int consume_if(ajis_input *in, int ch) {
    int b = ajis_input_peek(in);
    if (b == ch) { (void)ajis_input_next(in, NULL); return 1; }
    return 0;
}


/* Skip whitespace and comments. */
static ajis_error_code skip_ignored(ajis_lexer *lx, ajis_error *err) {
    for (;;) {
        int b = ajis_input_peek(lx->in);
        if (b < 0) return AJIS_OK;

        /* whitespace */
        if (is_ws(b)) {
            (void)ajis_input_next(lx->in, NULL);
            continue;
        }

        /* comments start with '/' */
        if (b == '/') {
            /* need peek second byte */
            size_t save_off = lx->in->offset;
            uint32_t save_line = lx->in->line;
            uint32_t save_col = lx->in->column;

            (void)ajis_input_next(lx->in, NULL); /* consume '/' */
            int b2 = ajis_input_peek(lx->in);
            if (b2 < 0) {
                /* just a '/' at end -> treat as invalid token later; restore */
                lx->in->offset = save_off;
                lx->in->line = save_line;
                lx->in->column = save_col;
                return AJIS_OK;
            }

            if (b2 == '/') {
                /* line comment: consume until newline or EOF */
                (void)ajis_input_next(lx->in, NULL); /* consume second '/' */
                for (;;) {
                    int c = ajis_input_peek(lx->in);
                    if (c < 0) break;
                    c = ajis_input_next(lx->in, NULL);
                    if (c == '\n') break;
                }
                continue;
            }

            if (b2 == '*') {
                /* block comment: consume until '*/
                (void)ajis_input_next(lx->in, NULL); /* consume '*' */
                int prev = 0;
                for (;;) {
                    int c = ajis_input_next(lx->in, NULL);
                    if (c < 0) {
                        set_err(err, AJIS_ERR_UNTERMINATED_COMMENT, lx->in, "unterminated block comment");
                        return AJIS_ERR_UNTERMINATED_COMMENT;
                    }
                    if (prev == '*' && c == '/') break;
                    prev = c;
                }
                continue;
            }

            /* not a comment -> restore and stop skipping */
            lx->in->offset = save_off;
            lx->in->line = save_line;
            lx->in->column = save_col;
            return AJIS_OK;
        }

        /* nothing to skip */
        return AJIS_OK;
    }
}
static ajis_error_code lex_number(ajis_lexer *lx, ajis_token *out, ajis_error *err) {
    size_t start = lx->in->offset;

    /* optional leading minus */
    if (ajis_input_peek(lx->in) == '-') {
        (void)ajis_input_next(lx->in, NULL);
        if (!is_digit(ajis_input_peek(lx->in))) {
            set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "expected digit after '-'");
            return AJIS_ERR_INVALID_NUMBER;
        }
    }

    /* integer part */
    int saw_digit = 0;
    int sep_char = 0; /* 0 = none, otherwise one of ' ', '_' ',' */

    for (;;) {
        int b = ajis_input_peek(lx->in);
        if (b < 0) break;

        if (is_digit(b)) {
            saw_digit = 1;
            (void)ajis_input_next(lx->in, NULL);
            continue;
        }

        /* optional thousands separators: only if digit SEP digit */
        if (lx->opt.allow_number_separators && is_sep(b)) {
            int next = ajis_input_peek_ahead(lx->in, 1);
            if (!is_digit(next)) {
                /* not between digits => number ends here (e.g. "1000," or "1000 ") */
                break;
            }

            if (!saw_digit) {
                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "separator before any digit");
                return AJIS_ERR_INVALID_NUMBER;
            }

            int this_sep = b;
            if (sep_char == 0) sep_char = this_sep;
            else if (sep_char != this_sep) {
                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "mixed number separators");
                return AJIS_ERR_INVALID_NUMBER;
            }

            (void)ajis_input_next(lx->in, NULL); /* consume separator */
            continue;
        }

        break;
    }

    if (!saw_digit) {
        set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "expected digits");
        return AJIS_ERR_INVALID_NUMBER;
    }

    /* fraction */
    if (consume_if(lx->in, '.')) {
        if (!is_digit(ajis_input_peek(lx->in))) {
            set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "expected digit after '.'");
            return AJIS_ERR_INVALID_NUMBER;
        }
        while (is_digit(ajis_input_peek(lx->in))) {
            (void)ajis_input_next(lx->in, NULL);
        }
    }

    /* exponent */
    int b = ajis_input_peek(lx->in);
    if (b == 'e' || b == 'E') {
        (void)ajis_input_next(lx->in, NULL);
        int s = ajis_input_peek(lx->in);
        if (s == '+' || s == '-') (void)ajis_input_next(lx->in, NULL);

        if (!is_digit(ajis_input_peek(lx->in))) {
            set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "expected digit in exponent");
            return AJIS_ERR_INVALID_NUMBER;
        }
        while (is_digit(ajis_input_peek(lx->in))) {
            (void)ajis_input_next(lx->in, NULL);
        }
    }

    set_tok(out, AJIS_TOKEN_NUMBER, start, lx->in->offset - start);
    return AJIS_OK;
}

static ajis_error_code lex_string(ajis_lexer *lx, ajis_token *out, ajis_error *err) {
    /* current peek is '"' */
    (void)ajis_input_next(lx->in, NULL); /* consume opening quote */

    size_t start = lx->in->offset; /* content starts AFTER opening quote */

    for (;;) {
        int c = ajis_input_peek(lx->in);
        if (c < 0) {
            set_err(err, AJIS_ERR_UNEXPECTED_EOF, lx->in, "unterminated string");
            return AJIS_ERR_UNEXPECTED_EOF;
        }

        if (c == '"') {
            /* closing quote */
            size_t end = lx->in->offset; /* end is BEFORE closing quote */
            (void)ajis_input_next(lx->in, NULL); /* consume closing quote */
            set_tok(out, AJIS_TOKEN_STRING, start, end - start);
            return AJIS_OK;
        }

        if (c == '\\') {
            /* escape sequence: consume '\' then consume one more byte */
            (void)ajis_input_next(lx->in, NULL); /* consume '\' */
            int esc = ajis_input_next(lx->in, NULL);
            if (esc < 0) {
                set_err(err, AJIS_ERR_INVALID_ESCAPE, lx->in, "escape at end of input");
                return AJIS_ERR_INVALID_ESCAPE;
            }
            continue;
        }

        if (c == '\n' && !lx->opt.allow_multiline_strings) {
            set_err(err, AJIS_ERR_INVALID_STRING, lx->in, "newline in string (multiline disabled)");
            return AJIS_ERR_INVALID_STRING;
        }

        /* consume normal byte */
        (void)ajis_input_next(lx->in, NULL);
    }
}
static int is_alpha(int b) {
    return (b >= 'a' && b <= 'z') || (b >= 'A' && b <= 'Z');
}

static int match_keyword(ajis_lexer *lx, const char *kw) {
    /* returns 1 if matched and consumed, 0 otherwise (and does not consume) */
    size_t save_off = lx->in->offset;
    uint32_t save_line = lx->in->line;
    uint32_t save_col = lx->in->column;

    for (size_t i = 0; kw[i] != '\0'; i++) {
        int c = ajis_input_peek(lx->in);
        if (c < 0 || c != (unsigned char)kw[i]) {
            /* restore */
            lx->in->offset = save_off;
            lx->in->line = save_line;
            lx->in->column = save_col;
            return 0;
        }
        (void)ajis_input_next(lx->in, NULL);
    }

    /* ensure keyword boundary: next char is not alpha/num/_ */
    int next = ajis_input_peek(lx->in);
    if (next >= 0) {
        int is_ident = is_alpha(next) || (next >= '0' && next <= '9') || next == '_';
        if (is_ident) {
            lx->in->offset = save_off;
            lx->in->line = save_line;
            lx->in->column = save_col;
            return 0;
        }
    }

    return 1;
}


/* ---------- public API ---------- */

ajis_error_code ajis_lexer_next(ajis_lexer *lx, ajis_token *out_tok, ajis_error *err) {
    if (!lx || !lx->in || !out_tok) return AJIS_ERR_UNKNOWN;

    /* default output */
    set_tok(out_tok, AJIS_TOKEN_INVALID, 0, 0);
    if (err) {
    err->code = AJIS_OK;
    err->location.line = 0;
    err->location.column = 0;
    err->location.offset = 0;
    err->context = NULL;
}


    /* skip whitespace + comments */
    ajis_error_code sk = skip_ignored(lx, err);
    if (sk != AJIS_OK) {
        set_tok(out_tok, AJIS_TOKEN_INVALID, 0, 0);
        return sk;
    }

    int b = ajis_input_peek(lx->in);
    if (b < 0) {
        set_tok(out_tok, AJIS_TOKEN_EOF, lx->in->offset, 0);
        return AJIS_OK;
    }

    /* structural single-byte tokens */
    switch (b) {
        case '{': (void)ajis_input_next(lx->in, NULL); set_tok(out_tok, AJIS_TOKEN_LBRACE,   lx->in->offset - 1, 1); return AJIS_OK;
        case '}': (void)ajis_input_next(lx->in, NULL); set_tok(out_tok, AJIS_TOKEN_RBRACE,   lx->in->offset - 1, 1); return AJIS_OK;
        case '[': (void)ajis_input_next(lx->in, NULL); set_tok(out_tok, AJIS_TOKEN_LBRACKET, lx->in->offset - 1, 1); return AJIS_OK;
        case ']': (void)ajis_input_next(lx->in, NULL); set_tok(out_tok, AJIS_TOKEN_RBRACKET, lx->in->offset - 1, 1); return AJIS_OK;
        case ':': (void)ajis_input_next(lx->in, NULL); set_tok(out_tok, AJIS_TOKEN_COLON,    lx->in->offset - 1, 1); return AJIS_OK;
        case ',': (void)ajis_input_next(lx->in, NULL); set_tok(out_tok, AJIS_TOKEN_COMMA,    lx->in->offset - 1, 1); return AJIS_OK;
        default: break;
    }

    /* string */
    if (b == '"') {
        return lex_string(lx, out_tok, err);
    }
    
 /* keywords: true/false/null */
    if (is_alpha(b)) {
        size_t start = lx->in->offset;

        if (match_keyword(lx, "true")) {
            set_tok(out_tok, AJIS_TOKEN_TRUE, start, 4);
            return AJIS_OK;
        }
        if (match_keyword(lx, "false")) {
            set_tok(out_tok, AJIS_TOKEN_FALSE, start, 5);
            return AJIS_OK;
        }
        if (match_keyword(lx, "null")) {
            set_tok(out_tok, AJIS_TOKEN_NULL, start, 4);
            return AJIS_OK;
        }

        set_err(err, AJIS_ERR_INVALID_TOKEN, lx->in, "unknown identifier");
        set_tok(out_tok, AJIS_TOKEN_INVALID, start, 0);
        return AJIS_ERR_INVALID_TOKEN;
    }
        /* number: starts with digit or '-' */
    if (is_digit(b) || b == '-') {
        return lex_number(lx, out_tok, err);
    }

    /* for now: anything else is invalid */
    set_err(err, AJIS_ERR_INVALID_TOKEN, lx->in, "unexpected byte (micro-lexer)");
    set_tok(out_tok, AJIS_TOKEN_INVALID, lx->in->offset, 0);
    return AJIS_ERR_INVALID_TOKEN;
}
