/*
just for future implimentation, so I won't have to rewrite a lot of stuff

*/
#include <stdint.h>
uint16_t CURRENT_PID = 0;
#define PROC_EXIT_SUCCESS 0
typedef struct {
  int priviledge_level;
  uint32_t PID;
  uint32_t *page_directory;
  uint32_t *child_procid;
} Process;
#define SYSTEM_MAX_PROCESSES 1024
Process proc_table[SYSTEM_MAX_PROCESSES];