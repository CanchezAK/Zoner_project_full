#include "zoner.h"

// #define UART_ID uart1

uint32_t zoner_command[3] = {0xB3217B00, 0xA6000100, 0x00000000};
uint8_t *zoner_literal_command = "set:mixer#switch#7#0#1";
//18 in 20 out 22 enable
uint8_t transmit_buffer[12];
uint8_t rx_mass[12] = {0};
bool txU_flag = 0;

void channel_config(uint8_t out, uint8_t in, uint8_t en)
{
    zoner_literal_command[22] = en;
    zoner_literal_command[20] = out;
    zoner_literal_command[18] = in;
    //zoner_command[0] |= ((0x7B + out + in + en) << 8);
    //zoner_command[2] |= (in << 24) | (out << 16) | (en << 8);
}

void zoner_tx(uint32_t *data)
{
    gpio_put(DIR, 1);
    for (uint8_t i = 0; i < 3; i++)
    {
        transmit_buffer[(i * 4)] = (data[i] >> 24 & 0xFF);
        transmit_buffer[(i * 4) + 1] = (data[i] >> 16 & 0xFF);
        transmit_buffer[(i * 4) + 2] = (data[i] >> 8 & 0xFF);
        transmit_buffer[(i * 4) + 3] = (data[i] & 0xFF);
    }
    uart_write_blocking(UART_ID, transmit_buffer, 12);
    gpio_put(DIR, 0);
    while(!txU_flag);
    txU_flag = 0;
}

void zoner_rx()
{
    uart_read_blocking(uart0, rx_mass, 12);
    txU_flag = 1;
}

void config_interrupt()
{
    irq_set_exclusive_handler(UART0_IRQ, zoner_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}
/*E.G.
channel_config(0x01, 0x02, 0x01);
zoner_tx(zoner_command);
*/