#include "debug.h"
#include "ytelse_comm.h"
#include "usb_helpers.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <libusb.h>

// /* Data to send to device every tick */
static unsigned char tickMessage[] = "tick";
static unsigned char receiveBuffer[512];
static int tickMessageLength = 4;

extern bool pendingWrite;
extern bool pendingReceive;

/* enum for main loop flow control */
enum {
	USB_FINALIZE,
	USB_DEVICE_CONNECTED,
	USB_DEVICE_DISCONNECTED,
	USB_DEVICE_FOUND,
	USB_DEVICE_NOT_FOUND,
	USB_DEVICE_INTERFACE_CLAIMED, 
	USB_DEVICE_INTERFACE_BUSY, 
	USB_DEVICE_INTERFACE_NOT_FOUND
};

/* Function prototypes */
void mainloop(libusb_context* context);
void testSendRecv(libusb_context* context, libusb_device_handle* efm_handle, int num_messages);
void printStartupMsg(void);

int main(void) {
	printStartupMsg();

	libusb_context* context = NULL;
	int rc = 0;

	/* Initialize libusb */
	colorprint("Initializing libusb...", DEFAULT);
	rc = libusb_init(&context);
	if (rc) {
		colorprint("Failed to initialize libusb! Exiting...", RED);
		return EXIT_FAILURE;
	}

	libusb_set_debug(NULL, 3); //Set max debug level

	mainloop(context);

	libusb_exit(context);
}

void mainloop(libusb_context* context) {

	int status = USB_DEVICE_NOT_FOUND;
	int rc = 0;
	libusb_device_handle* efm_handle = NULL;

	while (status != USB_FINALIZE) {
		/* Get device handle */
		colorprint("Establishing connection to EFM32 USB device...", DEFAULT);
		// Keep trying until it connects
		while(get_ytelse_mcu_handle(context, &efm_handle));

		debugprint("Successfully got EFM32 handle!", GREEN);
		status = USB_DEVICE_FOUND;

		/* Claim device USB interface */
		int interface = 0;
		debugprint("Claiming EFM32 USB interface...", DEFAULT);
		while(status != USB_DEVICE_INTERFACE_CLAIMED) {
			// Request resource from OS 
			rc = libusb_claim_interface(efm_handle, interface);
			
			if (rc == LIBUSB_ERROR_NOT_FOUND) {
				/* 
					Interface not found on current device. Should not happen.
					Just in order to do something, increment interface number
					and try again.
					TODO: Test this approach
				*/
				debugprint("Interface not found! Trying another...", RED);
				status = USB_DEVICE_INTERFACE_NOT_FOUND;
				interface++;
				interface = interface % 10;
			} else if (rc == LIBUSB_ERROR_BUSY) {
				/* 
					USB device claimed by some other process. Should not be a problem.
					Do nothing, wait for it to become free.
				*/
				debugprint("Device busy, trying again...", YELLOW);
				status = USB_DEVICE_INTERFACE_BUSY;
				sleep(1); //Wait a second before trying again.
			} else if (rc == LIBUSB_ERROR_NO_DEVICE) {
				/*
					Device has disconnected, and we need to find it again.
					Break from interface claiming loop and return to waiting
					for it to connect.
				*/
				debugprint("Device disconnected!", RED);
				status = USB_DEVICE_NOT_FOUND;
				break;
			} else {
				debugprint("Successfully claimed EFM32 USB interface!", GREEN);
				status = USB_DEVICE_INTERFACE_CLAIMED;
			}
		}

		char nameBuffer[200];
		int nameLength = getDeviceName(efm_handle, nameBuffer, 200);
		if (nameLength < 0) {
			colorprint("Failed to get device name...", YELLOW);
		} else {
			colorprint("Connected to:", BLUE);
			colorprint(nameBuffer, BLUE);
		}
		colorprint("Connection established!", GREEN);

		/* 
			TODO: Get commands from stdin and direct program flow from there
		*/

		sendRecvWait(context, efm_handle);
		status = USB_FINALIZE;
	}
	//Close connection to device
	libusb_close(efm_handle);
}

void testSendRecv(libusb_context* context, libusb_device_handle* efm_handle, int num_messages) {
	memset(receiveBuffer, 0, 512);

	for (int i = 0; i < num_messages; i++) {
		debugprint("Attempting to send tick message to EFM32", BLUE);
		if (pendingWrite) {
			debugprint("Message already waiting in queue!", RED);
		} else {
			sendAsyncMessage(efm_handle, tickMessage, tickMessageLength);
			debugprint("Message sent!", GREEN);
		}

		if (pendingReceive) {
			debugprint("Still waiting for message!", RED);
		} else {
			debugprint("Setting up receive...", GREEN);
			memset(receiveBuffer, 0, 512);
			receiveAsyncMessage(efm_handle, receiveBuffer);
		}

		printf("receiveBuffer = %s\n", receiveBuffer);

		libusb_handle_events(context);
		usleep(500000);
	}
}

void sendRecvWait(libusb_context* context, libusb_device_handle* efm_handle) {
	memset(receiveBuffer, 0, 512);
	debugprint("Attempting to send tick message to Ytelse MCU", BLUE);
	while (1) {
		if (!pendingWrite) {
			sendAsyncMessage(efm_handle, tickMessage, tickMessageLength);
			break;
		}
	}
	debugprint("Message sent!", GREEN);
	while (1) {
		if (!pendingReceive) {
			receiveAsyncMessage(efm_handle, receiveBuffer);
			break;
		}
	}

	while (pendingReceive) {
		libusb_handle_events(context);
	}

	printf("receiveBuffer = %s\n", receiveBuffer);
}

void printStartupMsg(void) {

	/* TODO: Horrible ASCII art, make it better */
	printf("\n");
	printf("\n");
	printf("********************************************\n");
	printf("*           USB Host for Ytelse            *\n");
	printf("********************************************\n");
	printf("* /^^^^^|     /^^^^^^|       /^^^^^^^^^^|  *\n");
	printf("* | MCU | === | FPGA | ====  | USB Host |  *\n");
	printf("* |_____/     |______/       |__________   *\n");
	printf("*   ||                           ||        *\n");
	printf("*   ||                           ||        *\n");
	printf("*    ============================//        *\n");
	printf("********************************************\n");
	#ifdef DEBUG
	printf("*               DEBUG MODE                 *\n");
	printf("********************************************\n");
	#endif


}