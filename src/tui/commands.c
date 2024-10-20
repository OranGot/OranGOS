#include "../util/printf.h"
#include "../util/strutil.h"
#include "console.h"
#include <stdint.h>
void run_command(char *cmd) {
  flip_console_mode();
  if (strcmp(cmd, ">help", 5) == 0) {
    printf("no help here sorry\n");
  } else if (strcmp(cmd, ">clear", 5) == 0) {
    clear_screen();
    // terminal_putentryat('>', 15 | (0 << 4), 0, 0);
    terminal_move(0, 0);

  } else {

    printf("INVALID COMMAND: %s\n", cmd);
  }
  flip_console_mode();
}