// System includes
#include "hardware/flash.h"
#include "pico/multicore.h"
// DF Player Mini includes
#include "uart_pio.pio.h"
#include "DFPlayerMini/df.h"
// ZONER includes
#include "Zoner/zoner.h"
// KBD includes
#include "scan_kbd/scan_kbd.h"
#include "hardware/clocks.h"
#include "led_lib/led.h"
// ZONER defines

// DF Player Mini variables
extern uint8_t data[10];
extern char flag;
unsigned char once_play = 0;
// ZONER variables
//extern uint32_t zoner_command[3];
extern uint8_t zoner_command[];
uint8_t tx_flag = 0;
// KBD variables
extern struct KBD_Config read;
extern uint8_t coordxy[2];
extern uint8_t img_mass[8];
// System defines
#define MUTE 11
#define FLASH_TARGET_OFFSET (256 * 1024)
// System variables
const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
// System fuctions
void redact_scene(uint8_t sc);

uint8_t scene[4];
bool zone_flags[8];
uint8_t flag_redact = 0;
uint8_t flag_active_scene = 0;
uint8_t flag_active_zone = 0;
absolute_time_t orient;
bool micro_pressed = 0;

void poll_led()
{
    if (read.is_pressed_sound == 1 && micro_pressed == 0)
    {
        coord(1, 4, 1);
        put_pixel(coordxy);
        gpio_put(MUTE, 0);
        micro_pressed = 1;
    }
    else if (read.is_pressed_sound == 0 && micro_pressed == 1)
    {
        gpio_put(MUTE, 1);
        coord(1, 4, 0);
        put_pixel(coordxy);
        micro_pressed = 0;
    }

    if (read.is_pressed_scene & (1 << 0))
    {
        read.is_pressed_zone = scene[0];
        // flag_active_zone = scene[0];
        flag_active_scene |= (1 << 0);
        coord(3, 3, 1);
        put_pixel(coordxy);
    }
    else if ((!(read.is_pressed_scene & (1 << 0))) && flag_active_scene & (1 << 0))
    {
        coord(3, 3, 0);
        put_pixel(coordxy);
        flag_active_scene &= ~(1 << 0);
    }

    if (read.is_pressed_scene & (1 << 1))
    {
        read.is_pressed_zone = scene[1];
        // flag_active_zone = 0xFF;
        flag_active_scene |= (1 << 1);
        coord(1, 1, 1);
        put_pixel(coordxy);
    }
    else if ((!(read.is_pressed_scene & (1 << 1))) && flag_active_scene & (1 << 1))
    {
        flag_active_scene &= ~(1 << 1);
        coord(1, 1, 0);
        put_pixel(coordxy);
    }

    if (read.is_pressed_scene & (1 << 2))
    {
        read.is_pressed_zone = scene[2];
        // flag_active_zone = scene[2];
        flag_active_scene |= (1 << 2);
        coord(1, 5, 1);
        put_pixel(coordxy);
    }
    else if ((!(read.is_pressed_scene & (1 << 2))) && flag_active_scene & (1 << 2))
    {
        flag_active_scene &= ~(1 << 2);
        coord(1, 5, 0);
        put_pixel(coordxy);
    }

    if (read.is_pressed_scene & (1 << 3))
    {
        read.is_pressed_zone = scene[3];
        // flag_active_zone = scene[3];
        flag_active_scene |= (1 << 3);
        coord(1, 3, 1);
        put_pixel(coordxy);
    }
    else if ((!(read.is_pressed_scene & (1 << 3))) && flag_active_scene & (1 << 3))
    {
        flag_active_scene &= ~(1 << 3);
        coord(1, 3, 0);
        put_pixel(coordxy);
    }

    if (read.is_pressed_zone & (1 << 0) && !(flag_active_zone & (1 << 0)))
    {
        coord(2, 1, 1);
        put_pixel(coordxy);
        zone_flags[0] = 1;
        /*if (!flag_redact)
        {
            channel_config(0, 0, zone_flags[0]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 0);
    }
    else if ((!(read.is_pressed_zone & (1 << 0))) && flag_active_zone & (1 << 0))
    {
        coord(2, 1, 0);
        put_pixel(coordxy);
        zone_flags[0] = 0;
        /*if (!flag_redact)
        {
            channel_config(0, 0, zone_flags[0]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 0);
    }

    if (read.is_pressed_zone & (1 << 1) && !(flag_active_zone & (1 << 1)))
    {
        coord(2, 5, 1);
        put_pixel(coordxy);
        zone_flags[1] = 1;
        /*if (!flag_redact)
        {
            channel_config(1, 0, zone_flags[1]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 1);
    }
    else if ((!(read.is_pressed_zone & (1 << 1))) && flag_active_zone & (1 << 1))
    {
        coord(2, 5, 0);
        put_pixel(coordxy);
        zone_flags[1] = 0;
        /*if (!flag_redact)
        {
            channel_config(1, 0, zone_flags[1]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 1);
    }

    if (read.is_pressed_zone & (1 << 2) && !(flag_active_zone & (1 << 2)))
    {
        coord(2, 3, 1);
        put_pixel(coordxy);
        zone_flags[2] = 1;
        /*if (!flag_redact)
        {
            channel_config(2, 0, zone_flags[2]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 2);
    }
    else if ((!(read.is_pressed_zone & (1 << 2))) && flag_active_zone & (1 << 2))
    {
        coord(2, 3, 0);
        put_pixel(coordxy);
        zone_flags[2] = 0;
        /*if (!flag_redact)
        {
            channel_config(2, 0, zone_flags[2]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 2);
    }

    if (read.is_pressed_zone & (1 << 3) && !(flag_active_zone & (1 << 3)))
    {
        coord(4, 1, 1);
        put_pixel(coordxy);
        zone_flags[3] = 1;
        /*if (!flag_redact)
        {
            channel_config(3, 0, zone_flags[3]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 3);
    }
    else if ((!(read.is_pressed_zone & (1 << 3))) && flag_active_zone & (1 << 3))
    {
        coord(4, 1, 0);
        put_pixel(coordxy);
        zone_flags[3] = 0;
        /*if (!flag_redact)
        {
            channel_config(3, 0, zone_flags[3]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 3);
    }

    if (read.is_pressed_zone & (1 << 4) && !(flag_active_zone & (1 << 4)))
    {
        coord(4, 5, 1);
        put_pixel(coordxy);
        zone_flags[4] = 1;
        /*if (!flag_redact)
        {
            channel_config(4, 0, zone_flags[4]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 4);
    }
    else if ((!(read.is_pressed_zone & (1 << 4))) && flag_active_zone & (1 << 4))
    {
        coord(4, 5, 0);
        put_pixel(coordxy);
        zone_flags[4] = 0;
        /*if (!flag_redact)
        {
            channel_config(4, 0, zone_flags[4]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 4);
    }

    if (read.is_pressed_zone & (1 << 5) && !(flag_active_zone & (1 << 5)))
    {
        coord(4, 3, 1);
        put_pixel(coordxy);
        zone_flags[5] = 1;
        /*if (!flag_redact)
        {
            channel_config(5, 0, zone_flags[5]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 5);
    }
    else if ((!(read.is_pressed_zone & (1 << 5))) && flag_active_zone & (1 << 5))
    {
        coord(4, 3, 0);
        put_pixel(coordxy);
        zone_flags[5] = 0;
        /*if (!flag_redact)
        {
            channel_config(5, 0, zone_flags[5]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 5);
    }

    if (read.is_pressed_zone & (1 << 6) && !(flag_active_zone & (1 << 6)))
    {
        coord(3, 1, 1);
        put_pixel(coordxy);
        zone_flags[6] = 1;
        /*if (!flag_redact)
        {
            channel_config(6, 0, zone_flags[6]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 6);
    }
    else if ((!(read.is_pressed_zone & (1 << 6))) && flag_active_zone & (1 << 6))
    {
        coord(3, 1, 0);
        put_pixel(coordxy);
        zone_flags[6] = 0;
        /*if (!flag_redact)
        {
            channel_config(6, 0, zone_flags[6]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 6);
    }

    if (read.is_pressed_zone & (1 << 7) && !(flag_active_zone & (1 << 7)))
    {
        coord(3, 5, 1);
        put_pixel(coordxy);
        zone_flags[7] = 1;
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone |= (1 << 7);
    }
    else if ((!(read.is_pressed_zone & (1 << 7))) && flag_active_zone & (1 << 7))
    {
        coord(3, 5, 0);
        put_pixel(coordxy);
        zone_flags[7] = 0;
        /*if (!flag_redact)
        {
            channel_config(7, 0, zone_flags[7]);
            zoner_tx(zoner_command);
            sleep_ms(300);
        }*/
        flag_active_zone &= ~(1 << 7);
    }

    if (read.is_pressed_scene_write & (1 << 1))
    {
        coord(1, 1, 1);
        put_pixel(coordxy);
        sleep_ms(150);
        coord(1, 1, 0);
        put_pixel(coordxy);
        sleep_ms(150);
        flag_redact = 1;
    }
    else if ((!(read.is_pressed_scene_write & (1 << 1))) && flag_redact == 1)
    {
        scene[1] = read.is_pressed_zone;
        coord(1, 1, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 1, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 1, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 1, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 1, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 1, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        flag_redact = 0;
    }

    else if (read.is_pressed_scene_write & (1 << 0))
    {
        coord(3, 3, 1);
        put_pixel(coordxy);
        sleep_ms(150);
        coord(3, 3, 0);
        put_pixel(coordxy);
        sleep_ms(150);
        flag_redact = 2;
    }
    else if ((!(read.is_pressed_scene_write & (1 << 0))) && flag_redact == 2)
    {
        scene[0] = read.is_pressed_zone;
        coord(3, 3, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(3, 3, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(3, 3, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(3, 3, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(3, 3, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(3, 3, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        flag_redact = 0;
    }

    else if (read.is_pressed_scene_write & (1 << 2))
    {
        coord(1, 5, 1);
        put_pixel(coordxy);
        sleep_ms(150);
        coord(1, 5, 0);
        put_pixel(coordxy);
        sleep_ms(150);
        flag_redact = 3;
    }
    else if ((!(read.is_pressed_scene_write & (1 << 2))) && flag_redact == 3)
    {
        scene[2] = read.is_pressed_zone;
        coord(1, 5, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 5, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 5, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 5, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 5, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 5, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        flag_redact = 0;
    }
    else if (read.is_pressed_scene_write & (1 << 3))
    {
        coord(1, 3, 1);
        put_pixel(coordxy);
        sleep_ms(150);
        coord(1, 3, 0);
        put_pixel(coordxy);
        sleep_ms(150);
        flag_redact = 4;
    }
    else if ((!(read.is_pressed_scene_write & (1 << 3))) && flag_redact == 4)
    {
        scene[3] = read.is_pressed_zone;
        coord(1, 3, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 3, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 3, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 3, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 3, 1);
        put_pixel(coordxy);
        sleep_ms(250);
        coord(1, 3, 0);
        put_pixel(coordxy);
        sleep_ms(250);
        flag_redact = 0;
    }
}

void transmit_func()
{
    while (1)
    {
        if (!read.is_pressed_all || !read.is_pressed_clear)
        {
            if (read.is_pressed_zone & (1 << 0) && !(flag_active_zone & (1 << 0)))
            {
                if (!flag_redact)
                {
                    channel_config(0, 0, zone_flags[0]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 0))) && flag_active_zone & (1 << 0))
            {
                if (!flag_redact)
                {
                    channel_config(0, 0, zone_flags[0]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 1) && !(flag_active_zone & (1 << 1)))
            {
                if (!flag_redact)
                {
                    channel_config(1, 0, zone_flags[1]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 1))) && flag_active_zone & (1 << 1))
            {
                if (!flag_redact)
                {
                    channel_config(1, 0, zone_flags[1]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 2) && !(flag_active_zone & (1 << 2)))
            {
                if (!flag_redact)
                {
                    channel_config(2, 0, zone_flags[2]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 2))) && flag_active_zone & (1 << 2))
            {
                if (!flag_redact)
                {
                    channel_config(2, 0, zone_flags[2]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 3) && !(flag_active_zone & (1 << 3)))
            {
                if (!flag_redact)
                {
                    channel_config(3, 0, zone_flags[3]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 3))) && flag_active_zone & (1 << 3))
            {
                if (!flag_redact)
                {
                    channel_config(3, 0, zone_flags[3]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 4) && !(flag_active_zone & (1 << 4)))
            {
                if (!flag_redact)
                {
                    channel_config(4, 0, zone_flags[4]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 4))) && flag_active_zone & (1 << 4))
            {
                if (!flag_redact)
                {
                    channel_config(4, 0, zone_flags[4]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 5) && !(flag_active_zone & (1 << 5)))
            {
                if (!flag_redact)
                {
                    channel_config(5, 0, zone_flags[5]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 5))) && flag_active_zone & (1 << 5))
            {
                if (!flag_redact)
                {
                    channel_config(5, 0, zone_flags[5]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 6) && !(flag_active_zone & (1 << 6)))
            {
                if (!flag_redact)
                {
                    channel_config(6, 0, zone_flags[6]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 6))) && flag_active_zone & (1 << 6))
            {
                if (!flag_redact)
                {
                    channel_config(6, 0, zone_flags[6]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }

            if (read.is_pressed_zone & (1 << 7) && !(flag_active_zone & (1 << 7)))
            {
                if (!flag_redact)
                {
                    channel_config(7, 0, zone_flags[7]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
            else if ((!(read.is_pressed_zone & (1 << 7))) && flag_active_zone & (1 << 7))
            {
                if (!flag_redact)
                {
                    channel_config(7, 0, zone_flags[7]);
                    zoner_tx(zoner_command);
                    sleep_ms(300);
                }
            }
        }
        else if (read.is_pressed_all)
        {
            if (!flag_redact)
            {
                zoner_tx_group_all();
                read.is_pressed_all = 0;
            }
        }
        else if (read.is_pressed_clear)
        {
            if (!flag_redact)
            {
                zoner_tx_group_clear();
                read.is_pressed_clear = 0;
            }
        }
        if (read.is_pressed_func & (1 << 7))
        {
            read.is_pressed_func &= ~(1 << 7);
            play_sound_immediatly(2);
        }
        else if (read.is_pressed_func & (1 << 6))
        {
            read.is_pressed_func &= ~(1 << 6);
            play_sound_immediatly(3);
        }
    }
}

int main()
{
    stdio_init_all();
    orient._private_us_since_boot = 0;
    gpio_init(MUTE);
    gpio_set_dir(MUTE, GPIO_OUT);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    // ZONER Init
    uart_init(UART_ID, BAUD_RATE_h);
    gpio_set_function(UART_TX_PIN_h, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_h, GPIO_FUNC_UART);
    gpio_init(DIR);
    gpio_set_dir(DIR, GPIO_OUT);
    // DF Player Mini Init
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &uart_pio_program);
    uart_pio_program_init(pio, sm, offset, UART_TX_PIN, BAUD_RATE);
    sleep_ms(1000);
    set_volume(1);
    play_sound_immediatly(1);
    // KBD Init
    LED_Init();
    struct KBD_Config config;
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
    config.CFG |= GPI_IEN | KE_IEN;
    config.KP_GPIO1 |= ROW0 | ROW1 | ROW2 | ROW3 | ROW4 | ROW5;
    config.KP_GPIO2 |= COL0 | COL1 | COL2 | COL3 | COL4 | COL5;
    config.KP_GPIO3 = 0;
    uint8_t answer = KBD_Init(config);
    /*if (check_flash() == false)
    {
        load_default_param();
    }*/
    read.is_pressed_scene = 0;
    read.is_pressed_scene_write = 0;
    read.is_pressed_zone = 0;
    read.is_pressed_sound = 0;
    scene[0] = 0x12;
    scene[1] = 0x40;
    scene[2] = 0x03;
    scene[3] = 0xFF;
    //DEBUG!!!!!!!
    multicore_launch_core1(transmit_func);
    while (1)
    {
        poll_led();
        /*channel_config(0, 0, 1);
        zoner_tx(zoner_command);
        sleep_ms(300);
        channel_config(0, 0, 0);
        zoner_tx(zoner_command);
        sleep_ms(300);*/
    }
    return 0;
}
