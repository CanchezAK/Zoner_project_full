#include "hardware/spi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#define CMD_NOOP 0
#define CMD_DIGIT0 1
#define CMD_DECODEMODE 9
#define CMD_BRIGHTNESS 10
#define CMD_SCANLIMIT 11
#define CMD_SHUTDOWN 12
#define CMD_DISPLAYTEST 15
#define NUM_MODULES 4

#define coord_SCENEB(mass)  mass[0] = 1, mass[1] = 1

void LED_Init();
void put_pixel(uint8_t *coordxy);
void write_data(uint8_t *coord);
void coord(uint8_t x, uint8_t y, bool flash);
void clear_display();
void write_register_all(uint8_t reg, uint8_t data);
void write_register(uint8_t reg, uint8_t data);
void cs_deselect();
void cs_select();