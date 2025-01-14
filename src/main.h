#ifndef MAIN_H
#define MAIN_H

/**
 * @brief Functional test values for 1 channel 
 */
void ch1config(void);

/**
 * @brief Functional test values for all channels 
 */
void ch4config(void);


/**
 * @brief Combines high and low data bytes into single variable
 * 
 * @param high Upper 8 bits of channel data
 * @param low Lower 8 bits of channel data
 * 
 * @retval 16-bit combined channel data
 */
uint16_t concat(uint8_t high, uint8_t low);

/**
 * @brief Reads data from channel one and displays in mV
 */
void ch1readdata(void);

/**
 * @brief Clock test function. Delete in final version.
 */
void dmuxupdate(void);

/**
 * @brief Reads all register values for testing. Delete in final version
 */
void readall(void);

/**
 * @brief Main nRF52840 function. Needs significant cleanup before final.
 */
int main(void);

#endif