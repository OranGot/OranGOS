#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
void terminal_writestring(const char *data);
void terminal_write(const char *data, size_t size);
void terminal_setcolor(uint8_t color);
void terminal_initialize(void);
void terminal_putentryat(char c, uint8_t color, uint16_t x, uint16_t y);
void terminal_putchar(char c);
char *read_line(uint16_t line, char *buf);
void flip_console_mode(void);
void clear_screen(void);
void terminal_move(uint16_t y, uint16_t x);
#endif