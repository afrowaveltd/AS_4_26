#ifndef AJIS_TEST_COMMON_H
#define AJIS_TEST_COMMON_H

#include <stddef.h>

typedef struct TestBuffer {
    char*  data;   // NUL-terminated (pro debug výpisy), ale může obsahovat i NUL uvnitř pokud chceš
    size_t size;   // počet bytů načtených ze souboru
} TestBuffer;

TestBuffer test_read_file(const char* path);
void test_free_buffer(TestBuffer* buf);

void test_fail(const char* msg, const char* file, int line);

#define TEST_ASSERT(cond, msg) \
    do { if (!(cond)) test_fail((msg), __FILE__, __LINE__); } while (0)

#endif
