#include "hardware/uart.h"
#include "pico/stdlib.h"

#define UART_ID uart0
#define BAUD_RATE_h 115200
#define UART_TX_PIN_h 0
#define UART_RX_PIN_h 1
#define DIR 12

void channel_config(uint8_t out, uint8_t in, uint8_t en);
void zoner_tx(uint32_t* data);
void zoner_rx();
void config_interrupt();