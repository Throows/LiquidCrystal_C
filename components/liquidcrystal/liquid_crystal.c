/**
 * @file liquid_crystal.c
 * @author Romain Berthoule @ Throows (throowsdev@gmail.com)
 * @brief This is a port of the Arduino LiquidCrystal library for ESP-IDF (C/C++ compatible).
 * @version 1.0
 * @date 2023-04-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "liquid_crystal.h"

#include <driver/gpio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"

#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_CURSOR      0x04
#define LCD_DISPLAY     0x08
#define LCD_SHIFT       0x10
#define LCD_SETTINGS    0x20
#define LCD_CGRAM       0x40
#define LCD_DDRAM       0x80

// Cursor
#define LCD_SHIFT_L  0x02
#define LCD_SHIFT_R  0x00
#define LCD_INC_MODE 0x01
#define LCD_DEC_MODE 0x00

// Display
#define LCD_DISPLAY_ON  0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON   0x02
#define LCD_CURSOR_OFF  0x00
#define LCD_BLINK_ON    0x01
#define LCD_BLINK_OFF   0x00

// Shift
#define LCD_SHIFT_DISPLAY   0x08
#define LCD_SHIFT_CURSOR    0x00
#define LCD_SHIFT_RIGHT     0x04
#define LCD_SHIFT_LEFT      0x00

// Function set
#define LCD_8BIT_MODE   0x10
#define LCD_4BIT_MODE   0x00
#define LCD_2LINE_MODE  0x08
#define LCD_1LINE_MODE  0x00
#define LCD_5x10DOTS    0x04
#define LCD_5x7DOTS     0x00

// ====== Private Declarations ====== //
esp_err_t send_command(liquid_crystal_t *lcd, uint8_t value, bool write);
void pulse(gpio_num_t pin);

// ====== Public Functions ====== //
esp_err_t lcd_init(liquid_crystal_t *lcd)
{
    // Check if pins are valid
    if (lcd->rs_pin == 0 || lcd->enable_pin == 0) {
        ESP_LOGE("LCD", "RS, RW and Enable pins must be set");
        return ESP_ERR_INVALID_ARG;
    }

    if (lcd->num_lines > 4 || lcd->num_lines < 1) {
        ESP_LOGE("LCD", "Number of lines must be between 1 and 4");
        return ESP_ERR_INVALID_ARG;
    }

    // Set up pins
    gpio_set_direction(lcd->rs_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(lcd->rs_pin, 0); 
    gpio_set_direction(lcd->rw_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(lcd->rw_pin, 0);                         // set to write mode disable read mode
    gpio_set_direction(lcd->enable_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(lcd->enable_pin, 0); 

    int nb_bits = lcd->is_8bit ? 8 : 4;
    for (int i = 0; i < nb_bits; i++) {
        gpio_set_direction(lcd->data_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(lcd->data_pins[i], 0);
    }

    // Initialization phase
    uint8_t config = LCD_SETTINGS 
                | (lcd->is_8bit ? LCD_8BIT_MODE : LCD_4BIT_MODE) 
                | (lcd->is_2rows ? LCD_2LINE_MODE : LCD_1LINE_MODE) 
                | (lcd->is_large_font ? LCD_5x10DOTS : LCD_5x7DOTS);

    vTaskDelay(pdMS_TO_TICKS(50));          // Wait for LCD to power up

    // Refer to the datasheet for the initialization sequence
    if (lcd->is_8bit) {
        send_command(lcd, config, false);     
        vTaskDelay(pdMS_TO_TICKS(5));
        send_command(lcd, config, false);     
        vTaskDelay(pdMS_TO_TICKS(1));
        send_command(lcd, config, false);  
    } else {
        for (int i = 0; i < 4; i++) 
            gpio_set_level(lcd->data_pins[i], (0x03 >> i) & 0x01);
        pulse(lcd->enable_pin);
        vTaskDelay(pdMS_TO_TICKS(5));

        for (int i = 0; i < 4; i++) 
            gpio_set_level(lcd->data_pins[i], (0x03 >> i) & 0x01);
        pulse(lcd->enable_pin);
        vTaskDelay(pdMS_TO_TICKS(5));

        for (int i = 0; i < 4; i++) 
            gpio_set_level(lcd->data_pins[i], (0x03 >> i) & 0x01);
        pulse(lcd->enable_pin);
        vTaskDelay(pdMS_TO_TICKS(1));

        for (int i = 0; i < 4; i++) 
            gpio_set_level(lcd->data_pins[i], (0x02 >> i) & 0x01);
        pulse(lcd->enable_pin);

    }
    send_command(lcd, config, false);

    // Default settings
    send_command(lcd, LCD_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF, false);
    send_command(lcd, LCD_CURSOR | LCD_SHIFT_L  | LCD_DEC_MODE, false);

    return ESP_OK;
}

esp_err_t lcd_home(liquid_crystal_t *lcd)
{
    esp_err_t err = send_command(lcd, LCD_HOME, false);
    vTaskDelay(pdMS_TO_TICKS(50));
    return err;
}

esp_err_t lcd_clear(liquid_crystal_t *lcd)
{
    esp_err_t err = send_command(lcd, LCD_CLEAR, false);
    vTaskDelay(pdMS_TO_TICKS(50));
    return err;
}

esp_err_t lcd_display_mode(liquid_crystal_t *lcd, bool enabled, bool cursor, bool blink)
{
    return send_command(lcd, LCD_DISPLAY 
                            | (enabled ? LCD_DISPLAY_ON : LCD_DISPLAY_OFF) 
                            | (cursor ? LCD_CURSOR_ON : LCD_CURSOR_OFF) 
                            | (blink ? LCD_BLINK_ON : LCD_BLINK_OFF), false);
}

esp_err_t lcd_scroll_left(liquid_crystal_t *lcd)
{
    return send_command(lcd, LCD_SHIFT | LCD_SHIFT_DISPLAY | LCD_SHIFT_LEFT, false);
}

esp_err_t lcd_scroll_right(liquid_crystal_t *lcd)
{
    return send_command(lcd, LCD_SHIFT | LCD_SHIFT_DISPLAY | LCD_SHIFT_RIGHT, false);
}

esp_err_t lcd_left_to_right(liquid_crystal_t *lcd, bool autoscroll)
{
    return send_command(lcd, LCD_CURSOR | LCD_SHIFT_L | (autoscroll ? LCD_INC_MODE : LCD_DEC_MODE), false);
}

esp_err_t lcd_right_to_left(liquid_crystal_t *lcd, bool autoscroll)
{
    return send_command(lcd, LCD_CURSOR | LCD_SHIFT_R | (autoscroll ? LCD_INC_MODE : LCD_DEC_MODE), false);
}

esp_err_t lcd_create_char(liquid_crystal_t *lcd, uint8_t location, const uint8_t *charmap)
{
    if (location > 7) 
        return ESP_ERR_INVALID_ARG;
    uint8_t loc = location % 8;
    send_command(lcd, LCD_CGRAM | (loc << 3), false);
    for (int i = 0; i < 8; i++){
        send_command(lcd, charmap[i], true);
    }
    return ESP_OK;
}

esp_err_t lcd_set_cursor(liquid_crystal_t *lcd, uint8_t col, uint8_t row)
{
    if (row >= 4) 
        row = 3;
    if (row >= lcd->num_lines)
        row = lcd->num_lines - 1;
    return send_command(lcd, LCD_DDRAM | (col + lcd->row_offsets[row]), false);
}

esp_err_t lcd_print(liquid_crystal_t *lcd, const char *data)
{
    size_t size = strlen(data);
    const uint8_t *data_ptr = (const uint8_t *)data;
    for (int i = 0; i < size; i++) {
        lcd_write(lcd, data_ptr[i]);   
    }
    return ESP_OK;
}

esp_err_t lcd_write(liquid_crystal_t *lcd, const uint8_t data)
{
    return send_command(lcd, data, true);
}

// ====== Private Functions ====== //
esp_err_t send_command(liquid_crystal_t *lcd, const uint8_t value, bool write) 
{
    gpio_set_level(lcd->rs_pin, write);
    if (lcd->is_8bit) {
        for (int i = 0; i < 8; i++) 
            gpio_set_level(lcd->data_pins[i], (value >> i) & 0x01);
        pulse(lcd->enable_pin);
    } else {
        uint8_t msb_val = value >> 4;
        for (int i = 0; i < 4; i++) 
            gpio_set_level(lcd->data_pins[i], (msb_val >> i) & 0x01);
        pulse(lcd->enable_pin);
        for (int i = 0; i < 4; i++) 
            gpio_set_level(lcd->data_pins[i], (value >> i) & 0x01);
        pulse(lcd->enable_pin);
    }
    vTaskDelay(pdMS_TO_TICKS(1));
    return ESP_OK;
}

void pulse(gpio_num_t pin)
{
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(1));    // thus for non-blocking delay
    gpio_set_level(pin, 1);
    vTaskDelay(pdMS_TO_TICKS(1));                          
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
}
