#ifndef AJIS_ERROR_H
#define AJIS_ERROR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
   AJIS Error Codes
   ============================================================ */

typedef enum ajis_error_code {
    AJIS_OK = 0,

    /* Generic */
    AJIS_ERR_UNKNOWN,
    AJIS_ERR_UNEXPECTED_EOF,
    AJIS_ERR_INVALID_TOKEN,
    AJIS_ERR_INVALID_SYNTAX,

    /* Structural */
    AJIS_ERR_DUPLICATE_KEY,
    AJIS_ERR_MISSING_COLON,
    AJIS_ERR_MISSING_COMMA,
    AJIS_ERR_TRAILING_COMMA,

    /* Literals */
    AJIS_ERR_INVALID_NUMBER,
    AJIS_ERR_INVALID_STRING,
    AJIS_ERR_INVALID_ESCAPE,
    AJIS_ERR_INVALID_HEX,
    AJIS_ERR_INVALID_BINARY,
    AJIS_ERR_INVALID_CHAR,

    /* Comments */
    AJIS_ERR_UNTERMINATED_COMMENT,

    /* Limits */
    AJIS_ERR_DEPTH_LIMIT,
    AJIS_ERR_SIZE_LIMIT

} ajis_error_code;


/* ============================================================
   AJIS Error Location
   ============================================================ */

typedef struct ajis_error_location {
    uint32_t line;        /* 1-based */
    uint32_t column;      /* 1-based */
    size_t   offset;      /* byte offset from start */
} ajis_error_location;


/* ============================================================
   AJIS Error Object
   ============================================================ */

typedef struct ajis_error {
    ajis_error_code code;
    ajis_error_location location;

    /* Optional short context (non-owning) */
    const char *context;
} ajis_error;


/* ============================================================
   Helpers
   ============================================================ */

static inline ajis_error ajis_error_ok(void) {
    ajis_error err;
    err.code = AJIS_OK;
    err.location.line = 0;
    err.location.column = 0;
    err.location.offset = 0;
    err.context = NULL;
    return err;
}

static inline void ajis_error_reset(ajis_error *err) {
    if (!err) return;
    err->code = AJIS_OK;
    err->location.line = 0;
    err->location.column = 0;
    err->location.offset = 0;
    err->context = NULL;
}


static inline int ajis_error_is_ok(const ajis_error *err) {
    return err && err->code == AJIS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* AJIS_ERROR_H */
