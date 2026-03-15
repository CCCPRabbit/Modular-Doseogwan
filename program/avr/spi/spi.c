#include <avr/io.h>

#define SCK_DDR     DDRB
#define SCK_PIN     PB5
#define MOSI_DDR    DDRB
#define MOSI_PIN    PB3
#define MISO_DDR    DDRB
#define MISO_PIN    PB4
#define CS_DDR      DDRB
#define CS_PORT     PORTB
#define CS_PIN      PB2

void SPIInit(void)
{
    CS_PORT |= (1 << CS_PIN);
    SCK_DDR |= (1 << SCK_PIN);
    MOSI_DDR |= (1 << MOSI_PIN);
    CS_DDR |= (1 << CS_PIN);
    MISO_DDR &= ~(1 << MISO_PIN);
    
    // SPI 설정: Master, MSB first, Mode 0, F_CPU/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    SPSR = 0;
}

uint8_t SPITransmitByte(uint8_t data)
{
    SPDR=data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void SPITransmitMultiByte(uint8_t *tx_data,uint8_t *rx_data,uint8_t length)
{
    for(uint8_t i=0;i<length;i++)
    {
        SPDR=tx_data[i];
        while (!(SPSR & (1 << SPIF)));
        rx_data[i]=SPDR;
    }
}