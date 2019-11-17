<!--
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.4.0
-->

![work-in-progress](http://worktrade.eu/img/uc.gif "These materials require additional work and are not ready for general use.")

---

# DESCRIPTION
In this test program, I will use the ESP8266 built in
SSD drive to store WiFi configuration information for my program.
This will will allow me to keep my WiFi credentials private
and not stored in my firmware (and therefore not posted to Github!).

* After sensing it can't connect to a WiFi network,
the ESP8266 will become an Access Point (enter AP mode) and create its own WiFi network
* The ESP8266 will server a webpage so you can enter the SSID and password you wish it to connect with
* These credentials will be stored within the ESP8266 flash memory
* when rebooted, the stored network credentials will be use to connect to the local WiFi

SPIFFS and JSON and store configuration data.

# PHYSICAL DESIGN
## Hardware
* [HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E](https://www.amazon.com/gp/product/B010O1G1ES)

## Wiring
None required.

# SOFTWARE DESIGN
## Sources
Program code and/or ideas taken from:

* [ESP8266 Storing Wi-Fi Settings In Flash Auto-Switch AP/Station Modes ](https://www.youtube.com/watch?v=lyoBWH92svk&t=36s)
* [wifi_modes_switch](https://github.com/acrobotic/Ai_Tips_ESP8266/tree/master/wifi_modes_switch)

## Libraries
Arduino IDE or ESP libraries used:

* [ESP8266 WiFi Library]()
* [ESP8266 WiFi Web Server Library]()
* [Arduino JSON Library]()

# SERIAL MONITOR
To monitor the programs activities while operating, power the NodeMCU via
USB connection to a Linux computer and execute the following: `screen /dev/ttyUSB0 9600,cs8`.
To terminate monitoring, enter: `CNTR-a :quit`.

# TESTING

# USAGE
No commands or parameter are entered.

# DOCUMENTATION

## Other



################################################################################



* [Tech Note 030 - ESP8266 Using SPIFFS to replace SD Card storage](https://www.youtube.com/watch?v=_UBPgdp1Yug)
* [#121 SPIFFS and JSON to save configurations on an ESP8266](https://www.youtube.com/watch?v=jIOTzaeh7fs)
* [Decoding and Encoding JSON with Arduino or ESP8266](https://randomnerdtutorials.com/decoding-and-encoding-json-with-arduino-or-esp8266/)
* [ESP8266 Storing Wi-Fi Settings In Flash Auto-Switch AP/Station Modes](https://www.youtube.com/watch?v=lyoBWH92svk&t=36s)
* [ESP8266 Web Server Files With SPIFFS Flash Memory Using Arduino IDE (Mac OSX and Windows)](https://www.youtube.com/watch?v=pfJROpQg-Is&t=687s)



Every ESP8266 has a built in SSD Drive and its a good place to store sensitive information,
like configuration and credentials,
you would rather read into your program instead of of hard code into the programs source code file.

# What is JSON?
JSON stands for JavaScript Object Notation. JSON is a lightweight text-based open standard design for exchanging data.

JSON is primarily used for serializing and transmitting structured data over network connection – transmit data between a server and a client. It is often used in services like APIs (Application Programming Interfaces) and web services that provide public data.

# ArduinoJson library
With the ArduinoJson library,
you can decode (parse a JSON string) and encode (generate a JSON string),
and it works with the ESP8266 and ESP32.

# Step X: Install What is Needed
Find and install your library:

```bash
# search for the json processing library
$ arduino-cli lib search arduinojson
Name: "ArduinoJson"
  Author:  Benoit Blanchon <blog.benoitblanchon.fr>
  Maintainer:  Benoit Blanchon <blog.benoitblanchon.fr>
  Sentence:  An efficient and elegant JSON library for Arduino.
  Paragraph:  ArduinoJson supports ✔ serialization, ✔ deserialization, ✔ MessagePack, ✔ fixed allocation, ✔ zero-copy, ✔ streams, and more. It is the most popular Arduino library on GitHub ❤❤❤❤❤. Check out arduinojson.org for a comprehensive documentation.
  Website:  https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
  Category:  Data Processing
  Architecture:  *
  Types:  Contributed
  Versions:  [4.0.0, 5.0.0, 5.0.1, 5.0.2, 5.0.3, 5.0.4, 5.0.5, 5.0.6, 5.0.7, 5.0.8, 5.1.0-beta.1, 5.1.0-beta.2, 5.1.0, 5.1.1, 5.2.0, 5.3.0, 5.4.0, 5.5.0, 5.5.1, 5.6.0, 5.6.1, 5.6.2, 5.6.3, 5.6.4, 5.6.5, 5.6.6, 5.6.7, 5.7.0, 5.7.1, 5.7.2, 5.7.3, 5.8.0, 5.8.1, 5.8.2, 5.8.3, 5.8.4, 5.9.0, 5.10.0, 5.10.1, 5.11.0, 5.11.1, 5.11.2, 5.12.0, 5.13.0, 5.13.1, 5.13.2, 5.13.3, 5.13.4, 5.13.5, 6.0.0-beta, 6.0.1-beta, 6.1.0-beta, 6.2.0-beta, 6.2.1-beta, 6.2.2-beta, 6.2.3-beta, 6.3.0-beta, 6.4.0-beta, 6.5.0-beta, 6.6.0-beta, 6.7.0-beta, 6.8.0-beta, 6.9.0, 6.9.1, 6.10.0, 6.10.1]

# install the library
$ arduino-cli lib install ArduinoJson
ArduinoJson@6.10.1 downloaded
Installed ArduinoJson@6.10.1

# list your installed libraries
# note: only lists libraries installed with 'ardunio-cli lib'
$ arduino-cli lib list
Name        	Installed	Location
ArduinoJson 	6.10.1   	sketchbook
MD_MAX72XX  	3.0.2    	sketchbook
MD_Parola   	3.0.2    	sketchbook
PubSubClient	2.7      	sketchbook
````
