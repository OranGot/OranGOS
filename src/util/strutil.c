#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
uint64_t strlen(const char *str) {

  uint64_t len = 0;
  while (str[len])
    len++;
  return len;
}
uint8_t strcmp(char *s1, char *s2, uint64_t c) {
  if (c != 0) {
    for (uint64_t i = 0; i < c; i++) {
      if (s1[i] != s2[i]) {
        return 1;
      }
    }
  }
  return 0;
}
void *memcpy(void *dest, const void *src, size_t n) {
  uint8_t *pdest = (uint8_t *)dest;
  const uint8_t *psrc = (const uint8_t *)src;

  for (size_t i = 0; i < n; i++) {
    pdest[i] = psrc[i];
  }

  return dest;
}
void *memset(void *s, int c, size_t n) {
  uint8_t *p = (uint8_t *)s;

  for (size_t i = 0; i < n; i++) {
    p[i] = (uint8_t)c;
  }

  return s;
}
int strshft(char* buf, int sc, uint32_t len){
    for(uint16_t bfctr = len; bfctr> 0; bfctr++){
        if(bfctr+sc > len){
            if(sc > 0){
                continue;
            }
            else if(sc < 0){
                buf[bfctr] = '\0';
            }
            else{
                return 1;
            }
        }
        else if(bfctr + sc < 0){
            if(sc > 0){
                buf[bfctr] = '\0';
            }
            else if(sc < 0){
                continue;
            }
            else{
                return 1;
            }
        }
        else{
            buf[bfctr] = buf[bfctr + sc];
        }

    }
    return 0;
}
