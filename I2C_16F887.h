/*
 * File:   I2C_16F887.h
 * Author: Adrengineer
 *
 * Created on 1 March 2019, 11:30
 */

#ifndef I2C_16F887_H
#define	I2C_16F887_H

 
    #include "config_LCD1602_I2C_16F887.h"//needed for f_cristal


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */



void I2C_Init(const unsigned long baud);           
void I2C_Wait();                               
void I2C_Start();                              
void I2C_Restart();                            
void I2C_Stop();                                
bit I2C_Write(unsigned char dato );            
char I2C_Write2(unsigned char dato);
bit I2C_WriteByte (unsigned char devAddr, unsigned char dato); 
unsigned short I2C_Read(unsigned short a);                
unsigned char I2C_ReadByte(unsigned char devAddr);
bit I2C_ReadBytes(unsigned char devAddr, unsigned char fRegAddr, unsigned char nBytes, unsigned char* pVar);
void IdleI2C ( void ) ;

bit I2C_Start2(void);
bit I2C_Restart2(void);
bit I2C_Stop2(void);
char I2C_Write2(unsigned char dato );
// Comment a function and leverage automatic documentation with slash star star


/** ****************************************************************************
 * @brief This function initialize the I2C bus with a desirable baud rate
 * @param baud: desirable baud rate
 * @return 
**************************************************************************** **/
void I2C_Init(const unsigned long baud){
   
    TRISCbits.TRISC3 = 1;           //pin SCL as digital input
    TRISCbits.TRISC4 = 1;           //pin SDA as digital input
    
    SSPCON = 0b00101000;            // I2C PIC enable as master mode BaudRate = Fosc/(4*SSPADD+1)
    SSPCON2 = 0;                    // I2C communication started
    SSPADD = (_XTAL_FREQ/(4*baud))-1;  
    SSPSTAT = 0;                    // Standard BaudRate
    
}

/** ****************************************************************************
 * @brief Wait while communication is stated, enable reception, enable stop, 
 *      enable restart, transmitting
 * @param 
 * @return 
**************************************************************************** **/
void I2C_Wait(){
    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}


/** ****************************************************************************
 * @brief Start I2C communication
 * @param 
 * @return 0: whether there is a collision
**************************************************************************** **/
void I2C_Start() {
    I2C_Wait();        
    SSPCON2bits.SEN = 1; //Start communication
    while (SSPCON2bits.SEN);
}
 
bit I2C_Start2(){
    
    SSPCON2bits.SEN = 1; //To initiate a Start condition, the user sets the Start Enable bit, SEN
    __delay_ms(1000);
    if (PIR2bits.BCLIF == 1){ 
        PIR2bits.BCLIF = 0; 
        return 0;
    }
    while (SSPCON2bits.SEN)
        PORTDbits.RD0 = 0 ;
    PIR1bits.SSPIF = 0; 
    return 1;    
    
}


/** ****************************************************************************
 * @brief Restart I2C communication
 * @param 
 * @return 0: whether there is a collision
**************************************************************************** **/
void I2C_Restart(){
    I2C_Wait();         
    SSPCON2bits.RSEN = 1; //Restart I2C communication
    while (SSPCON2bits.RSEN); 
}

bit I2C_Restart2(){
    
    SSPCON2bits.RSEN = 1; 
    if (PIR2bits.BCLIF == 1){ 
        PIR2bits.BCLIF = 0; 
        return 0; 
    }
    while (SSPCON2bits.RSEN); 
    PIR1bits.SSPIF = 0; 
    return 1;    
    
}


/** ****************************************************************************
 * @brief Stop I2C communication
 * @param 
 * @return 0: whether there is a collision
**************************************************************************** **/
void I2C_Stop() {
    I2C_Wait();        
    SSPCON2bits.PEN = 1; 
    while (SSPCON2bits.PEN); 
    IdleI2C ( );            
}

bit I2C_Stop2(){
    
    SSPCON2bits.PEN = 1; 
    if (PIR2bits.BCLIF == 1){ 
        PIR2bits.BCLIF = 0; 
        return 0;
    }
    while (SSPCON2bits.PEN); 
    PIR1bits.SSPIF = 0; 
    return 1;    
    
}
/** ****************************************************************************
 * @brief Send a dato through the I2C
 * @param dato: 8-bits data 
 * @return 0: whether slave get the data
**************************************************************************** **/
bit I2C_Write(unsigned char dato) {
    I2C_Wait();   
    SSPBUF = dato;  
    return  SSPCON2bits.ACKSTAT; 
}
 
char I2C_Write2(unsigned char dato) {
    
    SSPBUF = dato;  
    if (SSPCONbits.WCOL){
        SSPCONbits.WCOL = 0; //WCOL must be cleared by software before the next transmission.
        return 3; // Collision,  I2C? conditions were not valid for a transmission to be started
    }
    while (SSPSTATbits.BF)
        PORTDbits.RD0 = 0; //After the eighth bit is shifted out (the falling edge of the eighth clock),the BF flag is cleared
   
    while (!PIR1bits.SSPIF )
        PORTDbits.RD0 = 0;
    PIR1bits.SSPIF = 0; //The MSSP module generates an interrupt at the end of the ninth clock cycle by setting the SSPIF bit and is cleared by software.
    
    return SSPCON2bits.ACKSTAT;//If the master receives an Acknowledge, the Acknowledge Status bit,ACKSTAT, is cleared. If not, the bit is set
}

/** ****************************************************************************
 * @brief Write just ONE byte in an 8-bit device register.
 * @param devAddr: I2C slave device address
 *        dato: 8-bits data 
 * @return 0: whether slave get the data
**************************************************************************** **/
bit I2C_WriteByte(unsigned char  devAddr, unsigned char dato){
    I2C_Start();
    I2C_Write((devAddr << 1) + 0);
    __delay_us(100);                //necessary for I2C LCD
    I2C_Write(dato);
    __delay_us(100);
    I2C_Stop();
    __delay_us(100);
    return  SSPCON2bits.ACKSTAT;
}

/** ****************************************************************************
 * @brief The master read the data from the slave
 * @param a: to indicate if we are read more data
 * @return data read
***************************************************************************** **/
unsigned short I2C_Read(unsigned short a){
 
    unsigned short datoleido;       
 
    I2C_Wait();                     
    SSPCON2bits.RCEN = 1;           
    
    I2C_Wait();                     
    datoleido = SSPBUF;             
 
    I2C_Wait();                     
 
    SSPCON2bits.ACKDT = (a)?0:1;    
 
    SSPCON2bits.ACKEN = 1;          
    return datoleido;
}
/** ****************************************************************************
 * @brief Read ONE byte from an 8-bit device register.
 * @param devAddr: I2C slave device address
 * @return byte read
***************************************************************************** **/
unsigned char I2C_ReadByte(unsigned char devAddr){
 
    unsigned short datoleido; 
    
    I2C_Start();
    I2C_Write((devAddr ) + 1);
        
    I2C_Wait();         
    SSPCON2bits.RCEN = 1; 
    
    I2C_Wait();         
    datoleido = SSPBUF;   
 
    I2C_Wait();        
 
    SSPCON2bits.ACKDT = 1;  
 
    SSPCON2bits.ACKEN = 1;
    I2C_Stop();
    return datoleido;
}

/** ****************************************************************************
* @Brief: Read multiple bytes from an 8-bit device register.
 * @param devAddr: I2C slave device address
 *      fRegAddr: First register to read from
 *      nBytes: Number of bytes to read
 *      pVar: Pointer to variable where read data will be stored in
 *      statOp: Status of operation, true if every ACK was received
 * @Return:
***************************************************************************** **/
bit I2C_ReadBytes(unsigned char devAddr, unsigned char fRegAddr, unsigned char nBytes, unsigned char* pVar) {
}


void IdleI2C ( void ){
    while ( ( SSPCON2 & 0x1F ) | ( SSPSTATbits.R_W ) )
            continue;
}
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */
