//System includes
#include "hardware/flash.h"
//DF Player Mini includes
#include "uart_pio.pio.h"
#include "DFPlayerMini/df.h"
//ZONER includes
#include "Zoner/zoner.h"
//KBD includes
#include "KBD/kbd.h"
//ZONER defines


//DF Player Mini variables
extern uint8_t data[10];
extern char flag;
unsigned char once_play = 0;
//ZONER variables
extern uint32_t zoner_command[3];
uint8_t tx_flag = 0;
//System defines
#define FLASH_TARGET_OFFSET (256 * 1024)
//System variables
const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
//System fuctions
void redact_scene(uint8_t sc);

uint8_t scene[4][8];
uint8_t zone_flags[8];
uint8_t flag_redact = 0;
absolute_time_t micros = 0;
absolute_time_t orient = 0;

void flag_callback(uint gpio, uint32_t events)
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
}

void redact_scene(uint8_t sc)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        scene[sc][i] = zone_flags[i];
    }
    flash_range_program(FLASH_TARGET_OFFSET + sc, scene[sc], FLASH_PAGE_SIZE);      //Adding data to FLASH memory. In theory, that's must adding data from one zone to defined for it memory address. In fact - didn't tested.
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
            keyboard_led(k, false);
        }
            break;
        case 1:
        for (uint8_t i = 0; i < 8; i++)
        {
            channel_config(scene[0][i], 0, scene[0][i] > 0 ? true : false);
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
}

int main()
{
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    //ZONER Init
    uart_init(UART_ID, BAUD_RATE_h);
    gpio_set_function(UART_TX_PIN_h, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_h, GPIO_FUNC_UART);
    //DF Player Mini Init
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &uart_pio_program);
    uart_pio_program_init(pio, sm, offset, UART_TX_PIN, BAUD_RATE);
    sleep_ms(1000);
    set_volume(1);
    play_sound_immediatly(1);
    //KBD Init
    gpio_init(KBD_INT0);
    gpio_set_dir(KBD_INT0, GPIO_IN);
    gpio_pull_up(KBD_INT0);
    gpio_init(KBD_INT1);
    gpio_set_dir(KBD_INT1, GPIO_IN);
    gpio_pull_up(KBD_INT1);
    gpio_set_irq_enabled_with_callback(KBD_INT0, GPIO_IRQ_EDGE_FALL, true, &flag_callback);
    gpio_set_irq_enabled_with_callback(KBD_INT1, GPIO_IRQ_EDGE_FALL, true, &flag_callback);
    keyboard_i2c_init();
    keyboard_init();
    if (check_flash() == false)
    {
        load_default_param();
    }
    while(1)
    {
        if (flag_redact == 1)
        {
            if (get_absolute_time() - orient >= 200000)
            {
                keyboard_led(6, false);
                orient = get_absolute_time();
            }
            else if (get_absolute_time() - orient >= 100000 && get_absolute_time() - orient < 200000)
            {
                keyboard_led(6, true);
            }
        }
        static uint8_t iteration = 0;
        if (flag_redact == 2)
        {
            keyboard_led(6, false);
            sleep_ms(200);
            keyboard_led(6, true);
            sleep_ms(200);
            keyboard_led(6, false);
            sleep_ms(200);
            keyboard_led(6, true);
            sleep_ms(200);
            keyboard_led(6, false);
            sleep_ms(200);
            keyboard_led(6, true);
            sleep_ms(200);
            flag_redact = 0;
        }
        if (tx_flag == 1)
        {
            zoner_tx(zoner_command);
        }
        asm("nop");
    }
    return 0;
}

