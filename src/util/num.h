#include <stdbool.h>
#include <stdint.h>
inline uint8_t get_bit_from_num(uint32_t num, uint8_t bit) {
  return (uint8_t)(num & (1 << bit));
}
inline uint32_t set_bit_of_num(uint32_t num, uint8_t bit, bool state) {
  return num & ((uint32_t)state << bit);
}