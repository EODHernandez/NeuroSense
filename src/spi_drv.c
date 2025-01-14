#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include "spi_drv.h"
#include "main.h"
#include "operations.h"
#include "defs.h"

/* Activate logging capability at the info level */
LOG_MODULE_REGISTER(CSI080_SPI, LOG_LEVEL_INF);


/* Define options and configure SPI */
#define SPIOP           SPI_WORD_SET(8) | SPI_TRANSFER_MSB
struct spi_dt_spec spispec = SPI_DT_SPEC_GET(CSI080_NODE, SPIOP, 0);

bool csi080_spi_start(void)
{
        printf("Starting SPI\r\n");

        /* Start SPI by verifying ready */
        int err = spi_is_ready_dt(&spispec);
        if (!err)
        {
                LOG_ERR("Failed to start SPI");
                return false;
        }

        return true;
}

void csi080_spi_write(uint8_t reg, uint8_t value)
{
        /* Build tx buffer. Update to support burst write. */
        uint8_t tx_buf[] = {WRITE, reg, value};
        struct spi_buf tx_spi_buf = {.buf = tx_buf, .len = sizeof(tx_buf)};
        struct spi_buf_set tx_spi_buf_set = {.buffers = &tx_spi_buf, .count = 1};

        /* Write tx buffer over SPI */
        int err = spi_write_dt(&spispec, &tx_spi_buf_set);
        if (err < 0) 
        {
                LOG_ERR("spi_write_dt() failed, err %d", err);
        }
}

void csi080_spi_read(uint8_t reg, uint8_t size, uint8_t *rx_data)
{
        /* Build tx buffer. CSI080 requires tx before rx to identify
        proper register. */
        uint8_t tx_buf[] = {READ, reg};
        struct spi_buf tx_spi_buf = {.buf = (void *)&tx_buf, .len = 2};
        struct spi_buf_set tx_spi_buf_set = {.buffers = &tx_spi_buf, .count = 1};

        /* Build rx buffer. Burst read when size > 3 */
        struct spi_buf rx_spi_buf = {.buf = rx_data, .len = size};
        struct spi_buf_set rx_spi_buf_set = {.buffers = &rx_spi_buf, .count = 1};
        /* Write tx buffer then read to rx buffer using SPI */
	int err = spi_transceive_dt(&spispec, &tx_spi_buf_set, &rx_spi_buf_set);

        if (err < 0) 
        {
                LOG_ERR("spi_transceive_dt() failed, err %d", err);
        }
}
/* Merge into csi080_spi_read()? */
void csi080_spi_read_t(uint8_t reg, uint8_t size, uint8_t *rx_data)
{

        uint8_t tx_buf[] = {READ_T, reg};
        struct spi_buf tx_spi_buf = {.buf = (void *)&tx_buf, .len = 2};
        struct spi_buf_set tx_spi_buf_set = {.buffers = &tx_spi_buf, .count = 1};

        struct spi_buf rx_spi_buf = {.buf = rx_data, .len = size};
        struct spi_buf_set rx_spi_buf_set = {.buffers = &rx_spi_buf, .count = 1};

	int err = spi_transceive_dt(&spispec, &tx_spi_buf_set, &rx_spi_buf_set);

        if (err < 0) 
        {
                LOG_ERR("spi_transceive_dt() failed, err %d", err);
        }
}

int write_validate(uint8_t reg, uint8_t value, uint8_t size)
{

        uint8_t data[size];
        csi080_spi_read(reg,size,data);
        // printf("VAL: Reg:%X Size:%u\r\n",reg, size);
        // printf("VAL: Data: %X %X %X\r\n",data[0],data[1],data[2]);
        if (value != data[size-1])
        {
                printf("Value error on 0x%02X\n",reg);
                printf("Actual value is 0x%02X\n",data[size-1]);
                printf("Expected value is 0x%02X\n",value);

                return 1;
        }
        else
        {
                return 0;
        }
}
