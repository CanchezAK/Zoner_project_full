#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>

#define KBD_ADDR 0x34
#define KBD_READ_ADDR 0x34 | 1

#define INT_PIN 8

#define CFG_REG 0x01
#define INT_STAT_REG 0x02
#define KEY_LCK_EC_REG 0x03
#define KEY_EVENT_A_REG 0x04
#define KEY_EVENT_B_REG 0x05
#define KEY_EVENT_C_REG 0x06
#define KEY_EVENT_D_REG 0x07
#define KEY_EVENT_E_REG 0x08
#define KEY_EVENT_F_REG 0x09
#define KEY_EVENT_G_REG 0x0A
#define KEY_EVENT_H_REG 0x0B
#define KEY_EVENT_I_REG 0x0C
#define KEY_EVENT_J_REG 0x0D
#define KP_LCK_TIMER_REG 0x0E
#define Unlock1_REG 0x0F
#define Unlock2_REG 0x10
#define GPIO_INT_STAT1_REG 0x11
#define GPIO_INT_STAT2_REG 0x12
#define GPIO_INT_STAT3_REG 0x13
#define GPIO_DAT_STAT1_REG 0x14
#define GPIO_DAT_STAT2_REG 0x15
#define GPIO_DAT_STAT3_REG 0x16
#define GPIO_DAT_OUT1_REG 0x17
#define GPIO_DAT_OUT2_REG 0x18
#define GPIO_DAT_OUT3_REG 0x19
#define GPIO_INT_EN1_REG 0x1A
#define GPIO_INT_EN2_REG 0x1B
#define GPIO_INT_EN3_REG 0x1C
#define KP_GPIO1_REG 0x1D
#define KP_GPIO2_REG 0x1E
#define KP_GPIO3_REG 0x1F
#define GPI_EM1_REG 0x20
#define GPI_EM2_REG 0x21
#define GPI_EM3_REG 0x22
#define GPIO_DIR1_REG 0x23
#define GPIO_DIR2_REG 0x24
#define GPIO_DIR3_REG 0x25
#define GPIO_INT_LVL1_REG 0x26
#define GPIO_INT_LVL2_REG 0x27
#define GPIO_INT_LVL3_REG 0x28
#define DEBOUNCE_DIS1_REG 0x29
#define DEBOUNCE_DIS2_REG 0x2A
#define DEBOUNCE_DIS3_REG 0x2B
#define GPIO_PULL1_REG 0x2C
#define GPIO_PULL2_REG 0x2D
#define GPIO_PULL3_REG 0x2E
//CFG_REG Config bits
#define AI (1 << 7)
#define GPI_E_CFG (1 << 6)
#define OVR_FLOW_M (1 << 5)
#define INT_CFG (1 << 4)
#define OVR_FLOW_IEN (1 << 3)
#define K_LCK_IEN (1 << 2)
#define GPI_IEN (1 << 1)
#define KE_IEN (1 << 0)
//INT_STAT_REG Config bits
#define CAD_INT (1 << 4)
#define OVR_FLOW_INT (1 << 3)
#define K_LCK_INT (1 << 2)
#define GPI_INT (1 << 1)
#define K_INT (1 << 0)
//KEY_LCK_EC_REG Config bits
#define K_LCK_EN (1 << 6)
#define LCK2 (1 << 5)
#define LCK1 (1 << 4)
#define KEC3 (1 << 3)
#define KEC2 (1 << 2)
#define KEC1 (1 << 1)
#define KEC0 (1 << 0)
//KEY_EVENT_A_REG Config bits
#define KEA7 (1 << 7)
#define KEA6 (1 << 6)
#define KEA5 (1 << 5)
#define KEA4 (1 << 4)
#define KEA3 (1 << 3)
#define KEA2 (1 << 2)
#define KEA1 (1 << 1)
#define KEA0 (1 << 0)
//KP_LCK_TIMER_REG Config bits
#define KL7 (1 << 7)
#define KL6 (1 << 6)
#define KL5 (1 << 5)
#define KL4 (1 << 4)
#define KL3 (1 << 3)
#define KL2 (1 << 2)
#define KL1 (1 << 1)
#define KL0 (1 << 0)
//Unlock1_REG Config bits
#define UK1_7 (1 << 7)
#define UK1_6 (1 << 6)
#define UK1_5 (1 << 5)
#define UK1_4 (1 << 4)
#define UK1_3 (1 << 3)
#define UK1_2 (1 << 2)
#define UK1_1 (1 << 1)
#define UK1_0 (1 << 0)
//Unlock2_REG Config bits
#define UK2_7 (1 << 7)
#define UK2_6 (1 << 6)
#define UK2_5 (1 << 5)
#define UK2_4 (1 << 4)
#define UK2_3 (1 << 3)
#define UK2_2 (1 << 2)
#define UK2_1 (1 << 1)
#define UK2_0 (1 << 0)
//KP_GPIO1_REG & GPI_EMx_REG Config bits
#define ROW7 (1 << 7)
#define ROW6 (1 << 6)
#define ROW5 (1 << 5)
#define ROW4 (1 << 4)
#define ROW3 (1 << 3)
#define ROW2 (1 << 2)
#define ROW1 (1 << 1)
#define ROW0 (1 << 0)
//KP_GPIO2_REG & GPI_EMx_REG Config bits
#define COL7 (1 << 7)
#define COL6 (1 << 6)
#define COL5 (1 << 5)
#define COL4 (1 << 4)
#define COL3 (1 << 3)
#define COL2 (1 << 2)
#define COL1 (1 << 1)
#define COL0 (1 << 0)
//KP_GPIO3_REG & GPI_EM3_REG Config bits
#define COL9 (1 << 1)
#define COL8 (1 << 0)

//Short macro
#define SCENE_QUESTION read.KEY_EVENT_A / 10 == 5 && read.KEY_EVENT_A % 10 == 0 || read.KEY_EVENT_A / 10 == 4 && read.KEY_EVENT_A % 10 == 0 || read.KEY_EVENT_A / 10 == 3 && read.KEY_EVENT_A % 10 == 0 || read.KEY_EVENT_A / 10 == 2 && read.KEY_EVENT_A % 10 == 0
#define MILLION 1000000

//Function declaration
struct KBD_Config{
    uint8_t CFG,
    KP_GPIO1,
    KP_GPIO2,
    KP_GPIO3,
    UNLOCK1,
    UNLOCK2,
    KP_LCK_TIMER,
    KEY_EVENT_A,
    available,
    reg,
    is_pressed,
    is_pressed_scene,
    //is_pressed_scene1,
    //is_pressed_scene2,
    //is_pressed_scene3,
    is_pressed_scene_write;
    uint16_t is_pressed_zone;
    //is_pressed_scene1_write,
    //is_pressed_scene2_write,
    //is_pressed_scene3_write;
    absolute_time_t is_pressed_time_start,
    is_pressed_time_stop;
};
unsigned char KBD_Init(struct KBD_Config config);
unsigned char KBD_ReadKeypad();
void KBD_ClearISRflags();
void KBD_Debug();