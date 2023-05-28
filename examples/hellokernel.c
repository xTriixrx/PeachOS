#include "kernel.h"
#include <stdint.h>
#include <stddef.h>

void terminal_initialize();
size_t strlen(const char* str);
uint16_t terminal_make_char(char c, char color);
void terminal_write(uint16_t* vd_mem, int x, int y, char c, char color);

void kernel_main()
{
    terminal_initialize();
    
    char* hello = "Hello World!";
    char * helloItr = hello;

    uint16_t* vd_mem = (uint16_t*) (0xB8000);

    uint8_t color = 0;

    while (1)
    {
        int y = (VGA_HEIGHT / 2);
        for (int x = (VGA_WIDTH / 2); *helloItr != '\0'; x++)
        {
            terminal_write(vd_mem, x, y, *helloItr++, (char) color);
        }

        helloItr = hello;

        if (++color == 16)
        {
            color = 0;
        }

        // Brute force busy wait to cycle through colors slower
        for (int i = 0; i < 100000000; i++) {}
    }
}

void terminal_write(uint16_t* vd_mem, int x, int y, char c, char color)
{
    vd_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_initialize()
{
    uint16_t* vd_mem = (uint16_t*) (0xB8000);
    
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vd_mem[(y * VGA_WIDTH) + x] = terminal_make_char(' ', 0);
        }
    }
}



uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}