#ifndef AJIS_TOKEN_H
#define AJIS_TOKEN_H

#include <stddef.h>
#include <stdint.h>

/*
* ajis_span
* ---------
* Represents a slice of the original input buffer.
* Offset and length are expressed in BYTES.
*
* This is the fundamental bridge between lexer and parser.
*/

typedef struct {
    size_t offset;  /* byte offset from the beginning of input */
    size_t length;  /* length in bytes */
} ajis_span;

/*
* ajis_token_type
* ---------------
* Token types produced by the AJIS lexer.
*
* Syntax tokens are ASCII by design.
* String and number tokens carry a span for deferred processing.
*/

typedef enum {
AJIS_TOKEN_EOF = 0,


/* structural tokens */
AJIS_TOKEN_LBRACE, /* { */
AJIS_TOKEN_RBRACE, /* } */
AJIS_TOKEN_LBRACKET, /* [ */
AJIS_TOKEN_RBRACKET, /* ] */
AJIS_TOKEN_COLON, /* : */
AJIS_TOKEN_COMMA, /* , */


/* value tokens */
AJIS_TOKEN_STRING,
AJIS_TOKEN_NUMBER,
AJIS_TOKEN_TRUE,
AJIS_TOKEN_FALSE,
AJIS_TOKEN_NULL,

/* binary literals */
AJIS_TOKEN_HEX_BINARY,   /* hex"..." */
AJIS_TOKEN_B64_BINARY,   /* b64"..." */

/* error / invalid token */
AJIS_TOKEN_INVALID
} ajis_token_type;

/*
* ajis_token
* ----------
* A single lexical token.
*
* For STRING and NUMBER tokens, `span` refers to the raw slice
* of the input buffer (no decoding performed here).
*/
typedef struct {
ajis_token_type type;
ajis_span span;
} ajis_token;


#endif /* AJIS_TOKEN_H */