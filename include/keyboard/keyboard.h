#ifndef KEYBOARD_H
#define KEYBOARD_H

struct process;

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct keyboard
{
    KEYBOARD_INIT_FUNCTION init;
    char name[20];
    struct keyboard* next;
};

char keyboard_pop();
void keyboard_init();
void keyboard_push(char);
void keyboard_backspace(struct process*);

#endif