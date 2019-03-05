/*
 * File:   LCD1602_I2C_16F887main.c
 * Author: Adrengineer
 *
 * Created on 1 March 2019, 11:30
 */


#include <stdio.h>
#include "I2C_16F887.h"
#include "config_LCD1602_I2C_16F887.h"
#include "LCD1602_I2C_16F887.h"

#define devAddr     0x27 ////0b0100A2A1A0x --> A2A1A0=111

int t;

void main(void) {

    LCD_Init(100000);
    LCD_Command(CURSOR_ON_BLINK);

    LCD_Putc(0x68);
    LCD_Putc(0x68);
    LCD_Command(CURSOR_RIGHT );
    LCD_Print("ADRENGINEER");

    __delay_ms(200);

    while(1){
        LCD_Command(LCD_SECOND_ROW);
        printf("ADRENGINEER %d",t);
        t++;
        __delay_ms(200);
    }
  
}

