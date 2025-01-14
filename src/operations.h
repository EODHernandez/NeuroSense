#ifndef CMD_DECODE_H
#define CMD_DECODE_H


/**
 * @brief Writes value to register and verifies change
 * 
 * @param reg 8-Bit register address
 * @param val 8-Bit value to write to the register
 * @param size Size in bytes of requested data
 */
void update_reg(uint8_t reg, uint8_t val, uint8_t size);


/**
 * @brief Writes value of register
 * 
 * @param reg 8-Bit register address
 * @param size Size in bytes of requested data
 */
void read_reg(uint8_t reg, uint8_t size);


/**
 * @brief Collects data from desired number of channels
 *  and displays in mV
 * 
 * @param channels How many channels to pull from (1-4)
 * @param samples Number of samples to collect
 */
void data_collect(uint8_t channels, unsigned samples);


/**
 * @brief Runs preset configuarion on desired number
 *  of channels
 * 
 * @param channels How many channels to configure from (1-4)
 */
void chconfig(uint8_t channels);

/**
 * @brief Decodes data received from the UART buffer
 */
void decode(void);


#endif