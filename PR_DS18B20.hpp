#pragma once

#include <arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h> //https://milesburton.com/Dallas_Temperature_Control_Library

#include "PR_GetTau.hpp"

#ifndef	PR_DS18B20_PRECISION_DEFAULT
	#define	PR_DS18B20_PRECISION_DEFAULT		9		//[9...12]
#endif

#ifndef	PR_DS18B20_UPDATE_PERIOD_DEF
	#define	PR_DS18B20_UPDATE_PERIOD_DEF		15		//[ms]
#endif

#ifndef	PR_DS18B20_INVALID_TEMPERATURE_VALUE
	#define	PR_DS18B20_INVALID_TEMPERATURE_VALUE	-127	//[degC] returned value if no correct measured temperature available 
#endif

#define	DS18B20_CONV_TIME						2000	//[ms]

class PR_DS18B20 {
        public:
			PR_DS18B20( const uint8_t pin, const uint8_t resolution = PR_DS18B20_PRECISION_DEFAULT, 
						const int16_t invalidTemp = PR_DS18B20_INVALID_TEMPERATURE_VALUE );
            
			void 	setResolution(const uint8_t resolution);
			void	setUpdatePeriod(const uint32_t updatePeriod);
			
			void	loop();
			
			float 	get();
			
        protected:
    		OneWire                 oneWire;    			//for shadow  temp sensor
			DallasTemperature       sensors;       		// Pass our oneWire reference to Dallas Temperature. 
			
			uint32_t				_lastTime;
			float 					_lastMeasuredTemp;
			int16_t					_invalidTemp;
			uint32_t				_updatePeriod;		//[ms]
			
};

PR_DS18B20::PR_DS18B20( const uint8_t pin, const uint8_t resolution, const int16_t invalidTemp )
	:	oneWire(pin), 
		sensors(oneWire),		// Pass our oneWire reference to Dallas Temperature. 
		_invalidTemp(invalidTemp)
	{
	sensors.begin();
	sensors.setResolution(resolution);
	sensors.setWaitForConversion(false);	//switch on async mode

	_lastMeasuredTemp = _invalidTemp;
	_updatePeriod = PR_DS18B20_UPDATE_PERIOD_DEF;
}

void 	PR_DS18B20::setResolution(const uint8_t resolution ) {

	sensors.setResolution(resolution);
}   

void	PR_DS18B20::setUpdatePeriod(const uint32_t updatePeriod) {	_updatePeriod = updatePeriod; }

float 	PR_DS18B20::get()	{	return _lastMeasuredTemp;	}

void	PR_DS18B20::loop() {	

	typedef enum TempReqState {
		REQUESTED			= 2,
		READY 				= 1, 
		IDLE 				= 0 
	} TempReqState_t;

	//request and read temp asynchronous
	static TempReqState_t state = IDLE;
	
	switch (state) {
		case IDLE:
			if ( PR_getTauMS(_lastTime) > _updatePeriod ) {
				sensors.requestTemperatures();
				_lastTime = PR_getNowMS();
				state = REQUESTED;
			}
		break;
		case REQUESTED:
			if ( PR_getTauMS(_lastTime) > DS18B20_CONV_TIME ) {
				_lastMeasuredTemp = sensors.getTempCByIndex(0);
				_lastTime = PR_getNowMS();
				state = IDLE;
			}
		break;
	}
}





