#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>            // ArduPilot Mega Vector/Matrix math Library
#include <SPI.h>                // Arduino SPI library
#include "AP_OpticalFlow.h"     // ArduCopter OpticalFlow Library
#include <Streaming.h>          

// Constants
//------------------------------------------------------------------------------------
enum {NUM_SENSOR=2};
const int csPin[NUM_SENSOR] = {49,53};   // Chip select pins
const int resetPin[NUM_SENSOR] = {48,47}; // Chip reset pins
const int resolution = 1600;             // allowed values 400,1600 counts/inch
const int frameRate = 2000;              // allowed range [2000,6469]
const int shutterSpeed = 5000;           // allowed arange [1000,9000] clock cycles (where does this come from?)
const int loopDt = 0;                    // loop delay (ms)
const bool showSettings = false;


FastSerialPort0(Serial);        
AP_OpticalFlow_ADNS3080 flowSensor[NUM_SENSOR] = {
    AP_OpticalFlow_ADNS3080(csPin[0]),
    AP_OpticalFlow_ADNS3080(csPin[1])
};           

void setup()
{
    Serial.begin(115200);
    //Serial.begin(1000000);

    for (int i=0; i<NUM_SENSOR;i++)
    {
        pinMode(resetPin[i],OUTPUT);
        digitalWrite(resetPin[i],HIGH);
    }
    delay(100);
    for (int i=0; i<NUM_SENSOR; i++)
    {
        digitalWrite(resetPin[i],LOW);
    }
    delay(1500);


    // Setup sensor
    // ----------------------------------------------------------------------------
    for (int i=0; i<NUM_SENSOR; i++)
    {
        if( flowSensor[i].init(true) == false ) {
            Serial << "Sensor initialization failed" << endl;
        }
        flowSensor[i].set_orientation(AP_OPTICALFLOW_ADNS3080_PINS_FORWARD);
        flowSensor[i].set_field_of_view(AP_OPTICALFLOW_ADNS3080_08_FOV);
        flowSensor[i].set_resolution(resolution);        
        flowSensor[i].set_frame_rate_auto(false);
        flowSensor[i].set_frame_rate(frameRate);        
        flowSensor[i].set_shutter_speed_auto(false);
        flowSensor[i].set_shutter_speed(shutterSpeed);
    }
    delay(1000);

    // Display sensor settings 
    // ----------------------------------------------------------------------------
    if (showSettings)
    {
        for (int i=0; i<NUM_SENSOR; i++)
        {
            Serial << endl;
            Serial << "Sensor settings" << i << endl;
            Serial << "--------------------------------------------------" << endl;
            Serial << "resolution:         " << flowSensor[i].get_resolution() << endl;
            Serial << "frame_rate_auto:    " << flowSensor[i].get_frame_rate_auto() << endl;
            Serial << "frame_rate:         " << flowSensor[i].get_frame_rate() << endl;
            Serial << "shutter_speed_auto: " << flowSensor[i].get_shutter_speed_auto() << endl;
            Serial << "shutter_speed:      " << flowSensor[i].get_shutter_speed() << endl;
            Serial << endl;
        }
    }
}


void loop()
{
    static bool running = false;
    static unsigned long lastTime = 0;
    unsigned long currTime = micros();

    while (Serial.available()>0)
    {
        char cmd = Serial.read();
        if (cmd == 'r')
        {
            running = true;
        }
        else if (cmd == 's')
        {
            running = false;
        }
    }

    if (running)
    {
        for (int i=0; i<NUM_SENSOR; i++)
        {
            flowSensor[i].update();
        }


        for (int i=0; i<NUM_SENSOR; i++)
        {
            Serial << flowSensor[i].raw_dx << ", " << flowSensor[i].raw_dy; 
            if (i < NUM_SENSOR-1)
            {
                Serial << ", ";
            }
        }
        //Serial << "," <<  currTime - lastTime;
        Serial << endl;
    }

    lastTime = currTime;

    //delay(loopDt);
}
