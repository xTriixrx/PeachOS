#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

char tolower(char);
bool isdigit(char);
int tonumericdigit(char);
size_t strlen(const char*);
size_t strnlen(const char*, int);
char* strcpy(char*, const char*);
char* strncpy(char*, const char*, int);
int strncmp(const char*, const char*, int);
int istrncmp(const char*, const char*, int);
int strnlen_terminator(const char*, int, char);

#endif