#pragma once

#include <arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h> //https://milesburton.com/Dallas_Temperature_Control_Library

#include "PR_GetTau.hpp"

#ifndef	PR_DS18B20_UPDATE_PERIOD_DEF
	#define	PR_DS18B20_UPDATE_PERIOD_DEF			1000	//[ms]
#endif

#ifndef	PR_DS18B20_INVALID_TEMPERATURE_VALUE
	#define PR_DS18B20_INVALID_TEMPERATURE_VALUE	123		//[degC] returned value if no correct measured temperature available 
#endif

#ifndef	PR_DS18B20_NUMBER_OF_SENSORS
	#define PR_DS18B20_NUMBER_OF_SENSORS			1		//number of sensors connected
#endif	
								
class PR_DS18B20 : public DallasTemperature {
        public:
			PR_DS18B20(const uint8_t pin, const uint32_t updatePeriod = PR_DS18B20_UPDATE_PERIOD_DEF);
            
			void		setUpdatePeriod(const uint32_t updatePeriod);
			bool		request();
			
			bool		loop();
			float 		getC();
			float 		getC(const uint8_t sensorID);
			uint32_t	getTimeStamp();	   
			
        protected:
			OneWire         _oneWire;

			uint32_t		_updatePeriod;									//[ms]
			bool 			_isRequested 		= false;
			float 			_lastMeasuredTemp[PR_DS18B20_NUMBER_OF_SENSORS];

			uint32_t		_lastTime 			= 0;
			uint32_t		_lastTempTimeStamp;
};

PR_DS18B20::PR_DS18B20(const uint8_t pin, const uint32_t updatePeriod) : DallasTemperature() , _oneWire(pin) {
	
	setOneWire(&_oneWire);
	begin();
	setWaitForConversion(false);		//switch on async mode

	for (uint8_t n=0; n < PR_DS18B20_NUMBER_OF_SENSORS; n++) {
		_lastMeasuredTemp[n] = PR_DS18B20_INVALID_TEMPERATURE_VALUE;
	}
	_lastTempTimeStamp = millis();
	_updatePeriod = updatePeriod;
}

void	PR_DS18B20::setUpdatePeriod(const uint32_t updatePeriod) {	_updatePeriod = updatePeriod; }

float 	PR_DS18B20::getC()	{	return getC(0);	}

float 	PR_DS18B20::getC(const uint8_t sensorID)	{	

	if (sensorID < PR_DS18B20_NUMBER_OF_SENSORS) {
		return _lastMeasuredTemp[sensorID];	
	} 
	else return PR_DS18B20_INVALID_TEMPERATURE_VALUE;
			 
}

uint32_t	PR_DS18B20::getTimeStamp() {	return _lastTempTimeStamp; }


bool	PR_DS18B20::request() {
	
	if ( _isRequested ) return false;
	
	_lastTime -= _updatePeriod;	//force requestTemperatures() in loop()
	loop();
	return true;
}

//@ return true if new temperature read out
bool	PR_DS18B20::loop() {	

	if (_isRequested) {
		if ( PR_getTauMS(_lastTime) > millisToWaitForConversion(getResolution()) ) {
				
			for (uint8_t n = 0; n <PR_DS18B20_NUMBER_OF_SENSORS; n++) {	
				
				_lastMeasuredTemp[n] = getTempCByIndex(n);
				
				if (_lastMeasuredTemp[n] == DEVICE_DISCONNECTED_C) _lastMeasuredTemp[n] = PR_DS18B20_INVALID_TEMPERATURE_VALUE;
			}
			_lastTime = PR_getNowMS();
			_lastTempTimeStamp = _lastTime; 
			_isRequested = false;
			return true;
		}			  
	}
	else {
		if ( PR_getTauMS(_lastTime) > _updatePeriod ) {
				requestTemperatures();
				_lastTime = PR_getNowMS();
				_isRequested = true;
		}
	}
	return false;
}





