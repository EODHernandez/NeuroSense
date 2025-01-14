#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>
#include "uart_drv.h"
#include "spi_drv.h"
#include "operations.h"
#include "main.h"
#include "defs.h"

/* Define Constants */
#define THREAD_STACK    8192
#define THREAD_PRIORITY 1

/* Configure UART */
const struct device *uart = DEVICE_DT_GET(UART_NODE);

/* Define Receive Buffer */
static uint8_t rx_buff[RCV_BUFF_SIZE] = {0};

/* Shortens struct name for clarity */
struct uart_event_rx rx;

/* Identifies external struct definitions */
extern struct k_fifo uart_queue;
extern struct k_sem uart_sem;
extern struct k_sem decode_sem;
extern struct k_sem reset_sem;



void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    /* Create FIFO queue struct */
    fifo_uart data_in;

    /* Switch to handle multiple event types */
    switch (evt->type)
    {
        /* When UART data is in rx buffer */
        case UART_RX_RDY:

            /* Verifies control by taking semaphore. */
            k_sem_take(&uart_sem, K_FOREVER);

            /* Saves UART data to rx struct. */
            rx = evt->data.rx;

            /* Saves data to FIFO queue */
            data_in.fifo_reserved = NULL;
            strcpy(data_in.data, rx.buf);
            data_in.length = rx.len;

            /* DEBUG PRINTS */
            // printf("RX buff is: %s\r\n",rx.buf);
            // printf("Data is: %s\r\n", data_in.data);
            // printf("Data length is: %u\r\n",data_in.length);

            /* Send data to FIFO queue */
            k_fifo_put(&uart_queue, &data_in);

            /* Release control to decode thread */
            k_sem_give(&decode_sem);


            k_sleep(K_MSEC(100));
            /* Disables rx to force instant reset by UART_RX_DISABLED
            event case. */
            uart_rx_disable(dev);
            
            break;

        /* When UART rx functionality is disabled */
        case UART_RX_DISABLED:

            /* Restarts UART rx system */
            uart_rx_enable(dev, rx_buff, sizeof(rx_buff), RCV_TIMEOUT);
            break;

        /* Other events exists, any useful? */
        default:
            break;
    }
}


void uart_main(void)
{

    if(!device_is_ready(uart))
        printf("UART is not ready");

    /* UART output value. Informs interface of ready to rx status.
    May not be needed depending on python interface. */
    static uint8_t tx_buff[] = {"UART Started\r\n"};

    /* Points UART to callback function (uart_cb()) */
    uart_callback_set(uart, uart_cb, NULL);

    /* Sends UART tx buffer */
    uart_tx(uart, tx_buff, sizeof(tx_buff), SYS_FOREVER_MS);

    /* Starts UART rx system */
    uart_rx_enable(uart, rx_buff, sizeof(rx_buff), RCV_TIMEOUT);

    while (1)
    {
        k_sleep(K_MSEC(100));
    }

}

K_THREAD_DEFINE(uart_main_thread, THREAD_STACK, uart_main, NULL, NULL, NULL,
                THREAD_PRIORITY+1, K_ESSENTIAL, 0);
K_THREAD_DEFINE(uart_cb_thread, THREAD_STACK, uart_cb, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
