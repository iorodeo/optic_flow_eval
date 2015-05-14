#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>            // ArduPilot Mega Vector/Matrix math Library
#include <SPI.h>                // Arduino SPI library
#include "AP_OpticalFlow.h"     // ArduCopter OpticalFlow Library
#include <Streaming.h>          

// Constants
//------------------------------------------------------------------------------------
const int csPin = 53;           // Chip select pin
const int resolution = 1600;    // allowed values 400,1600 counts/inch
const int frameRate = 2000;     // allowed range [2000,6469]
const int shutterSpeed = 5000;  // allowed arange [1000,9000] clock cycles (where does this come from?)
const int loopDt = 5;           // loop delay (ms)
const bool showSettings = false;

FastSerialPort0(Serial);        
AP_OpticalFlow_ADNS3080 flowSensor = AP_OpticalFlow_ADNS3080(csPin);           

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Setup sensor
    // ----------------------------------------------------------------------------
    if( flowSensor.init(true) == false ) {
        Serial << "Sensor initialization failed" << endl;
    }
    flowSensor.set_orientation(AP_OPTICALFLOW_ADNS3080_PINS_FORWARD);
    flowSensor.set_field_of_view(AP_OPTICALFLOW_ADNS3080_08_FOV);
    flowSensor.set_resolution(resolution);        
    flowSensor.set_frame_rate_auto(false);
    flowSensor.set_frame_rate(frameRate);        
    flowSensor.set_shutter_speed_auto(false);
    flowSensor.set_shutter_speed(shutterSpeed);
    delay(1000);

    // Display sensor settings 
    // ----------------------------------------------------------------------------
    if (showSettings)
    {
        Serial << endl;
        Serial << "Sensor settings" << endl;
        Serial << "--------------------------------------------------" << endl;
        Serial << "resolution:         " << flowSensor.get_resolution() << endl;
        Serial << "frame_rate_auto:    " << flowSensor.get_frame_rate_auto() << endl;
        Serial << "frame_rate:         " << flowSensor.get_frame_rate() << endl;
        Serial << "shutter_speed_auto: " << flowSensor.get_shutter_speed_auto() << endl;
        Serial << "shutter_speed:      " << flowSensor.get_shutter_speed() << endl;
        Serial << endl;
    }
}


void loop()
{
    static bool running = false;
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
        flowSensor.update();
        Serial << flowSensor.dx << "," << flowSensor.dy << endl;
    }
    delay(5);
}
