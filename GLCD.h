/* * File:   GLCD.h
 * Author: Mostafa Eshra
 *
 * Description: GLCD (KS0108) constants, commands, and function prototypes.
 */

#ifndef GLCD_H
#define GLCD_H

#include <stdint.h>
#include "DIO.h" // Dependency for pin control

// --- GLCD Hardware Definitions (Control Pins) ---
// Note: Based on your main.c, control pins are on PORTA and Data pins are on PORTC

#define GLCD_CONTROL_PORT      &PORTA
#define GLCD_CONTROL_PORT_DIR  &DDRA
#define GLCD_DATA_PORT         &PORTC
#define GLCD_DATA_PORT_DIR     &DDRC

#define GLCD_RS_PIN            PA0 // Register Select (Command/Data)
#define GLCD_RW_PIN            PA1 // Read/Write (Read=HIGH, Write=LOW)
#define GLCD_E_PIN             PA2 // Enable (Pulse HIGH to execute)
#define GLCD_CS1_PIN           PA3 // Chip Select 1 (Left Half: Col 0-63)
#define GLCD_CS2_PIN           PA4 // Chip Select 2 (Right Half: Col 64-127)
#define GLCD_RST_PIN           PA5 // Reset (Active LOW)


// --- GLCD Display Dimensions ---

#define GLCD_WIDTH             128 // 128 columns (pixels)
#define GLCD_HEIGHT            64  // 64 rows (pixels)
#define GLCD_PAGES             8   // 8 pages (1 page = 8 vertical pixels)


// --- GLCD Controller Commands (KS0108) ---

// Chip Selects are handled internally in GLCD.c
#define GLCD_DISPLAY_OFF           (0x3E)
#define GLCD_DISPLAY_ON            (0x3F)

// Page Address (0-7) command: B8h OR page number
#define GLCD_SET_PAGE_ADDR         (0xB8) 

// Column Address (0-63) command: 40h OR column number
#define GLCD_SET_COLUMN_ADDR       (0x40)

// Start Line Address (0-63) command: C0h OR line number
#define GLCD_START_LINE_ADDR       (0xC0)

#define High 0x01
#define Low 0x80


// --- Public Function Prototypes ---

void GLCD_Init(void);
void GLCD_ClearScreen(void);

// Data write function, used internally and needed for clearing artifacts
void GLCD_Data(uint8_t data);

// *** The new cursor setting function ***
// Sets the address pointer (page 0-7, column 0-127) for subsequent operations
void GLCD_GoToPageColumn(uint8_t page, uint8_t column);

void GLCD_WriteChar(uint8_t page, uint8_t column, char ch);
void GLCD_WriteString(uint8_t page, uint8_t column, const char* str);

void int_to_string(int32_t value, char *buffer);
static void reverse(char s[]);

void GLCD_Write_Per(const char* str);
void GLCD_Draw_Signal(char Signal_High);

#endif // GLCD_H
