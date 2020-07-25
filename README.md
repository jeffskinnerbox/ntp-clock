<!--
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.4.0
-->


<div align="center">
<img src="http://www.foxbyrd.com/wp-content/uploads/2018/02/file-4.jpg" title="These materials require additional work and are not ready for general use." align="center">
</div>


---

# DESCRIPTION
ESP8266 NodeMCU driven clock display synchronized with a NIST time server via NTP

Test program for ESP8266 NodeMCU + 7-segment display + backpack from Adafruit.
NodeMCU will drive the display as a clock synchronized, via NTP, with a NIST time server.

# PHYSICAL DESIGN
## Hardware
* [HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E](https://www.amazon.com/gp/product/B010O1G1ES)
* [Adafruit 1.2" 4-Digit 7-Segment Display w/I2C Backpack](https://www.adafruit.com/product/1270)
* [Diymore BME280 Temperature Humidity Barometric Pressure Sensor Module](https://www.amazon.com/gp/product/B01LETIESU/)

## Wiring
* 4-Digit 7-Segment Clock Display
    * Connect display's backpack I2C clock pin "C" (SCL) with NodeMCU D1 pin
    * Connect display's backpack I2C data pin "D" (SDA) pin with NodeMCU D2 pin
    * Connect display's backpack GND pin "-" with with NodeMCU GND
    * Connect display's backpack VCC pin "+" with NodeMCU Vin (5V)
    * Connect display's backpack pin "IO" with with NodeMCU 3.3V
* BME280 Temperature, Humidity, and Pressure Sensor
    * Connect BME280's VIN pin to the 3.3V output on the NodeMCU
    * Connect BME280's GRD pin to the GRD on the NodeMCU
    * Connect BME280's SCL pin to the I2C clock D1 pin on the NodeMCU
    * Connect BME280's SDA pin to the I2C data D2 pin on the NodeMCU

# SOFTWARE DESIGN
## Sources
Program code and/or ideas taken from:

## Libraries
Arduino IDE or ESP libraries used:

* ???

# SERIAL MONITOR
To monitor the programs activities while operating, power the NodeMCU via
USB connection to a Linux computer and execute the following: `screen /dev/ttyUSB0 9600,cs8`.
To terminate monitoring, enter: `CNTR-a :quit`.

# TESTING
For testing, you may want to "see" the ticking of the clock.  To "see the tick-tock"
on the serial connection, set HEARTBEAT to true.  You may also want to have more
frequent calls to NTP to refeash the time.  To speed up the refreashing, modify REFRESH
to a value like 5000UL.

To test the MQTT capabilities, use a public MQTT broker site
(e.g. `broker.mqtt-dashboard.com`)
and Mosquitto clients like this:

* `mosquitto_sub -v -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/time"`
* `mosquitto_sub -v -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/drift"`
* `mosquitto_pub -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/command" -m "1"`
* `mosquitto_sub -v -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/#"`

# USAGE
commands sent via MQTT:

* '1' - turn on red LED so it blinks with the tick-tock
* '2' - calculate drift sinse last time refresh
* '3' - force a time refresh with NTP server

# SOURCES
* https://learn.adafruit.com/adafruit-led-backpack?view=all
* https://www.arduinoslovakia.eu/blog/2017/7/esp8266---ntp-klient-a-letny-cas?lang=en
* https://github.com/adafruit/Adafruit_LED_Backpack/blob/master/examples/clock_sevenseg_ds1307/clock_sevenseg_ds1307.ino

# DOCUMENTATION
* https://pubsubclient.knolleary.net/

## Other


