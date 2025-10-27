/* 
 * File:   DIO.h
 * Author: Mostafa Eshra
 */

#ifndef DIO_H
#define	DIO_H

#ifndef DIO_H_INCLUDED
#define DIO_H_INCLUDED

#define INPUT   0
#define OUTPUT   1
#define INPUT_PORT  0x00
#define OUTPUT_PORT   0xFF

#define LOW      0
#define HIGH     1
#define OFF      0
#define ON     1


void DIO_Set_PORT_DIR(volatile unsigned char *  PORTx, char DIR);
void DIO_Set_PORT_VALUE(volatile unsigned char * PORTx, char Val);
void DIO_Read_PORT(volatile unsigned char* PORTx, unsigned char* val);



void DIO_Set_PIN_DIR(volatile unsigned char * PORTx, char PIN, char DIR);
void DIO_Set_PIN_VALUE(volatile unsigned char* PORTx,char PIN, char Val);
void DIO_Read_PIN(volatile unsigned char * PORTx, char PIN, char* val);
void DIO_Toggle_PIN(volatile unsigned char* PORTx, char PIN);

#endif // DIO_H_INCLUDED

#endif	/* DIO_H */