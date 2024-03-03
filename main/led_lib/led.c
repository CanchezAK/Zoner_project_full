#include "led.h"

uint8_t img_mass[8];
uint8_t coordxy[2];

void LED_Init()
{
    spi_init(spi1, 10 * 1000 * 1000);
    gpio_set_function(14, GPIO_FUNC_SPI);
    gpio_set_function(15, GPIO_FUNC_SPI);
    gpio_init(13);
    gpio_set_dir(13, GPIO_OUT);
    gpio_put(13, 1);

    write_register_all(CMD_SHUTDOWN, 0);
    write_register_all(CMD_DISPLAYTEST, 0);
    write_register_all(CMD_SCANLIMIT, 7);  // Use all lines
    write_register_all(CMD_DECODEMODE, 0); // No BCD decode, just use bit pattern.
    write_register_all(CMD_SHUTDOWN, 1);
    write_register_all(CMD_BRIGHTNESS, 8);
    clear_display();
}

void write_register_all(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    cs_select();
    for (int i = 0; i< NUM_MODULES;i++)
    {
        while (!spi_write_blocking(spi1, buf, 2));
    }
    cs_deselect();
}

void write_register(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    cs_select();
    spi_write_blocking(spi1, buf, 2);
    cs_deselect();
    sleep_ms(1);
}

inline void cs_deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(13, 1);
    asm volatile("nop \n nop \n nop");
}

inline void cs_select()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(13, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

bool put_pixel(uint8_t *coordxy)
{
    write_data(coordxy);
    return (1);
}

void clear_display()
{
    for (int i=0; i<9; i++)
    {
        coord(i, 0, false);
        write_data(coordxy);
    }
}

void write_data(uint8_t *coord)
{
    write_register_all(CMD_DIGIT0 + coord[0], coord[1]);
}

void coord(uint8_t x, uint8_t y, bool flash)
{
    coordxy[0] = x > 0 ? x - 1 : 0;
    if (flash == 1)
    {
        if (y > 0)
        {
            img_mass[x - 1] |= (1 << y - 1);
        }
        else if (y <= 0)
        {
            img_mass[x - 1] = 0;
        }
    }
    else if (flash == 0)
    {
        if (y > 0)
        {
            img_mass[x - 1] &= ~(1 << y - 1);
        }
        else if (y <= 0)
        {
            img_mass[x - 1] = 0;
        }
    }
    coordxy[1] = img_mass[x-1];
}