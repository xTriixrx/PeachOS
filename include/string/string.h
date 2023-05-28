#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

bool isdigit(char);
int tonumericdigit(char);
size_t strlen(const char*);
size_t strnlen(const char*, int);

#endif