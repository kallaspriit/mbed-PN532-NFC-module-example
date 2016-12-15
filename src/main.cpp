#include "mbed.h"

#include "Application.hpp"

int main() {
	Serial serial(USBTX, USBRX);
	serial.baud(115200);

	printf("### Starting PN532 NFC module test application ###\n");

	Application application(&serial);

	application.run();
}
