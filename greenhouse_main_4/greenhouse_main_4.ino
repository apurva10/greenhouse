#include "Wire.h"
#include "SeeedOLED.h"
#include "DHT.h"
#include "SI114X.h"
#include "MoistureSensor.h"

//Pin setup Arduino UNO board.
#define moistureSensorPort1 A0
#define moistureSensorPort2 A1
#define moistureSensorPort3 A2
#define moistureSensorPort4 A3
#define DHTPIN 4
#define rotaryEncoderOutpA 11
#define rotaryEncoderOutpB 10
#define pumpRelay 8
#define pumpButton 7
#define flowSensor 3
#define waterLevelSwitch 12
#define lightRelay 6


/*
################### ARDUINO UNO ###################
#|_____________ANALOG_IN____________POWER________|#
#|             | | | | | | | | | <> | | | | | | ||#
#||A0|      |12C|     |12C|     |12C|     |12C|  |#
#|-----------------------------------------------|#
#||A1|      |D8|      |D7|      |D6|      |D5|   |#
#|-----------------------------------------------|#
#||A2|      |D4|      |D3|      |D2|      |UART| |#
#|-----------------------------------------------|#
#||A3| | | | | | | | | | | | <> | | | | | | | | ||#
#|_____DIGITAL_(PWM_~)___________________________|#
################### ARDUINO UNO ###################
*/

//Global variables.
//Moisture sensors.
MoistureSensor moistureSensor1;           //Create moistureSensor1 from the MoistureSensor class.
MoistureSensor moistureSensor2;           //Create moistureSensor2 from the MoistureSensor class.
MoistureSensor moistureSensor3;           //Create moistureSensor2 from the MoistureSensor class.
MoistureSensor moistureSensor4;           //Create moistureSensor2 from the MoistureSensor class.
int moistureValue1;                       //Individual moisture sensor value for moisture sensor 1.
int moistureValue2;                       //Individual moisture sensor value for moisture sensor 2.
int moistureValue3;                       //Individual moisture sensor value for moisture sensor 3.
int moistureValue4;                       //Individual moisture sensor value for moisture sensor 4.
int moistureValue;                        //Mean value of all 4 moisture sensors.
bool moistureDry1 = false;                //Activates warning message on display. 'true' if soil sensor1 is too dry.
bool moistureWet1 = false;                //Activates warning message on display. 'true' if soil sensor1 is too wet. 
bool moistureDry2 = false;                //Activates warning message on display. 'true' if soil sensor2 is too dry.
bool moistureWet2 = false;                //Activates warning message on display. 'true' if soil sensor2 is too wet. 
bool moistureDry3 = false;                //Activates warning message on display. 'true' if soil sensor3 is too dry.
bool moistureWet3 = false;                //Activates warning message on display. 'true' if soil sensor3 is too wet. 
bool moistureDry4 = false;                //Activates warning message on display. 'true' if soil sensor4 is too dry.
bool moistureWet4 = false;                //Activates warning message on display. 'true' if soil sensor4 is too wet. 
bool moistureDry = false;
bool moistureWet = false;

//Temperature and humidity sensor.
const uint8_t DHTTYPE = DHT11;            //DHT11 = Arduino UNO model is being used.
DHT humiditySensor(DHTPIN, DHTTYPE);      //Create humidity sensor from DHT class.
float tempValue;
//float humidValue;

//Light sensor.
SI114X lightSensor;                       //Light sensor object created.
uint16_t lightValue;                      //Light read out, probably presented in lumens unit. CHECK WHICH SCALE THE LIGHT IS PRESENTED!!!
uint16_t uvValue;                       
//uint16_t irValue;                       //IR read out not in use.

//Water pump and flow sensor.
volatile int rotations;
int flowValue;
bool pumpState = false;                   //Variable is set to 'true' when water pump is running.

//Water level switch.
bool waterLevelValue;

//Rotary encoder to adjust temperature threshold.
int tempPosition = 56;
int aLastState;

//Delay variable to check value of millis() counter be used as delay without stopping program execution.
unsigned long timePrev = 0;
unsigned long timeNow = 0;  
int timeDiff;

const unsigned char greenhouse[] PROGMEM= {
  //Startup image 1.
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x60, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x87, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x03,
0xff, 0xfc, 0x00, 0x00, 0x00, 0x80, 0xf0, 0x20, 0x00, 0x00, 0x80, 0xc0, 0xc0,
0x60, 0xe0, 0xc0,
0xc0, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0x60, 0xe0, 0xc0, 0xc0, 0x80, 0x00,
0x00, 0x80, 0xc0,
0xc0, 0xe0, 0x60, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0x60,
0xe0, 0xc0, 0xc0,
0x80, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xc0, 0xc0, 0xf8, 0xf8, 0x00,
0x00, 0x00, 0x00,
0x00, 0xc0, 0xc0, 0xe0, 0x60, 0xc0, 0xc0, 0x80, 0x00, 0xc0, 0xf0, 0xf0, 0xf0,
0xc0, 0x00, 0xc0,
0xc0, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0xe0,
0xe0, 0xc0, 0xc0,
0xf8, 0xf8, 0x00, 0xd8, 0xd8, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0xe0, 0x60, 0xc0,
0xc0, 0x80, 0x00,
0x00, 0x03, 0x0f, 0x1e, 0x3c, 0x70, 0xe3, 0xcf, 0x9f, 0x30, 0x00, 0x00, 0x00,
0x00, 0x70, 0xbf,
0xcf, 0xe3, 0x70, 0x78, 0x3e, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x33, 0x77, 0x66,
0x66, 0x66, 0x6c,
0x7d, 0x18, 0x00, 0x1f, 0x3f, 0x76, 0x66, 0x66, 0x66, 0x76, 0x37, 0x07, 0x00,
0x0f, 0x3f, 0x7f,
0x66, 0x66, 0x66, 0x66, 0x77, 0x27, 0x07, 0x00, 0x1f, 0x3f, 0x76, 0x66, 0x66,
0x66, 0x76, 0x37,
0x07, 0x00, 0x0f, 0x3f, 0x71, 0x60, 0x60, 0x60, 0x60, 0x31, 0x7f, 0x7f, 0x00,
0x00, 0x00, 0x00,
0x11, 0x37, 0x67, 0x66, 0x66, 0x6c, 0x7d, 0x38, 0x00, 0x00, 0x3f, 0x7f, 0x3f,
0x00, 0x00, 0x1f,
0x3f, 0x70, 0x60, 0x60, 0x70, 0x7f, 0x7f, 0x00, 0x0f, 0x3f, 0x71, 0x60, 0x60,
0x60, 0x60, 0x31,
0x7f, 0x7f, 0x00, 0x7f, 0x7f, 0x00, 0x06, 0x1f, 0x3b, 0x60, 0x60, 0x60, 0x60,
0x71, 0x3f, 0x1f,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x01, 0x01,
0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x48, 0x48, 0x48, 0xb0,
0x00, 0xc0, 0x20,
0x20, 0x20, 0xc0, 0x00, 0xc0, 0x20, 0x20, 0x20, 0xc0, 0x00, 0x40, 0xa0, 0xa0,
0xa0, 0x20, 0x00,
0x00, 0x20, 0xf0, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
0x08, 0xf8, 0x08,
0x08, 0x00, 0xc0, 0x20, 0x20, 0x20, 0xf8, 0x00, 0xc0, 0xa0, 0xa0, 0xa0, 0xc0,
0x00, 0x20, 0xa0,
0xa0, 0xa0, 0xc0, 0x00, 0x40, 0xa0, 0xa0, 0xa0, 0x20, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x48, 0x48, 0x48, 0x08, 0x00, 0x20,
0x40, 0x80, 0x40,
0x20, 0x00, 0x00, 0x20, 0xf0, 0x20, 0x20, 0x00, 0xc0, 0xa0, 0xa0, 0xa0, 0xc0,
0x00, 0xe0, 0x00,
0x20, 0x20, 0xc0, 0x00, 0xc0, 0x20, 0x20, 0x20, 0xf8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02, 0x01,
0x00, 0x01, 0x02,
0x02, 0x02, 0x01, 0x00, 0x01, 0x02, 0x02, 0x02, 0x01, 0x00, 0x02, 0x02, 0x02,
0x02, 0x01, 0x00,
0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
0x02, 0x03, 0x02,
0x02, 0x00, 0x01, 0x02, 0x02, 0x02, 0x03, 0x00, 0x01, 0x02, 0x02, 0x02, 0x00,
0x00, 0x01, 0x02,
0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00, 0x00, 0x08, 0x06,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x82, 0x02, 0x00, 0x02,
0x01, 0x01, 0x01,
0x02, 0x00, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x01, 0x02, 0x02, 0x02, 0x00,
0x00, 0x03, 0x00,
0x00, 0x00, 0x03, 0x00, 0x01, 0x02, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x82, 0x8c, 0x60, 0x1c, 0x02, 0x00, 0x1c, 0x22, 0x22, 0x22,
0x1c, 0x00, 0x1e,
0x20, 0x20, 0x00, 0x3e, 0x00, 0x00, 0x3e, 0x04, 0x02, 0x02, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x3e, 0x04, 0x02, 0x02, 0x00, 0x1c, 0x2a, 0x2a, 0x2a, 0x0c, 0x00,
0x12, 0x2a, 0x2a,
0x2a, 0x1c, 0x20, 0x1c, 0x22, 0x22, 0x22, 0x14, 0x00, 0x3f, 0x00, 0x02, 0x02,
0x3c, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00 
};

const unsigned char features[] PROGMEM= {
  //Startup image 2.
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void startupDisplay() {
  //Initialize the OLED Display and show startup images.
  Wire.begin();
  SeeedOled.init();

  SeeedOled.clearDisplay();       //Clear display and set start position to top left corner.
  SeeedOled.setHorizontalMode();
  SeeedOled.setNormalDisplay();   //Set display to normal mode (non-inverse mode).
  SeeedOled.setPageMode();        //Set addressing mode to Page Mode.

/*
  //Show startup images when starting up the system.
  //Startup image 1.
  SeeedOled.drawBitmap(greenhouse, (128*64)/8);   //Show greenhouse logo. Second parameter in drawBitmap function specifies the size of the image in bytes. Fullscreen image = 128 * 64 pixels / 8.
  delay(4000);                                    //Image shown for 4 seconds.
  SeeedOled.clearDisplay();                       //Clear the display.

  //Startup image 2.
  SeeedOled.drawBitmap(features, (128*64)/8);     //Show greenhouse logo. Second parameter in drawBitmap function specifies the size of the image in bytes. Fullscreen image = 128 * 64 pixels / 8.
  delay(3000);                                    //Image shown for 3 seconds.
  SeeedOled.clearDisplay();                       //Clear the display.
*/
}

void valuesDisplay() {
  //Clear redundant value figures from previous read out for all sensor values.
  SeeedOled.setTextXY(0, 42);
  SeeedOled.putString("   ");
  SeeedOled.setTextXY(1, 42);
  SeeedOled.putString("      ");
  SeeedOled.setTextXY(2, 42);
  SeeedOled.putString("   ");
  SeeedOled.setTextXY(3, 42);
  SeeedOled.putString("      ");
  SeeedOled.setTextXY(4, 42);
  SeeedOled.putString("    ");
  SeeedOled.setTextXY(5, 42);
  SeeedOled.putString("      ");

  //Printing read out values from the greenhouse to display.
  /*
  ========================
  |Moisture sensor value.|
  ========================*/
  SeeedOled.setTextXY(0, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("Moisture: ");        //Print string to display.
  SeeedOled.setTextXY(0, 42);
  SeeedOled.putNumber(moistureValue);       //Print moisture value to display.

  /*
  ====================
  |Temp sensor value.|
  ====================*/
  SeeedOled.setTextXY(1, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("Temp: ");            //Print string to display.
  SeeedOled.setTextXY(1, 42);
  SeeedOled.putNumber(tempValue);           //Print temperature value to display.

  /*
  ========================
  |UV-light sensor value.|
  ========================*/
  SeeedOled.setTextXY(2, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("UV-light: ");        //Print string to display.
  SeeedOled.setTextXY(2, 42);
  SeeedOled.putNumber(uvValue);             //Print UV-light value to display.

  /*
  =====================
  |Light sensor value.|
  =====================*/
  SeeedOled.setTextXY(3, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("Light: ");           //Print string to display.
  SeeedOled.setTextXY(3, 42);
  SeeedOled.putNumber(lightValue);          //Print light value in lumen value to display.
  SeeedOled.putString("lm");                 //Print unit of the value.

  /*
  =======================
  |Temp threshold value.|
  =======================*/
  SeeedOled.setTextXY(4, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("Temp lim: ");        //Print string to display.
  SeeedOled.setTextXY(4, 42);
  SeeedOled.putNumber(tempPosition / 2);    //Print temperature threshold value to display. Value 24 corresponds to 12°C, temp value is doubled to reduce rotary sensitivity and increase knob rotation precision.
  
  /*
  ==========================
  |Water flow sensor value.|
  ==========================*/
  SeeedOled.setTextXY(5, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("Flow Sens: ");       //Print string to display.
  SeeedOled.setTextXY(5, 42);
  SeeedOled.putNumber(flowValue);           //Print water flow value to display.
  SeeedOled.putString("L/h");               //Print unit of the value.

  //Printing separator line to separate read out values from error/warning messages.
  SeeedOled.setTextXY(6, 0);                //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
  SeeedOled.putString("----------------");  //Print string to display.

  /*
  =========================
  |Error/Warning messages.|
  =========================*/
  timeNow = millis();                                   //Read millis() value to be used as delay to present multiple warning messages sharing display space.
  timeDiff = timeNow - timePrev;
  Serial.println(timeDiff);
  if(timeDiff <= 2400) {
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("                        ");    //Clear row to enable other warnings to be printed to display.
    if(moistureDry == true) {                           //If moisture sensor measure a too low value, "To dry!" is printed to display.
      SeeedOled.setTextXY(7, 0);                        //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
      SeeedOled.putString("Too dry!");                  //If moisture sensor measure a too low value, "To dry!" is printed to display.
    }
    else if(moistureWet == true) {                      //If moisture sensor measure a too high value, "To wet!" is printed to display.
      SeeedOled.setTextXY(7, 0);                        //Set cordinates to where it will print text. X = row (0-7), Y = column (0-127).
      SeeedOled.putString("Too wet!");
    }
  }

  if(waterLevelValue == true && timeDiff > 2400 && timeDiff <= 4800) {
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("                        ");    //Clear row to enable other warnings to be printed to display.
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("Refill tank!");                //If water level switch measure that water tank is empty, "Refill tank!" is printed to display.
  }

  if(tempValue > tempPosition/2 && timeDiff > 4800 && timeDiff <= 7200) { 
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("                        ");    //Clear row to enable other warnings to be printed to display.
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("Too warm!");                   //If measured temperature is higher than preset temperature threshold, "Too warm!" is printed to display.
  }

if(pumpState == true && flowValue < 8 && waterLevelValue == false && timeDiff > 7200 && timeDiff <= 9600) { //If flow sensor value is but is less than a certain value without the water level sensor is giving an warning message, there is a problem with the water tank hose.
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("                        ");    //Clear row to enable other warnings to be printed to display.
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("Check tank hose!");            //If measured water flow is below a certain value without the water level sensor indicating the water tank is empty, there is a problem with the water tank hose. "Check water hose!" is printed to display.
  }
  if(timeDiff > 9600) {
    SeeedOled.setTextXY(7, 0);                          //Set cordinates to the warning message will be printed.
    SeeedOled.putString("                        ");    //Clear row to enable other warnings to be printed to display.
    timePrev = millis();                                //Loop warning messages from start.
  }
  
}

void lightRead() {
  lightValue = lightSensor.ReadVisible();
  uvValue = lightSensor.ReadUV();
  //irValue = lightSensor.ReadIR();
}

void pumpStart() {
  //Calculate water flow (Liter/hour) by counting number of rotations that flow sensor makes.
  rotations = 0;                        
  delay(1000);                          //Count number of rotations during one second to calculate water flow in Liter/hour. 
  flowValue = (rotations * 60) / 7.5;   //Calculate the flow rate in Liter/hour.

  //Starting/stopping pump with a button.
  bool pushButton = digitalRead(pumpButton);
  
  if(pushButton == true) {
    digitalWrite(pumpRelay, HIGH);      //Start water pump if button is being pressed.
    pumpState = true;
  }
  else {
    digitalWrite(pumpRelay, LOW);       //Stop water pump if button not is being pressed.
    pumpState = false;
  }
}

void flowCount() {
  //Interupt function to count number of rotations that flow sensor makes when water is being pumped.
  rotations++;
}

void waterLevelRead() {
  waterLevelValue = digitalRead(waterLevelSwitch);  //Read water level sensor. If variable is 'false' water level is OK. If 'true' the water level is too low.
}

ISR(TIMER1_COMPA_vect) {  //Timer interrupt 100Hz to read temperature threshold value set by rotary encoder.
  //Reading preset temperature threshold set by rotation encoder knob.
  int minTemp = 38;   //19°C is the starting value. Temp value is doubled to reduce rotary sensitivity and increase knob rotation precision.
  int maxTemp = 80;
  int aState;

  aState = digitalRead(rotaryEncoderOutpA);                                      //Reads the current state of the rotary knob, outputA.
  
  if(aState != aLastState) {                                        //A turn on rotary knob is detected by comparing previous and current state of outputA.
    if(digitalRead(rotaryEncoderOutpB) != aState && tempPosition <= maxTemp) { //If outputB state is different to outputA state, that meant the encoder knob is rotation clockwise.
      tempPosition++;                                               //Clockwise rotation means increasing position value. Position value is only increased if less than max value.
    }
    else if(tempPosition > minTemp) {
      tempPosition--;                                               //Counter clockwise rotation means decreasing position value.
    }
  }
  aLastState = aState;                                              //Updates the previous state of outputA with current state.
}

void lightStart() {
  if(uvValue < 3) {                     //Turn on LED light if measured UV value is below a certain value.
    digitalWrite(lightRelay, HIGH);
  }
  else {
    digitalWrite(lightRelay, LOW);      //Turn off LED light if measured UV value is above a certain value.
  }
}

int moistureMeanValue(int moistureValue1, int moistureValue2, int moistureValue3, int moistureValue4) {
  int moistureValues[4] = {moistureValue1, moistureValue2, moistureValue3, moistureValue4};
  int moistureMax = 0;                                      //Variable used to store a moisture value when comparing it to other moisture sensor values and finally store the highest moisture value.
  int moistureMin = moistureValues[0];                      //First value in array of values used as reference value. Variable used to store a moisture value when comparing it to other moisture sensor values and finally store the lowest moisture value.
  int maxIndex;                                            
  int minIndex;
  int moistureSum = 0;
  int moistureMean;
  
  //Since four different moisture sensors are used to measure soil moisture in the four different post and specific watering for each individual pots is not possible. The watering action is only based upon a mean value of the moisture readouts. Min and max value are sorted out and not used in case any sensor is not working correctly. 
  for(int i=0; i<sizeof(moistureValues)/sizeof(int); i++) { //Looping through all measured moisture values to find the highest and lowest moisture values.
    if(moistureValues[i] > moistureMax) {                   //Identify the highest measured moisture value.
      moistureMax = moistureValues[i];
      maxIndex = i;                                         //Identify which moisture sensor that has the max value to be able to delete it from mean moisture value calculation.
    }

    if(moistureValues[i] < moistureMin) {                   //Identify the lowest measured moisture value.
      moistureMin = moistureValues[i];
      minIndex = i;                                         //Identify which moisture sensor that has the min value to be able to delete it from mean moisture value calculation.
    }
  }

  //Remove maximum and minimum moisture value from moisture array.
  moistureValues[minIndex] = 0;                             
  moistureValues[maxIndex] = 0;                             

  for(int i=0; i<sizeof(moistureValues)/sizeof(int); i++) {
    moistureSum += moistureValues[i];                       //Sum the remaining moisture sensor values.
  }
  moistureMean = moistureSum / 2;                           //Calculate mean moisture value with max and min values excluded.
  return moistureMean;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  startupDisplay();                       //Initialize the OLED Display and show startup images.
  
  pinMode(moistureSensorPort1, INPUT);
  pinMode(moistureSensorPort2, INPUT);
  pinMode(moistureSensorPort3, INPUT);
  pinMode(moistureSensorPort4, INPUT);
  
  pinMode(flowSensor, INPUT);
  attachInterrupt(1, flowCount, RISING);  //Initializing interrupt to enable water flow sensor to calculate water flow pumped by water pump.
 
  pinMode(pumpRelay, OUTPUT);
  pinMode(pumpButton, INPUT);
  
  pinMode(waterLevelSwitch, INPUT);
  
  pinMode(lightRelay, OUTPUT);
  
  pinMode(rotaryEncoderOutpA, INPUT);
  pinMode(rotaryEncoderOutpB, INPUT);
  aLastState = digitalRead(rotaryEncoderOutpA);      //Read initial position value.
  
  humiditySensor.begin();                 //Initializing humidity sensor.
  
  lightSensor.Begin();                    //Initializing light sensor.

  //Enable time interrupt to read temperature threshold value set by rotary encoder.
  cli();  //Stop interrupts.
  //Set timer1 interrupt at 100Hz
  TCCR1A = 0;     //Set entire TCCR1A register to 0.
  TCCR1B = 0;     //Set entire TCCR1B register to 0.
  TCNT1 = 0;      //Initialize counter value to 0.
  OCR1A = 156;    //match reg. = 16MHz / (prescaler * desired interrupt freq. - 1) = 16000000 / (1024 * 100 - 1) = 156 (must be < 65536).
  TCCR1B |= (1 << WGM12);               //Turn on CTC mode.
  TCCR1B |= (1 << CS12) | (1 << CS10);  //Set CS10 and CS12 bits for 1024 prescaler.
  TIMSK1 |= (1 << OCIE1A);              //Enable timer compare interrupt.
  sei();  //Allow interrupts again.
}

void loop() {
  // put your main code here, to run repeatedly:
  valuesDisplay();                                    //Printing read out values from the greenhouse to display.
  moistureValue1 = moistureSensor1.moistureRead(moistureSensorPort1, &moistureDry1, &moistureWet1);     //Read moistureSensor1 value to check soil humidity.
  moistureValue2 = moistureSensor2.moistureRead(moistureSensorPort2, &moistureDry2, &moistureWet2);     //Read moistureSensor2 value to check soil humidity.
  moistureValue3 = moistureSensor3.moistureRead(moistureSensorPort3, &moistureDry3, &moistureWet3);     //Read moistureSensor3 value to check soil humidity.
  moistureValue4 = moistureSensor4.moistureRead(moistureSensorPort4, &moistureDry4, &moistureWet4);     //Read moistureSensor4 value to check soil humidity.
  moistureValue = moistureMeanValue(moistureValue1, moistureValue2, moistureValue3, moistureValue4);    //Mean value from all sensor readouts.
  tempValue = humiditySensor.readTemperature(false);  //Read temperature value from DHT-sensor. "false" gives the value in °C.
  //humidValue = humiditySensor.readHumidity();         //Read humidity value from DHT-sensor.
  lightRead();                                        //Read light sensor UV value.
  pumpStart();                                        //Start pump to pump water to plant.
  waterLevelRead();                                   //Check water level in water tank.
  lightStart();
}
