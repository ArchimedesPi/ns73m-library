// Written by Liam M.
// in 2013
//
// Thanks to Mike Yancey and skywodd
// for *way* better tuning code
// Original code + code by Mike Yancey, skywodd
// Redistributing under GPLv3

// To start:
// reset();
// forced_subcarrier_pilot_tone();
// unlockDetOff_2mw();
// 

#ifndef NS73M_Library_h
#define NS73M_Library_h

#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#define DEBUGMODE
// TNX skywodd
#define setRegister(reg, mask, value) ((reg) = ((reg) & ~(mask)) | (value))

class NS73M
{
	public:

		// Typedef ENUMS.
		// Some by skywodd.
		// TNX.

		typedef enum { 
			BAND0,
			BAND1,
			BAND2,
			BAND3
		}  FMBand_t;

		typedef enum {

		}

		NS73M(int _ck, int _da, int _la);
		void unlatch();
		void reset();
		void tune(long frequency);
		void forced_subcarrier_pilot_tone();
		void unlockDetOff_2mW();
		void setOscBand2();
		void setInputsAndState();
		void setChargePumps();
		void powerOutputLevel(byte powerLevel);
		int ck;
		int da;
		int la;
	private:
		byte registers[9];
		byte spi_send(byte address, byte data);
		void setBroadcastFreqRaw(byte low, byte high);
		void setBand(FMBand_t band);
};

#endif