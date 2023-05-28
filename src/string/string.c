#include "string.h"

size_t strlen(const char* str)
{
    size_t len = 0;
    
    while (str[len])
    {
        len++;
    }

    return len;
}

size_t strnlen(const char* str, int max)
{
    size_t len = 0;
    
    while (str[len] && len < max)
    {
        len++;
    }

    return len;
}

bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

int tonumericdigit(char c)
{
    return c - 48;
}