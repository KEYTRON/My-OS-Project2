// test_memory.c - тест функций памяти
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_memcpy() {
    char src[] = "Memory test data";
    char dst[50];
    
    memcpy(dst, src, strlen(src) + 1);
    if (strcmp(dst, src) == 0) {
        printf("✓ memcpy test passed\n");
    } else {
        printf("✗ memcpy test failed\n");
    }
}

void test_memset() {
    char buffer[20];
    memset(buffer, 'A', 19);
    buffer[19] = '\0';
    
    if (buffer[0] == 'A' && buffer[18] == 'A') {
        printf("✓ memset test passed\n");
    } else {
        printf("✗ memset test failed\n");
    }
}

void test_memcmp() {
    char arr1[] = "Hello";
    char arr2[] = "Hello";
    char arr3[] = "World";
    
    if (memcmp(arr1, arr2, 5) == 0 && memcmp(arr1, arr3, 5) != 0) {
        printf("✓ memcmp test passed\n");
    } else {
        printf("✗ memcmp test failed\n");
    }
}

int main() {
    printf("=== Memory Functions Test ===\n\n");
    
    test_memcpy();
    test_memset();
    test_memcmp();
    
    printf("\n=== Memory tests completed ===\n");
    return 0;
}
