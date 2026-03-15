void SPIInit(void);
unsigned char SPITransmitByte(unsigned char data);
void SPITransmitMultiByte(unsigned char *tx_data,uint8_t *rx_data,uint8_t length);