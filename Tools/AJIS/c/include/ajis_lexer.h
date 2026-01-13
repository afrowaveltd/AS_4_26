#ifndef AJIS_LEXER_H
#define AJIS_LEXER_H

#include "ajis_input.h"
#include "ajis_token.h"
#include "ajis_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ajis_lexer_options {
    int allow_multiline_strings; /* default 0 (JSON-like) */
    int allow_number_separators;   /* default 0 (JSON-like) */
} ajis_lexer_options;

typedef struct ajis_lexer {
    ajis_input *in;
    ajis_lexer_options opt;
} ajis_lexer;

/* Initialize lexer */
static inline void ajis_lexer_init(ajis_lexer *lx, ajis_input *in, ajis_lexer_options opt) {
    lx->in = in;
    lx->opt = opt;
}

int ajis_input_peek_ahead(const ajis_input *in, size_t ahead);

/*
 * Produce next token.
 * Returns AJIS_OK on success, otherwise error code and fills `err`.
 */
ajis_error_code ajis_lexer_next(ajis_lexer *lx, ajis_token *out_tok, ajis_error *err);

#ifdef __cplusplus
}
#endif

#endif /* AJIS_LEXER_H */
