/*
    Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
    Version:      0.1.0

    DESCRIPTION:
    Test program for NodeMCU + 7-segment display + backpack from Adafruit.

    NOTES:
    Wiring
        Connect display's backpack I2C clock pin "C" (SCL) with NodeMCU D1 pin
        Connect display's backpack I2C data pin "D" (SDA) pin with NodeMCU D2 pin
        Connect display's backpack GND pin "-" with with NodeMCU GND
        Connect display's backpack VCC pin "+" with NodeMCU Vin (5V)
        Connect display's backpack pin "IO" with with NodeMCU 3.3V

    SOURCES:
    https://learn.adafruit.com/adafruit-led-backpack?view=all
    https://github.com/adafruit/Adafruit_LED_Backpack/blob/master/examples/clock_sevenseg_ds1307/clock_sevenseg_ds1307.ino
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>


// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
#define DISPLAY_ADDRESS   0x70

Adafruit_7segment clockDisplay = Adafruit_7segment();  // Create display object
bool blinkColon = false;        // colon on the display turned on (this will be on/off every second)
int displayValue = 0;           // value written to the display


void setup() {
    // Setup Serial port to print debug output
    Serial.begin(9600);
    Serial.println("Clock starting!");

    clockDisplay.begin(DISPLAY_ADDRESS);   // Setup the display
    clockDisplay.setBrightness(15);        // set brightness of the display (value 1 to 15, 15 = default = max brightness)
}


void loop() {
  // print the time value to the display
  clockDisplay.print(displayValue, DEC);

  // blink the colon by flipping its value every loop iteration
  blinkColon = !blinkColon;

  // print the colon status value to the display
  clockDisplay.drawColon(blinkColon);

  // Now push out to the display the new values that were set above.
  clockDisplay.writeDisplay();

  // for debugging
  Serial.print("displayValue = ");
  Serial.println(displayValue);

  delay(1000);         // pause for 1000 milliseconds = 1 second

  displayValue += 1;   // increase the display value
}

