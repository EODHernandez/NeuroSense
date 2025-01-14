""" Starting point for CSI080 Python interface. """

# Import modules
import time
import sys
import csv
import numpy as np
import serial
import serial.serialutil
import matplotlib.pyplot as plt
import serial.tools
import serial.tools.list_ports

# Import Local Modules
import cfg
import sig_filt
import menu

# Define constants


def serial_init():
    """ Initializes and opens serial port to nRF52840 UART.
     User must enter proper COM port. """

    # Loop until connection is successful.
    while True:
        ports = serial.tools.list_ports.comports()
        print()
        print("Available ports:")
        for com_port in ports:
            print(com_port)
        print()
        port = input("Enter serial port of nRF52840 using 'COM#' format:").upper()

        # Attempt serial connection and clear buffers.
        try:
            cfg.ser = serial.Serial(port, timeout=0.2, write_timeout=0.2,
                                baudrate=115200, rtscts=True)
            cfg.ser.reset_input_buffer()
            cfg.ser.reset_output_buffer()
            # Break out of "while" loop.
            break

        # Catches connection errors.
        except serial.serialutil.SerialException:
            print(f"Unable to connect to {port}.\n")


def serial_reset():
    """ Resets RX and TX buffers. """

    cfg.ser.reset_input_buffer()
    cfg.ser.reset_output_buffer()
    print("\nBuffers reset.\n")


def serial_write(send_str):
    """ Sends a string over the open serial connection.
     Other data formats will be rejected. """

    try:

        # Reads any data in UART buffer.
        serial_read()

        # Converts string to bytes and sends over serial.
        send_bytes = send_str.encode()
        cfg.ser.write(send_bytes)
        print(f"sent data: {send_bytes}")

        # Delay needed to allow data time to transfer.
        time.sleep(0.1)

        # Reads any data in UART buffer.
        serial_read()

        input("Press the 'Enter' key to continue...")
        menu.menu_top()

    except KeyboardInterrupt:
        print("Exiting per keyboard interrupt")
        sys.exit()

    except serial.serialutil.SerialException:
        print("nRF52840 disconnected!")
        sys.exit()

    except AttributeError:
        print("\nData into function must be a string.\n"
              f"Input data is {type(send_str)}.\n")
        input("Press the 'Enter' key to continue...")
        menu.menu_top()


def serial_write_old():
    """ Old serial write function. Delete later. """

    # if not isinstance(ser, serial.serialwin32.Serial):
    #     serial_init()
    # Inf loop: Need to modify to allow leaving serial_write()
    while True:
        try:

            # Read buffer if data available. Use serial_read() instead.
            while cfg.ser.in_waiting > 0:
                ser_bytes = cfg.ser.read_until()
                ser_str = ser_bytes.decode()
                print(ser_str)

            ser_str = ""
            ser_in = input("Enter hex value:")

            if ser_in == "exit":
                print("Exiting")
                time.sleep(0.5)
                sys.exit()

            int_in = int(ser_in, 16)
            send_str = str(int_in)+"\x00"
            send_bytes = send_str.encode()
            cfg.ser.write(send_bytes)
            print(f"sent data: {send_bytes}")
            time.sleep(0.01)

                # Display read data. Remove in future.

        except KeyboardInterrupt:
            print("Exiting per keyboard interrupt")
            sys.exit()

        except serial.serialutil.SerialException:
            print("nRF52840 disconnected!")
            sys.exit()


def serial_read():
    """ Reads from UART buffer until empty. """

    try:
        while cfg.ser.in_waiting > 0:
            ser_bytes = cfg.ser.read_until()
            ser_str = ser_bytes.decode()
            print(ser_str)

    # Catches and exits using CTRL-C
    except KeyboardInterrupt:
        print("Exiting per keyboard interrupt")
        sys.exit()


    # Catches disconnects and exits script
    except serial.serialutil.SerialException:
        print("nRF52840 disconnected!")
        sys.exit()


def data_collect(chs_hex, smp_hex):
    """ Tentative data collection function. Requires streamlining. """

    data1 = []
    data2 = []
    data3 = []
    data4 = []
    fig = ""
    send_str = cfg.COLLECT_DATA+chs_hex+smp_hex+cfg.TERM

    # Converts string to bytes and sends over serial.
    send_bytes = send_str.encode()
    cfg.ser.write(send_bytes)
    print(f"sent data: {send_bytes}")

    # Delay needed to allow data time to transfer.
    time.sleep(0.1)

    while True:
        try:
            # Read buffer if data avilable.
            while cfg.ser.in_waiting >= 0:
                ser_bytes = cfg.ser.read_until()
                ser_str = ser_bytes.decode()

                # Start signal collection. Move to own function.
                if ser_str == "data_start\r\n":
                    print("Starting sampling")
                    start_time = time.time()
                    # Loop until forced break.
                    while True:

                        # Read buffer. Call serial_read() instead?
                        ser_bytes = cfg.ser.read_until()
                        ser_str = ser_bytes.decode()

                        # Stop signal collection.
                        if ser_str == "data_end\r\n":

                            # Save Raw Data
                            with open("raw.csv","w") as f:
                                writer = csv.writer(f)
                                writer.writerow(data1)
                                writer.writerow(data2)
                                writer.writerow(data3)
                                writer.writerow(data4)


                            # Finds total sample time and rate
                            end_time = time.time()
                            time.sleep(0.1)
                            s_time = end_time - start_time
                            n_samp = len(data1)
                            samp_sec = n_samp/s_time
                            print(f"Collected {n_samp} samples in {s_time:.2f} seconds\n")
                            print(f"{samp_sec:.2f} samples per second\n")
                            time_samp = np.linspace(0,s_time,n_samp)

                            ser_str = ""

                            # Plots number of signals used.
                            if chs_hex == "01":
                                fig, ax1 = plt.subplots(1, 1)
                                data1 = sig_filt.filter_signal2(data1)
                                ax1.plot(time_samp,data1)
                            elif chs_hex == "02":
                                fig, (ax1, ax2) = plt.subplots(2, 1)
                                data1 = sig_filt.filter_signal2(data1)
                                data2 = sig_filt.filter_signal2(data2)
                                ax1.plot(time_samp,data1)
                                ax2.plot(time_samp,data2)
                            elif chs_hex == "03":
                                fig, (ax1, ax2, ax3) = plt.subplots(3, 1)
                                data1 = sig_filt.filter_signal2(data1)
                                data2 = sig_filt.filter_signal2(data2)
                                data3 = sig_filt.filter_signal2(data3)
                                ax1.plot(time_samp,data1)
                                ax2.plot(time_samp,data2)
                                ax3.plot(time_samp,data3)
                            elif chs_hex == "04":
                                fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1)
                                data1 = sig_filt.filter_signal2(data1)
                                data2 = sig_filt.filter_signal2(data2)
                                data3 = sig_filt.filter_signal2(data3)
                                data4 = sig_filt.filter_signal2(data4)
                                ax1.plot(time_samp,data1)
                                ax2.plot(time_samp,data2)
                                ax3.plot(time_samp,data3)
                                ax4.plot(time_samp,data4)

                            fig.suptitle("Neural Impulse Output")
                            fig.supxlabel("Time (s)")
                            fig.supylabel("Voltage (mV)")
                            plt.tight_layout()
                            plt.show()

                            # Save Raw Data
                            with open("clean.csv","w") as f:
                                writer = csv.writer(f)
                                writer.writerow(data1)
                                writer.writerow(data2)
                                writer.writerow(data3)
                                writer.writerow(data4)


                            # Loop until cleaned signal is acceptable
                            # Used for tuning, remove in final version.
                            good = 1
                            while good == 0:
                                # Call filtering function
                                # data_clean = sig_filt.filter_signal(data1, T_HOLD)
                                data_clean = sig_filt.filter_signal2(data1)

                                # Plot clean signal. Should be only signal
                                # plotted in final version
                                plt.plot(time_samp,data_clean)
                                plt.show()

                                # Re-filter if needed. Avoids resampling.
                                good = int(input("Enter 0 if bad, 1 if good: "))

                            # Break out of signal collection loop
                            menu.menu_top()

                        # Splits received data into individual channels.
                        if ser_str == "ch1\r\n":
                            ser_bytes = cfg.ser.read_until()
                            ser_str = ser_bytes.decode()
                            ser_data = float(ser_str)
                            data1.append(ser_data)
                        elif ser_str == "ch2\r\n":
                            ser_bytes = cfg.ser.read_until()
                            ser_str = ser_bytes.decode()
                            ser_data = float(ser_str)
                            data2.append(ser_data)
                        elif ser_str == "ch3\r\n":
                            ser_bytes = cfg.ser.read_until()
                            ser_str = ser_bytes.decode()
                            ser_data = float(ser_str)
                            data3.append(ser_data)
                        elif ser_str == "ch4\r\n":
                            ser_bytes = cfg.ser.read_until()
                            ser_str = ser_bytes.decode()
                            ser_data = float(ser_str)
                            data4.append(ser_data)

                print(ser_str)

        # Catches and exits using CTRL-C
        except KeyboardInterrupt:
            print("Exiting per keyboard interrupt")
            sys.exit()

        # Catches disconnects and exits script
        except serial.serialutil.SerialException:
            print("nRF52840 disconnected!")
            sys.exit()


def main():
    """ Main function for interface. """
    serial_init()
    menu.menu_top()



# Calls main() when run as script.
if __name__ == "__main__":
    main()
