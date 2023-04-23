/**
 * @file liquid_crystal.h
 * @author Romain Berthoule @ Throows (throowsdev@gmail.com)
 * @brief This is a port of the Arduino LiquidCrystal library for ESP-IDF (C/C++ compatible).
 * @version 1.0
 * @date 2023-04-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LIQUID_CRYSTAL_H
#define LIQUID_CRYSTAL_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Liquid Crystal device descriptor
 * 
 */
typedef struct {
    uint8_t rs_pin;             //!< Register select pin
    uint8_t rw_pin;             //!< Read/write pin
    uint8_t enable_pin;         //!< Enable pin
    uint8_t data_pins[8];       //!< Data pins (4 or 8 pins) leave unused pins as GPIO_NUM_NC
    uint8_t num_lines;          //!< Number of lines (1 to 4)
    uint8_t row_offsets[4];     //!< Row offsets to determine from your LCD specs
    bool is_8bit;               //!< 8-bit mode
    bool is_2rows;              //!< 2-row mode
    bool is_large_font;         //!< Large font mode (5x10 dots)
} liquid_crystal_t;

/**
 * @brief Initialize the LCD
 * 
 * @param lcd           The device descriptor pointer
 * @return esp_err_t    ESP_INVALID_ARG if the device descriptor is not correct, ESP_OK otherwise
 */
esp_err_t lcd_init(liquid_crystal_t *lcd);

/**
 * @brief Send a home command to the LCD
 * 
 * @param lcd           The device descriptor pointer
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_home(liquid_crystal_t *lcd);

/**
 * @brief Send a clear command to the LCD
 * 
 * @param lcd           The device descriptor pointer
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_clear(liquid_crystal_t *lcd);

/**
 * @brief Send a set cursor command to the LCD
 * 
 * @param lcd           The device descriptor pointer
 * @param enabled       Enable the display (true) or disable it (false)
 * @param cursor        Enable the cursor (true) or disable it (false)
 * @param blink         Enable the cursor blink (true) or disable it (false)
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_display_mode(liquid_crystal_t *lcd, bool enabled, bool cursor, bool blink);

/**
 * @brief Scroll the display to the left one time
 * 
 * @param lcd           The device descriptor pointer
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_scroll_left(liquid_crystal_t *lcd);

/**
 * @brief Scroll the display to the right one time
 * 
 * @param lcd           The device descriptor pointer
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_scroll_right(liquid_crystal_t *lcd);

/**
 * @brief Set the text direction from left to right
 * 
 * @param lcd           The device descriptor pointer
 * @param autoscroll    Enable the autoscroll (true) or disable it (false)
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_left_to_right(liquid_crystal_t *lcd, bool autoscroll);

/**
 * @brief Set the text direction from right to left
 * 
 * @param lcd           The device descriptor pointer
 * @param autoscroll    Enable the autoscroll (true) or disable it (false)
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_right_to_left(liquid_crystal_t *lcd, bool autoscroll);

/**
 * @brief Create a custom character
 * 
 * @param lcd           The device descriptor pointer
 * @param location      The location of the character (0 to 7)
 * @param charmap       The character map (8 bytes)
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_create_char(liquid_crystal_t *lcd, uint8_t location, const uint8_t *charmap);

/**
 * @brief Set the cursor position
 * 
 * @param lcd           The device descriptor pointer
 * @param col           The column
 * @param row           The row
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_set_cursor(liquid_crystal_t *lcd, uint8_t col, uint8_t row); 

/**
 * @brief Print a string to the LCD
 * 
 * @param lcd           The device descriptor pointer
 * @param data          The string to print
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_print(liquid_crystal_t *lcd, const char* data);

/**
 * @brief Print a single char to the LCD
 * 
 * @param lcd           The device descriptor pointer
 * @param data          The char to print
 * @return esp_err_t    ESP_OK
 */
esp_err_t lcd_write(liquid_crystal_t *lcd, const uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // LIQUID_CRYSTAL_H