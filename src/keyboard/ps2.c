#include "io/io.h"
#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "idt/idt.h"
#include "task/task.h"
#include "keyboard/ps2.h"
#include "keyboard/keyboard.h"

int ps2_keyboard_init();
void ps2_keyboard_handle_interrupt();

static uint8_t keyboard_scan_set_one[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0D, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`',
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

struct keyboard ps2_keyboard = {
    .name = {"PS2"},
    .init = ps2_keyboard_init
};

int ps2_keyboard_init()
{
    idt_register_interrupt_callback(ISR_KEYBOARD_INTERRUPT, ps2_keyboard_handle_interrupt);
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}

uint8_t ps2_keyboard_scancode_to_char(uint8_t scancode)
{
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);

    if (scancode > size_of_keyboard_set_one)
    {
        return 0;
    }

    char c = keyboard_scan_set_one[scancode];

    return c;
}

void ps2_keyboard_handle_interrupt()
{
    kernel_page();
    uint8_t scancode = 0;

    scancode = insb(KEYBOARD_INPUT_PORT);
    insb(KEYBOARD_INPUT_PORT); // Ignore rogue bytes

    if (scancode & PS2_KEYBOARD_KEY_RELEASE)
    {
        return;
    }

    uint8_t c = ps2_keyboard_scancode_to_char(scancode);
    
    if (c != 0)
    {
        keyboard_push(c);
    }

    task_page();
}

struct keyboard* ps2_init()
{
    return &ps2_keyboard;
}