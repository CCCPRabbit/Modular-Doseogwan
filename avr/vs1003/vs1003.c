#include "vs1003.h"

// SCI (제어 레지스터) 쓰기
void VS1003_WriteRegister(uint8_t address, uint16_t value) {
    while (!(VS_DREQ_PIN_REG & (1 << VS_DREQ_PIN))); // DREQ가 High가 될 때까지 대기
    
    VS_XCS_PORT &= ~(1 << VS_XCS_PIN); // XCS Low (제어 명령 시작)
    
    SPITransmitByte(VS_WRITE_CMD);
    SPITransmitByte(address);
    SPITransmitByte((value >> 8) & 0xFF); // High Byte
    SPITransmitByte(value & 0xFF);        // Low Byte
    
    VS_XCS_PORT |= (1 << VS_XCS_PIN); // XCS High (제어 명령 종료)
}

// SCI (제어 레지스터) 읽기
uint16_t VS1003_ReadRegister(uint8_t address) {
    uint16_t result = 0;
    
    while (!(VS_DREQ_PIN_REG & (1 << VS_DREQ_PIN))); 
    
    VS_XCS_PORT &= ~(1 << VS_XCS_PIN); 
    
    SPITransmitByte(VS_READ_CMD);
    SPITransmitByte(address);
    result = SPITransmitByte(0xFF) << 8; // High Byte 수신
    result |= SPITransmitByte(0xFF);     // Low Byte 수신
    
    VS_XCS_PORT |= (1 << VS_XCS_PIN); 
    
    return result;
}

// VS1003 초기화 시퀀스
void VS1003_Init(void) {
    VS_XCS_PORT |= (1<<VS_XCS_PIN);
    VS_XCS_DDR |= (1<<VS_XCS_PIN);
    VS_XDCS_PORT |= (1<<VS_XDCS_PIN);
    VS_XDCS_DDR |= (1<<VS_XDCS_PIN);
    VS_RESET_DDR |= (1<<VS_RESET_PIN);
    // 하드웨어 리셋
    VS_RESET_PORT &= ~(1 << VS_RESET_PIN);
    _delay_ms(10);
    VS_RESET_PORT |= (1 << VS_RESET_PIN);
    _delay_ms(10);
    
    // 더미 바이트 전송으로 동기화
    SPITransmitByte(0xFF);

    VS1003_WriteRegister(SCI_AUDATA,44101); // 44.1kHz stereo
    
    // 소프트웨어 리셋 및 SM_SDINEW 모드 설정
    VS1003_WriteRegister(SCI_MODE, 0x0804);
    _delay_ms(10);
    
    // 클럭 배수기 설정 (내부 클럭 증폭)
    VS1003_WriteRegister(SCI_CLOCKF, 0xB800);
    _delay_ms(10);
    
    // 볼륨 설정 (0x0000이 최대음량, 0xFEFE가 음소거)
    VS1003_SetVolume(0, 0);
}

// SDI (오디오 데이터) 전송 - 일반적으로 32바이트씩 전송합니다.
void VS1003_SendMusicData(uint8_t *data, uint8_t length) {
    while (!(VS_DREQ_PIN_REG & (1 << VS_DREQ_PIN))); // DREQ 확인
    
    VS_XDCS_PORT &= ~(1 << VS_XDCS_PIN); // XDCS Low (데이터 전송 시작)
    
    for (uint8_t i = 0; i < length; i++) {
        SPITransmitByte(data[i]);
    }
    
    VS_XDCS_PORT |= (1 << VS_XDCS_PIN); // XDCS High
}

// 볼륨 설정
void VS1003_SetVolume(uint8_t left, uint8_t right) {
    uint16_t vol = (left << 8) | right;
    VS1003_WriteRegister(SCI_VOL, vol);
}