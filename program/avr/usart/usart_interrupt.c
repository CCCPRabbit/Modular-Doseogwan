/**
 * @file usart_interrupt.c
 * @author CCCPRabbit (you@domain.com)
 * @brief 
 * @version 1.0
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_interrupt.h"

/* UART Buffer Defines */
#define UART_RX_BUFFER_SIZE 64     /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_SIZE 16
#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)

#if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)
#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif

/* Static Variables */
static unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART_RxHead;
static volatile unsigned char UART_RxTail;
static unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;

/* Prototypes */
void InitUART(unsigned char ubrr_val);
unsigned char ReceiveByte(void);
void TransmitByte(unsigned char data);

/* Initialize UART */
void InitUART(unsigned char ubrr_val)
{
	#ifdef RS485_DRIVE_ENABLE
	RS485_DE_DDR|=(1<<RS485_DE_PIN);
	#endif
	/* Set the baud rate */
	UBRR0H = (unsigned char)(ubrr_val>>8);
	UBRR0L = (unsigned char)ubrr_val;
	/* Enable UART receiver and transmitter */
	UCSR0B = ((1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0) |(1<<TXCIE0));

	/* Flush buffer */
	UART_RxTail = 0;
	UART_RxHead = 0;
	UART_TxTail = 0;
	UART_TxHead = 0;
}

/* Interrupt handlers */
ISR(USART_RX_vect)
{
	unsigned char data;
	unsigned char tmphead;
	
	/* Read the received data */
	data = UDR0;                 
	/* Calculate buffer index */
	tmphead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxHead = tmphead;      

	if (tmphead == UART_RxTail) {
		/* ERROR! Receive buffer overflow */
	}
	/* Store received data in buffer */
	UART_RxBuf[tmphead] = data;
}

ISR(USART_UDRE_vect)
{
	unsigned char tmptail;

	/* Check if all data is transmitted */
	if (UART_TxHead != UART_TxTail) {
		/* Calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		/* Store new index */
		UART_TxTail = tmptail;
		//TODO add RS485 drive enable
		/* Start transmission */
		UDR0 = UART_TxBuf[tmptail];
	} else {       
		/* Disable UDRE interrupt */
		UCSR0B &= ~(1<<UDRIE0);
	}
}

ISR(USART_TX_vect)
{
	#ifdef RS485_DRIVE_ENABLE
	RS485_DE_PORT&=~(1<<RS485_DE_PIN);
	#endif
}

unsigned char ReceiveByteAvailable(void)
{
	return (UART_RxHead-UART_RxTail)&UART_RX_BUFFER_MASK;
}

unsigned char ReceiveByte(void)
{
	unsigned char tmptail;
	
	/* Wait for incoming data */
	if (UART_RxHead == UART_RxTail)	return 0;
	/* Calculate buffer index */
	tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxTail = tmptail;
	             
	/* Return data */
	return UART_RxBuf[tmptail];
}

void TransmitByte(unsigned char data)
{
	unsigned char tmphead;
	
	/* Calculate buffer index */
	tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
	/* Wait for free space in buffer */
	while (tmphead == UART_TxTail);
	/* Store data in buffer */
	UART_TxBuf[tmphead] = data;
	/* Store new index */
	UART_TxHead = tmphead;
	#ifdef RS485_DRIVE_ENABLE
	RS485_DE_PORT|=(1<<RS485_DE_PIN);
	#endif
	UCSR0B |= (1<<UDRIE0);
}
