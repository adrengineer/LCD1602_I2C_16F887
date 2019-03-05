/*
 * File:   LCD1602_I2C_16F887.h
 * Author: Adrengineer
 *
 * Created on 1 March 2019, 11:30
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD1602_I2C_16F887_H
#define	LCD1602_I2C_16F887_H

// TODO Insert appropriate #include <>
    #include "config_LCD1602_I2C_16F887.h"// include processor files - each processor file is guarded. 
    #include "I2C_16F887.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    #define     LCD_FIRST_ROW               0x80
    #define     LCD_SECOND_ROW              0xC0
  
    #define     LCD_TURN_OFF            0x00 //0
    #define     LCD_TURN_ON             0x08 //8

    #define     CLEAR                   0x01 //1.53 ms
    #define     RETURN_HOME             0x02 //1.53 ms

    #define     DISPLAY_OFF             0x08 //Display off, but data remain in DDRAM
    #define     CURSOR_OFF              0x0C //Display on, but cursor off
    #define     CURSOR_ON               0x0E //Display on, Cursor On
    #define     CURSOR_ON_BLINK         0x0F //Display on, Cursor On Blinking

    #define     CURSOR_LEFT             0x10 //Shift cursor to left and decrease
    #define     CURSOR_RIGHT            0x14 //Shift cursor to right and increase
    #define     SHIFT_LEFT              0x18 //Shift all LCD to left
    #define     SHIFT_RIGHT             0x1C //Shift all LCD to right

    #define     _8b_1L_5x8              0x30 //8 bits mode, 1 line, 5x8 dots
    #define     _8b_2L_5x8              0x38 //8 bits mode, 2 line, 5x8 dots 
    #define     _8b_1L_5x11             0x34 //8 bits mode, 1 line, 5x8 dots 
    #define     _8b_2L_5x11             0x3C //8 bits mode, 2 line, 5x8 dots 
    #define     _4b_1L_5x8              0x20 //4 bits mode, 1 line, 5x11 dots 
    #define     _4b_2L_5x8              0x28 //4 bits mode, 2 line, 5x11 dots 
    #define     _4b_1L_5x11             0x24 //4 bits mode, 1 line, 5x11 dots 
    #define     _4b_2L_5x11             0x2C //4 bits mode, 2 line, 5x11 dots 

    #define LCD_BL 0x08
    #define LCD_EN 0x04
    #define LCD_RW 0x02
    #define LCD_RS 0x01

    

    #define devAddr     0x27 ////0b0100A2A1A0x --> A2A1A0=111

// TODO Insert declarations
    void LCD_Init(const unsigned long speed);
    void LCD_BusI2C(char dato);
    
    void LCD_Parameter(char data);
    void LCD_Command(char cmd);
    void LCD_Gotoxy( unsigned char x, unsigned char y);
    void LCD_Putc( unsigned char c);
    void LCD_Print(unsigned char *dato);
    char LCD_Getc( unsigned char x, unsigned char y);//This function is not checked
    char lcd_read_byte();//This function is not checked
    void putch(char p);
    
    
    char lcdline;

/** ****************************************************************************
 * @Brief: This function initilaizes ports and lcd in mode 4 bits
 * @Param speed: baud rate 
 * @Returm
 * ************************************************************************* **/
void LCD_Init(const unsigned long speed){
      
    I2C_Init(speed); //Configuration of MSSP as I2C communication
    __delay_ms(10);
    if(!I2C_Start2()) I2C_Restart2();
    I2C_Write2((devAddr << 1) + 0);
    
    LCD_BusI2C (0x00);//lcd_rs = 0; lcd_rw = 0; lcd_enable = 0;        
    
    __delay_ms(15);      
    
    LCD_Command(0x30);    
    __delay_ms(5);        
    LCD_Command(0x30);
    __delay_us(100);
    LCD_Command(0x30);
    __delay_us(40);
    
    LCD_Command(0x02);              
    __delay_us(40);

    LCD_Command(_4b_2L_5x8);      
    __delay_us(40);

    LCD_Command(0x01);      
    __delay_ms(2);
    LCD_Command(0x0C);      
    __delay_us(40);
    LCD_Command(0x06);    
    __delay_us(40);
      
}

/** ****************************************************************************
 * @Brief: This function writes data in the LCD I2C bus
 * @Param dato: data to write
 * @Returm
 * ************************************************************************* **/
void LCD_BusI2C(unsigned char dato){ 
    
    I2C_Write2(dato);
}


/** ****************************************************************************
 * @Brief: This function writes a command in the LCD I2C bus
 * @Param cmd: command to write
 * @Returm
 * ************************************************************************* **/
void LCD_Command(unsigned char cmd){  
     unsigned char lcddata; 
 
    // Write high nibble
     lcddata = (cmd & 0xF0) |LCD_BL;

        //1.- RS=0; W/R=0
         LCD_BusI2C (lcddata);
        //2.-After Tas=40ns (Address Set-up time) activated enable
         __delay_us (10);
         LCD_BusI2C (lcddata | LCD_EN);
        //3.- Minimum enable pulse with (230ns)
         __delay_us(100);
        //4.- Desactivate enable and wait Tah=10ns(Address hold time)
         LCD_BusI2C (lcddata & ~LCD_EN); // Reset LCD bus
         __delay_us(100);
         
    // Write low nibble
     lcddata = (cmd <<4) |LCD_BL;
     
        //1.- RS=0; W/R=0
         LCD_BusI2C (lcddata);
        //2.-After Tas=40ns (Address Set-up time) activated enable
         __delay_us (10);
         LCD_BusI2C (lcddata | LCD_EN);
        //3.- Minimum enable pulse with (230ns)
         __delay_us(100);
        //4.-Desactivate enable and wait Tah=10ns(Address hold time)
         LCD_BusI2C (lcddata & ~LCD_EN); // Reset LCD bus
         __delay_us(100);
 }

/** ****************************************************************************
 * @Brief: This function writes a parameter in the bus
 * @Param data: parameter to write
 * @Returm
 * ************************************************************************* **/
void LCD_Parameter(unsigned char data){
     unsigned char lcddata;
      
    // Write high nibble
     lcddata = (data & 0xF0)  |LCD_BL|LCD_RS; // Get high nibble; and 0xF0 for
                                              // deleted low nibble
        //1.- RS=1; W/R=0
         LCD_BusI2C (lcddata);
        //2.-After Tas=40ns (Address Set-up time) activated enable
         __delay_us (10);
         LCD_BusI2C (lcddata | LCD_EN);
        //3.- Minimum enable pulse with (230ns)
         __delay_us(100);
        //4.-Desactivated enable and wait Tah=10ns(Address hold time)
         LCD_BusI2C (lcddata & ~LCD_EN); // Reset LCD bus
         __delay_us(100);
 
    // Write low nibble
     lcddata = (data <<4) |LCD_BL|LCD_RS; // Get low nibble
     
        //1.- RS=1; W/R=0
         LCD_BusI2C (lcddata);
        //2.-After Tas=40ns (Address Set-up time) activated enable
         __delay_us (10);
         LCD_BusI2C (lcddata | LCD_EN);
        //3.- Minimum enable pulse with (230ns)
         __delay_us(100);
        //4.-Desactivated enable and wait Tah=10ns(Address hold time)
         LCD_BusI2C (lcddata & ~LCD_EN); // Reset LCD bus
         __delay_us(100);
}

/** ****************************************************************************
 * @Brief: This function sets the pointer a specific location
 * @Param x: x location
 *        y: y location
 * @Returm
 * ************************************************************************* **/
void LCD_Gotoxy( unsigned char x, unsigned char y) {
    char address;

    switch(y) {
        case 1 : address=0x80;break;
        case 2 : address=0xc0;break;
        case 3 : address=0x94;break;
        case 4 : address=0xd4;break;
    }
    address+=x-1;
    LCD_Command(address);
}

/** ****************************************************************************
 * @Brief: This function sends a character to LCD. will display c on the next position 
 *      of the LCD. The following have special meaning:            
 *      \f  Clear display                             
 *      \n  Go to start of second line              
 *      \b  Move back one position 
 * @Param c: character to send
 * @Returm
 * ************************************************************************* **/
void LCD_Putc(char c) {
   switch (c) {
     case '\f': LCD_Command(1);
        lcdline=1;
        __delay_ms(2);
        break;
     case '\n': LCD_Gotoxy(1,++lcdline);
        break;
     case '\b': LCD_Command(0x10);       
        break;
     default: LCD_Parameter(c);        
        break;
   }
}

/** ****************************************************************************
 * @Brief: This function print a string
 * @Param *dato: string to print on the LCD
 * @Returm
 * ************************************************************************* **/
void LCD_Print(unsigned char *dato){
    while (*dato) // Whilst it's not null
    {
        LCD_Parameter(*dato); 
        dato++; 
    }
}

/** ****************************************************************************
 * @Brief:  WITH THIS FUNCTION AND STDIO LIBRARY IS POSIBLE USE PRINTF
 * @Param p: parameter to write
 * @Returm
 * ************************************************************************* **/
void putch(char p){
    
    LCD_Parameter(p);
}


/** ****************************************************************************
 * @Brief: This function get character from LCD memory
 * @Param x: x location of the desirable character
 *        y: y location of the desirable character
 * @Returm the desirable character
 * ************************************************************************* **/
//next functions are not checked
char LCD_Getc( unsigned char x, unsigned char y) {
   char value;

    /*LCD_Gotoxy(x,y);
    lcd_rs=1;
    value = lcd_read_byte();
    lcd_rs=0;
    return(value);*/
}
//next functions are not checked
char lcd_read_byte() {
      char low,high;

      /*TRISD = LCD_READ;
      lcd_rw = 1;
      Delay1TCYx(1);
      lcd_enable = 1;
      Delay1TCYx(1);
      high = lcd_data;
      lcd_enable = 0;
      Delay1TCYx(1);
      lcd_enable = 1;
      __delay_us(1);
      low = lcd_data;
      lcd_enable = 0;
      TRISD=LCD_WRITE;
      return( (high<<4) | low);*/
}
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */
