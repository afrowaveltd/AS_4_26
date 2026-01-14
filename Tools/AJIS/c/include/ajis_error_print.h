#ifndef AJIS_ERROR_PRINT_H
#define AJIS_ERROR_PRINT_H

#include "ajis_error.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
   Pretty Error Printing (Rust-style)
   ============================================================ */

/* Print a pretty error message with source context
   
   Example output:
   
   Error: invalid digit group size (must be 3)
     --> test.ajis:20:9
      |
   20 | [9,10,11,12,13,14,15,16],
      |     ^^ inconsistent grouping
      |
*/

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

static inline void ajis_error_print_pretty(
    FILE* out,
    const char* filename,
    const char* source,
    size_t source_len,
    const ajis_error* err
) {
    if (!out || !err || err->code == AJIS_OK) return;

    /* Print error header */
    fprintf(out, "\033[1;31mError:\033[0m %s", ajis_error_code_name(err->code));
    if (err->context) {
        fprintf(out, " (%s)", err->context);
    }
    fprintf(out, "\n");

    /* Print location */
    fprintf(out, "  \033[1;34m-->\033[0m %s:%u:%u\n", 
        filename ? filename : "<input>",
        err->location.line,
        err->location.column);

    if (!source || source_len == 0 || err->location.offset >= source_len) {
        return;
    }

    /* Find the line containing the error */
    size_t line_start = err->location.offset;
    while (line_start > 0 && source[line_start - 1] != '\n') {
        line_start--;
    }

    size_t line_end = err->location.offset;
    while (line_end < source_len && source[line_end] != '\n' && source[line_end] != '\r') {
        line_end++;
    }

    /* Calculate line number width for alignment */
    int line_num_width = snprintf(NULL, 0, "%u", err->location.line);
    if (line_num_width < 2) line_num_width = 2;

    /* Print empty line with bar */
    fprintf(out, " %*s \033[1;34m|\033[0m\n", line_num_width, "");

    /* Print the source line */
    fprintf(out, " \033[1;34m%*u |\033[0m ", line_num_width, err->location.line);
    fwrite(source + line_start, 1, line_end - line_start, out);
    fprintf(out, "\n");

    /* Print the pointer line */
    fprintf(out, " %*s \033[1;34m|\033[0m ", line_num_width, "");
    
    /* Print spaces up to error column */
    size_t col_offset = err->location.offset - line_start;
    for (size_t i = 0; i < col_offset; i++) {
        fprintf(out, " ");
    }
    
    /* Print error indicator (use ^^ or multiple ^ for multi-char errors) */
    fprintf(out, "\033[1;31m^");
    
    /* Try to highlight multiple characters if context suggests it */
    size_t highlight_len = 1;
    if (err->context) {
        /* Heuristic: if context mentions "separator", "group", etc., highlight 1-2 chars */
        if (strstr(err->context, "separator") || strstr(err->context, "group")) {
            highlight_len = 2;
        }
    }
    
    for (size_t i = 1; i < highlight_len && (err->location.offset + i) < line_end; i++) {
        fprintf(out, "^");
    }
    
    fprintf(out, "\033[0m");
    
    /* Print context message on the same line if available */
    if (err->context) {
        fprintf(out, " %s", err->context);
    }
    
    fprintf(out, "\n");

    /* Print empty line with bar */
    fprintf(out, " %*s \033[1;34m|\033[0m\n", line_num_width, "");
}

#ifdef __cplusplus
}
#endif

#endif /* AJIS_ERROR_PRINT_H */
