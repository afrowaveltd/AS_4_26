#include "test_common.h"
#include <stdio.h>
#include <stdlib.h>

TestBuffer test_read_file(const char* path) {
    TestBuffer buf = {0};

    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[TEST] Cannot open file: %s\n", path);
        return buf;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "[TEST] fseek(SEEK_END) failed: %s\n", path);
        fclose(f);
        return buf;
    }

    long len = ftell(f);
    if (len < 0) {
        fprintf(stderr, "[TEST] ftell failed: %s\n", path);
        fclose(f);
        return buf;
    }

    if (fseek(f, 0, SEEK_SET) != 0) {
        fprintf(stderr, "[TEST] fseek(SEEK_SET) failed: %s\n", path);
        fclose(f);
        return buf;
    }

    // +1 for NUL terminator (helps debug prints)
    buf.data = (char*)malloc((size_t)len + 1);
    if (!buf.data) {
        fprintf(stderr, "[TEST] Out of memory reading: %s\n", path);
        fclose(f);
        return buf;
    }

    size_t read = fread(buf.data, 1, (size_t)len, f);
    fclose(f);

    if (read != (size_t)len) {
        fprintf(stderr, "[TEST] Short read: %s (expected %ld, got %zu)\n", path, len, read);
        free(buf.data);
        buf.data = NULL;
        buf.size = 0;
        return buf;
    }

    buf.data[len] = '\0';
    buf.size = (size_t)len;
    return buf;
}

void test_free_buffer(TestBuffer* buf) {
    if (!buf) return;
    free(buf->data);
    buf->data = NULL;
    buf->size = 0;
}

void test_fail(const char* msg, const char* file, int line) {
    fprintf(stderr, "[TEST FAIL] %s\n  at %s:%d\n", msg, file, line);
    exit(1);
}
