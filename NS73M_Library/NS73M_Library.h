#ifndef NS73M_Library_h
#define NS73M_Library_h

#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

class NS73M
{
	public:
		NS73M(int _ck, int _da, int _la);
		void unlatch();
		void reset();
		void forced_subcarrier_pilot_tone();
		void unlockDetOff_2mW();
		void setBroadcastFreq(byte low, byte high);
		void setOscBand2();
		void setInputsAndState();
		void setChargePumps();
		int ck;
		int da;
		int la;
	private:
		byte spi_send(byte address, byte data);
};

#endif