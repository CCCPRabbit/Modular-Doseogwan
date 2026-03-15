#ifndef _USART_INTERRUPT_H_
#define _USART_INTERRUPT_H_

//#define RS485_DRIVE_ENABLE
#ifdef RS485_DRIVE_ENABLE
#define RS485_DE_DDR    DDRD
#define RS485_DE_PORT   PORTD
#define RS485_DE_PIN    PD2
#endif

void InitUART(unsigned char ubrr_val);
unsigned char ReceiveByteAvailable(void);
unsigned char ReceiveByte(void);
void TransmitByte(unsigned char data);

#endif