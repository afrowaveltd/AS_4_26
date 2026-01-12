#ifndef AJIS_INPUT_H
#define AJIS_INPUT_H

#include <stddef.h>
#include <stdint.h>
#include "ajis_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
   AJIS Input Source
   ============================================================ */

typedef struct ajis_input {
    const uint8_t *data;   /* UTF-8 bytes */
    size_t length;         /* total size in bytes */
    size_t offset;         /* current byte offset */

    uint32_t line;         /* 1-based */
    uint32_t column;       /* 1-based */

} ajis_input;


/* ============================================================
   Initialization
   ============================================================ */

static inline void ajis_input_init(
    ajis_input *in,
    const void *data,
    size_t length
) {
    in->data   = (const uint8_t *)data;
    in->length = length;
    in->offset = 0;
    in->line   = 1;
    in->column = 1;
}


/* ============================================================
   Basic Operations
   ============================================================ */

/* Return non-zero if end of input */
static inline int ajis_input_eof(const ajis_input *in) {
    return in->offset >= in->length;
}


/* Peek next byte without consuming */
static inline int ajis_input_peek(const ajis_input *in) {
    if (ajis_input_eof(in))
        return -1;
    return in->data[in->offset];
}


/* Consume and return next byte */
static inline int ajis_input_next(
    ajis_input *in,
    ajis_error *err
) {
    if (ajis_input_eof(in)) {
        if (err) {
            err->code = AJIS_ERR_UNEXPECTED_EOF;
            err->location.line = in->line;
            err->location.column = in->column;
            err->location.offset = in->offset;
            err->context = NULL;
        }
        return -1;
    }

    uint8_t ch = in->data[in->offset++];
    
    if (ch == '\n') {
        in->line++;
        in->column = 1;
    } else {
        in->column++;
    }

    return ch;
}


/* ============================================================
   Location Helper
   ============================================================ */

static inline ajis_error_location ajis_input_location(
    const ajis_input *in
) {
    ajis_error_location loc;
    loc.line   = in->line;
    loc.column = in->column;
    loc.offset = in->offset;
    return loc;
}

#ifdef __cplusplus
}
#endif

#endif /* AJIS_INPUT_H */
