#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/pwm.h>
#include "spi_drv.h"
#include "uart_drv.h"
#include "operations.h"
#include "defs.h"


/* Activate logging capability at the info level */
LOG_MODULE_REGISTER(CSI080_MAIN, LOG_LEVEL_INF);


/* Define and initialize FIFO queue and semaphores.
Prevents SPI and UART overlap. */
K_FIFO_DEFINE(uart_queue);
K_SEM_DEFINE(uart_sem, 1, 1);
K_SEM_DEFINE(decode_sem, 0, 1);
K_SEM_DEFINE(reset_sem, 0, 1);
K_TIMER_DEFINE(ch_read_timer,NULL,NULL);



/* Configure GPIO */
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec reset = GPIO_DT_SPEC_GET(USER_NODE, reset_gpios);

/* Configure PWM */
static const struct pwm_dt_spec clk0 = PWM_DT_SPEC_GET_BY_IDX(USER_NODE, 0);



/**
 * @brief Functional test values for 1 channel 
 */
void ch1config(void)
{
        uint8_t size = 3;
        uint8_t reg, value;
        int valid = 0;

        /* Enable bandgap reference. All channels off. */
        reg = CONTROL_3, value = 0x20;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Configure bandgap trims */
        reg = CONTROL_2, value = 0x74;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set Ch1=E1E2, Ch2=E3E4, Ch3=E5E6, Ch4=E7E8
        Disable capacitor bleed down */
        reg = CONTROL_1, value = 0x50;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set HPF = 3 Hz, Gain = 4 V/V */
        reg = CH1CONFIG_A, value = 0x90;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Enable bandgap reference. Enable Ch1 */
        reg = CONTROL_3, value = 0x21;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Disable Ch1 Resets */
        reg = CH1CONFIG_B, value = 0x00;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        // k_sleep(K_MSEC(500));

        if (valid == 0)
        {
                printf("Registers Set for 1 Channel\n");
        }

        else
        {
                printf("Validation Failure\r\n");
        }
}

void ch4config(void)
{
        uint8_t size = 3;
        uint8_t reg, value;
        int valid = 0;

        /* Enable bandgap reference. All channels off. */
        reg = CONTROL_3, value = 0x20;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Configure bandgap trims */
        reg = CONTROL_2, value = 0x74;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set Ch1=E1E2, Ch2=E3E4, Ch3=E5E6, Ch4=E7E8
        Disable capacitor bleed down */
        reg = CONTROL_1, value = 0x50;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set Ch1 HPF = 3 Hz, Gain = 4 V/V */
        reg = CH1CONFIG_A, value = 0x92;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set Ch2 HPF = 3 Hz, Gain = 4 V/V */
        reg = CH2CONFIG_A, value = 0x92;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set Ch3 HPF = 3 Hz, Gain = 4 V/V */
        reg = CH3CONFIG_A, value = 0x92;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Set Ch4 HPF = 3 Hz, Gain = 4 V/V */
        reg = CH4CONFIG_A, value = 0x92;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Enable bandgap reference. Enable all Channels */
        reg = CONTROL_3, value = 0x2F;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Disable Ch1 Resets */
        reg = CH1CONFIG_B, value = 0x00;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Disable Ch2 Resets */
        reg = CH2CONFIG_B, value = 0x00;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Disable Ch3 Resets */
        reg = CH3CONFIG_B, value = 0x00;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        /* Disable Ch4 Resets */
        reg = CH4CONFIG_B, value = 0x00;
        csi080_spi_write(reg, value);
        valid = valid + write_validate(reg, value, size);

        // k_sleep(K_MSEC(500));

        if (valid == 0)
        {
                printf("Registers Set for All Channels\n");
        }

        else
        {
                printf("Validation Failure\r\n");
        }
}


/**
 * @brief Combines high and low data bytes into single variable
 * 
 * @param high Upper 8 bits of channel data
 * @param low Lower 8 bits of channel data
 * 
 * @retval 16-bit combined channel data
 */
uint16_t concat(uint8_t high, uint8_t low)
{
        /* Shifts high right 8 bits and appends low */
        return (high << 8) | low;
}


/**
 * @brief Reads data from channel one and displays in mV
 */
void ch1readdata(void) /* Make ch#readdata(chan) 
                        or merge into "data_collect"*/
{
        uint8_t size = 4;
        uint8_t reg, data[size];
        uint16_t data_out;
        float v_out;

        reg = DATA_CH1L; /* Burst read also reads from DATA_CH1H */
        csi080_spi_read(reg, size, data);
        data_out = concat(data[3], data[2]);
        v_out = data_out * 24.4;
        printf("%f\n",v_out/1000);
}


/**
 * @brief Clock test function. Delete in final version.
 */
void dmuxupdate(void)
{
        uint8_t reg, value, value2, value3;
        reg = 0x15;
        value = 0x03;
        value2 = 0x20;
        value3 = 0x21;

        printf("Enabling test mode\n");
        csi080_spi_write(reg, value);
        csi080_spi_write(reg, value2);
        csi080_spi_write(reg, value3);

        reg = 0x017;
        value = 0x20;
        printf("Setting DMUX out to clocks\n");
        csi080_spi_write(reg, value);
        k_sleep(K_MSEC(500));
}


/**
 * @brief Reads all register values for testing. Delete in final version
 */
void readall(void)
{
        uint8_t size = 3;
        int i, j;
        uint8_t reg, data[size];
        uint16_t full_reg;

        for (j = 0x000; j <= 0x100; j += 0x100)
        {
                for (i = 0x00; i <= 0xff; i++)
                {
                        reg = i;
                        full_reg = i + j;
                        if (j == 0)
                        {
                                csi080_spi_read(reg, size, data);
                        }

                        else
                        {
                                csi080_spi_read_t(reg, size, data);
                        }
                        
                        if (data[2] != 0)
                        {
                                printf("Register: %#.3X. Value: %#.2X\n",full_reg, data[2]);
                        }
                }
        }
        printf("Reading of all registers is complete.\n");
        k_sleep(K_MSEC(500));
}




/**
 * @brief Main nRF52840 function. Needs significant cleanup before final.
 */
int main(void)
{
        /* Define variables */
        int err;

        /* Configure push button interupts */
        gpio_pin_configure_dt(&button0, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_pin_configure_dt(&button1, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);

        /* Configure reset pin */
        gpio_pin_configure_dt(&reset, GPIO_OUTPUT_INACTIVE);

        /* Delay to allow for device startup */
        k_sleep(K_MSEC(500));

        /* Verify SPI, GPIO, and PWM are ready */
        err = csi080_spi_start();
        if (!err) 
        {
                printf("Error: SPI device failed to start, err: %d\n", err);
        }

        err = gpio_is_ready_dt(&button0);
        if (!err) 
        {
                printf("Error: GPIO device is not ready, err: %d\n", err);
        }

        err = pwm_is_ready_dt(&clk0);
        if (!err) 
        {
                printf("Error: PWM device is not ready, err: %d\n", err);
        }

        /* Set PWM pulse length for external clock */
        pwm_set_pulse_dt(&clk0, 500);

        /* Prompt for input */
        printf("System Ready...\n");
        
        /* Wait for button press */
        while (1)
        {
                int button0_status = gpio_pin_get_dt(&button0);
                if (button0_status != 0)
                {               
                        ch1config();
                }
                int button1_status = gpio_pin_get_dt(&button1);
                if (button1_status != 0)
                {
                        /* Signal data start point, collect channel data
                        and signal data end point */
                        printf("data_start\n");
                        for (int i = 0; i < 5000; i++)
                        {
                                ch1readdata();
                        } 
                        k_sleep(K_MSEC(100));
                        printf("data_end\n");
                }
        }
        return 0;
}

