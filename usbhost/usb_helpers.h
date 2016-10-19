#ifndef __USB_HELPERS_H_
#define __USB_HELPERS_H_

#include <libusb.h>

/* Simple function that prints all USB devices attached to system */
void discover_devices(libusb_context* context);
/* Function that requests and prints names from all USB devices attached to the system */
void get_device_names(libusb_context* context);

#endif /* __USB_HELPERS_H_ */