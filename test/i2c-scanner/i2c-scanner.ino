/*
    Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
    Version:      0.1.0

    DESCRIPTION:
    This scans the I2C bus in an attempt to discover any device attached.
    Devices with higher bit address might not be seen properly.

    NOTES:
    For other I2C scanning options, check out the following article:
    Embed With Elliot: I2C Bus Scanning - https://hackaday.com/2015/06/25/embed-with-elliot-i2c-bus-scanning/

    IMPORTANT NOTICE:
    Beware of "Soft WDT reset" error.
    The ESP8266 has the watchdog(WDT) turned on by default. If the watchdog timer
    isn't periodically reset while in the "loop{}" or "setup{}", then it will
    automatically reset your ESP8266 after 1 second. The watchdog is reset for you
    every time loop{} runs or you call delay() or yield(), but if you have
    blocking code then the watchdog may time out, resulting in the ESP being reset/booted.

    SOURCES:
    This program (or code that looks like it) can be found in many places.
    For example on the Arduino.cc forum. This version is a modification of
    https://gist.github.com/tfeldmann/5411375
*/

#include <Wire.h>


void setup() {
    Wire.begin();
    Serial.begin(9600);
    Serial.println("\nSetting up I2C Scanner ...");
}


void loop() {
    byte error, address;
    int nDevices = 0;

    Serial.println("Scanning ...");

    for(address = 1; address < 127; address++ ) {
      
        /* uses the return value of the Write.endTransmisstion
        to see if a device did acknowledge to the address */
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        // interpret the return code
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            nDevices++;
        } else if (error == 1) {
            Serial.print("Data too long to fit in transmit buffer at address 0x");
/*
        } else if (error == 2) {
            Serial.print("Recieved NACK on transmit of address at address 0x");
*/
        } else if (error == 3) {
            Serial.print("Recieved NACK on transmit of data at address 0x");
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
        }

        // print the address you just scanned
        if (address < 16)
            Serial.print("0");
        Serial.print(address, HEX);
        Serial.print ("  (");
        Serial.print(address, DEC);
        Serial.println(")");

        delay(100);
        //yield();   // to avoid watchdog timeout
   }

    if (nDevices == 0)
        Serial.println("No I2C devices found!");
    else {
        Serial.print (nDevices, DEC);
        Serial.println(" device(s) found");
    }

    Serial.println("\n------------------------------------------\n");

    delay(5000);           // wait 5 seconds and repeat the scan
}
