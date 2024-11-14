#ifndef CURSOR_H
#define CURSOR_H
#include <stdint.h>
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
uint16_t get_cursor_position(void);
void update_cursor(int x, int y);
#endif