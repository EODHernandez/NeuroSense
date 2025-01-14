""" Primary Menu for CSI080 Interface """

# Import Modules
import time
import sys

# Import Local Modules
import start
import cfg

def menu_top():
    """ Top Level Menu """

    # Clears terminal window for clarity.
    print("\033c", end="")

    # Verify serial port is open.
    if not cfg.ser.is_open:
        print("Serial port is not connected.")

    # Basic menu, currently for testing. Will build out
    # as functions are created
    print(f"Welcome.\nConnected to {cfg.ser.name}\n")
    print("1) Update register\n"
          "2) Read register\n"
          "3) Collect data\n"
          "4) Configure Channels\n"
          "5) Read UART buffer\n"
          "6) Clear UART rx/tx buffers\n"
          "0) Exit\n")

    while True:
        entry = int(input("Which option? "))

        # Match/Case used to select menu options
        match entry:
            case 1: # Go to register update function
                reg_update()

            case 2: # Go to register read function
                reg_read()

            case 3: # Go to data collection function
                data_collect_menu()

            case 4: # Go to channel preset config function
                config_chan()

            case 5: # Outputs UART buffer if not empty
                print("\033c", end="")

                if cfg.ser.in_waiting == 0:
                    print("UART buffer is empty.")
                else:
                    start.serial_read()

                input("Press the 'Enter' key to continue...")
                menu_top()

            case 6: # Clears UART buffer
                print("\033c", end="")
                start.serial_reset()
                input("Press the 'Enter' key to continue...")
                menu_top()

            case 0: # Exits program
                print("Exiting")
                time.sleep(1)
                print("\033c", end="")
                sys.exit()

            case _: # Default case
                print("Invalid selection, try again.\n")


def reg_read():
    """ Register read menu """

    print("\033c", end="")

    print("Read Register\n")

    # Pulls register name from reg_addr function
    reg = reg_addr()

    # Sends command over UART
    print("\033c", end="")
    print(f"Reading value of {reg}.")
    send_str = cfg.READ_REG+cfg.REGS[reg]+cfg.NULL_VAL+cfg.TERM
    start.serial_write(send_str)


def reg_update():
    """ Register update menu """
    print("\033c", end="")
    print("Update Register\n\n")

    # Pulls register name from reg_addr function
    reg = reg_addr()

    loop_in = True
    loop_out = True

    # Loops used to detect binary or hex inputs.
    # Defaults to binary if either applies (1, 10, 11) 
    while loop_out:
        while loop_in:

            print("\033c", end="")

            val = input(f"Enter updated value, in binary or hex, for {reg}: ")
            try:
                val = int(val,2)
                print("Detected binary.")
                loop_in = False
            except ValueError:
                try:
                    val = int(val,16)
                    print("Detected hex.")
                    loop_in = False
                except ValueError:
                    print("\nNew value must be in binary or hex.\n")
                    input("Press the 'Enter' key to continue...")

        val_hex = f"{val:02x}".upper()

        if val < 0 or val > 255:
            print(f"\nValue must be 0x00 to 0xFF. You entered 0x{val_hex}.\n")
            input("Press the 'Enter' key to continue...")

        else:
            loop_out = False

    # Sends command over UART
    print(f"Updating {reg} to 0x{val_hex}.")
    send_str = cfg.UPDATE_REG+cfg.REGS[reg]+val_hex+cfg.TERM
    start.serial_write(send_str)


def reg_addr():
    """ Register select submenu """

    print("1) Control 1\n"
          "2) Control 2\n"
          "3) Control 3\n"
          "4) Control 4\n"
          "5) Channel 1, Config A\n"
          "6) Channel 1, Config B\n"
          "0) Return to top\n")

    while True:
        entry = int(input("Which Register? "))

        match entry:
            case 1:
                reg = "CONTROL_1"

            case 2:
                reg = "CONTROL_2"

            case 3:
                reg = "CONTROL_3"

            case 4:
                reg = "CONTROL_4"

            case 5:
                reg = "CH1CONFIG_A"

            case 6:
                reg = "CH1CONFIG_B"

            case 0:
                menu_top()

            case _:
                print("Invalid selection, try again.\n")

        return reg



def data_collect_menu():
    """ Data collection menu """

    while True:

        print("\033c", end="")

        # Selects number of channels to read from. 
        print("How many channels to collect from?")
        chan = int(input("Enter an integer from 1-4: "))
        print()

        if chan < 1 or chan > 4:
            print(f"Value must be 1-4. You entered {chan}.\n")
            input("Press the 'Enter' key to continue...")

        # Enter number of samples to collect
        else:
            print("How many samples, in hundreds, to collect?")
            samp = int(input("Enter an integer from 1-255: "))
            print()

            if samp < 1 or samp > 255:
                print(f"Value must be 1-255. You entered {samp}.\n")
                input("Press the 'Enter' key to continue...")

            # Estimates collection time and prompts for confirmation
            else:
                print(f"Collect {samp*100:,} samples from {chan} channels?\n"
                    f"(approx. {(samp*100)/((-30*chan)+174):.2f} seconds)")
                check = input('Enter "Y" or "N": ').upper()

                # Sends command over UART
                if check == "Y":
                    samp_hex = f"{samp:02x}".upper()
                    chs_hex = f"{chan:02x}".upper()
                    start.data_collect(chs_hex, samp_hex)

                else:
                    pass

def config_chan():
    """ Performs a preset configuration of selected channel """
    print("\033c", end="")

    # Intended to select number of channels to configure.
    # Removed input to streamline testing.

    # chs = int(input("Which channel? "))
    chs = 4
    chs_hex = f"{chs:02x}".upper()
    send_str = cfg.PRESET_CFG+chs_hex+cfg.NULL_VAL+cfg.TERM
    start.serial_write(send_str)
