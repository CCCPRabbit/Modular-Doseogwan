/**
 * @file nrf24l01p.h
 * @author CCCPRabbit (you@domain.com)
 * @brief 
 * @version 1.0
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef NRF24L01P_H
#define NRF24L01P_H

#include <stdint.h>
#include <stdbool.h>

/* ==================== Configuration Macros ==================== */

// Pin configuration (can be modified according to your setup)
#define NRF24_CE_DDR        DDRD
#define NRF24_CE_PORT       PORTD
#define NRF24_CE_PIN        PD7

#define NRF24_CSN_DDR       DDRB
#define NRF24_CSN_PORT      PORTB
#define NRF24_CSN_PIN       PB2

// Pin control macros
#define NRF24_CE_LOW()      (NRF24_CE_PORT &= ~(1 << NRF24_CE_PIN))
#define NRF24_CE_HIGH()     (NRF24_CE_PORT |= (1 << NRF24_CE_PIN))
#define NRF24_CSN_LOW()     (NRF24_CSN_PORT &= ~(1 << NRF24_CSN_PIN))
#define NRF24_CSN_HIGH()    (NRF24_CSN_PORT |= (1 << NRF24_CSN_PIN))

/* ==================== nRF24L01P Register Addresses ==================== */

#define NRF24_REG_CONFIG        0x00  // Configuration register
#define NRF24_REG_EN_AA         0x01  // Enable Auto Acknowledgement
#define NRF24_REG_EN_RXADDR     0x02  // Enable RX addresses
#define NRF24_REG_SETUP_AW      0x03  // Setup address width
#define NRF24_REG_SETUP_RETR    0x04  // Setup retransmission
#define NRF24_REG_RF_CH         0x05  // RF channel
#define NRF24_REG_RF_SETUP      0x06  // RF setup
#define NRF24_REG_STATUS        0x07  // Status register
#define NRF24_REG_OBSERVE_TX    0x08  // Transmit observe register
#define NRF24_REG_RPD           0x09  // Received Power Detector
#define NRF24_REG_RX_ADDR_P0    0x0A  // RX address pipe 0
#define NRF24_REG_RX_ADDR_P1    0x0B  // RX address pipe 1
#define NRF24_REG_RX_ADDR_P2    0x0C  // RX address pipe 2
#define NRF24_REG_RX_ADDR_P3    0x0D  // RX address pipe 3
#define NRF24_REG_RX_ADDR_P4    0x0E  // RX address pipe 4
#define NRF24_REG_RX_ADDR_P5    0x0F  // RX address pipe 5
#define NRF24_REG_TX_ADDR       0x10  // TX address
#define NRF24_REG_RX_PW_P0      0x11  // Payload width pipe 0
#define NRF24_REG_RX_PW_P1      0x12  // Payload width pipe 1
#define NRF24_REG_RX_PW_P2      0x13  // Payload width pipe 2
#define NRF24_REG_RX_PW_P3      0x14  // Payload width pipe 3
#define NRF24_REG_RX_PW_P4      0x15  // Payload width pipe 4
#define NRF24_REG_RX_PW_P5      0x16  // Payload width pipe 5
#define NRF24_REG_FIFO_STATUS   0x17  // FIFO status register
#define NRF24_REG_DYNPD         0x1C  // Enable dynamic payload
#define NRF24_REG_FEATURE       0x1D  // Feature register

/* ==================== nRF24L01P Commands ==================== */

#define NRF24_CMD_R_REGISTER        0x00  // Read register
#define NRF24_CMD_W_REGISTER        0x20  // Write register
#define NRF24_CMD_R_RX_PAYLOAD      0x61  // Read RX payload
#define NRF24_CMD_W_TX_PAYLOAD      0xA0  // Write TX payload
#define NRF24_CMD_FLUSH_TX          0xE1  // Flush TX FIFO
#define NRF24_CMD_FLUSH_RX          0xE2  // Flush RX FIFO
#define NRF24_CMD_REUSE_TX_PL       0xE3  // Reuse last TX payload
#define NRF24_CMD_R_RX_PL_WID       0x60  // Read RX payload width
#define NRF24_CMD_W_ACK_PAYLOAD     0xA8  // Write ACK payload
#define NRF24_CMD_W_TX_PAYLOAD_NOACK 0xB0 // Write TX payload without ACK
#define NRF24_CMD_NOP               0xFF  // No Operation

/* ==================== CONFIG Register Bits ==================== */

#define NRF24_CONFIG_MASK_RX_DR     6  // RX_DR interrupt mask
#define NRF24_CONFIG_MASK_TX_DS     5  // TX_DS interrupt mask
#define NRF24_CONFIG_MASK_MAX_RT    4  // MAX_RT interrupt mask
#define NRF24_CONFIG_EN_CRC         3  // Enable CRC
#define NRF24_CONFIG_CRCO           2  // CRC encoding (0=1byte, 1=2bytes)
#define NRF24_CONFIG_PWR_UP         1  // Power state (0=Power Down, 1=Power Up)
#define NRF24_CONFIG_PRIM_RX        0  // RX/TX mode (0=TX, 1=RX)

/* ==================== STATUS Register Bits ==================== */

#define NRF24_STATUS_RX_DR          6  // Data ready RX
#define NRF24_STATUS_TX_DS          5  // Data sent TX
#define NRF24_STATUS_MAX_RT         4  // Max retransmit reached
#define NRF24_STATUS_RX_P_NO        1  // RX pipe number (3 bits)
#define NRF24_STATUS_TX_FULL        0  // TX FIFO full

/* ==================== FIFO_STATUS Register Bits ==================== */

#define NRF24_FIFO_TX_REUSE         6  // TX payload reuse
#define NRF24_FIFO_TX_FULL          5  // TX FIFO full
#define NRF24_FIFO_TX_EMPTY         4  // TX FIFO empty
#define NRF24_FIFO_RX_FULL          1  // RX FIFO full
#define NRF24_FIFO_RX_EMPTY         0  // RX FIFO empty

/* ==================== Constants ==================== */

#define NRF24_MAX_PAYLOAD_SIZE      32  // Maximum payload size
#define NRF24_ADDR_WIDTH            5   // Default address width (3-5 bytes)

/* ==================== Enumeration Definitions ==================== */

/**
 * @brief Data rate
 */
typedef enum {
    NRF24_DATA_RATE_250KBPS = 0,  // 250 kbps
    NRF24_DATA_RATE_1MBPS = 1,    // 1 Mbps
    NRF24_DATA_RATE_2MBPS = 2     // 2 Mbps
} nrf24_data_rate_t;

/**
 * @brief Output power
 */
typedef enum {
    NRF24_POWER_M18DBM = 0,  // -18 dBm
    NRF24_POWER_M12DBM = 1,  // -12 dBm
    NRF24_POWER_M6DBM = 2,   // -6 dBm
    NRF24_POWER_0DBM = 3     // 0 dBm
} nrf24_power_t;

/**
 * @brief CRC length
 */
typedef enum {
    NRF24_CRC_DISABLED = 0,  // CRC disabled
    NRF24_CRC_8BIT = 1,      // 8-bit CRC
    NRF24_CRC_16BIT = 2      // 16-bit CRC
} nrf24_crc_t;

/**
 * @brief Address width
 */
typedef enum {
    NRF24_ADDR_WIDTH_3BYTES = 3,  // 3 bytes
    NRF24_ADDR_WIDTH_4BYTES = 4,  // 4 bytes
    NRF24_ADDR_WIDTH_5BYTES = 5   // 5 bytes
} nrf24_addr_width_t;

/**
 * @brief Operation mode
 */
typedef enum {
    NRF24_MODE_TX = 0,  // Transmit mode
    NRF24_MODE_RX = 1   // Receive mode
} nrf24_mode_t;

/**
 * @brief Transmission status
 */
typedef enum {
    NRF24_TX_SUCCESS = 0,      // Transmission successful
    NRF24_TX_TIMEOUT = 1,      // Timeout
    NRF24_TX_MAX_RETRIES = 2   // Maximum retries exceeded
} nrf24_tx_status_t;

/* ==================== Function Prototypes ==================== */

/**
 * @brief Initialize nRF24L01P
 * @return true: success, false: failure
 */
bool nrf24_init(void);

/**
 * @brief Read register
 * @param reg Register address
 * @return Register value
 */
uint8_t nrf24_read_register(uint8_t reg);

/**
 * @brief Write register
 * @param reg Register address
 * @param value Value to write
 */
void nrf24_write_register(uint8_t reg, uint8_t value);

/**
 * @brief Read multi-byte register
 * @param reg Register address
 * @param buffer Buffer to store data
 * @param length Number of bytes to read
 */
void nrf24_read_register_multi(uint8_t reg, uint8_t *buffer, uint8_t length);

/**
 * @brief Write multi-byte register
 * @param reg Register address
 * @param buffer Data buffer to write
 * @param length Number of bytes to write
 */
void nrf24_write_register_multi(uint8_t reg, const uint8_t *buffer, uint8_t length);

/**
 * @brief Set operation mode
 * @param mode Mode (NRF24_MODE_TX or NRF24_MODE_RX)
 */
void nrf24_set_mode(nrf24_mode_t mode);

/**
 * @brief Set RF channel
 * @param channel Channel number (0-125)
 */
void nrf24_set_channel(uint8_t channel);

/**
 * @brief Set data rate
 * @param rate Data rate
 */
void nrf24_set_data_rate(nrf24_data_rate_t rate);

/**
 * @brief Set output power
 * @param power Output power
 */
void nrf24_set_power(nrf24_power_t power);

/**
 * @brief Set CRC
 * @param crc CRC length
 */
void nrf24_set_crc(nrf24_crc_t crc);

/**
 * @brief Set address width
 * @param width Address width (3-5 bytes)
 */
void nrf24_set_address_width(nrf24_addr_width_t width);

/**
 * @brief Set TX address
 * @param address Address array (maximum 5 bytes)
 * @param length Address length
 */
void nrf24_set_tx_address(const uint8_t *address, uint8_t length);

/**
 * @brief Set RX address
 * @param pipe Pipe number (0-5)
 * @param address Address array
 * @param length Address length (pipes 0,1 multi-byte, pipes 2-5 single byte)
 */
void nrf24_set_rx_address(uint8_t pipe, const uint8_t *address, uint8_t length);

/**
 * @brief Enable RX pipe
 * @param pipe Pipe number (0-5)
 * @param enable true: enable, false: disable
 */
void nrf24_enable_rx_pipe(uint8_t pipe, bool enable);

/**
 * @brief Enable Auto Acknowledgement
 * @param pipe Pipe number (0-5)
 * @param enable true: enable, false: disable
 */
void nrf24_enable_auto_ack(uint8_t pipe, bool enable);

/**
 * @brief Set retransmission
 * @param delay Retransmit delay (0-15, in units of 250us + 250us)
 * @param count Retransmit count (0-15)
 */
void nrf24_set_retransmit(uint8_t delay, uint8_t count);

/**
 * @brief Set payload size
 * @param pipe Pipe number (0-5)
 * @param size Payload size (1-32 bytes)
 */
void nrf24_set_payload_size(uint8_t pipe, uint8_t size);

/**
 * @brief Enable dynamic payload
 * @param pipe Pipe number (0-5)
 * @param enable true: enable, false: disable
 */
void nrf24_enable_dynamic_payload(uint8_t pipe, bool enable);

/**
 * @brief Transmit data
 * @param data Data to transmit
 * @param length Data length (maximum 32 bytes)
 * @return Transmission status
 */
nrf24_tx_status_t nrf24_transmit(const uint8_t *data, uint8_t length);

/**
 * @brief Check if data is available
 * @return true: data available, false: no data
 */
bool nrf24_data_available(void);

/**
 * @brief Get RX pipe number
 * @return Pipe number (0-5), 0xFF if no data
 */
uint8_t nrf24_get_rx_pipe(void);

/**
 * @brief Receive data
 * @param buffer Buffer to store received data
 * @param length Maximum length to receive
 * @return Actual number of bytes received
 */
uint8_t nrf24_receive(uint8_t *buffer, uint8_t length);

/**
 * @brief Flush TX FIFO
 */
void nrf24_flush_tx(void);

/**
 * @brief Flush RX FIFO
 */
void nrf24_flush_rx(void);

/**
 * @brief Read status register
 * @return Status register value
 */
uint8_t nrf24_get_status(void);

/**
 * @brief Clear status flags
 * @param flags Flag bitmask to clear
 */
void nrf24_clear_status(uint8_t flags);

/**
 * @brief Power up
 */
void nrf24_power_up(void);

/**
 * @brief Power down
 */
void nrf24_power_down(void);

/**
 * @brief Test communication (verify by register read/write)
 * @return true: communication success, false: communication failure
 */
bool nrf24_test_connection(void);

/**
 * @brief Reset all states
 */
void nrf24_reset(void);

#endif // NRF24L01P_H
