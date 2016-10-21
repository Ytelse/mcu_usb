#include "efm32_comm.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Device IDs
#define VENDOR_ID 0x10c4
#define PRODUCT_ID 0x0007

// Device configuration and interface ID.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN  0x81
#define EP_OUT 0x01

//Buffer size
#define BUF_SIZE 64

/* Error codes */
enum {EFM32_DEVICE_USB_OPEN_SUCCESS, EFM32_DEVICE_NOT_FOUND, EFM32_DEVICE_USB_OPEN_FAILURE};

// /* Buffer to receive incoming messages */
// static char receiveBuffer[BUF_SIZE];

// /* Indicates whether a write (OUT transaction is currently pending */
bool pendingWrite = false;
bool pendingReceive = false;

// void *async_read_context = NULL;
// void *async_write_context = NULL;

int get_efm32gg_usb_handle(libusb_context* context, libusb_device_handle** dev_handle) {
	libusb_device** device_list = NULL;
	libusb_device* efm_dev = NULL;
	ssize_t count = 0;
	int rc = 0;
	
	count = libusb_get_device_list(context, &device_list);
	if (count < 0) {
		colorprint("FATAL ERROR: USB Device list has fewer than 0 entries!", RED);
		return EFM32_DEVICE_NOT_FOUND;
	}

	for (size_t i = 0; i < count; i++) {
		libusb_device* device = device_list[i];
		struct libusb_device_descriptor desc;

		rc = libusb_get_device_descriptor(device, &desc);
		if (rc) {continue;} //Fail quitely, we will keep trying from main loop

		if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID) {
			efm_dev = device;
		}

	}

	if (!efm_dev) {
		debugprint("Could not find EFM32 USB device.", RED);
		return EFM32_DEVICE_NOT_FOUND;
	}
	
	rc = libusb_open(efm_dev, dev_handle);
	if (rc) {
		libusb_free_device_list(device_list, 1);
		debugprint("Failed to open EFM32 USB device!", RED);
		return EFM32_DEVICE_USB_OPEN_FAILURE;
	}

	libusb_free_device_list(device_list, 1);
	return EFM32_DEVICE_USB_OPEN_SUCCESS;
}

static void LIBUSB_CALL tickWriteFinished(struct libusb_transfer* transfer) {
	debugprint("Tick write finished!", CYAN);
	libusb_free_transfer(transfer);
	pendingWrite = false;
}

void sendMessage(libusb_device_handle* efm_handle, unsigned char* message, int msgSize) {
	struct libusb_transfer* transfer = NULL;
	int rc;
	//Allocate a transfer with 0 isochronous packages
	transfer = libusb_alloc_transfer(0);

	libusb_fill_bulk_transfer(
		transfer,
		efm_handle,
		EP_OUT,
		message,
		msgSize,
		&tickWriteFinished,
		NULL,
		1000
	);

	rc = libusb_submit_transfer(transfer);
	if (rc == LIBUSB_ERROR_NO_DEVICE) {
		debugprint("Device has disconnected!", RED);
	} else if (rc == LIBUSB_ERROR_BUSY) {
		debugprint("transfer already submitted!", YELLOW);
	} else if (rc == LIBUSB_ERROR_NOT_SUPPORTED){
		debugprint("Transfer flags not supported!", RED);
	} else {
		pendingWrite = true;
	}
}

static void LIBUSB_CALL dataReceived(struct libusb_transfer* transfer) {
	debugprint("Received data from device!", MAGENTA);
	libusb_free_transfer(transfer);
	pendingReceive = false;
}

void receiveMessage(libusb_device_handle* efm_handle, unsigned char* buffer) {
	struct libusb_transfer* transfer = NULL;
	int rc;

	transfer = libusb_alloc_transfer(0);

	libusb_fill_bulk_transfer(
		transfer,
		efm_handle,
		EP_IN,
		buffer,
		64,
		&dataReceived,
		NULL,
		10000
	);

	rc = libusb_submit_transfer(transfer);

	/* TODO: Fix error handling */
	if (rc) {

	} else {
		
	}

	pendingReceive = true;
}
