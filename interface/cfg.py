""" Module for universal variables and configurations """

# Universal variable for serial interface.
ser = ""

# Universal dictionary to hold register addresses.
# NEED TO ADD REMAINING REG ADDRESSES!
REGS = {
    "STATUS": "00",
    "DATA_CH1L": "01",
    "DATA_CH1H": "02",
    "DATA_CH2L": "03",
    "DATA_CH2H": "04",
    "DATA_CH3L": "05",
    "DATA_CH3H": "06",
    "DATA_CH4L": "07",
    "DATA_CH4H": "08",
    "CONTROL_1": "09",
    "CONTROL_2": "0A",
    "CONTROL_3": "0B",
    "CONTROL_4": "0C",
    "CH1CONFIG_A": "0D",
    "CH1CONFIG_B": "0E",
    "CH2CONFIG_A": "0F",
    "CH2CONFIG_B": "10",
    "CH3CONFIG_A": "11",
    "CH3CONFIG_B": "12",
    "CH4CONFIG_A": "13",
    "CH4CONFIG_B": "14"
}


# Univeral CONSTANTS for operation identifiers
UPDATE_REG = "U"
READ_REG = "R"
PRESET_CFG = "P"
COLLECT_DATA = "C"

TERM = "\x00"
NULL_VAL = "00"
