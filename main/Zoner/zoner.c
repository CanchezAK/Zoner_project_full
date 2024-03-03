#include "zoner.h"

// #define UART_ID uart1

//uint32_t zoner_command[3] = {0xB3217B00, 0xA6000100, 0x00000000};
uint8_t zoner_command[] = "set:mixer#switch#7#0#1";         //17 in 19 out 21 en
uint8_t transmit_buffer[12];
//uint8_t rx_mass[12] = {0};
//bool txU_flag = 0;

void channel_config(uint8_t out, uint8_t in, uint8_t en)
{
    /*zoner_command[0] |= ((0x7B + out + in + en) << 8);
    zoner_command[2] |= (in << 24) | (out << 16) | (en << 8);*/
    zoner_command[17] = in + '0';
    zoner_command[19] = out + '0';
    zoner_command[21] = en + '0';
}

//void zoner_tx(uint32_t *data)
void zoner_tx(uint8_t *data)
{
    gpio_put(DIR, 1);
    /*for (uint8_t i = 0; i < 3; i++)
    {
        transmit_buffer[(i * 4)] = (data[i] >> 24 & 0xFF);
        transmit_buffer[(i * 4) + 1] = (data[i] >> 16 & 0xFF);
        transmit_buffer[(i * 4) + 2] = (data[i] >> 8 & 0xFF);
        transmit_buffer[(i * 4) + 3] = (data[i] & 0xFF);
    }
    uart_write_blocking(UART_ID, transmit_buffer, 12);*/
    uart_write_blocking(UART_ID, data, 22);
}

void zoner_tx_group_all()
{
    gpio_put(DIR, 1);
    char *tx_message = "set:mixer#switch#0#0-7#1";
    uart_write_blocking(UART_ID, tx_message, 24);
    sleep_ms(300);
}

void zoner_tx_group_clear()
{
    gpio_put(DIR, 1);
    char *tx_message = "set:mixer#switch#0#0-7#0";
    uart_write_blocking(UART_ID, tx_message, 24);
    sleep_ms(300);
}

/*E.G.
channel_config(0x01, 0x02, 0x01);
zoner_tx(zoner_command);
*/