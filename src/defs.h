#ifndef DEFS_H
#define DEFS_H

/* FIFO structure for transfering data from uart_cb()
to decode() */
typedef struct fifo_uart {
    void *fifo_reserved; // FIFO reserved
    char data[10];
    int length;
} fifo_uart;

/* Define global constants */
    /* Registers */
#define STATUS          0x00
#define DATA_CH1L       0x01
#define DATA_CH1H       0x02
#define DATA_CH2L       0x03
#define DATA_CH2H       0x04
#define DATA_CH3L       0x05
#define DATA_CH3H       0x06
#define DATA_CH4L       0x07
#define DATA_CH4H       0x08
#define CONTROL_1       0x09
#define CONTROL_2       0x0A
#define CONTROL_3       0x0B
#define CONTROL_4       0x0C
#define CH1CONFIG_A     0x0D
#define CH1CONFIG_B     0x0E
#define CH2CONFIG_A     0x0F
#define CH2CONFIG_B     0x10
#define CH3CONFIG_A     0x11
#define CH3CONFIG_B     0x12
#define CH4CONFIG_A     0x13
#define CH4CONFIG_B     0x14
    /* OPCODEs */
#define COLLECT_DATA    67 //"C"
#define READ_REG        82 //"R"
#define UPDATE_REG      85 //"U"
#define PRESET_CFG      80 //"P"
    /* Read/Write */
#define READ            0x30
#define READ_T          0x31
#define WRITE           0x20
#define WRITE_T         0x21
    /* Misc */
#define SLEEP_TIME      K_MSEC(1000)
#define RCV_BUFF_SIZE   16
#define RCV_TIMEOUT     100


/* Define global nodes */
#define SW0_NODE        DT_ALIAS(sw0)
#define SW1_NODE        DT_ALIAS(sw1)
#define USER_NODE       DT_PATH(zephyr_user)
#define CSI080_NODE     DT_NODELABEL(csi080)
#define UART_NODE       DT_NODELABEL(uart0)


#endif