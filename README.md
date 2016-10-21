# mcu_usb

This repo contains files for communicating with EFM32GG over USB.

#usbdevice_dk

The usbdevice_dk folder contains the files necessary to build and flash a USB device program for EFM32GG DK3750. 

#usbdevice_stk

The usbdevice_stk folder contains more or less the same as usbdevice_dk, just for STK3700.

#usbhost

The usbhost folder contains the files necessary to build a program to run a USB host on the computer you connect the EFM32GG USB device to. There is no need to connect the device or run the program in any specific order, everything should work automagically.