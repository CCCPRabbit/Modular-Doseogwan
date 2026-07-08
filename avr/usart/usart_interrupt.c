/**
 * @file usart_interrupt.c
 * @author CCCPRabbit
 * @date 2026-06-25
 * @brief usart function
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
static uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;
static uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;

/* Prototypes */
void InitUART(uint8_t ubrr_val,uint8_t uart_2x);
uint8_t ReceiveByte(void);
void TransmitByte(uint8_t data);

/* Initialize UART */
void InitUART(uint8_t ubrr_val,uint8_t uart_2x)
{
	#ifdef RS485_DRIVE_ENABLE
	RS485_DE_DDR|=(1<<RS485_DE_PIN);
	#endif
	/* Set the baud rate */
	UBRR0H = (uint8_t)(ubrr_val>>8);
	UBRR0L = (uint8_t)ubrr_val;
	UCSR0A = (uart_2x<<U2X0);
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
	uint8_t data;
	uint8_t tmphead;
	
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
	uint8_t tmptail;

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

uint8_t ReceiveByteAvailable(void)
{
	return (UART_RxHead-UART_RxTail)&UART_RX_BUFFER_MASK;
}

uint8_t ReceiveByte(void)
{
	uint8_t tmptail;
	
	/* Wait for incoming data */
	if (UART_RxHead == UART_RxTail)	return 0;
	/* Calculate buffer index */
	tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxTail = tmptail;
	             
	/* Return data */
	return UART_RxBuf[tmptail];
}

void TransmitByte(uint8_t data)
{
	uint8_t tmphead;
	
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

void USARTPuts(char *str)
{
	uint16_t i=0;
	while(str[i]!=0)
	{
		TransmitByte(str[i]);
		i++;
	}
}