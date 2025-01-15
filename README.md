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

    The interface communicates to and from the nRF25840 using UART. Pyserial's packetizer functions were initially used to provide
communication, but proved problematic. Utilization of the standard UART buffer was simple and provided required functions.
