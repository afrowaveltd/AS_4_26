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

/* thousands separators are only allowed in the INTEGER part */
static int is_sep(int b) { return b == ' ' || b == '_' || b == ','; }

static int consume_if(ajis_input *in, int ch) {
    int b = ajis_input_peek(in);
    if (b == ch) {
        (void)ajis_input_next(in, NULL);
        return 1;
    }
    return 0;
}

/* ---------- skipping (ws + comments) ---------- */

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
                /* block comment: */
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

/* ---------- lexers ---------- */

static int is_hex_digit(int b) {
    return (b >= '0' && b <= '9') ||
           (b >= 'a' && b <= 'f') ||
           (b >= 'A' && b <= 'F');
}
static int is_bin_digit(int b) { return b == '0' || b == '1'; }
static int is_oct_digit(int b) { return b >= '0' && b <= '7'; }

static int is_base64_char(int b) {
    return (b >= 'A' && b <= 'Z') ||
           (b >= 'a' && b <= 'z') ||
           (b >= '0' && b <= '9') ||
           b == '+' || b == '/' || b == '=';
}

/* Lex hex binary literal: hex"DEADBEEF" */
static ajis_error_code lex_hex_binary(ajis_lexer *lx, ajis_token *out, ajis_error *err) {
    size_t start = lx->in->offset;
    
    /* consume 'hex"' */
    for (int i = 0; i < 4; i++) {
        (void)ajis_input_next(lx->in, NULL);
    }
    
    /* consume hex digits until closing quote */
    int digit_count = 0;
    for (;;) {
        int c = ajis_input_peek(lx->in);
        if (c < 0) {
            set_err(err, AJIS_ERR_UNEXPECTED_EOF, lx->in, "unterminated hex binary literal");
            return AJIS_ERR_UNEXPECTED_EOF;
        }
        
        if (c == '"') {
            /* closing quote */
            (void)ajis_input_next(lx->in, NULL);
            
            /* hex must have even number of digits (pairs of bytes) */
            if (digit_count % 2 != 0) {
                set_err(err, AJIS_ERR_INVALID_STRING, lx->in, "hex binary must have even number of digits");
                return AJIS_ERR_INVALID_STRING;
            }
            
            set_tok(out, AJIS_TOKEN_HEX_BINARY, start, lx->in->offset - start);
            return AJIS_OK;
        }
        
        if (!is_hex_digit(c)) {
            set_err(err, AJIS_ERR_INVALID_STRING, lx->in, "invalid character in hex binary literal");
            return AJIS_ERR_INVALID_STRING;
        }
        
        digit_count++;
        (void)ajis_input_next(lx->in, NULL);
    }
}

/* Lex base64 binary literal: b64"SGVsbG8=" */
static ajis_error_code lex_b64_binary(ajis_lexer *lx, ajis_token *out, ajis_error *err) {
    size_t start = lx->in->offset;
    
    /* consume 'b64"' */
    for (int i = 0; i < 4; i++) {
        (void)ajis_input_next(lx->in, NULL);
    }
    
    /* consume base64 characters until closing quote */
    for (;;) {
        int c = ajis_input_peek(lx->in);
        if (c < 0) {
            set_err(err, AJIS_ERR_UNEXPECTED_EOF, lx->in, "unterminated b64 binary literal");
            return AJIS_ERR_UNEXPECTED_EOF;
        }
        
        if (c == '"') {
            /* closing quote */
            (void)ajis_input_next(lx->in, NULL);
            set_tok(out, AJIS_TOKEN_B64_BINARY, start, lx->in->offset - start);
            return AJIS_OK;
        }
        
        if (!is_base64_char(c)) {
            set_err(err, AJIS_ERR_INVALID_STRING, lx->in, "invalid character in b64 binary literal");
            return AJIS_ERR_INVALID_STRING;
        }
        
        (void)ajis_input_next(lx->in, NULL);
    }
}

static ajis_error_code lex_number(ajis_lexer *lx, ajis_token *out, ajis_error *err) {
    size_t start = lx->in->offset;

    /* optional leading minus */
    if (ajis_input_peek(lx->in) == '-') {
        (void)ajis_input_next(lx->in, NULL);
        if (ajis_input_peek(lx->in) < 0) {
            set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "expected digit after '-'");
            return AJIS_ERR_INVALID_NUMBER;
        }
    }

    /* ------------------------------------------------------------
       Base-prefixed integers: 0x..., 0b..., 0o...
       - separators allowed only between digits
       - separators may be: space, '_' or ',' (when enabled)
       - separators must not be mixed within one literal
       - no fraction or exponent allowed
       ------------------------------------------------------------ */
    if (ajis_input_peek(lx->in) == '0') {
        int p1 = ajis_input_peek_ahead(lx->in, 1);
        int is_hex = (p1 == 'x' || p1 == 'X');
        int is_bin = (p1 == 'b' || p1 == 'B');
        int is_oct = (p1 == 'o' || p1 == 'O');

        if (is_hex || is_bin || is_oct) {
            /* consume '0' and prefix */
            (void)ajis_input_next(lx->in, NULL);
            (void)ajis_input_next(lx->in, NULL);

            int (*is_digit_base)(int) = is_hex ? is_hex_digit : (is_bin ? is_bin_digit : is_oct_digit);
            ajis_error_code bad_code = is_hex ? AJIS_ERR_INVALID_HEX : (is_bin ? AJIS_ERR_INVALID_BINARY : AJIS_ERR_INVALID_NUMBER);

            int saw_digit = 0;
            int sep_char = 0; /* 0 = none, otherwise one of ' ', '_' ',' */
            int last_was_sep = 0;
            int group_len = 0;     /* digits in current group */
            int saw_sep = 0;       /* have we seen any separator */
            int group_size = 0;    /* expected group size (2 or 4 for hex, 4 for bin, 3 for oct) */

            for (;;) {
                int b = ajis_input_peek(lx->in);
                if (b < 0) break;

                if (is_digit_base(b)) {
                    saw_digit = 1;
                    last_was_sep = 0;
                    group_len++;
                    (void)ajis_input_next(lx->in, NULL);
                    continue;
                }

                if (lx->opt.allow_number_separators && is_sep(b)) {
                    int next = ajis_input_peek_ahead(lx->in, 1);
                    if (!is_digit_base(next)) {
                        /* not between digits -> stop (e.g., "0xDEAD_" should fail later) */
                        break;
                    }
                    if (!saw_digit) {
                        set_err(err, bad_code, lx->in, "separator directly after base prefix");
                        return bad_code;
                    }
                    if (last_was_sep) {
                        set_err(err, bad_code, lx->in, "double separator in base literal");
                        return bad_code;
                    }
                    if (sep_char == 0) sep_char = b;
                    else if (sep_char != b) {
                        set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "mixed number separators");
                        return AJIS_ERR_INVALID_NUMBER;
                    }

                    /* Grouping validation for different bases */
                    if (!saw_sep) {
                        /* First separator: establish expected group size from first group */
                        if (is_hex) {
                            /* Hex allows grouping by 2 or 4, determine from first group */
                            if (group_len >= 1 && group_len <= 2) {
                                group_size = 2;
                            } else if (group_len >= 3 && group_len <= 4) {
                                group_size = 4;
                            } else {
                                set_err(err, AJIS_ERR_INVALID_HEX, lx->in, "hex grouping must be by 2 or 4 digits");
                                return AJIS_ERR_INVALID_HEX;
                            }
                        } else if (is_bin) {
                            /* Binary must be grouped by 4, first group can be 1-4 */
                            if (group_len < 1 || group_len > 4) {
                                set_err(err, AJIS_ERR_INVALID_BINARY, lx->in, "binary first group must be 1-4 bits");
                                return AJIS_ERR_INVALID_BINARY;
                            }
                            group_size = 4;
                       } else if (is_oct) {
                            /* Octal grouped like decimal: groups of 3, first group 1-3 */
                            if (group_len < 1 || group_len > 3) {
                                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "octal first group must be 1-3 digits");
                                return AJIS_ERR_INVALID_NUMBER;
                            }
                        group_size = 3;
                       }
                        saw_sep = 1;

                    } else {
                        /* Subsequent separators: must match established group size */
                        if (group_size > 0 && group_len != group_size) {
                            set_err(err, bad_code, lx->in, "inconsistent digit grouping");
                            return bad_code;
                        }
                    }

                    group_len = 0;
                    last_was_sep = 1;
                    (void)ajis_input_next(lx->in, NULL);
                    continue;
                }

            /* Disambiguate token comma/space vs digit-group separator.
               Treat ',' and ' ' as group separators ONLY when exactly 3 digits follow.
               Otherwise, end the number here and let the lexer emit COMMA / whitespace. */
            if (b == ',' || b == ' ') {
                int run = 0;
                for (int k = 1; k <= 4; k++) {
                    int c = ajis_input_peek_ahead(lx->in, k);
                    if (!is_digit_base(c)) break;
                    run++;
                }
                /* For hex: allow 2 or 4, for binary: 4, for octal: any length (no strict grouping) */
                int valid_run = 0;
                if (is_hex && (run == 2 || run == 4)) valid_run = 1;
                else if (is_bin && run == 4) valid_run = 1;
                else if (is_oct && run == 3) valid_run = 1;
                
                if (!valid_run) {
                    break;
                }
            }

                break;
            }

            if (!saw_digit) {
                set_err(err, bad_code, lx->in, "expected digits after base prefix");
                return bad_code;
            }
            if (last_was_sep) {
                set_err(err, bad_code, lx->in, "separator at end of base literal");
                return bad_code;
            }
            /* Final group validation if separators were used */
            if (saw_sep && group_size > 0 && group_len != group_size) {
                set_err(err, bad_code, lx->in, "inconsistent digit grouping at end");
                return bad_code;
            }

            /* Disallow fraction/exponent for base-prefixed integers */
            int tail = ajis_input_peek(lx->in);
            if (tail == '.' || tail == 'e' || tail == 'E') {
                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "base literal cannot have fraction or exponent");
                return AJIS_ERR_INVALID_NUMBER;
            }

            set_tok(out, AJIS_TOKEN_NUMBER, start, lx->in->offset - start);
            return AJIS_OK;
        }
    }

    /* ------------------------------------------------------------
       Decimal numbers (with optional separators in INTEGER part)
       - grouping by 3 when separators are used
       - separators must not be mixed within one literal
       - no separators in fraction/exponent
       ------------------------------------------------------------ */

    /* Check for leading zero: single '0' is okay, but '0' followed by separator + digit or another digit is invalid */
    if (ajis_input_peek(lx->in) == '0') {
        int next = ajis_input_peek_ahead(lx->in, 1);
        /* 0 followed by digit is invalid (e.g., "00", "01", "0123") */
        if (is_digit(next)) {
            set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "leading zero not allowed");
            return AJIS_ERR_INVALID_NUMBER;
        }
        /* 0 followed by separator + digit is invalid (e.g., "0_000", "0 123") */
        if (lx->opt.allow_number_separators && is_sep(next)) {
            int after_sep = ajis_input_peek_ahead(lx->in, 2);
            if (is_digit(after_sep)) {
                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "leading zero with separator not allowed");
                return AJIS_ERR_INVALID_NUMBER;
            }
        }
    }

    /* integer part */
    int saw_digit = 0;
    int sep_char = 0;   /* 0 = none, otherwise one of ' ', '_' ',' */
    int group_len = 0;  /* digits in current group */
    int saw_sep = 0;    /* have we seen any separator in integer part */

    for (;;) {
        int b = ajis_input_peek(lx->in);
        if (b < 0) break;

        if (is_digit(b)) {
            saw_digit = 1;
            group_len++;
            (void)ajis_input_next(lx->in, NULL);
            continue;
        }

        /* optional thousands separators: only if digit SEP digit */
        if (lx->opt.allow_number_separators && is_sep(b)) {
            /* Disambiguate comma/space as separator vs token boundary.
               For decimal: treat ',' and ' ' as separators ONLY when exactly 3 digits follow.
               Otherwise, end number here. Underscore is always a separator. */
            if (b == ',' || b == ' ') {
                int run = 0;
                for (int k = 1; k <= 4; k++) {
                    int c = ajis_input_peek_ahead(lx->in, k);
                    if (!is_digit(c)) break;
                    run++;
                }
                /* For decimal: must be exactly 3 digits to be separator */
                if (run != 3) {
                    break; /* Not a separator, end number here */
                }
            }
            
            int next = ajis_input_peek_ahead(lx->in, 1);
            if (!is_digit(next)) {
                /* not between digits => number ends here (e.g. "1000," or "1000 ") */
                break;
            }

            if (!saw_digit) {
                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "separator before any digit");
                return AJIS_ERR_INVALID_NUMBER;
            }

            /* grouping validation */
            if (!saw_sep) {
                /* first group: 1..3 digits */
                if (group_len < 1 || group_len > 3) {
                    set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "invalid first digit group size");
                    return AJIS_ERR_INVALID_NUMBER;
                }
                saw_sep = 1;
            } else {
                /* subsequent groups must be exactly 3 */
                if (group_len != 3) {
                    set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "invalid digit group size (must be 3)");
                    return AJIS_ERR_INVALID_NUMBER;
                }
            }

            int this_sep = b;
            if (sep_char == 0) sep_char = this_sep;
            else if (sep_char != this_sep) {
                set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "mixed number separators");
                return AJIS_ERR_INVALID_NUMBER;
            }

            group_len = 0;
            (void)ajis_input_next(lx->in, NULL); /* consume separator */
            continue;
        }

        break;
    }

    if (!saw_digit) {
        set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "expected digits");
        return AJIS_ERR_INVALID_NUMBER;
    }

    /* if separators were used, the last group must be exactly 3 digits */
    if (saw_sep && group_len != 3) {
        set_err(err, AJIS_ERR_INVALID_NUMBER, lx->in, "invalid last digit group size (must be 3)");
        return AJIS_ERR_INVALID_NUMBER;
    }

    /* fraction (no separators allowed here) */
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

    /* token span covers entire raw number as written */
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
            /* escape sequence: consume '\\' then consume one more byte */
            (void)ajis_input_next(lx->in, NULL);
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
    ajis_error_reset(err);

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

    /* keywords: true/false/null, and binary literals hex"/b64" */
    if (is_alpha(b)) {
        size_t start = lx->in->offset;

        /* Check for binary literals first (hex" and b64") */
        if (b == 'h') {
            /* Check for hex" */
            if (ajis_input_peek_ahead(lx->in, 1) == 'e' &&
                ajis_input_peek_ahead(lx->in, 2) == 'x' &&
                ajis_input_peek_ahead(lx->in, 3) == '"') {
                return lex_hex_binary(lx, out_tok, err);
            }
        } else if (b == 'b') {
            /* Check for b64" */
            if (ajis_input_peek_ahead(lx->in, 1) == '6' &&
                ajis_input_peek_ahead(lx->in, 2) == '4' &&
                ajis_input_peek_ahead(lx->in, 3) == '"') {
                return lex_b64_binary(lx, out_tok, err);
            }
        }

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
