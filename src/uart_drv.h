#ifndef UART_DRV_H
#define UART_DRV_H

/* Declare struct */
struct uart_event;


/**
 * @brief UART Callback function. Provides event handling.
 * 
 * @param dev Pointer to UART device
 * @param evt Pointer to UART event
 * @param user_data Pointer to user provided variables - unused
 */
void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);


/**
 * @brief Main UART function
 */
void uart_main(void);

#endif