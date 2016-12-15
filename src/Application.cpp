#include "Application.hpp"

Application::Application(Serial *serial) :
 	serial(serial),
	spi(PN532_SPI_MOSI, PN532_SPI_MISO, PN532_SPI_SCK),
	nfc(spi, PN532_SPI_CHIP_SELECT),
	led(LED1)
{}

void Application::run() {
	setup();

	timer.start();

	while (true) {
		int deltaUs = timer.read_us();
		timer.reset();

		loop(deltaUs);
	}
}

void Application::setup() {
	serial->printf("# setting up nfc module.. ");

	nfc.addEventListener(this);

	if (!nfc.initialize()) {
		error("module not found!");
	}

	serial->printf("done!\n");

	NFC::Version version = nfc.getVersion();

	printf("# found chip PN50%X, firmware version: %d.%d\n", version.hardwareVersion, version.softwareMajorVersion, version.softwareMinorVersion);
}

void Application::loop(int deltaUs) {
	led = !led;

	nfc.checkForTag();
}

// this is called on every read
void Application::onTagRead(NfcTag &tag) {
	serial->printf("# read '%s' tag with uid: %s\n", tag.getTagType().c_str(), tag.getUidString().c_str());

	showTagInfo(tag);
}

// this is called the first time a new tag is read
void Application::onTagEnter(NfcTag &tag) {
	serial->printf("# enter '%s' tag with uid: %s\n", tag.getTagType().c_str(), tag.getUidString().c_str());

	showTagInfo(tag);
}

// this is called when the tag is removed
void Application::onTagExit(std::string lastTagUid) {
	serial->printf("> exit '%s'\n", lastTagUid.c_str());
}

void Application::showTagInfo(NfcTag &tag) {
	if (tag.hasNdefMessage()) {
		NdefMessage message = tag.getNdefMessage();
		int recordCount = message.getRecordCount();

		for (int i = 0; i < recordCount; i++) {
			NdefRecord record = message.getRecord(i);
			std::string recordId = record.getId();
			std::string recordType = record.getType();
			std::string payload = nfc.getRecordPayload(record);

			serial->printf("# - %d type: %s '%s'\n", i, recordType.c_str(), payload.c_str());
		}
	} else {
		serial->printf("# found tag but it does not contain a message\n");
	}
}
