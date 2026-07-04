/*
 * lcd.c
 *
 *  Created on: Aug 16, 2025
 *      Author: bhavy
 */


// lcd.c
#include "lcd.h"

// Private function to send a nibble (4 bits) to the LCD
static void LCD_SendNibble(uint8_t nibble) {
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Private function to pulse the EN pin
static void LCD_PulseEnable(void) {
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(1);  // Short delay for enable pulse
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);  // Delay after pulse
}

// Function to send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);  // RS=0 for command

    // Send high nibble
    LCD_SendNibble(cmd >> 4);
    LCD_PulseEnable();

    // Send low nibble
    LCD_SendNibble(cmd & 0x0F);
    LCD_PulseEnable();

    HAL_Delay(2);  // Delay for command execution
}

// Function to send data (character) to the LCD
void LCD_SendData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);  // RS=1 for data

    // Send high nibble
    LCD_SendNibble(data >> 4);
    LCD_PulseEnable();

    // Send low nibble
    LCD_SendNibble(data & 0x0F);
    LCD_PulseEnable();

    HAL_Delay(1);  // Short delay for data
}

// Initialize the LCD in 4-bit mode
void LCD_Init(void) {
    // Assuming GPIO pins are already initialized as outputs in your main code (e.g., via MX_GPIO_Init)

    HAL_Delay(50);  // Wait for LCD to power up (>40ms)

    // Initialization sequence for 4-bit mode
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // Send 0x03 three times
    LCD_SendNibble(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);

    LCD_SendNibble(0x03);
    LCD_PulseEnable();
    HAL_Delay(1);

    LCD_SendNibble(0x03);
    LCD_PulseEnable();
    HAL_Delay(1);

    // Set to 4-bit mode
    LCD_SendNibble(0x02);
    LCD_PulseEnable();
    HAL_Delay(1);

    // Function set: 4-bit, 2 lines, 5x8 font
    LCD_SendCommand(0x28);

    // Display on, cursor off, blink off
    LCD_SendCommand(0x0C);

    // Clear display
    LCD_SendCommand(0x01);
    HAL_Delay(2);

    // Entry mode: increment cursor, no shift
    LCD_SendCommand(0x06);
}

// Print a string to the LCD
void LCD_PrintString(const char *str) {
    while (*str) {
        LCD_SendData((uint8_t)*str++);
    }
}

// Set cursor position (row 0-1, col 0-15)
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 + col : 0xC0 + col;
    LCD_SendCommand(address);
}

// Clear the LCD display
void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}
