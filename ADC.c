/* 
 * File:   ADC.c
 * Author: Mostafa Eshra
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "ADC.h"


void init_ADC(char ADC_CH, char ADC_REF, char ADC_PRE){

    ADC_select_CH(ADC_CH);
    ADC_select_REF(ADC_REF);
    ADC_select_PRE(ADC_PRE);

    // Enable ADC NOW
    ADC_EN();
}
void ADC_select_CH(char ADC_CH){
    ADMUX &= 0xE0;  // xxx00000
    ADMUX |= ADC_CH;
}
void ADC_select_REF(char ADC_REF){
    // AREF, AVCC, Internal_2.56
    ADMUX &= ~((1<<REFS1)|(1<<REFS0));
    ADMUX |= (ADC_REF<<6);

}
void ADC_select_PRE(char ADC_PRE){
    ADCSRA &= 0xF8; // xxxxx000
    ADCSRA |= ADC_PRE;
}
void ADC_EN(){
    ADCSRA |= (1<<ADEN);
}
void ADC_SC(){
    ADCSRA |= (1<<ADSC);
}
int  ADC_read(){
    // Pooling on Flag
    while(!(ADCSRA & (1<<ADIF)));
    return ADCW;
}