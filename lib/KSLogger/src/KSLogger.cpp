#include "KSLogger.h"


// implementations of abstract functions from Stream
int KSSerialAndTelnet::read() {
	return -1;
/*
	if (disconnected())
		return -1;
	return client.read();
	*/
}

int KSSerialAndTelnet::available() {
	return 0;
/*
	if (disconnected())
		return 0;
	return client.available();
	*/
}

int KSSerialAndTelnet::peek() {
	return -1;
	/*
	if (disconnected())
		return -1;
	return client.peek();
	*/
}


size_t KSSerialAndTelnet::write(uint8_t val) {
	if (_telnet)
		_telnet->write(val);
	return Serial.write(val);
}

size_t KSSerialAndTelnet::write(const uint8_t *buf, size_t size) {
	if (_telnet)
		_telnet->write(buf, size);
  	return Serial.write(buf, size);
}

void KSSerialAndTelnet::flush() {
	if (_telnet)
		_telnet->flush();
  	Serial.flush();
}


// instance
#if defined(USE_TELNET2)
	KSSerialAndTelnet SerialAndTelnet(&Telnet2);
#else
	KSSerialAndTelnet SerialAndTelnet(&Telnet);
#endif

