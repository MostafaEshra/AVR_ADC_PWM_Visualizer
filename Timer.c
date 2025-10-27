/* 
 * File:   Timer.c
 * Author: Mostafa Eshra
 */

#include <avr/io.h>
#include "Timer.h"

#include "DIO.h"

void init_Timer0(char TIMER_MODE, char TIMER_CLOCK_SOURCE) {

    // TCCR0
    switch (TIMER_MODE) {
        case TIMER0_MODE_NORMAL:
            TCCR0 &= ~((1 << WGM01) | (1 << WGM00));
            break;
        case TIMER0_MODE_PWM:
            TCCR0 &= ~((1 << WGM01) | (1 << WGM00));
            TCCR0 |= (1 << WGM00);
            break;
        case TIMER0_MODE_CTC:
            TCCR0 &= ~((1 << WGM01) | (1 << WGM00));
            TCCR0 |= (1 << WGM01);
            break;
        case TIMER0_MODE_FPWM:
            TCCR0 |= ((1 << WGM01) | (1 << WGM00));
            break;
    }
    TCCR0 |= TIMER_CLOCK_SOURCE;
}

void Timer0_INT_ENABLE(char TIMER_INT) {
    switch (TIMER_INT) {
        case TIMER0_INT_TOV:
            TIMSK |= (1 << TOIE0);
            break;
        case TIMER0_INT_OCF:
            TIMSK |= (1 << OCIE0);
            break;
    }
}

void Timer0_SET_COMP_VAL(char TIMER_COMP_VAL) {
    OCR0 = TIMER_COMP_VAL;
}

void Timer0_COMP_MODE(char TIMER0_COMP_MODE){
    switch(TIMER0_COMP_MODE){
        case TIMER0_COMP_MODE_CTC_TOGGLE:
            DIO_Set_PIN_DIR(&PORTB, PB3, OUTPUT);
            TCCR0 |= (1<<COM00);
            break;
        case TIMER0_COMP_MODE_PWM_SET_ON_COUNT_UP:
            DIO_Set_PIN_DIR(&PORTB, PB3, OUTPUT);
            TCCR0 |= (1 << COM01);
            TCCR0 &= ~(1 << COM00);
            break;
    }
}