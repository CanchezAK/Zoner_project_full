#include "kbd.h"

unsigned char val = 0x3D;

void keyboard_i2c_init()
{
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(26, GPIO_FUNC_I2C);
    gpio_set_function(27, GPIO_FUNC_I2C);
    gpio_pull_up(26);
    gpio_pull_up(27);   
    bi_decl(bi_2pins_with_func(26, 27, GPIO_FUNC_I2C));
}

void keyboard_init()
{
    /*Writing config register*/
    unsigned char data[2] = {0x03, 0xF0};
    i2c_write_blocking(i2c1, DEV_ADDR_KBD0, data, 2, false);

    data[0] = 0x03;
    data[1] = 0x00;
    i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);

    data[0] = 0x03;
    data[1] = 0xFF;
    i2c_write_blocking(i2c1, DEV_ADDR_KBD2, data, 2, false);
    /*End of writing config register*/

    /*Enabling LED's*/
    data[0] = 0x01;
    data[1] = 0x04;
    i2c_write_blocking(i2c1, DEV_ADDR_KBD0, data, 2, false);

    data[0] = 0x01;
    data[1] = 0x3D;
    i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);

    data[0] = 0x01;
    data[1] = 0x00;
    i2c_write_blocking(i2c1, DEV_ADDR_KBD2, data, 2, false);
    i2c_write_blocking(i2c1, DEV_ADDR_KBD0, 0x00, 1, false);
    i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
    i2c_write_blocking(i2c1, DEV_ADDR_KBD2, 0x00, 1, false);
    /*End of enabling LED's*/
}

void keyboard_led(unsigned char LED, bool state)
{
    unsigned char data[2];
    switch (LED)
    {

    case 6:
        data[0] = 0x01;
        if (state == true)
        {
            data[1] = val | 1;
        }
        else if (state == false)
        {
             data[1] = val & 0x3C;
        }
        val = data[1];
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
        break;
    case 5:
         data[0] = 0x01;
         if (state == true)
         {
            data[1] = 0x04;
         }
         else if (state == false)
         {
            data[1] = 0x00;
         }
         i2c_write_blocking(i2c1, DEV_ADDR_KBD0, data, 2, false);
         i2c_write_blocking(i2c1, DEV_ADDR_KBD0, 0x00, 1, false);
        break;
    case 4:
        data[0] = 0x01;
        if (state == true)
        {
            data[1] = val | 4;
        }
        else if (state == false)
        {
            data[1] = val & 0x1D;
        }
        val = data[1];
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
        break;
    case 3:
        data[0] = 0x01;
        if (state == true)
        {
            data[1] = val | 0x10;
        }
        else if (state == false)
        {
            data[1] = val & 0x2D;
        }
        val = data[1];
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
        break;
    case 2:
        data[0] = 0x01;
        if (state == true)
        {
            data[1] = val | 0x08;
        }
        else if (state == false)
        {
            data[1] = val & 0x35;
        }
        val = data[1];
        printf("%02x", val);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
        break;
    case 1:
        data[0] = 0x01;
        if (state == true)
        {
            data[1] = val | 0x04;
        }
        else if (state == false)
        {
            data[1] = val & 0x39;
        }
        val = data[1];
        printf("val is: %02x\r\n", val);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
        break;
    case 0:
        data[0] = 0x01;
        data[1] = 0x3D;
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, data, 2, false);
        i2c_write_blocking(i2c1, DEV_ADDR_KBD1, 0x00, 1, false);
        break;
    
    default:
        break;
    }
}

unsigned char keyboard()
{
    unsigned char key;
    unsigned char key0;
    unsigned char key1;
    i2c_read_blocking(i2c1, DEV_ADDR_KBD0, &key0, 1, false);
    key0 = key0 << 3;
    key0 = key0 >> 7;
    i2c_read_blocking(i2c1, DEV_ADDR_KBD2, &key1, 1, false);
    key1 = key1 << 2;
    key1 = key1 >> 2;
    key = (key0-1) | key1;
    return key;
}