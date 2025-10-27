/* * File:   GLCD.c
 * Author: Mostafa Eshra
 * Description: Low-level driver for KS0108-based 128x64 GLCD.
 */

#include "GLCD.h"
#include "DIO.h" 
#include "GLCD_Font.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>

// --- Internal Helper Functions ---

// Waits for the controller chip to finish its current operation
void GLCD_BusyWait(void) {
    char busy_flag; // Variable to hold the result of the DIO_Read_PIN function

    // 1. Set GLCD data port to input (Use GLCD_DATA_PORT for direction setting)
    DIO_Set_PORT_DIR(GLCD_DATA_PORT, 0x00);
    
    // 2. Control sequence for reading busy flag
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RW_PIN, HIGH); // Read mode
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RS_PIN, LOW);  // Command mode
    
    // 3. Loop until Busy Flag (DB7) is LOW (not busy)
    do {
        // Toggle Enable pin to read the status
        DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_E_PIN, HIGH);
        _delay_us(1); 
        DIO_Read_PIN(GLCD_DATA_PORT, 7, &busy_flag); // Read the value of DB7
        DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_E_PIN, LOW);
        _delay_us(1); 
    } while (busy_flag == HIGH); // Repeat if DB7 (Busy Flag) is HIGH
    
    // 4. Set GLCD data port back to output for writing data
    DIO_Set_PORT_DIR(GLCD_DATA_PORT, 0xFF);
    
    // 5. Reset control pins
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RW_PIN, LOW); // Write mode
}

// Selects the desired chip (CS1 or CS2)
void GLCD_SelectChip(uint8_t chip) {
    // Disable all chips first
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_CS1_PIN, LOW);
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_CS2_PIN, LOW);
    
    // Enable the selected chip
    if (chip == 1) {
        DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_CS1_PIN, HIGH);
    } else if (chip == 2) {
        DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_CS2_PIN, HIGH);
    }
    GLCD_BusyWait(); 
}

// Sends a command byte to the currently selected chip
void GLCD_Command(uint8_t cmd) {
    GLCD_BusyWait(); // Wait until the chip is ready

    // 1. Control signals: RS=LOW (Command), RW=LOW (Write)
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RS_PIN, LOW); 
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RW_PIN, LOW); 
    
    // 2. Put data on bus and pulse Enable
    DIO_Set_PORT_VALUE(GLCD_DATA_PORT, cmd);
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_E_PIN, HIGH);
    _delay_us(1); 
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_E_PIN, LOW);
}

// Sends a data byte (pixels) to the currently selected chip
void GLCD_Data(uint8_t data) {
    GLCD_BusyWait(); // Wait until the chip is ready

    // 1. Control signals: RS=HIGH (Data), RW=LOW (Write)
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RS_PIN, HIGH);
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RW_PIN, LOW); 
    
    // 2. Put data on bus and pulse Enable
    DIO_Set_PORT_VALUE(GLCD_DATA_PORT, data);
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_E_PIN, HIGH);
    _delay_us(1); 
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_E_PIN, LOW);
}

// --- Public Driver Functions ---

// Initializes GLCD control ports and sends initialization commands
void GLCD_Init(void) {
    // 1. Configure Control Pins (OUTPUT) - Use GLCD_CONTROL_PORT
    DIO_Set_PIN_DIR(GLCD_CONTROL_PORT, GLCD_RS_PIN, OUTPUT);
    DIO_Set_PIN_DIR(GLCD_CONTROL_PORT, GLCD_RW_PIN, OUTPUT);
    DIO_Set_PIN_DIR(GLCD_CONTROL_PORT, GLCD_E_PIN, OUTPUT);
    DIO_Set_PIN_DIR(GLCD_CONTROL_PORT, GLCD_CS1_PIN, OUTPUT);
    DIO_Set_PIN_DIR(GLCD_CONTROL_PORT, GLCD_CS2_PIN, OUTPUT);
    DIO_Set_PIN_DIR(GLCD_CONTROL_PORT, GLCD_RST_PIN, OUTPUT);
    
    // 2. Configure Data Port (OUTPUT) - Use GLCD_DATA_PORT
    DIO_Set_PORT_DIR(GLCD_DATA_PORT, 0xFF);

    // 3. Hardware Reset (briefly pull RST low)
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RST_PIN, LOW);
    _delay_ms(10);
    DIO_Set_PIN_VALUE(GLCD_CONTROL_PORT, GLCD_RST_PIN, HIGH);
    _delay_ms(10);
    
    // 4. Send initialization commands to both chips
    // Select Chip 1 (Left Half)
    GLCD_SelectChip(1);
    GLCD_Command(GLCD_DISPLAY_ON);
    GLCD_Command(GLCD_START_LINE_ADDR + 0);
    
    // Select Chip 2 (Right Half)
    GLCD_SelectChip(2);
    GLCD_Command(GLCD_DISPLAY_ON);
    GLCD_Command(GLCD_START_LINE_ADDR + 0);
    
    // Clear the whole display
    GLCD_ClearScreen();
}

// Simple string reversal helper for int_to_string
static void reverse(char s[]) {
    uint8_t i, j;
    char c;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// Custom function to convert integer value to a string (itoa)
void int_to_string(int32_t value, char *buffer) {
    uint8_t i = 0;
    
    // Handle 0
    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }
    
    // Process digits in reverse order
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }
    buffer[i] = '\0';
    
    reverse(buffer);
}

// Clears the entire GLCD screen (fills all memory with 0x00)
void GLCD_ClearScreen(void) {
    for (uint8_t page = 0; page < GLCD_PAGES; page++) {
        // Clear Chip 1
        GLCD_SelectChip(1);
        GLCD_Command(GLCD_SET_PAGE_ADDR + page);
        GLCD_Command(GLCD_SET_COLUMN_ADDR + 0);

        for (uint8_t col = 0; col < (GLCD_WIDTH / 2); col++) {
            GLCD_Data(0x00);
        }

        // Clear Chip 2
        GLCD_SelectChip(2);
        GLCD_Command(GLCD_SET_PAGE_ADDR + page);
        GLCD_Command(GLCD_SET_COLUMN_ADDR + 0);

        for (uint8_t col = 0; col < (GLCD_WIDTH / 2); col++) {
            GLCD_Data(0x00);
        }
    }
}

// *** CRUCIAL CURSOR-SETTING FUNCTION (GoToPageColumn) ***
// This function translates the 128-column address into a chip select and local column address.
void GLCD_GoToPageColumn(uint8_t page, uint8_t column) {
    if (page >= GLCD_PAGES || column >= GLCD_WIDTH) {
        return; // Ignore invalid coordinates
    }

    uint8_t chip;
    uint8_t local_column;

    // 1. Determine which chip to address (Left or Right)
    if (column < (GLCD_WIDTH / 2)) {
        // Column 0-63 (Left Chip)
        chip = 1;
        local_column = column;
    } else {
        // Column 64-127 (Right Chip)
        chip = 2;
        // Local column is relative to the chip (e.g., global 64 is local 0)
        local_column = column - (GLCD_WIDTH / 2);
    }
    
    // 2. Select the determined chip
    GLCD_SelectChip(chip);

    // 3. Send the address commands to the chip
    GLCD_Command(GLCD_SET_PAGE_ADDR + page);
    GLCD_Command(GLCD_SET_COLUMN_ADDR + local_column);
}

// Writes a single character at the specified starting position
void GLCD_WriteChar(uint8_t page, uint8_t column, char ch) {
    
    // Check if character is in the defined font range
    uint8_t font_index = ch - FONT_START_CHAR;
    if (ch < FONT_START_CHAR || ch > FONT_END_CHAR) {
        font_index = 0; // Default to space if character is undefined
    }

    // Go to the specified starting position
    GLCD_GoToPageColumn(page, column);

    // Draw the 5 columns of the character data
    for (uint8_t i = 0; i < FONT_WIDTH; i++) {
        GLCD_Data(FONT_DATA[font_index][i]);
    }

    // Draw one blank column for spacing
    GLCD_Data(0x00);
}

// Writes a string starting at the specified page and column
void GLCD_WriteString(uint8_t page, uint8_t column, const char* str) {
    uint8_t current_col = column;

    for (uint8_t i = 0; i < strlen(str); i++) {
        if (current_col > (GLCD_WIDTH / 2) - (FONT_WIDTH + 1)) {
            // If the character cannot fit on the current chip (Chip 1 ends at 63),
            // move the cursor to the start of the next chip (column 64).
            if (current_col < (GLCD_WIDTH / 2)) {
                current_col = GLCD_WIDTH / 2; // Sets it to 64
            }
        }
    
        GLCD_WriteChar(page, current_col, str[i]);

        // Characters are 5 pixels + 1 space = 6 pixels wide
        current_col += (FONT_WIDTH + 1); 

        // Check if we hit the end of the display, if so, stop writing
        if (current_col >= GLCD_WIDTH) {
            return;
        }
    }
}

void GLCD_Write_Per(const char* str) {
    // Clear the part of the percentage 
        GLCD_GoToPageColumn(0, 100);
        for (uint8_t i = 0; i < 100; i++) {
            GLCD_Data(0x00);
        }
        GLCD_WriteString(0, 100, str);
}

void GLCD_Draw_Signal(char Signal_High) {
    GLCD_GoToPageColumn(5,0);
    for (uint8_t i = 0; i < Signal_High; i++) {
        GLCD_Data(High);
    }
    if (Signal_High < 64) {
        if (Signal_High != 0) {
            GLCD_GoToPageColumn(5,63);
            GLCD_Data(0xFF);
            GLCD_GoToPageColumn(6,63);
            GLCD_Data(0xFF);
            GLCD_GoToPageColumn(5,(Signal_High));
            GLCD_Data(0xFF);
            GLCD_GoToPageColumn(6,(Signal_High));
            GLCD_Data(0xFF);
            for (uint8_t i = 0; i < (64 - Signal_High - 2); i++) {
                GLCD_Data(Low);
            }
        }
        else {
            for (uint8_t i = 0; i < (64 - Signal_High); i++) {
                GLCD_Data(Low);
            }
        }      
    }
    GLCD_GoToPageColumn(5,64);
    for (uint8_t i = 0; i < Signal_High; i++) {
        GLCD_Data(High);
    }
    if (Signal_High < 64) {
        if (Signal_High != 0) {
            GLCD_GoToPageColumn(5, (64 + Signal_High));
            GLCD_Data(0xFF);
            GLCD_GoToPageColumn(6,(64 + Signal_High));
            GLCD_Data(0xFF);
            for (uint8_t i = 0; i < (64 - Signal_High - 2); i++) {
                GLCD_Data(Low);
            }
        }
        else {
            for (uint8_t i = 0; i < (64 - Signal_High - 2); i++) {
                GLCD_Data(Low);
            }
        }   
    }
        
    _delay_ms(100);
        
    //clear the part of the signal
    for(uint8_t p = 5; p < 7; p++) {
        GLCD_GoToPageColumn(p, 0);
        for (uint8_t i = 0; i < 64; i++) {
            GLCD_Data(0x00);
        }
        GLCD_GoToPageColumn(p, 64);
        for (uint8_t i = 0; i < 64; i++) {
            GLCD_Data(0x00);
        }
    }    
}
