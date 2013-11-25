#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "NS73M_Library.h"

// TNX skywodd for the next bunch of #defines
#define setRegister(reg, mask, value) ((reg) = ((reg) & ~(mask)) | (value))

/* Registers address */
#define REG0 0x00
#define REG1 0x01
#define REG2 0x02
#define REG3 0x03
#define REG4 0x04
// No register 5
#define REG6 0x06
// No register 7
#define REG8 0x08
#define REG14 0x0E

// REGISTER 0 ----------------------------------
// Power SW
#define PW_MASK 1
#define PW_ON 1
#define PW_OFF 0

// Crystal SW
#define PDX_MASK 2
#define PDX_ON 0
#define PDX_OFF 2

// Mute SW
#define MUTE_MASK 4
#define MUTE 4
#define UNMUTE 0 

// Pre-emphasis SW
#define PE_MASK 16
#define PE_ON 0
#define PE_OFF 16

// Pre-emphasis selection
#define EMS_MASK 32
#define EMS_50 0  // Europe
#define EMS_75 32 // USA

// Audio input level
#define INPUT_MASK 192
#define INPUT_LOW 0
#define INPUT_MEDIUM 64
#define INPUT_HIGH 128

// REGISTER 1 ----------------------------------
// Pilot tune
#define PLT_MASK 8
#define PLT_ON 0
#define PLT_OFF 8

// Force subcarrier
#define SUBC_MASK 64
#define SUBC_ON 0
#define SUBC_OFF 64

// REGISTER 2 ----------------------------------
// Tx power level
#define POWER_MASK 3
#define POWER_LOW 1
#define POWER_MEDIUM 2
#define POWER_HIGH 3

// Locked Detect
#define ULD_MASK 4
#define ULD_ON 4
#define ULD_OFF 0

// REGISTER 6 ----------------------------------
// Charge pumps
#define CIA_MASK 30
#define CIA_1_25 2
#define CIA_80 6
#define CIB_320 24

// REGISTER 8 ----------------------------------
#define FREQUENCY_BAND_MASK 3

// End of the #defines by skywodd.



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

void NS73M::tune(long frequency) {

	// The variable to store the computed channel in
	int computedChannel;

	// Decide what band we're broadcasting on!
	if (aFrequency < 88500000) {                       // Band 3
    	i2c_send(0x08, B00011011);
    	#ifdef DEBUGMODE
    	Serial.println("Band 3");
    	#endif
    }  
    else if (aFrequency < 97900000) {                 // Band 2
    	i2c_send(0x08, B00011010);
    	#ifdef DEBUGMODE
    	Serial.println("Band 2");
    	#endif
    }
    else if (aFrequency < 103000000) {                  // Band 1 
    	i2c_send(0x08, B00011001);
    	#ifdef DEBUGMODE
    	Serial.println("Band 1");
    	#endif
    }
    else {
    	// Must be OVER 103.000.000,                    // Band 0
    	i2c_send(0x08, B00011000);
    	#ifdef DEBUGMODE
    	Serial.println("Band 0");
    	#endif
    }

    // Thanks Mike Yancey and skywodd for this code!

    // Do some math to figure out what channel we're transmitting on
    computedChannel = (frequency + 304000) / 8192;
    // Figure out the low byte to put into Register 3
  	byte low_channel_byte = new_frequency & 255;
  	// Figure out the high byte to put into Register 4
  	byte high_channel_byte = new_frequency >> 8;
  	this->setBroadcastFreqRaw(low_channel_byte, high_channel_byte);

  	this->reset();

  	#ifdef DEBUGMODE
  	Serial.print("Tuned to ");
  	Serial.println(frequency);
  	

}

void NS73M::setBroadcastFreqRaw(byte low, byte high) {
	this->spi_send(0x03, low);
	this->spi_send(0x04, high);
}

void NS73M::setOscBand2() {
	this->spi_send(0x08, B00011010);
}

//void NS73M::setBand(FMBand_t) {
//	setRegister(registers[8], FREQUENCY_BAND_MASK, frequency_band);
//}

void NS73M::setInputsAndState() {
	this->spi_send(0x00, B10100001);
}

void NS73M::setChargePumps() {
	this->spi_send(0x06, B00011110);
}

void NS73M::powerOutputLevel(byte powerLevel) {
  switch(powerLevel) {
    case OUTPUT_LEVEL_LOW:
	  setRegister(registers[2], POWER_MASK, POWER_LOW);
	  break;
    case OUTPUT_LEVEL_MEDIUM:
	  setRegister(registers[2], POWER_MASK, POWER_MEDIUM);
	  break;
	case OUTPUT_LEVEL_HIGH:
	  setRegister(registers[2], POWER_MASK, POWER_HIGH);
	  break;
  }
  spi_send(REG2, registers[2]);
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
	
