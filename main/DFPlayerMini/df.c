#include "df.h"
#include "C:\Users\admin\Documents\rp2040\check\build\main\uart_pio.pio.h"

char flag = 0;

uint8_t data[10] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x02, 0xFE, 0xF7, 0xEF};

    void transmit_data()
    {
        for(uint8_t i = 0; i <= 9; i++)
        {
            uart_pio_program_putc(pio0, 0, (char)data[i]);
        }
        sleep_ms(500);
    }

    void checksum()
    {
        int16_t checksum = 0;
        checksum = checksum - data[1] - data[2] - data[3] - data[4] - data[5] - data[6];
        data[7] = checksum >> 8;
        data[8] = checksum;
    }
    
    void set_equalizer(uint8_t equ)
    {
        data[3] = 0x07;
        data[5] = 0x00;
        data[6] = equ;
        checksum();
        transmit_data();
    }

    void play_cycle(uint16_t num_sound)
    {
        data[3] = 0x08;
        data[5] = (num_sound >> 8);
        data[6] = (num_sound & 0xFF);
        checksum();
        transmit_data();
    }

    void play_sound_immediatly(uint16_t num_sound)
    {
        data[3] = 0x03;
        data[5] = (num_sound >> 8);
        data[6] = (num_sound & 0xFF);
        checksum();
        transmit_data();
    }

    void play_sound_after_ending(uint16_t num_sound)
    {
        if (flag == 1)
        {
            data[3] = 0x03;
            data[5] = (num_sound >> 8);
            data[6] = (num_sound & 0xFF);
            checksum();
            transmit_data();
            flag = 0;
        }
    }

    void play_next_track()
    {
        data[3] = 0x01;
        data[5] = 0x00;
        data[6] = 0x00;
        checksum();
        transmit_data();

    }

    void play_previous_track()
    {
        data[3] = 0x02;
        data[5] = 0x00;
        data[6] = 0x00;
        checksum();
        transmit_data();
    }

    void set_volume(uint8_t vol)
    {
        data[3] = 0x06;
        data[5] = 0x00;
        data[6] = (vol > 30 ? 30 : vol < 0 ? 0 : vol);
        checksum();
        transmit_data();
    }
    void volume_plus()
    {
        data[3] = 0x04;
        data[5] = 0x00;
        data[6] = 0x00;
        checksum();
        transmit_data();
    }

    void volume_minus()
    {
        data[3] = 0x05;
        data[5] = 0x00;
        data[6] = 0x00;
        checksum();
        transmit_data();
    }

    void stop()
    {
        data[3] = 0x16;
        data[5] = 0x00;
        data[6] = 0x00;
        checksum();
        transmit_data();
    }