#include "efm32_comm.h"
#include "usb_helpers.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <libusb.h>

//Uncomment to list all USB devices on the system
//#define LIST

// /* Data to send to device every tick */
static unsigned char tickMessage[] = "tick";
static int tickMessageLength = 4;

extern bool pendingWrite;

/* Function prototypes */
void printStartupMsg(void);
void sendMsgs(libusb_device_handle* efm_handle, libusb_context* context);

int main(void) {
	printStartupMsg();

	libusb_context* context = NULL;
	int rc = 0;

	debugprint("Initializing libusb...", DEFAULT);
	rc = libusb_init(&context);
	assert(rc == 0);

	libusb_set_debug(NULL, 3); //Set max debug level
	#ifdef LIST
	debugprint("Discovering devices...", DEFAULT);
	discover_devices(context);

	debugprint("Attempting to get device names...", DEFAULT);
	get_device_names(context);
	#endif


	debugprint("Attempting to find EFM32 Example USB...", DEFAULT);
	libusb_device_handle* efm_handle = NULL;
	get_efm32gg_usb_handle(context, &efm_handle);

	debugprint("Successfully got EFM32 handle!", GREEN);

	debugprint("Claiming EFM32 USB interface 0...", DEFAULT);
	libusb_claim_interface(efm_handle, 0);
	debugprint("Successfully claimed EFM32 USB interface 0!", GREEN);

	while (1) {
		debugprint("Attempting to send tick message to EFM32", BLUE);
		if (pendingWrite) {
			debugprint("Message already waiting in queue!", RED);
		} else {
			sendMessage(efm_handle, tickMessage, tickMessageLength);
			debugprint("Message sent!", GREEN);
		}
		libusb_handle_events(context);
		usleep(500000);
	}


	sendMsgs(efm_handle, context);

	libusb_close(efm_handle);
	libusb_exit(NULL);
}

void sendMsgs(libusb_device_handle* efm_handle, libusb_context* context) {

	for (int i = 0; i < 20; i++) {
		debugprint("Attempting to send tick message to EFM32", BLUE);
		sendMessage(efm_handle, tickMessage, tickMessageLength);
		while (pendingWrite) {libusb_handle_events(context);}
 	}
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