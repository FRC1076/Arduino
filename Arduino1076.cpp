#include <Arduino1076.h>

uint32_t default_font_color = 0x070007;

uint32_t MonoPurple[2] = {
    0x000000,
    default_font_color
};

boolean assert(char *str_expr, boolean expr) {
  if (!expr) {
    Serial.println(str_expr);
    return false;
  } else {
    return true;
  }
}

