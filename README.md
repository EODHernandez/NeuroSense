# NeuroSense
 
Created by ASU EE Capstone Team 1 - Spring-Fall 2024
- Michael Cortesio
- Matthew Healy-Lisiecki
- Brian Hernandez
- Rohan Sagi
- Ryan Scott

    This project was designed to read neural impulses from up to 4 channels of the CSI080 and output the waveform graphically. 

    The interface is written in Python. TO initate, run \interface\start.py using any Python interpreter. The required non-standard
packages are:
- numpy
- pyserial
- matplotlib
- scipi

    The interface communicates to and from the nRF25840 using UART over USB. Pyserial's packetizer functions were initially used to provide
communication, but proved problematic. Utilization of the standard UART buffer was simple and provided required functions. A prepended opcode
sent by the Python interface is used by the nRF52840 to select the proper function. All opcodes and other constants are in cfg.py and defs.h.

    main.c initiates core functions that are in the other .c files. Many functions in main.c itself are no longer used but have been kept for
testing reasons. operations.c decodes the UART input from the interface to run the proper function for the desired command. operations.c also
formats the SPI output to the CSI080.

    spi_drv.c and uart_drv.c contain the core functions for SPI and UART communications. Use of FIFO queues, semephores, and threads are used
to prevent SPI or UART interupting the other.

    Pinout has been modified from the recommended in the documentation from Cirtec to simplify connectivity using jumpers. The connection
diagram is below. All pins can be re-assigned as needed using the overlay file.

    Standard nRF52840 build and flash via VSCode is used to program the board. The same USB connection used for programming is used for operation.

- - - Note on connection: Ensure the HV pins on the bonker board are utilized. Output from the nRF52840 is 3.3V, the non-HV pins only support 1.8V.
nRF52840 output voltage can be adjusted downward, with some difficulty, if the non-HV pins are to be utilized.

nRF52840    <----->     CSI080
GND         <----->     CN3 Pin 1
GND         <----->     CN17 Pin 1
5V          <----->     CN3 Pin 2
VDD         <----->     CN17 Pin 2
P1.10       <----->     RST_HV
P1.11       <----->     CLK_HV
P1.12       <----->     CSN_HV
P1.13       <----->     SDI_HV
P1.14       <----->     SDO_HV
P1.15       <----->     SCK_HV

    I can be contacted via GitHub (EODHernandez) with any questions.

    Brian Hernandez
