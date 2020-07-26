
// ESP8266 libraries (~/.arduino15/packages/esp8266)
//#include <Wire.h>
//#include <WiFiUdp.h>
//#include <ESP8266WiFi.h>

// Arduino libraries (~/src/arduino/libraries)
//#include <timer.h>
//#include <Adafruit_GFX.h>

// Arduino Sketchbooks libraries (~/src/arduino/sketchbooks/libraries)
//#include <TimeLib.h>
//#include <Timezone.h>
//#include <PubSubClient.h>
//#include <Adafruit_LEDBackpack.h>

// build-date project's include files (~/src/ntp-clock)
//#include "debug.h"
//#include "credentials.h"
//#include "WiFiHandler.h"


#define VERSION "0.3.1"
#define VER VERSION " - "  __DATE__ " at " __TIME__
const char version[] = VER;

#define BUF_SIZE 50
char string[BUF_SIZE];

//------------------------------------------------------------------------------

void BuildDate(char *buffer) {

    sprintf(buffer, "%s - %s at %s", VERSION, __DATE__, __TIME__);

}


void setup() {

    // setup serial port to print debug output
    Serial.begin(9600);
    while (!Serial) {}                        // wait for serial port to connect

   BuildDate(string);
   Serial.println("\n\r");
   Serial.print("string = ");
   Serial.println(string);

   Serial.println("\n\r");
   Serial.print("version = ");
   Serial.println(version);

}


void loop() {
}
