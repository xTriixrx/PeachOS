#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stddef.h>

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define PURPLE 5
#define BROWN 6
#define GRAY 7
#define DARK_GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_PURPLE 13
#define YELLOW 14
#define WHITE 15
#define SPACE ' '
#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

extern uint16_t g_term_row;
extern uint16_t g_term_col;
extern uint16_t* g_vd_mem;

void panic(const char*);
void print(const char*);
void terminal_initialize();
void term_write(char, char);
uint16_t term_makechar(char, char);
void term_putchar(int, int, char, char);

#endif