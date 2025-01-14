#ifndef SPI_DRV_H
#define SPI_DRV_H

/**
 * @brief CSI080 SPI Startup Function
 *
 * @details CSI080 SPI Startup Function: Verifies SPI is configured and ready
 *
 * @retval true if SPI started
 * @retval false if SPI failed to start 
 */
bool csi080_spi_start(void);

/**
 * @brief CSI080 SPI Write Function.
 *
 * @details CSI080 SPI Write function: Sends the write instruction, register address, and register value to the CSI080 over SPI.
 *
 * @param reg 8-bit target register address.
 * @param value 8-bit value to write to the register.
 */
void csi080_spi_write(uint8_t reg, uint8_t value);

/**
 * @brief CSI080 SPI Read Function.
 *
 * @details CSI080 SPI Read function: Sends the read instruction and target register address to the CSI080 over SPI.
 *
 * @param reg 8-bit target register address.
 * @param size Size in bytes of requested data.
 * @param rx_data Variable to store the received data.
 */
void csi080_spi_read(uint8_t reg, uint8_t size, uint8_t *rx_data);

/**
 * @brief CSI080 SPI Test Register Read Function.
 *
 * @details CSI080 SPI Test Register Read function: Sends the read instruction and target register address to the CSI080 over SPI for 9-bit addresses.
 *
 * @param reg 8-bit target register address.
 * @param size Size in bytes of requested data.
 * @param rx_data Variable to store the received data.
 */
void csi080_spi_read_t(uint8_t reg, uint8_t size, uint8_t *rx_data);

/**
* @brief Performs write validation on updated register values
*
* @param reg 8-bit targer register address
* @param value 8-bit expected register value
* @param size Size in bytes of requested data
* 
* @retval 0 if write validated
* @retval 1 if write invalid
*/
int write_validate(uint8_t reg, uint8_t value, uint8_t size);

#endif