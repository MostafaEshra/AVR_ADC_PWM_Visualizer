/* 
 * File:   ADC.h
 * Author: Mostafa Eshra
 */

#ifndef ADC_H
#define	ADC_H

#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#define ADC_REF_AREF      0
#define ADC_REF_AVCC      1
#define ADC_REF_2_56V     3

#define ADC_CH0       0
#define ADC_CH1       1
#define ADC_CH2       2
#define ADC_CH3       3
#define ADC_CH4       4
#define ADC_CH5       5
#define ADC_CH6       6
#define ADC_CH7       7

#define ADC_CH0_CH1_X1   16


#define ADC_PRE_2     1
#define ADC_PRE_4     2
#define ADC_PRE_8     3
#define ADC_PRE_16    4
#define ADC_PRE_32    5
#define ADC_PRE_64    6
#define ADC_PRE_128   7


void init_ADC(char ADC_CH, char ADC_REF, char ADC_PRE);
void ADC_select_CH(char ADC_CH);
void ADC_select_REF(char ADC_REF);
void ADC_EN();
void ADC_select_PRE(char ADC_PRE);
void ADC_SC();
int  ADC_read();

#endif // ADC_H_INCLUDED


#endif	/* ADC_H */