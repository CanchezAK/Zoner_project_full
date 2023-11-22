#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#define DEV_ADDR_KBD0 0x20
#define DEV_ADDR_KBD1 0x21
#define DEV_ADDR_KBD2 0x22

void keyboard_init();
void keyboard_led(unsigned char LED, bool state);
unsigned char keyboard();
void keyboard_i2c_init();