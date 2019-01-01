


#define PR_DS18B20_NUMBER_OF_SENSORS			2		// 

#include "PR_DS18B20.hpp"

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

#define	UPDATE_PERIOD		6000		//[ms]

// Pass our oneWire reference to Dallas Temperature. 
PR_DS18B20 sensor(ONE_WIRE_BUS, UPDATE_PERIOD);


void setup(void)
{
  // start serial port
  Serial.begin(115200);
  Serial.println("PR_DS18B20 v2 Dallas Temperature IC Control Library Demo");

}


void loop(void)
{ 
	float 	temperature0, temperature1;
	bool	isNewTempAvailable;
	
	isNewTempAvailable = sensor.loop();
	
	temperature0 = sensor.getC();
	temperature1 = sensor.getC(1);
	
	Serial.print("temperature0= ");	Serial.println(temperature0);
	Serial.print("temperature1= ");	Serial.println(temperature1);
	
	if (isNewTempAvailable) { 
		Serial.print("new temperature read at time = ");	Serial.println(sensor.getTimeStamp() );	
	}
	
	delay(3000);

}