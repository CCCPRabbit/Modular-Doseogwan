/**
 * @file nrf24l01p.c
 * @author CCCPRabbit (you@domain.com)
 * @brief 
 * @version 1.0
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "nrf24l01p.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "spi.h"

/* ==================== Internal Function Prototypes ==================== */

static void nrf24_write_command(uint8_t cmd);

/* ==================== SPI Related Functions ==================== */

/**
 * @brief Send command (no data)
 * @param cmd Command
 */
static void nrf24_write_command(uint8_t cmd) {
    NRF24_CSN_LOW();
    SPITransmitByte(cmd);
    NRF24_CSN_HIGH();
}

/* ==================== Register Access Functions ==================== */

/**
 * @brief Read register
 */
uint8_t nrf24_read_register(uint8_t reg) {
    uint8_t value;
    
    NRF24_CSN_LOW();
    SPITransmitByte(NRF24_CMD_R_REGISTER | (reg & 0x1F));
    value = SPITransmitByte(0xFF);
    NRF24_CSN_HIGH();
    
    return value;
}

/**
 * @brief Write register
 */
void nrf24_write_register(uint8_t reg, uint8_t value) {
    NRF24_CSN_LOW();
    SPITransmitByte(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    SPITransmitByte(value);
    NRF24_CSN_HIGH();
}

/**
 * @brief Read multi-byte register
 */
void nrf24_read_register_multi(uint8_t reg, uint8_t *buffer, uint8_t length) {
    uint8_t tx_buffer[5]={0xff,};
    NRF24_CSN_LOW();
    SPITransmitByte(NRF24_CMD_R_REGISTER | (reg & 0x1F));
    SPITransmitMultiByte(tx_buffer,buffer,length);
    NRF24_CSN_HIGH();
}

/**
 * @brief Write multi-byte register
 */
void nrf24_write_register_multi(uint8_t reg, const uint8_t *buffer, uint8_t length) {
    uint8_t rx_buffer[5];
    NRF24_CSN_LOW();
    SPITransmitByte(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    SPITransmitMultiByte(buffer,rx_buffer,length);
    NRF24_CSN_HIGH();
}

/* ==================== Initialization and Configuration Functions ==================== */

/**
 * @brief Initialize nRF24L01P
 */
bool nrf24_init(void) {
    // Wait for module stabilization (minimum 100ms)
    _delay_ms(100);
    
    // Start in Power Down mode
    nrf24_power_down();
    
    // Default configuration
    nrf24_write_register(NRF24_REG_CONFIG, 0x08);  // Enable CRC (1 byte), Power Down
    nrf24_write_register(NRF24_REG_EN_AA, 0x3F);   // Enable Auto ACK for all pipes
    nrf24_write_register(NRF24_REG_EN_RXADDR, 0x03); // Enable pipe 0, 1
    nrf24_write_register(NRF24_REG_SETUP_AW, 0x03);  // 5-byte address
    nrf24_write_register(NRF24_REG_SETUP_RETR, 0x5F); // 1500us delay, 15 retries
    nrf24_write_register(NRF24_REG_RF_CH, 76);       // Channel 76 (2476 MHz)
    nrf24_write_register(NRF24_REG_RF_SETUP, 0x0E);  // 2Mbps, 0dBm
    
    // Set default address (5 bytes)
    uint8_t default_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nrf24_set_tx_address(default_addr, 5);
    nrf24_set_rx_address(0, default_addr, 5);
    nrf24_set_rx_address(1, default_addr, 5);
    
    // Set payload size (32 bytes)
    nrf24_set_payload_size(0, 32);
    nrf24_set_payload_size(1, 32);
    
    // Clear STATUS register
    nrf24_clear_status(0x70);
    
    // Flush FIFOs
    nrf24_flush_tx();
    nrf24_flush_rx();
    
    // Power Up
    nrf24_power_up();
    
    // Test communication
    return nrf24_test_connection();
}

/**
 * @brief Test communication
 */
bool nrf24_test_connection(void) {
    uint8_t test_addr[5] = {0x12, 0x34, 0x56, 0x78, 0x9A};
    uint8_t read_addr[5];
    
    // Write test address
    nrf24_write_register_multi(NRF24_REG_TX_ADDR, test_addr, 5);
    
    // Read address
    nrf24_read_register_multi(NRF24_REG_TX_ADDR, read_addr, 5);
    
    // Compare
    for (uint8_t i = 0; i < 5; i++) {
        if (test_addr[i] != read_addr[i]) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Reset all states
 */
void nrf24_reset(void) {
    nrf24_power_down();
    _delay_ms(10);
    nrf24_init();
}

/* ==================== Mode and Configuration Functions ==================== */

/**
 * @brief Set operation mode
 */
void nrf24_set_mode(nrf24_mode_t mode) {
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    
    if (mode == NRF24_MODE_RX) {
        config |= (1 << NRF24_CONFIG_PRIM_RX);
        nrf24_write_register(NRF24_REG_CONFIG, config);
        NRF24_CE_HIGH();  // Activate RX mode
        _delay_us(130);   // Wait for RX settling
    } else {
        config &= ~(1 << NRF24_CONFIG_PRIM_RX);
        nrf24_write_register(NRF24_REG_CONFIG, config);
        NRF24_CE_LOW();
    }
}

/**
 * @brief Set RF channel
 */
void nrf24_set_channel(uint8_t channel) {
    if (channel > 125) {
        channel = 125;
    }
    nrf24_write_register(NRF24_REG_RF_CH, channel);
}

/**
 * @brief Set data rate
 */
void nrf24_set_data_rate(nrf24_data_rate_t rate) {
    uint8_t rf_setup = nrf24_read_register(NRF24_REG_RF_SETUP);
    
    rf_setup &= ~((1 << 5) | (1 << 3));  // Clear RF_DR_LOW, RF_DR_HIGH
    
    switch (rate) {
        case NRF24_DATA_RATE_250KBPS:
            rf_setup |= (1 << 5);  // RF_DR_LOW = 1
            break;
        case NRF24_DATA_RATE_1MBPS:
            // Both 0
            break;
        case NRF24_DATA_RATE_2MBPS:
            rf_setup |= (1 << 3);  // RF_DR_HIGH = 1
            break;
    }
    
    nrf24_write_register(NRF24_REG_RF_SETUP, rf_setup);
}

/**
 * @brief Set output power
 */
void nrf24_set_power(nrf24_power_t power) {
    uint8_t rf_setup = nrf24_read_register(NRF24_REG_RF_SETUP);
    
    rf_setup &= ~(0x06);  // Clear RF_PWR bits
    rf_setup |= (power << 1);  // Set new power
    
    nrf24_write_register(NRF24_REG_RF_SETUP, rf_setup);
}

/**
 * @brief Set CRC
 */
void nrf24_set_crc(nrf24_crc_t crc) {
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    
    config &= ~((1 << NRF24_CONFIG_EN_CRC) | (1 << NRF24_CONFIG_CRCO));
    
    if (crc == NRF24_CRC_8BIT) {
        config |= (1 << NRF24_CONFIG_EN_CRC);
    } else if (crc == NRF24_CRC_16BIT) {
        config |= (1 << NRF24_CONFIG_EN_CRC) | (1 << NRF24_CONFIG_CRCO);
    }
    
    nrf24_write_register(NRF24_REG_CONFIG, config);
}

/**
 * @brief Set address width
 */
void nrf24_set_address_width(nrf24_addr_width_t width) {
    nrf24_write_register(NRF24_REG_SETUP_AW, width - 2);
}

/**
 * @brief Set TX address
 */
void nrf24_set_tx_address(const uint8_t *address, uint8_t length) {
    if (length > 5) {
        length = 5;
    }
    nrf24_write_register_multi(NRF24_REG_TX_ADDR, address, length);
    
    // Set RX pipe 0 to same address for Auto ACK
    nrf24_write_register_multi(NRF24_REG_RX_ADDR_P0, address, length);
}

/**
 * @brief Set RX address
 */
void nrf24_set_rx_address(uint8_t pipe, const uint8_t *address, uint8_t length) {
    if (pipe > 5) {
        return;
    }
    
    uint8_t reg = NRF24_REG_RX_ADDR_P0 + pipe;
    
    // Pipes 0, 1 use full address
    if (pipe <= 1) {
        if (length > 5) {
            length = 5;
        }
        nrf24_write_register_multi(reg, address, length);
    } else {
        // Pipes 2-5 use only LSB
        nrf24_write_register(reg, address[0]);
    }
}

/**
 * @brief Enable RX pipe
 */
void nrf24_enable_rx_pipe(uint8_t pipe, bool enable) {
    if (pipe > 5) {
        return;
    }
    
    uint8_t en_rxaddr = nrf24_read_register(NRF24_REG_EN_RXADDR);
    
    if (enable) {
        en_rxaddr |= (1 << pipe);
    } else {
        en_rxaddr &= ~(1 << pipe);
    }
    
    nrf24_write_register(NRF24_REG_EN_RXADDR, en_rxaddr);
}

/**
 * @brief Enable Auto Acknowledgement
 */
void nrf24_enable_auto_ack(uint8_t pipe, bool enable) {
    if (pipe > 5) {
        return;
    }
    
    uint8_t en_aa = nrf24_read_register(NRF24_REG_EN_AA);
    
    if (enable) {
        en_aa |= (1 << pipe);
    } else {
        en_aa &= ~(1 << pipe);
    }
    
    nrf24_write_register(NRF24_REG_EN_AA, en_aa);
}

/**
 * @brief Set retransmission
 */
void nrf24_set_retransmit(uint8_t delay, uint8_t count) {
    if (delay > 15) {
        delay = 15;
    }
    if (count > 15) {
        count = 15;
    }
    
    nrf24_write_register(NRF24_REG_SETUP_RETR, (delay << 4) | count);
}

/**
 * @brief Set payload size
 */
void nrf24_set_payload_size(uint8_t pipe, uint8_t size) {
    if (pipe > 5 || size > 32 || size == 0) {
        return;
    }
    
    nrf24_write_register(NRF24_REG_RX_PW_P0 + pipe, size);
}

/**
 * @brief Enable dynamic payload
 */
void nrf24_enable_dynamic_payload(uint8_t pipe, bool enable) {
    if (pipe > 5) {
        return;
    }
    
    // Enable EN_DPL in FEATURE register
    uint8_t feature = nrf24_read_register(NRF24_REG_FEATURE);
    if (enable) {
        feature |= (1 << 2);  // EN_DPL
    } else {
        feature &= ~(1 << 2);
    }
    nrf24_write_register(NRF24_REG_FEATURE, feature);
    
    // Set pipe in DYNPD register
    uint8_t dynpd = nrf24_read_register(NRF24_REG_DYNPD);
    if (enable) {
        dynpd |= (1 << pipe);
    } else {
        dynpd &= ~(1 << pipe);
    }
    nrf24_write_register(NRF24_REG_DYNPD, dynpd);
}

/* ==================== Transmit/Receive Functions ==================== */

/**
 * @brief Transmit data
 */
nrf24_tx_status_t nrf24_transmit(const uint8_t *data, uint8_t length) {
    uint8_t rx_buffer[NRF24_MAX_PAYLOAD_SIZE];
    if (length > NRF24_MAX_PAYLOAD_SIZE) {
        length = NRF24_MAX_PAYLOAD_SIZE;
    }
    
    // Set to TX mode
    nrf24_set_mode(NRF24_MODE_TX);
    
    // Write payload
    NRF24_CSN_LOW();
    SPITransmitByte(NRF24_CMD_W_TX_PAYLOAD);
    SPITransmitMultiByte(data,rx_buffer,length);
    NRF24_CSN_HIGH();
    
    // Start transmission (CE pulse)
    NRF24_CE_HIGH();
    _delay_us(15);  // Minimum 10us required
    NRF24_CE_LOW();
    
    // Wait for transmission complete (timeout: 1 second)
    uint16_t timeout = 10000;
    uint8_t status;
    
    while (timeout--) {
        status = nrf24_get_status();
        
        // Transmission complete
        if (status & (1 << NRF24_STATUS_TX_DS)) {
            nrf24_clear_status(1 << NRF24_STATUS_TX_DS);
            return NRF24_TX_SUCCESS;
        }
        
        // Maximum retries exceeded
        if (status & (1 << NRF24_STATUS_MAX_RT)) {
            nrf24_clear_status(1 << NRF24_STATUS_MAX_RT);
            nrf24_flush_tx();
            return NRF24_TX_MAX_RETRIES;
        }
        
        _delay_us(100);
    }
    
    // Timeout
    nrf24_flush_tx();
    return NRF24_TX_TIMEOUT;
}

/**
 * @brief Check if data is available
 */
bool nrf24_data_available(void) {
    uint8_t status = nrf24_get_status();
    
    // Check if data is in RX FIFO
    if (status & (1 << NRF24_STATUS_RX_DR)) {
        return true;
    }
    
    // Check RX_P_NO field (not 111 means data available)
    uint8_t pipe = (status >> NRF24_STATUS_RX_P_NO) & 0x07;
    if (pipe < 6) {
        return true;
    }
    
    return false;
}

/**
 * @brief Get RX pipe number
 */
uint8_t nrf24_get_rx_pipe(void) {
    uint8_t status = nrf24_get_status();
    uint8_t pipe = (status >> NRF24_STATUS_RX_P_NO) & 0x07;
    
    if (pipe < 6) {
        return pipe;
    }
    
    return 0xFF;  // No data
}

/**
 * @brief Receive data
 */
uint8_t nrf24_receive(uint8_t *buffer, uint8_t length) {
    uint8_t payload_length;
    uint8_t tx_buffer[NRF24_MAX_PAYLOAD_SIZE]={0xff,};
    
    // Read actual length if dynamic payload is enabled
    uint8_t feature = nrf24_read_register(NRF24_REG_FEATURE);
    if (feature & (1 << 2)) {  // EN_DPL
        NRF24_CSN_LOW();
        SPITransmitByte(NRF24_CMD_R_RX_PL_WID);
        payload_length = SPITransmitByte(0xFF);
        NRF24_CSN_HIGH();
        
        if (payload_length > 32) {
            nrf24_flush_rx();
            return 0;
        }
    } else {
        payload_length = length;
    }
    
    // Check buffer size
    if (payload_length > length) {
        payload_length = length;
    }
    
    // Read payload
    NRF24_CSN_LOW();
    SPITransmitByte(NRF24_CMD_R_RX_PAYLOAD);
    SPITransmitMultiByte(tx_buffer,buffer,length);
    NRF24_CSN_HIGH();
    
    // Clear RX_DR flag
    nrf24_clear_status(1 << NRF24_STATUS_RX_DR);
    
    return payload_length;
}

/* ==================== FIFO and Status Functions ==================== */

/**
 * @brief Flush TX FIFO
 */
void nrf24_flush_tx(void) {
    nrf24_write_command(NRF24_CMD_FLUSH_TX);
}

/**
 * @brief Flush RX FIFO
 */
void nrf24_flush_rx(void) {
    nrf24_write_command(NRF24_CMD_FLUSH_RX);
}

/**
 * @brief Read status register
 */
uint8_t nrf24_get_status(void) {
    //TODO optimize by using NOP commnad
    return nrf24_read_register(NRF24_REG_STATUS);
}

/**
 * @brief Clear status flags
 */
void nrf24_clear_status(uint8_t flags) {
    nrf24_write_register(NRF24_REG_STATUS, flags);
}

/* ==================== Power Management Functions ==================== */

/**
 * @brief Power up
 */
void nrf24_power_up(void) {
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    config |= (1 << NRF24_CONFIG_PWR_UP);
    nrf24_write_register(NRF24_REG_CONFIG, config);
    _delay_ms(5);  // Wait for power up stabilization (minimum 1.5ms)
}

/**
 * @brief Power down
 */
void nrf24_power_down(void) {
    NRF24_CE_LOW();
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    config &= ~(1 << NRF24_CONFIG_PWR_UP);
    nrf24_write_register(NRF24_REG_CONFIG, config);
}
