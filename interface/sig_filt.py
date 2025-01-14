""" Signal Filtering module. Will be modified as needed
to meet requirements. """

# Import modules
from scipy import signal


def filter_signal2(sig):
    # wind = int(input("Enter Window: "))
    wind = 125
    # poly = int(input("Enter poly order: "))
    poly = 6
    filt_sig = signal.savgol_filter(sig,wind,poly)
    return filt_sig
