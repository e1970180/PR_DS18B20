#pragma once

#include <arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h> //https://milesburton.com/Dallas_Temperature_Control_Library

#include "PR_GetTau.hpp"

#ifndef	PR_DS18B20_UPDATE_PERIOD_DEF
	#define	PR_DS18B20_UPDATE_PERIOD_DEF			15		//[ms]
#endif

#ifndef	PR_DS18B20_INVALID_TEMPERATURE_VALUE
	#define PR_DS18B20_INVALID_TEMPERATURE_VALUE	123		//[degC] returned value if no correct measured temperature available 
#endif

class PR_DS18B20 : public DallasTemperature {
        public:
			PR_DS18B20(const uint8_t pin, const uint32_t updatePeriod = PR_DS18B20_UPDATE_PERIOD_DEF);
            
			void	setUpdatePeriod(const uint32_t updatePeriod);
			
			float	loop();
			float 	getC();
			
        protected:
			OneWire         _oneWire;
			float 			_lastMeasuredTemp;
			uint32_t		_updatePeriod;			//[ms]
			
};

PR_DS18B20::PR_DS18B20(const uint8_t pin, const uint32_t updatePeriod) : DallasTemperature() , _oneWire(pin) {
	
	setOneWire(&_oneWire);
	begin();
	setWaitForConversion(false);		//switch on async mode

	_lastMeasuredTemp = PR_DS18B20_INVALID_TEMPERATURE_VALUE;
	_updatePeriod = updatePeriod;
}

void	PR_DS18B20::setUpdatePeriod(const uint32_t updatePeriod) {	_updatePeriod = updatePeriod; }

float 	PR_DS18B20::getC()	{	return _lastMeasuredTemp;	}

float	PR_DS18B20::loop() {	

	static bool 	isRequested = false;
	static uint32_t	_lastTime = 0;
	
	if (isRequested) {
		if ( PR_getTauMS(_lastTime) > millisToWaitForConversion(getResolution()) ) {
				
				_lastMeasuredTemp = getTempCByIndex(0);
				if (_lastMeasuredTemp == DEVICE_DISCONNECTED_C) _lastMeasuredTemp = PR_DS18B20_INVALID_TEMPERATURE_VALUE;		
				_lastTime = PR_getNowMS();
				isRequested = false;
			}
	}
	else {
		if ( PR_getTauMS(_lastTime) > _updatePeriod ) {
				requestTemperatures();
				_lastTime = PR_getNowMS();
				isRequested = true;
		}
	}
	return _lastMeasuredTemp;
}





