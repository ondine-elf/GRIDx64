#ifndef TETOS_STDLIB_H
#define TETOS_STDLIB_H

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
char *itoa(int value, char *str, int base);
size_t strlen(const char* str);
int memcmp(const void *s1, const void *s2, size_t n);

void *_memset(void *s, int c, size_t n);
void *_memcpy(void *dest, const void *src, size_t n);
void *_memmove(void *dest, const void *src, size_t n);
int _memcmp(const void *s1, const void *s2, size_t n);

size_t _strlen(const char *s);

char *_strcpy(char *dest, const char *src);
char *_strncpy(char *dest, const char *src, size_t n);

char *_strcat(char *dest, const char *src);
char *_strncat(char *dest, const char *src, size_t n);

int _strcmp(const char *s1, const char *s2);
int _strncmp(const char *s1, const char *s2, size_t n);

char *_strchr(const char *s, int c);
char *_strrchr(const char *s, int c);
char *_strstr(const char *haystack, const char *needle);




#endif