/**
 * @file spi.h
 * @author CCCPRabbit
 * @date 2026-06-25
 * @brief spi lib header
 */
#ifndef _SPI_H
#define _SPI_H

#include <avr/io.h>

#define SPI_SET_BAUDRATE(TARGET_FREQ)   do { \
    if ((F_CPU) / 2 <= (TARGET_FREQ)) { \
        /* 분주비 2: SPI2X=1, SPR1=0, SPR0=0 */ \
        SPCR &= ~((1 << SPR1) | (1 << SPR0)); \
        SPSR |= (1 << SPI2X); \
    } else if ((F_CPU) / 4 <= (TARGET_FREQ)) { \
        /* 분주비 4: SPI2X=0, SPR1=0, SPR0=0 */ \
        SPCR &= ~((1 << SPR1) | (1 << SPR0)); \
        SPSR &= ~(1 << SPI2X); \
    } else if ((F_CPU) / 8 <= (TARGET_FREQ)) { \
        /* 분주비 8: SPI2X=1, SPR1=0, SPR0=1 */ \
        SPCR &= ~(1 << SPR1); \
        SPCR |= (1 << SPR0); \
        SPSR |= (1 << SPI2X); \
    } else if ((F_CPU) / 16 <= (TARGET_FREQ)) { \
        /* 분주비 16: SPI2X=0, SPR1=0, SPR0=1 */ \
        SPCR &= ~(1 << SPR1); \
        SPCR |= (1 << SPR0); \
        SPSR &= ~(1 << SPI2X); \
    } else if ((F_CPU) / 32 <= (TARGET_FREQ)) { \
        /* 분주비 32: SPI2X=1, SPR1=1, SPR0=0 */ \
        SPCR |= (1 << SPR1); \
        SPCR &= ~(1 << SPR0); \
        SPSR |= (1 << SPI2X); \
    } else if ((F_CPU) / 64 <= (TARGET_FREQ)) { \
        /* 분주비 64: SPI2X=0, SPR1=1, SPR0=0 */ \
        SPCR |= (1 << SPR1); \
        SPCR &= ~(1 << SPR0); \
        SPSR &= ~(1 << SPI2X); \
    } else { \
        /* 분주비 128 (최대 분주): SPI2X=0, SPR1=1, SPR0=1 */ \
        SPCR |= (1 << SPR1) | (1 << SPR0); \
        SPSR &= ~(1 << SPI2X); \
    } \
} while(0)

void SPIInit(void);
uint8_t SPITransmitByte(uint8_t data);
void SPITransmit(const uint8_t *tx_data,uint16_t length);
void SPIReceive(uint8_t *rx_data,uint16_t length,uint8_t dummy);
void SPITransmitReceive(uint8_t *tx_data,uint8_t *rx_data,uint16_t length);

#endif