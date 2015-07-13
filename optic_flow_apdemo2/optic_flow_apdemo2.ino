/*
 *  Example of AP_OpticalFlow library.
 *  Code by Randy Mackay. DIYDrones.com
 */

#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>            // ArduPilot Mega Vector/Matrix math Library
#include <SPI.h>                // Arduino SPI library
#include "AP_OpticalFlow.h" // ArduCopter OpticalFlow Library

////////////////////////////////////////////////////////////////////////////////
// Serial ports
////////////////////////////////////////////////////////////////////////////////
//
// Note that FastSerial port buffers are allocated at ::begin time,
// so there is not much of a penalty to defining ports that we don't
// use.
//

enum {NUM_SENSOR=2}; 
int csPin[NUM_SENSOR] = {49,53};
int currSensorNum = 0;

FastSerialPort0(Serial);        // FTDI/console

AP_OpticalFlow_ADNS3080 flowSensor[NUM_SENSOR] = {
    AP_OpticalFlow_ADNS3080(csPin[0]),
    AP_OpticalFlow_ADNS3080(csPin[1]),
};           


void setup()
{
    Serial.begin(115200);
    Serial.println("ArduPilot Mega OpticalFlow library test ver 1.5");

    delay(1000);

    // flowSensor[i] initialization
    for (int i=0; i<NUM_SENSOR; i++)
    {
        if( flowSensor[i].init(true) == false ) {
            Serial.print("Failed to initialise ADNS3080 ");
        }
        flowSensor[i].set_orientation(AP_OPTICALFLOW_ADNS3080_PINS_FORWARD);
        flowSensor[i].set_field_of_view(AP_OPTICALFLOW_ADNS3080_08_FOV);
    }

    delay(1000);
}

//
// display menu
//
void display_menu()
{
    Serial.println();
    Serial.print("current sensor: ");
    Serial.println(currSensorNum,DEC);
    Serial.println();
    Serial.println("please choose from the following options:");
    Serial.println("     c - display all config");
    Serial.println("     f - set frame rate");
    Serial.println("     i - display image");
    Serial.println("     I - display image continuously");
    Serial.println("     m - display motion");
    Serial.println("     r - set resolution");
    Serial.println("     s - set shutter speed");
    Serial.println("     z - clear all motion");
    Serial.println("     a - frame rate auto/manual");
    Serial.println("     o - switch to other sensor");
    Serial.println();
}

//
// display config
//
void display_config()
{
    Serial.println();
    Serial.print("current sensor: ");
    Serial.println(currSensorNum,DEC);
    Serial.println();
    Serial.print("Config: ");
    Serial.print(flowSensor[currSensorNum].read_register(ADNS3080_CONFIGURATION_BITS),BIN);
    delayMicroseconds(50);
    Serial.print(",");
    Serial.print(flowSensor[currSensorNum].read_register(ADNS3080_EXTENDED_CONFIG),BIN);
    delayMicroseconds(50);
    Serial.println();

    // product id
    Serial.print("\tproduct id:     ");
    Serial.print(flowSensor[currSensorNum].read_register(ADNS3080_PRODUCT_ID),HEX);
    delayMicroseconds(50);
    Serial.print(" (hex)");
    Serial.println();

    // frame rate
    Serial.print("\tframe rate:     ");
    Serial.print(flowSensor[currSensorNum].get_frame_rate());
    if( flowSensor[currSensorNum].get_frame_rate_auto() == true ) {
        Serial.print(" (auto)");
    }else{
        Serial.print(" (manual)");
    }
    Serial.println();

    // resolution
    Serial.print("\tresolution:     ");
    Serial.print(flowSensor[currSensorNum].get_resolution());
    Serial.println();

    // shutter speed
    Serial.print("\tshutter speed:  ");
    Serial.print(flowSensor[currSensorNum].get_shutter_speed());
    if( flowSensor[currSensorNum].get_shutter_speed_auto() ) {
        Serial.print(" (auto)");
    }else{
        Serial.print(" (manual)");
    }
    Serial.println();
}

//
// set frame rate
//
void set_frame_rate()
{
    int value;

    // frame rate
    Serial.print("frame rate:     ");
    Serial.print(flowSensor[currSensorNum].get_frame_rate());
    if( flowSensor[currSensorNum].get_frame_rate_auto() == true ) {
        Serial.print(" (auto)");
    }else{
        Serial.print(" (manual)");
    }
    Serial.println();

    Serial.println("Choose new frame rate:");
    Serial.println("\ta) auto");
    Serial.println("\t2) 2000 f/s");
    Serial.println("\t3) 3000 f/s");
    Serial.println("\t4) 4000 f/s");
    Serial.println("\t5) 5000 f/s");
    Serial.println("\t6) 6400 f/s");
    Serial.println("\tx) exit (leave unchanged)");

    // get user input
    Serial.flush();
    while( !Serial.available() ) {
        delay(20);
    }
    value = Serial.read();

    if( value == 'a' || value == 'A')
        flowSensor[currSensorNum].set_frame_rate_auto(true);
    if( value == '2' )
        flowSensor[currSensorNum].set_frame_rate(2000);
    if( value == '3' )
        flowSensor[currSensorNum].set_frame_rate(3000);
    if( value == '4' )
        flowSensor[currSensorNum].set_frame_rate(4000);
    if( value == '5' )
        flowSensor[currSensorNum].set_frame_rate(5000);
    if( value == '6' )
        flowSensor[currSensorNum].set_frame_rate(6469);

    // display new frame rate
    Serial.print("frame rate:     ");
    Serial.print(flowSensor[currSensorNum].get_frame_rate());
    if( flowSensor[currSensorNum].get_frame_rate_auto() == true ) {
        Serial.print(" (auto)");
    }else{
        Serial.print(" (manual)");
    }
    Serial.println();
}

// display_image - captures and displays image from flowSensor 
void display_image()
{
    Serial.println("image data --------------");
    flowSensor[currSensorNum].print_pixel_data(&Serial);
    Serial.println("-------------------------");
}

// display_image - captures and displays image from flowSensor 
void display_image_continuously()
{
    int i;
    Serial.println("press any key to return to menu");

    Serial.flush();

    while( !Serial.available() ) {
        display_image();
        i=0;
        while( i<2  && !Serial.available() ) {
            delay(100);          // give the viewer a bit of time to catchup
            i++;
        }
    }

    Serial.flush();
}

//
// set resolutiojn
//
void set_resolution()
{
    int value;
    int resolution = flowSensor[currSensorNum].get_resolution();
    Serial.print("resolution: ");
    Serial.println(resolution);
    Serial.println("Choose new value:");
    Serial.println("    1) 1600");
    Serial.println("    4) 400");
    Serial.println("    x) exit");
    Serial.println();

    // get user input
    Serial.flush();
    while( !Serial.available() ) {
        delay(20);
    }
    value = Serial.read();

    // update resolution
    if( value == '1' ) {
        flowSensor[currSensorNum].set_resolution(ADNS3080_RESOLUTION_1600);
    }
    if( value == '4' ) {
        flowSensor[currSensorNum].set_resolution(ADNS3080_RESOLUTION_400);
    }

    Serial.print("new resolution: ");
    Serial.println(flowSensor[currSensorNum].get_resolution());
}

//
// set shutter speed
//
void set_shutter_speed()
{
    int value;

    // shutter speed
    Serial.print("shutter speed:     ");
    Serial.print(flowSensor[currSensorNum].get_shutter_speed());
    if( flowSensor[currSensorNum].get_shutter_speed_auto() == true ) {
        Serial.print(" (auto)");
    }else{
        Serial.print(" (manual)");
    }
    Serial.println();

    Serial.println("Choose new shutter speed:");
    Serial.println("\ta) auto");
    Serial.println("\t1) 1000 clock cycles");
    Serial.println("\t2) 2000 clock cycles");
    Serial.println("\t3) 3000 clock cycles");
    Serial.println("\t4) 4000 clock cycles");
    Serial.println("\t5) 5000 clock cycles");
    Serial.println("\t6) 6000 clock cycles");
    Serial.println("\t7) 7000 clock cycles");
    Serial.println("\t8) 8000 clock cycles");
    Serial.println("\t9) 9000 clock cycles");
    Serial.println("\tx) exit (leave unchanged)");

    // get user input
    Serial.flush();
    while( !Serial.available() ) {
        delay(20);
    }
    value = Serial.read();

    if( value == 'a' || value == 'A')
        flowSensor[currSensorNum].set_shutter_speed_auto(true);
    if( value == '1' )
        flowSensor[currSensorNum].set_shutter_speed(1000);
    if( value == '2' )
        flowSensor[currSensorNum].set_shutter_speed(2000);
    if( value == '3' )
        flowSensor[currSensorNum].set_shutter_speed(3000);
    if( value == '4' )
        flowSensor[currSensorNum].set_shutter_speed(4000);
    if( value == '5' )
        flowSensor[currSensorNum].set_shutter_speed(5000);
    if( value == '6' )
        flowSensor[currSensorNum].set_shutter_speed(6000);
    if( value == '7' )
        flowSensor[currSensorNum].set_shutter_speed(7000);
    if( value == '8' )
        flowSensor[currSensorNum].set_shutter_speed(8000);
    if( value == '9' )
        flowSensor[currSensorNum].set_shutter_speed(9000);

    // display new shutter speed
    Serial.print("shutter speed:     ");
    Serial.print(flowSensor[currSensorNum].get_shutter_speed());
    if( flowSensor[currSensorNum].get_shutter_speed_auto() == true ) {
        Serial.print(" (auto)");
    }else{
        Serial.print(" (manual)");
    }
    Serial.println();
}

//
// display motion - show x,y and squal values constantly until user presses a key
//
void display_motion()
{
    boolean first_time = true;
    Serial.flush();

    // display instructions on how to exit
    Serial.println("press x to return to menu..");
    delay(1000);

    while( !Serial.available() ) {
        flowSensor[currSensorNum].update();
        flowSensor[currSensorNum].update_position(0,0,0,1,100);

        // check for errors
        if( flowSensor[currSensorNum]._overflow )
            Serial.println("overflow!!");

        // x,y,squal
        Serial.print("x/dx: ");
        Serial.print(flowSensor[currSensorNum].x,DEC);
        Serial.print("/");
        Serial.print(flowSensor[currSensorNum].dx,DEC);
        Serial.print("\ty/dy: ");
        Serial.print(flowSensor[currSensorNum].y,DEC);
        Serial.print("/");
        Serial.print(flowSensor[currSensorNum].dy,DEC);
        Serial.print("\tsqual:");
        Serial.print(flowSensor[currSensorNum].surface_quality,DEC);
        Serial.println();
        first_time = false;

        // short delay
        delay(100);
    }

    // flush the serial
    Serial.flush();
}

void loop()
{
    int value;

    // display menu to user
    display_menu();

    // wait for user to enter something
    while( !Serial.available() ) {
        delay(20);
    }

    // get character from user
    value = Serial.read();

    switch( value ) {

    case 'c':
        // display all config
        display_config();
        break;

    case 'f':
        // set frame rate
        set_frame_rate();
        break;

    case 'i':
        // display image
        display_image();
        break;

    case 'I':
        // display image continuously
        display_image_continuously();
        break;

    case 'm':
        // display motion
        display_motion();
        break;

    case 'r':
        // set resolution
        set_resolution();
        break;

    case 's':
        // set shutter speed
        set_shutter_speed();
        break;

    case 'z':
        // clear and reset everything
        flowSensor[currSensorNum].clear_motion();
        break;

    case 'o':
        // Switch to other sensor
        currSensorNum = (currSensorNum+1)%NUM_SENSOR;
        break;

    default:
        Serial.println("unrecognised command");
        Serial.println();
        break;
    }
}
