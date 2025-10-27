/* * File:   main.c
 * Author: Mostafa Eshra
 */

#include <avr/io.h>
#include <util/delay.h> // Added for GLCD delay functions
#include <stdint.h> // Added for uint8_t, int32_t types
#include <math.h>
#include <string.h>

// --- Include Dependencies ---
#include "ADC.h" // Assuming this contains ADC_select_CH, ADC_SC, ADC_read, init_ADC
#include "DIO.h"
#include "Timer.h"
#include "GLCD.h" // New GLCD Header

#define High 0x01
#define Low 0x80

int main(void)
{
    // --- System & Peripheral Setup ---
    // 1. Configure ADC input pin (PA6) as INPUT
    DIO_Set_PIN_DIR(&PORTA, PA6, INPUT); 
    
    // 2. Initialize ADC on Channel 6 (PA6)
    // Assuming ADC functions are available (e.g., init_ADC)
     init_ADC(ADC_CH6, ADC_REF_AREF, ADC_PRE_32); 
    
    // 3. Initialize Timer0 for Fast PWM mode (Output on PB3)
    init_Timer0(TIMER0_MODE_FPWM, TIMER0_CS_PRE_64);
    Timer0_COMP_MODE(TIMER0_COMP_MODE_PWM_SET_ON_COUNT_UP);
    
    // 4. Initialize GLCD (Control on PORTA, Data on PORTC)
    GLCD_Init();
    GLCD_ClearScreen();
    
    int adc_val = 0;
    float PWM_Per = 0;
    uint8_t duty_cycle_val;
    char buffer[5]; // Buffer for displaying 0-100 value (max 3 digits + '%' + '\0')
    
    // Display a static label once
    // Using GLCD_GoToPageColumn to explicitly set the cursor before writing the label
    GLCD_GoToPageColumn(0, 2); // Page 0, Column 2
    GLCD_WriteString(0, 2, "PWM Duty Cycle:");
    
    while (1) {
        // --- ADC to PWM Control Loop ---
        
        // Ensure the ADC is still reading Channel 6
         ADC_select_CH(ADC_CH6);
        
        // Start ADC conversion and read 10-bit value
         ADC_SC();
         adc_val = ADC_read();

        // Scale 10-bit value (0-1023) to 8-bit value (0-255) for OCR0
        duty_cycle_val = (uint8_t)(adc_val/4);
        Timer0_SET_COMP_VAL(duty_cycle_val);
        
        // --- GLCD Update Logic ---
        //calculate the PWM percentage
        PWM_Per = (adc_val/1023.0) * 100;
        
        int_to_string((uint8_t)(PWM_Per), buffer);

        
        // Add the '%' symbol to the end of the string
        uint8_t len = 0;
        while (buffer[len] != '\0') {
            len++;
        }
        buffer[len] = '%';
        buffer[len + 1] = '\0';
        
        GLCD_Write_Per(buffer);
        
        //3. Draw the signal
        char Signal_High = round((PWM_Per/100) * 64); 
                
        GLCD_Draw_Signal(Signal_High);
        
   }
    return 0;
}
