#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>
#include "uart_drv.h"
#include "spi_drv.h"
#include "main.h"
#include "defs.h"
#include "operations.h"

/* Define Constants */
#define THREAD_STACK    8192
#define THREAD_PRIORITY -1

/* Identifies external struct definitions */
extern struct k_fifo uart_queue;
extern struct k_sem uart_sem;
extern struct k_sem decode_sem;
extern struct k_sem reset_sem;
extern const k_tid_t uart_cb_thread;
extern struct k_timer ch_read_timer;


void update_reg(uint8_t reg, uint8_t val, uint8_t size)
{

    // printf("Register is 0x%02X. In dec: %u\r\n",reg, reg);
    // printf("Value is 0x%02X. In dec: %u\r\n",val,val);

    csi080_spi_write(reg,val);
    if (write_validate(reg,val,size) == 0)
    {
        printf("Register 0x%02X updated to 0x%02X\r\n",reg, val);
    }
    else
    {
        printf("Failed to update register 0x%02X\r\n",reg);
    }
}


void read_reg(uint8_t reg, uint8_t size)
{
    uint8_t data[size];

    // printf("Register is 0x%02X. In dec: %u\r\n",reg, reg);

    csi080_spi_read(reg,size,data);

    printf("Register: 0x%02X. Value: 0x%02X\n",reg, data[2]);
    // printf("Data: %X %X %X\r\n",data[0],data[1],data[2]);
}


void data_collect(uint8_t channels, unsigned samples)
{
    /* Fix to around 500 samples/sec */
    k_thread_suspend(uart_cb_thread);
    
    // uint8_t size = 4;
    uint8_t size = channels*2+2;
    uint8_t reg, data[size];
    uint16_t data_out;
    float v_out;
    reg = DATA_CH1L;

        
    printf("data_start\n");
    for (int i = 0; i < (samples); i++)
    {
        // k_timer_start(&ch_read_timer, K_USEC(500), K_NO_WAIT);

        // k_timer_status_sync(&ch_read_timer);

        csi080_spi_read(reg, size, data);

        for (int j = 0; j < (size); j++)
        {
            data_out = concat(data[j+3], data[j+2]);
            v_out = data_out * 24.4;
            printf("ch%d\n",j+1);
            printf("%f\n",v_out/1000);
        }
    }
    printf("data_end\n");
    k_sleep(K_MSEC(100));
    k_thread_resume(uart_cb_thread);

}

void chconfig(uint8_t channels)
{
    ch4config();
}

// READ ALL CONFIGS TO DISPLAY IN GUI?? OR AT LEAST SHOW IN TEXT


void decode(void)
{
    /* Define variables */
    char dat[10];
    uint8_t len;
    uint8_t size = 3;
    char reg_str[3] = "00";
    char val_str[3] = "00";
    unsigned reg;
    unsigned val;
    unsigned chan;
    unsigned samp;

    /* Create FIFO queue struct */
    fifo_uart *data_in;
    
    while (1)
    {
        /* Verify control by taking semaphore */
        k_sem_take(&decode_sem, K_FOREVER);

        /* Remove data from FIFO queue.
        Any executable code between "k_sem_take(...)" and "data_in = ..."
        will cause the execution to fail. */
        data_in = k_fifo_get(&uart_queue,K_FOREVER);

        /* Split data into individual variables */
        strcpy(dat, data_in->data);
        len = data_in->length;

        /* Convert string inputs to hex */
        reg_str[0] = dat[1]; reg_str[1] = dat[2];
        sscanf(reg_str, "%X", &reg);
        val_str[0] = dat[3]; val_str[1] = dat[4];
        sscanf(val_str, "%X", &val);

        switch (dat[0])
        {
            case UPDATE_REG:
                printf("Updating Register 0x%02X to 0x%02X\r\n", reg, val);
                update_reg(reg, val, size);
                break;

            case READ_REG:
                printf("Reading value of register 0x%02X\r\n", reg);
                read_reg(reg,size);
                break;

            case COLLECT_DATA:
                chan = reg;
                samp = val*100;

                printf("Starting data collection of %d channel(s) for %d samples\r\n",
                        chan, samp);
                data_collect(reg, samp);
                break;

            case PRESET_CFG:
                chan = reg;
                printf("Configuring %d channel(s)\r\n",chan);
                /* Make into sub_function for selection? */
                chconfig(chan); 
                break;

            default:
                printf("I don't know what you want\r\n");
                break;
        }

        k_sem_give(&uart_sem);
        

        k_sleep(K_MSEC(250));
    }
}
K_THREAD_DEFINE(decode_thread, THREAD_STACK, decode, NULL, NULL, NULL,
                THREAD_PRIORITY, K_ESSENTIAL, 0);