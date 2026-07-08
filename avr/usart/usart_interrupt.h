#ifndef _USART_INTERRUPT_H_
#define _USART_INTERRUPT_H_

#include <avr/io.h>

//#define RS485_DRIVE_ENABLE
#ifdef RS485_DRIVE_ENABLE
#define RS485_DE_DDR    DDRD
#define RS485_DE_PORT   PORTD
#define RS485_DE_PIN    PD2
#endif

void InitUART(uint8_t ubrr_val,uint8_t uart_2x);
unsigned char ReceiveByteAvailable(void);
unsigned char ReceiveByte(void);
void TransmitByte(unsigned char data);
void USARTPuts(char *str);

#endif