#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "NS73M_Library.h"


NS73M::NS73M(int _ck, int _da, int _la) {
	this->ck = _ck;
	this->da = _da;
	this->la = _la;
	
	pinMode(this->ck, OUTPUT);
	pinMode(this->da, OUTPUT);
	pinMode(this->la, OUTPUT);
	
}

void NS73M::unlatch() {
	digitalWrite(this->la, LOW);
}

void NS73M::reset() {
	this->spi_send(0x0E, B00000101);
}

void NS73M::forced_subcarrier_pilot_tone() {
	this->spi_send(0x01, B10110100);
}

void NS73M::unlockDetOff_2mW() {
	this->spi_send(0x02, B00000011);
}

void NS73M::setBroadcastFreq(byte low, byte high) {
	this->spi_send(0x03, low);
	this->spi_send(0x04, high);
}

void NS73M::setOscBand2() {
	this->spi_send(0x08, B00011010);
}

void NS73M::setInputsAndState() {
	this->spi_send(0x00, B10100001);
}

void NS73M::setChargePumps() {
	this->spi_send(0x06, B00011110);
}

byte NS73M::spi_send(byte address, byte data) {
	byte incoming_byte, x;
	byte addrbit, databit;
	
	this->unlatch();
	
	for (x = 0 ; x < 4 ; x++) {
		digitalWrite(this->ck, LOW);
		
		addrbit = (address >> x) & 1;
		digitalWrite(this->da, (addrbit == 1) ? HIGH : LOW );
		
		digitalWrite(this->ck, HIGH);
	}
	
	for (x = 0 ; x < 8 ; x++) {
		digitalWrite(this->ck, LOW);
		
		databit = (data >> x) & 1;
		digitalWrite(this->da, (databit == 1) ? HIGH : LOW );
		
		digitalWrite(this->ck, HIGH);
		
	}
	
	delayMicroseconds(1);
	
	digitalWrite(this->la, HIGH);
	delayMicroseconds(5);
	digitalWrite(this->la, LOW);
	
	return(incoming_byte);
	
}
	
