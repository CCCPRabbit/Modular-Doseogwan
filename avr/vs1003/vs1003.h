#ifndef _VS1003_H
#define _VS1003_H

#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

// 핀 정의
#define VS_DREQ_PORT    PORTD
#define VS_DREQ_PIN_REG PIND
#define VS_DREQ_PIN     PD2
#define VS_XDCS_DDR     DDRB
#define VS_XDCS_PORT    PORTB
#define VS_XDCS_PIN     PB1
#define VS_XCS_DDR      DDRB
#define VS_XCS_PORT     PORTB
#define VS_XCS_PIN      PB2
#define VS_RESET_DDR    DDRB
#define VS_RESET_PORT   PORTB
#define VS_RESET_PIN    PB0

// VS1003 명령어
#define VS_WRITE_CMD 0x02
#define VS_READ_CMD  0x03

// VS1003 레지스터 주소
#define SCI_MODE        0x00
#define SCI_STATUS      0x01
#define SCI_BASS        0x02
#define SCI_CLOCKF      0x03
#define SCI_DECODE_TIME 0x04
#define SCI_AUDATA      0x05
#define SCI_WRAM        0x06
#define SCI_WRAMADDR    0x07
#define SCI_HDAT0       0x08
#define SCI_HDAT1       0x09
#define SCI_AIADDR      0x0A
#define SCI_VOL         0x0B

//SCI_MODE_BIT
#define SM_OUTOFWAV 0x0008

// 함수 원형
void VS1003_Init(void);
void VS1003_WriteRegister(uint8_t address, uint16_t value);
uint16_t VS1003_ReadRegister(uint8_t address);
void VS1003_SendMusicData(uint8_t *data, uint8_t length);
void VS1003_SetVolume(uint8_t left, uint8_t right);

#endif // _VS1003_H