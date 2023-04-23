/**
 * @file main.c
 * @author Romain Berthoule (throowsdev@gmail.com)
 * @brief This is a example C LiquidCrystal library for the ESP-IDF (C++ compatible).
 * @version 0.1
 * @date 2023-04-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <string.h>

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freeRTOS/task.h>


#include "liquid_crystal.h"

const uint8_t hourglass[8][8] = {
    {0x00, 0x1F, 0x1F, 0x0E, 0x04, 0x0A, 0x11, 0x1F},
    {0x00, 0x1F, 0x1B, 0x0E, 0x04, 0x0E, 0x11, 0x1F},
    {0x00, 0x1F, 0x1B, 0x0E, 0x04, 0x0A, 0x15, 0x1F},
    {0x00, 0x1F, 0x19, 0x0E, 0x04, 0x0E, 0x15, 0x1F},
    {0x00, 0x1F, 0x19, 0x0E, 0x04, 0x0A, 0x17, 0x1F},
    {0x00, 0x1F, 0x11, 0x0E, 0x04, 0x0E, 0x17, 0x1F},
    {0x00, 0x1F, 0x11, 0x0E, 0x04, 0x0A, 0x1F, 0x1F},
    {0x00, 0x1F, 0x11, 0x0A, 0x04, 0x0E, 0x1F, 0x1F}
};

#define HOURGLASS_0 0x08
#define HOURGLASS_1 0x01
#define HOURGLASS_2 0x02
#define HOURGLASS_3 0x03
#define HOURGLASS_4 0x04
#define HOURGLASS_5 0x05
#define HOURGLASS_6 0x06
#define HOURGLASS_7 0x07

const uint8_t cycle[] = {
    HOURGLASS_0, 
    HOURGLASS_1, 
    HOURGLASS_2, 
    HOURGLASS_3, 
    HOURGLASS_4, 
    HOURGLASS_5, 
    HOURGLASS_6, 
    HOURGLASS_7
};


void app_main(void)
{
    liquid_crystal_t lcd = {
        .rs_pin = GPIO_NUM_10,
        .rw_pin = GPIO_NUM_11,
        .enable_pin = GPIO_NUM_12,
        .data_pins = {GPIO_NUM_35, GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_38},
        .num_lines = 2,
        .row_offsets = {0x00, 0x40, 0x14, 0x54},
        .is_8bit = false,
        .is_2rows = true,
        .is_large_font = false
    };

    // Initialize the LCD
    lcd_init(&lcd);

    // Register custom characters
    for (int i = 0; i < 8; i++) {
        lcd_create_char(&lcd, i, hourglass[i]);
    }

    lcd_clear(&lcd);
    lcd_home(&lcd);
    lcd_set_cursor(&lcd, 0, 0);
    lcd_print(&lcd, "> Hello World! <");
    uint8_t hourglassTime = 0;
    while (true) {
        lcd_set_cursor(&lcd, 4, 1);
        lcd_write(&lcd, cycle[hourglassTime]);
        lcd_set_cursor(&lcd, 11, 1);
        lcd_write(&lcd, cycle[7 - hourglassTime]);
        vTaskDelay(pdMS_TO_TICKS(1000));
        hourglassTime++;
        hourglassTime = hourglassTime % 8;
        
    }
}
