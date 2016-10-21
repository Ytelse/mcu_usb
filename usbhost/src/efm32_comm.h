#ifndef __EFM32_COMM_H_
#define __EFM32_COMM_H_

#include <libusb.h>
#include <stdbool.h>

extern bool pendingWrite;

int get_efm32gg_usb_handle(libusb_context* context, libusb_device_handle** dev_handle);
void sendMessage(libusb_device_handle* efm_handle, unsigned char* message, int msgSize);
void receiveMessage(libusb_device_handle* efm_handle, unsigned char* buffer);

#endif /* __EFM32_COMM_H_ */