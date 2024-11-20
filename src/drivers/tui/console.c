#include "../../include/strutil.h"
#include "../../include/commands.h"
#include "../../include/cursor.h"
#include "../../include/dbg.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
void terminal_putentryat(char c, uint8_t color, uint16_t x, uint16_t y);
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};
void move_cursor(uint16_t x, uint16_t y);
void read_line(uint16_t line, char *outbuf);
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

static const uint16_t VGA_WIDTH = 80;
static const uint16_t VGA_HEIGHT = 25;

uint16_t terminal_row;
uint16_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t *)0xB8000;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry('\0', terminal_color);
    }
  }
}
void clear_screen(void) {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry('\0', terminal_color);
    }
  }
  move_cursor(0, 0);
  terminal_row = 0;
  terminal_column = 0;
  return;
}
void terminal_move(uint16_t y, uint16_t x) {
  terminal_column = x;
  terminal_row = y;
}
void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(char c, uint8_t color, uint16_t x, uint16_t y) {
  const uint16_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}
#define SPACES_PER_TAB 4
bool is_term_mode = true;
void terminal_putchar(char c) {
  switch (c) {
  case '\n':

    if (is_term_mode) {
      terminal_column = 0;
      terminal_row++;
      char buf[VGA_WIDTH + 1];
      read_line(terminal_row - 1, buf);
      buf[VGA_WIDTH] = '\0';
      run_command(buf);
      terminal_putchar('>');
    } else {
      terminal_column = 0;
      terminal_row++;
    }
    // if(terminal_row >= VGA_HEIGHT-1){
    //     dbg_printf("shift needed\n");
    //     for(uint16_t i = 0; i< VGA_WIDTH * (VGA_HEIGHT-1); i++){
    //             terminal_buffer[i] = terminal_buffer[i+VGA_WIDTH];
    //     }
    //     for(uint8_t i = 0; i< VGA_WIDTH; i++){
    //         terminal_buffer[i+ VGA_WIDTH * (VGA_HEIGHT-1)] = '\0';
    //     }
    //     return;
    //       }
    return;
  case '\t':
    for (uint8_t i = 0; i < SPACES_PER_TAB; i++) {
      terminal_putchar(' ');
    }
    break;
  case '\b':
    if (terminal_column != 0) {
      terminal_column--;
      c = ' ';
      update_cursor(terminal_column, terminal_row);
      terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
      return;
    } else
      break;
  default:
    break;
  }
  update_cursor(terminal_column, terminal_row);
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
      terminal_row = 0;
  }
}

void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char *data) {
  terminal_write(data, strlen(data));
}
void read_line(uint16_t line, char *outbuf) {
  for (uint16_t i = 0; i < VGA_WIDTH; i++) {
    const uint16_t c = terminal_buffer[i + line * VGA_WIDTH];
    outbuf[i] = c & 0xFF;
  }
}
typedef struct {
  uint8_t bg;
  uint8_t fg;
  char c;
} VGA_entry;
VGA_entry get_entry(uint16_t x, uint16_t y) {
  uint16_t entry = terminal_buffer[y * VGA_WIDTH + x];
  VGA_entry retentry;
  retentry.c = entry | 0xFF00;
  retentry.fg = (((entry & 0x00FF) >> 8) & 0xF0) >> 4;
  retentry.bg = ((entry & 0x00FF) >> 8) & 0x0F;
  return retentry;
}
// my own cursor
void move_cursor(uint16_t x, uint16_t y) {
  VGA_entry entry = get_entry(x, y);
  terminal_putentryat(entry.c,
                      vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE), x, y);
}
void flip_console_mode(void) {
  if (is_term_mode)
    is_term_mode = false;
  else
    is_term_mode = true;
}
