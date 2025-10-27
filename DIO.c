/* 
 * File:   DIO.c
 * Author: Mostafa Eshra
 */

#include <avr/io.h>
#include "DIO.h"

void DIO_Set_PORT_DIR(volatile unsigned char *  PORTx, char DIR){
    *(PORTx-1) = DIR;
}
void DIO_Set_PORT_VALUE(volatile unsigned char * PORTx, char Val){
    *PORTx = Val;
}

void DIO_Read_PORT(volatile unsigned char* PORTx, unsigned char* val){
    *val = *(PORTx-2);
}


void DIO_Set_PIN_DIR(volatile unsigned char * PORTx, char PIN, char DIR){
    PORTx--;
    switch(DIR){
        case INPUT:
            *PORTx &= ~(1<<PIN);
            break;
        case OUTPUT:
            *PORTx |= (1<<PIN);
            break;
    }
}


void DIO_Set_PIN_VALUE(volatile unsigned char* PORTx,char PIN, char Val){

    switch(Val){
        case HIGH:
            *PORTx |= (1<<PIN);
            break;
        case LOW:
            *PORTx &= ~(1<<PIN);
            break;
    }
}


void DIO_Read_PIN(volatile unsigned char * PORTx, char PIN, char*val){
    PORTx-=2;

    if(*PORTx & (1<<PIN)){
        *val = 1;
    }
    else{
        *val = 0;
    }

    *val = (*PORTx & (1<<PIN))? HIGH:LOW;
}


void DIO_Toggle_PIN(volatile unsigned char* PORTx, char PIN){
    *PORTx ^= (1<<PIN);
}