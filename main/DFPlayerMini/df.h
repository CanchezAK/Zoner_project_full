#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pio.h"

#define Normal  0
#define Pop     1
#define Rock    2
#define Jazz    3
#define Classic 4
#define Bass    5

#define BAUD_RATE 9600
#define KBD_INT0 10
#define KBD_INT1 11
#define UART_TX_PIN 6

void checksum();
void set_equalizer(uint8_t equ);
void play_cycle(uint16_t num_sound);
void play_sound_immediatly(uint16_t num_sound);
void play_sound_after_ending(uint16_t num_sound);
void play_next_track();
void play_previous_track();
void set_volume(uint8_t vol);
void volume_minus();
void volume_plus();
void stop();