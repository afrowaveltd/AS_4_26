#include "../include/ajis_input.h"
#include "../include/ajis_lexer.h"
#include <stdio.h>
#include <string.h>

static const char* tok_name(ajis_token_type t) {
    switch (t) {
        case AJIS_TOKEN_EOF: return "EOF";
        case AJIS_TOKEN_LBRACE: return "LBRACE";
        case AJIS_TOKEN_RBRACE: return "RBRACE";
        case AJIS_TOKEN_LBRACKET: return "LBRACKET";
        case AJIS_TOKEN_RBRACKET: return "RBRACKET";
        case AJIS_TOKEN_COLON: return "COLON";
        case AJIS_TOKEN_COMMA: return "COMMA";
        case AJIS_TOKEN_STRING: return "STRING";
        case AJIS_TOKEN_INVALID: return "INVALID";
        case AJIS_TOKEN_TRUE: return "TRUE";
        case AJIS_TOKEN_FALSE: return "FALSE";
        case AJIS_TOKEN_NULL: return "NULL";
        case AJIS_TOKEN_NUMBER: return "NUMBER";
        
        default: return "OTHER";
    }
}

int main(void) {
        const char *src =
        "{\n"
        "  /* comment */\n"
        "  \"a\" /*key*/ : /*value*/ \"Nazd\\\"arek\",\n"
        "  // next line\n"
        "  \"b\": \"Line1\\nLine2\",\n"
        "  \"c\": true,\n"
        "  \"d\": false,\n"
        "  \"e\": null,\n"
        "  \"n1\": 1000,\n"
        "  \"n2\": -12.34,\n"
        "  \"n3\": 1e9,\n"
        "  \"n4\": 1_000,\n"
        "  \"n5\": 12 345.67e-10,\n"
        "  \"n6\": 0.000,001,\n"
        "  \"n7\": 1,000,000\n"
        "}\n";


    

    ajis_input in;
    ajis_input_init(&in, src, strlen(src));

    ajis_lexer lx;
    ajis_lexer_options opt;
    opt.allow_multiline_strings = 0; /* keep strict for now */
    opt.allow_number_separators = 1; /* allow separators in numbers */
    ajis_lexer_init(&lx, &in, opt);

    for (;;) {
        ajis_token tok;
        ajis_error err = ajis_error_ok();

        ajis_error_code rc = ajis_lexer_next(&lx, &tok, &err);
        if (rc != AJIS_OK) {
            printf("LEX ERROR: code=%d at line=%u col=%u off=%zu (%s)\n",
                (int)err.code, err.location.line, err.location.column, err.location.offset,
                err.context ? err.context : "no ctx");
            return 1;
        }

        printf("%s span(off=%zu,len=%zu)\n", tok_name(tok.type), tok.span.offset, tok.span.length);

        if (tok.type == AJIS_TOKEN_STRING) {
            /* print raw slice (content inside quotes) */
            printf("  raw=\"");
            for (size_t i = 0; i < tok.span.length; i++) {
                unsigned char ch = (unsigned char)src[tok.span.offset + i];
                if (ch == '\n') printf("\\n");
                else putchar(ch);
            }
            printf("\"\n");
        }

        if (tok.type == AJIS_TOKEN_EOF) break;
    }

    return 0;
}
