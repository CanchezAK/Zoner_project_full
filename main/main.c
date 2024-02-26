// System includes
#include "hardware/flash.h"
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
extern uint32_t zoner_command[3];
uint8_t tx_flag = 0;
// KBD variables
extern struct KBD_Config read;
extern uint8_t coordxy[2];
extern uint8_t img_mass[8];
// System defines
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

/*void flag_callback(uint gpio, uint32_t events)
{
    uint8_t key = keyboard();
    switch (key)
    {
    case 0x3F:
        break;
    case 0x3E:
    micros = get_absolute_time();
    orient = get_absolute_time();
    while (gpio_get(11) == 1)                   //Debouncer
    {

        if (get_absolute_time() - micros >= 2000000)
        {
            if (get_absolute_time() - orient >= 1000000)
            {
                keyboard_led(6, false);
                orient = get_absolute_time();
            }
            else if (get_absolute_time() - orient >= 500000 && get_absolute_time() - orient < 1000000)
            {
                keyboard_led(6, true);
            }
        }
    }
    if (get_absolute_time() - micros > 2000000)             //Wait 2 seconds long press
    {
        if (flag_redact == 1)
        {
            redact_scene(0);
            flag_redact = 2;
            printf ("\r\n\tREDACT DISABLED\t\r\n");
        }
        else if (flag_redact == 0)
        {
            printf ("\r\n\tREDACT ENABLED\t\r\n");
            flag_redact = 1;
        }
        micros = 0;
    }
    else if (get_absolute_time() - micros < 2000000 && get_absolute_time() - micros > 30000)            //Else activate scene mode
    {
        for (uint8_t sc = 0; sc < 8; sc++)
        {
            channel_config(scene[0][sc], 0, scene[0][sc] > 0 ? true : false);
            tx_flag = 1;
        }
        //Need to adding LED's control
        printf("\t\r\nCommand transfering complete\t\r\n");
        //zone_flags[0] = zone_flags[0] > 0 ? 0 : 1;
        //printf(" Zone Flag: %01d", zone_flags[0]);
    }
        break;
    default:
        break;
    }
    printf(" KEY is: %02x\r\n", key);
}*/
/*
void redact_scene(uint8_t sc)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        scene[sc][i] = zone_flags[i];
    }
    flash_range_program(FLASH_TARGET_OFFSET + sc, scene[sc], FLASH_PAGE_SIZE); // Adding data to FLASH memory. In theory, that's must adding data from one zone to defined for it memory address. In fact - didn't tested.
}

void activate_scene(uint8_t scene_val)
{
    switch (scene_val)
    {
    case 0:
        for (uint8_t i = 0; i < 8; i++)
        {
            channel_config(zone_flags[i], 0, 0);
            zoner_tx(zoner_command);
        }
        for (uint8_t k = 0; k < 18; k++)
        {
            // keyboard_led(k, false);
        }
        break;
    case 1:
        for (uint8_t i = 0; i < 8; i++)
        {
            channel_config(scene[0][i], 0, scene[0][i] > 0 ? true : false);
            zoner_tx(zoner_command);
        }
        break;
    default:
        break;
    }
}

void load_default_param()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            zone_flags[j] = 0x00;
            scene[i][j] = zone_flags[j];
        }
    }
    activate_scene(0);
}

bool check_flash()
{
    if (flash_target_contents[0] == 0xFF)
    {
        return false;
    }
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            scene[i][j] = flash_target_contents[j + (8 * i)];
        }
    }
    return true;
}*/

void poll_led()
{
    if (read.is_pressed_scene & (1 << 0))
    {
        read.is_pressed_zone = scene[0];
        flag_active_scene = 1;
        coord(3, 3, 1);
        put_pixel(coordxy);
    }
    else if (!(read.is_pressed_scene & (1 << 0)) && flag_active_scene == 1)
    {
        coord(3, 3, 0);
        put_pixel(coordxy);
        flag_active_scene = 0;
    }

    else if (read.is_pressed_scene & (1 << 1))
    {
        read.is_pressed_zone = scene[1];
        flag_active_scene = 2;
        coord(1, 1, 1);
        put_pixel(coordxy);
    }
    else if (!(read.is_pressed_scene && (1 << 1)) && flag_active_scene == 2)
    {
        flag_active_scene = 0;
        coord(1, 1, 0);
        put_pixel(coordxy);
    }

    else if (read.is_pressed_scene & (1 << 2))
    {
        read.is_pressed_zone = scene[2];
        flag_active_scene = 3;
        coord(1, 5, 1);
        put_pixel(coordxy);
    }
    else if (!(read.is_pressed_scene & (1 << 2)) && flag_active_scene == 3)
    {
        flag_active_scene = 0;
        coord(1, 5, 0);
        put_pixel(coordxy);
    }

    else if (read.is_pressed_scene & (1 << 3))
    {
        read.is_pressed_zone = scene[3];
        flag_active_scene = 4;
        coord(1, 3, 1);
        put_pixel(coordxy);
    }
    else if (!(read.is_pressed_scene & (1 << 3)) && flag_active_scene == 4)
    {
        flag_active_scene = 0;
        coord(1, 3, 0);
        put_pixel(coordxy);
    }

    if (read.is_pressed_zone & (1 << 0))
    {
        coord(2, 1, 1);
        put_pixel(coordxy);
        zone_flags[0] = 1;
        if (!flag_redact)
        {
            channel_config(0, 0, zone_flags[0]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 1;
    }
    else if (!(read.is_pressed_zone & (1 << 0)) && flag_active_zone == 1)
    {
        coord(2, 1, 0);
        put_pixel(coordxy);
        zone_flags[0] = 0;
        if (!flag_redact)
        {
            channel_config(0, 0, zone_flags[0]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 1))
    {
        coord(2, 5, 1);
        put_pixel(coordxy);
        zone_flags[1] = 1;
        if (!flag_redact)
        {
            channel_config(1, 0, zone_flags[1]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 2;
    }
    else if (!(read.is_pressed_zone & (1 << 1)) && flag_active_zone == 2)
    {
        coord(2, 5, 0);
        put_pixel(coordxy);
        zone_flags[1] = 0;
        if (!flag_redact)
        {
            channel_config(1, 0, zone_flags[1]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 2))
    {
        coord(2, 3, 1);
        put_pixel(coordxy);
        zone_flags[2] = 1;
        if (!flag_redact)
        {
            channel_config(2, 0, zone_flags[2]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 3;
    }
    else if (!(read.is_pressed_zone & (1 << 2)) && flag_active_zone == 3)
    {
        coord(2, 3, 0);
        put_pixel(coordxy);
        zone_flags[2] = 0;
        if (!flag_redact)
        {
            channel_config(2, 0, zone_flags[2]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 3))
    {
        coord(4, 1, 1);
        put_pixel(coordxy);
        zone_flags[3] = 1;
        if (!flag_redact)
        {
            channel_config(3, 0, zone_flags[3]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 4;
    }
    else if (!(read.is_pressed_zone & (1 << 3)) && flag_active_zone == 4)
    {
        coord(4, 1, 0);
        put_pixel(coordxy);
        zone_flags[3] = 0;
        if (!flag_redact)
        {
            channel_config(3, 0, zone_flags[3]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 4))
    {
        coord(4, 5, 1);
        put_pixel(coordxy);
        zone_flags[4] = 1;
        if (!flag_redact)
        {
            channel_config(4, 0, zone_flags[4]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 5;
    }
    else if (!(read.is_pressed_zone & (1 << 4)) && flag_active_zone == 5)
    {
        coord(4, 5, 0);
        put_pixel(coordxy);
        zone_flags[4] = 0;
        if (!flag_redact)
        {
            channel_config(4, 0, zone_flags[4]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 5))
    {
        coord(4, 3, 1);
        put_pixel(coordxy);
        zone_flags[5] = 1;
        if (!flag_redact)
        {
            channel_config(5, 0, zone_flags[5]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 6;
    }
    else if (!(read.is_pressed_zone & (1 << 5)) && flag_active_zone == 6)
    {
        coord(4, 3, 0);
        put_pixel(coordxy);
        zone_flags[5] = 0;
        if (!flag_redact)
        {
            channel_config(5, 0, zone_flags[5]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 6))
    {
        coord(3, 1, 1);
        put_pixel(coordxy);
        zone_flags[6] = 1;
        if (!flag_redact)
        {
            channel_config(6, 0, zone_flags[6]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 7;
    }
    else if (!(read.is_pressed_zone & (1 << 6)) && flag_active_zone == 7)
    {
        coord(3, 1, 0);
        put_pixel(coordxy);
        zone_flags[6] = 0;
        if (!flag_redact)
        {
            channel_config(6, 0, zone_flags[6]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
    }

    if (read.is_pressed_zone & (1 << 7))
    {
        coord(3, 5, 1);
        put_pixel(coordxy);
        zone_flags[7] = 1;
        if (!flag_redact)
        {
            channel_config(7, 0, zone_flags[7]);
            zoner_tx(zoner_command);
        }
        if (read.is_pressed)
        {
            read.is_pressed_scene = 0;
        }
        flag_active_zone = 8;
    }
    else if (!(read.is_pressed_zone & (1 << 7)) && flag_active_zone == 8)
    {
        coord(3, 5, 0);
        put_pixel(coordxy);
        zone_flags[7] = 0;
        if (!flag_redact)
        {
            channel_config(7, 0, zone_flags[7]);
            zoner_tx(zoner_command);
        }
        flag_active_zone = 0;
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
    else if (!(read.is_pressed_scene_write & (1 << 1)) && flag_redact == 1)
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
    else if (!(read.is_pressed_scene_write & (1 << 0)) && flag_redact == 2)
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
    else if (!(read.is_pressed_scene_write & (1 << 2)) && flag_redact == 3)
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
    else if (!(read.is_pressed_scene_write & (1 << 3)) && flag_redact == 4)
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

int main()
{
    stdio_init_all();
    orient._private_us_since_boot = 0;
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
    scene[0] = 0x12;
    scene[1] = 0x40;
    scene[2] = 0x03;
    scene[3] = 0xFF;
    while (1)
    {
        /*gpio_put(DIR, 0);
        sleep_ms(500);
        gpio_put(DIR, 1);
        sleep_ms(500);*/
        poll_led();
        /*if (flag_redact == 0)
        {
            if (read.is_pressed_scene0 == 1)
            {
                flag_redact = 1;
                //read.is_pressed_scene0 = 0;
            }
            else if (read.is_pressed_scene1)
            {
                flag_redact = 2;
                //read.is_pressed_scene1 = 0;
            }
            else if (read.is_pressed_scene2)
            {
                flag_redact = 3;
                //read.is_pressed_scene2 = 0;
            }
            else if (read.is_pressed_scene3)
            {
                flag_redact = 4;
                //read.is_pressed_scene3 = 0;
            }
        }

        else if (flag_redact >= 1)
        {
            redact_scene(flag_redact - 1);
            flag_redact = 5;
        }
        if (flag_redact == 5)
        {
            coord(3, 5, 1);
            put_pixel(coordxy);
            sleep_ms(200);
            coord(3, 5, 0);
            put_pixel(coordxy);
            coord(3, 5, 1);
            put_pixel(coordxy);
            sleep_ms(200);
            coord(3, 5, 0);
            put_pixel(coordxy);
            coord(3, 5, 1);
            put_pixel(coordxy);
            sleep_ms(200);
            coord(3, 5, 0);
            put_pixel(coordxy);
            sleep_ms(200);
            coord(3, 5, 1);
            put_pixel(coordxy);
            flag_redact = 0;
        }
        if (tx_flag == 1)
        {
            zoner_tx(zoner_command);
        }
        asm("nop");*/
    }
    return 0;
}
