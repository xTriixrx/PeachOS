#include "lib.h"
#include "string.h"

uint16_t g_term_row = 0;
uint16_t g_term_col = 0;
uint16_t* g_vd_mem = (uint16_t*) (0xB8000);

void panic(const char* msg)
{
    print(msg);
    while (1) {};
}

void print(const char* str)
{
    size_t len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        term_write(str[i], WHITE);
    }
}

void term_write(char c, char color)
{
    if (c == '\n')
    {
        g_term_row += 1;
        g_term_col = 0;
        return;
    }
    else if (c == '\t')
    {
        g_term_col += 4;
        return;
    }

    term_putchar(g_term_col, g_term_row, c, color);

    g_term_col += 1;

    if (g_term_col >= VGA_WIDTH)
    {
        g_term_col = 0;
        g_term_row += 1;
    }
}

void term_putchar(int x, int y, char c, char color)
{
    g_vd_mem[(y * VGA_WIDTH) + x] = term_makechar(c, color);
}

uint16_t term_makechar(char c, char color)
{
    return (color << 8) | c;
}

void terminal_initialize()
{
    uint16_t* vd_mem = (uint16_t*) (0xB8000);
    
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vd_mem[(y * VGA_WIDTH) + x] = term_makechar(SPACE, 0);
        }
    }
}