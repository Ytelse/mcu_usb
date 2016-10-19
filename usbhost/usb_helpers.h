#ifndef __USB_HELPERS_H_
#define __USB_HELPERS_H_

#include <libusb.h>

/* Simple function that prints all USB devices attached to system */
void discover_devices(libusb_context* context);
/* Function that requests and prints names from all USB devices attached to the system */
void get_device_names(libusb_context* context);
/* Function that gets all endpoint descriptors from the supplied USB device, returns errorcode to denote success/failure */
int get_endpoint_descriptors(
	libusb_context* context, 				/* in */  /* USB context, use NULL for default */
	libusb_device_handle* dev_handle, 		/* in */  /* USB device handle */
	struct libusb_endpoint_descriptor** ep_descs, 	/* out */ /* Descriptors for all endpoints discovered on device */
	int* num_ep_descs 						/* out */ /* Number of endpoints discovered on device */
	);

#endif /* __USB_HELPERS_H_ */