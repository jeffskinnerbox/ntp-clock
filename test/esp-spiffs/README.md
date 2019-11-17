<!--
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.4.0
-->

![work-in-progress](http://worktrade.eu/img/uc.gif "These materials require additional work and are not ready for general use.")

---



Fail to open file for write on SPIFFS #4311 - https://github.com/esp8266/Arduino/issues/4311


# DESCRIPTION
An example on how to use the SPIFFS (Serial Peripheral Interface Flash File System) with an NodeMCU (Arduino).
For a more detailed description [visit my website](http://deloarts.com/de/scripts/esp8266/spiffs).

## Background
The Arduino code is written with the most recent Arduino version (1.8.5). Furher I used the [SPIFFS-Plugin](https://github.com/esp8266/arduino-esp8266fs-plugin)

# PHYSICAL DESIGN
## Hardware
* [HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E](https://www.amazon.com/gp/product/B010O1G1ES)

## Wiring
* Connect KY-040 Rotary Encoder "+" pin with NodeMCU Vin (5V)

# SOFTWARE DESIGN
## Sources
Program code and/or ideas taken from:

* [ESP8266: SPIFFS](https://github.com/deloarts/esp-spiffs)

## Libraries
Arduino IDE or ESP libraries used:

* None required

# SERIAL MONITOR
To monitor the programs activities while operating, power the NodeMCU via
USB connection to a Linux computer and execute the following: `screen /dev/ttyUSB0 9600,cs8`.
To terminate monitoring, enter: `CNTR-a :quit`.

# TESTING
???

# USAGE
No commands or parameter are entered.

# SOURCES
* [DELOARTS: spiffs](http://deloarts.com/de/scripts/esp8266/spiffs/)

# DOCUMENTATION
* ???

## Other






