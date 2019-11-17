/* -----------------------------------------------------------------------------
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.4.0

DESCRIPTION
    Test program for ESP8266 NodeMCU + 7-segment display + backpack from Adafruit.
    NodeMCU will drive the display as a clock synchronized, via NTP, with a NIST time server.

PHYSICAL DESIGN
    Hardware
        Adafruit 1.2" 4-Digit 7-Segment Display w/I2C Backpack - https://www.adafruit.com/product/1270
        HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E - https://www.amazon.com/gp/product/B010O1G1ES

    Wiring
        Connect display's backpack I2C clock pin "C" (SCL) with NodeMCU D1 pin
        Connect display's backpack I2C data pin "D" (SDA) pin with NodeMCU D2 pin
        Connect display's backpack GND pin "-" with with NodeMCU GND
        Connect display's backpack VCC pin "+" with NodeMCU Vin (5V)
        Connect display's backpack pin "IO" with with NodeMCU 3.3V

MONITOR
    To monitor the clocks activities while operating, power the NodeMCU via
    USB connection to a Linux computer and execut the following:
        screen /dev/ttyUSB0 9600,cs8
    To terminate monitoring, enter:
        CNTR-a :quit

TESTING
    For testing, you may want to "see" the ticking of the clock.  To "see the tick-tock"
    on the serial connection, set HEARTBEAT to true.  You may also want to have more
    frequent calls to NTP to refeash the time.  To speed up the refreashing, modify REFRESH
    to a value like 5000UL.

    To test the MQTT capabilities, use a public MQTT broker site
    and Mosquitto clients like this:
        mosquitto_sub -v -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/time"
        mosquitto_sub -v -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/drift"
        mosquitto_pub -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/command" -m "1"

        mosquitto_sub -v -h broker.mqtt-dashboard.com -t "ntp-clock-jeff/#"

USAGE
    commands sent via MQTT:
        '1' - turn on red LED so it blinks with the tick-tock
        '2' - calculate drift sinse last time refresh
        '3' - force a time refresh with NTP server

SOURCES
    https://learn.adafruit.com/adafruit-led-backpack?view=all
    https://www.arduinoslovakia.eu/blog/2017/7/esp8266---ntp-klient-a-letny-cas?lang=en
    https://github.com/adafruit/Adafruit_LED_Backpack/blob/master/examples/clock_sevenseg_ds1307/clock_sevenseg_ds1307.ino

DOCUMENTATION
    https://pubsubclient.knolleary.net/

CREATED BY
    jeffskinnerbox@yahoo.com
------------------------------------------------------------------------------ */

#include <Wire.h>
#include <timer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Timezone.h>


// set to 1 to turn on debug statements to the serial output
// using F() to load strings in flash memory, not RAM
#define DEBUG true
#if DEBUG
    #define PRINT(s, x) { Serial.print(F(s)); Serial.println(x); }
    #define PRINTS(x) Serial.println(F(x))
    #define PRINTX(x) Serial.println(x, HEX)
#else
    #define PRINT(s, x)
    #define PRINTS(x)
    #define PRINTX(x)
#endif


void MQTTPublishDrift(unsigned long);
bool TimeRefresh(void *);


//------------------------------- WiFi Parameters ------------------------------

// credentials for wifi network
const char *ssid = "<my-wifi-ssid>";
const char *pass = "<my-wifi-password>";



//--------------------------- NodeMCU LED Parameters --------------------------

#define LED D0            // red LED on NodeMCU is at pin GPIO16 (D0)
int nodemcuLED = LOW;     // initial setting of nodmcu red led (LOW = on, HIGH = off)
bool blinkLED = true;     // should we blink the nodemcu red led at each clock tick-tock



//------------------------------- MQTT Parameters ------------------------------

// public MQTT brokers for testing
const char* mqtt_server = "broker.mqtt-dashboard.com";   // also see https://test.mosquitto.org/

// MQTT client object
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

// MQTT message buffer for publishing and subscription
const int MSGSIZE = 50;                // size of mqtt message buffer
char msg[MSGSIZE];                     // buffer to hold mqtt messages

// topics used by mqtt client for publishing and subscribing
#define TIMETOPIC  "ntp-clock-jeff/time"     // mqtt broker topic for current time
#define DRIFTTOPIC "ntp-clock-jeff/drift"    // mqtt broker topic of time drift between clock and NIST
#define ERRORTOPIC "ntp-clock-jeff/error"    // mqtt broker topic on application errors
#define CMDTOPIC   "ntp-clock-jeff/command"  // mqtt broker topic for sending commands NodeMCU
#define MQTTPORT   1883                      // port used by the mqtt broker



//------------------------ Clock Time Keeping Parameters -----------------------

// constants used in the clocks calculations
#define TICKTOCK          1000          // number of esp8266 milliseconds in a tick-tock second (ideally 1000)
#define REFRESH           3600000UL     // number of milliseconds between npt time refresh (one hour)
#define WIFITIME          10000         // attempt to connect with wifi for 10 seconds then abandon
#define DISPLAY_ADDRESS   0x70          // I2C address of the display
#define HEARTBEAT         false         // print '.' at each tick-tock of the clock
#define ONE_SECOND        1000UL        // milliseconds in one second
#define TWO_SECOND        2000UL        // milliseconds in two second
#define ONE_MINUTE        60000UL       // milliseconds in one minute
#define ONE_HOUR          3600000UL     // milliseconds in one hour
#define ONE_DAY           85400000UL    // milliseconds in one day
#define SEVENTYYEARS      2208988800UL  // seconds in seventy years

// error code posted on the clock display
#define INITIALIZING      0xAAAA        // display code meaning clock is initializing
#define NOWIFI            0xCCCC        // display code meaning can't get wifi
#define NOREFRESH         0xFFFF        // display code meaning too long in not getting ntp time
#define NONTPSERVER       0xEEEE        // display code meaning can't get ntp time to start clock

// variables used in the calculations of time
unsigned long ClockTicks = 0UL;         // number of clock ticks sinse last time set
unsigned long lastNTPResponse = 0UL;    // time of last successful ntp update
unsigned long oldUNIXepoch = 0UL;       // ntp previous unix epoch value

// ntp-clock is in USA Eastern Time Zone (New York)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};  // Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);



//-------------------------- Clock Display Parameters --------------------------

// create display object
Adafruit_7segment clockDisplay = Adafruit_7segment();

// create timers with default settings
auto timer1 = timer_create_default();   // timer to update clock display
auto timer2 = timer_create_default();   // timer to blink colon on clock display
auto timer3 = timer_create_default();   // timer to initialize clock with ntp time
auto timer4 = timer_create_default();   // timer to publish time to mqtt broker
auto timer5 = timer_create_default();   // timer to blink nodemcu red led

// variables used to control/operate the clock display
bool displayPM = false;                 // is the current time in the pm?
bool blinkColon = false;                // turn on the colon on the display
int Brightness = 6;                     // set brightness of the display (value 1 to 15)
int displayValue = 0;                   // value written to the display
int displayHours = 0;                   // hours value written to the display
int displayMinutes = 0;                 // minutes value written to the display
int displaySeconds = 0;                 // seconds value written to the display



//------------------------ NTP Timer Server Parameters -------------------------

// A UDP object, used by NTP, to send/receive packets
WiFiUDP udp;
unsigned int localPort = 2390;                // local port to listen for UDP packets

// time.nist.gov NTP server address
IPAddress timeServerIP;                       // ip address of NTP time server
const char* ntpServerName = "time.nist.gov";  // NIST time server
const int NTP_PACKET_SIZE = 48;               // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];           // buffer to hold incoming and outgoing packets



//------------------------------------------------------------------------------


// print an integer in ":00" format (with leading zero).
// input value assumed to be between 0 and 99.
void sPrintDigits(int val) {
    Serial.print(':');
    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
}


// print an integer in "00" format (with leading zero).
// input value assumed to be between 0 and 99.
void sPrintI00(int val) {
    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
    return;
}


void printTime(int hour, int minute, int second) {
    sPrintI00(hour);
    sPrintDigits(minute);
    sPrintDigits(second);
}


//Function to print time with time zone
void printTimeDateLoc(time_t t, char *tz, char *loc) {
    printTime(hour(t), minute(t), second(t));
    Serial.print(' ');
    Serial.print(dayShortStr(weekday(t)));
    Serial.print(' ');
    sPrintI00(day(t));
    Serial.print(' ');
    Serial.print(monthShortStr(month(t)));
    Serial.print(' ');
    Serial.print(year(t));
    Serial.print(' ');
    Serial.print(tz);
    Serial.print(' ');
    Serial.print(loc);
    Serial.println();
}


//------------------------------------------------------------------------------


// connect to wifi network
bool wifiConnect(const char *ssid, const char *password, unsigned long timeout) {
    unsigned long tout;

    // attempt first connect to a WiFi network
    Serial.print("\nAttempting connection to WiFi SSID ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // make subsequent connection attempts to wifi
    tout = timeout + millis();                // milliseconds of time given to making connection attempt
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        if (millis() > tout) {
            Serial.print("Failed to connect to WiFi!  ");
            Serial.print("Timed out after ");
            Serial.print(timeout);
            Serial.print(" milli-seconds. ");
            Serial.print("WiFi status exit code is ");
            Serial.println(WiFi.status());
            return false;
        }
        delay(500);
    }

    Serial.print("Successfully connected to WiFi!  ");
    Serial.print("IP address is ");
    Serial.println(WiFi.localIP());
    Serial.print("WiFi status exit code is ");
    Serial.println(WiFi.status());

    return true;
}


// terminate the wifi connect
void wifiTerminate() {
    Serial.print("\nDisconnecting from WiFi with SSID ");
    Serial.println(WiFi.SSID());

    WiFi.disconnect();

    Serial.println("\n-------------------------------------------------------");
}


// scan for nearby networks
void scanNetworks() {
    Serial.println("\nStarting Network Scan");
    byte numSsid = WiFi.scanNetworks();

    // print the list of networks seen
    Serial.print("SSID List:");
    Serial.println(numSsid);

    // print the network number and name for each network found
    for (int thisNet = 0; thisNet < numSsid; thisNet++) {
        Serial.print("   ");
        Serial.print(thisNet);
        Serial.print(") Network: ");
        Serial.println(WiFi.SSID(thisNet));
    }

    Serial.println("Network Scan Completed");
    Serial.println("\n-------------------------------------------------------");
}


// start listening for UDP messages on port localPort
void startUDP() {
    if (udp.begin(localPort))
        Serial.print("\nStarting UDP for NTP connection.  ");
    else
        Serial.print("Failed to start UDP listener.  ");

    Serial.print("Using local port ");
    Serial.println(udp.localPort());
}


// stop listening for UDP messages on port localPort
void stopUDP() {
    Serial.print("Stopping UDP on local port ");
    Serial.println(udp.localPort());
    Serial.print("\n");

    udp.stop();
}

//------------------------------------------------------------------------------


// handle errors by displaying a code and then restart
void errorHandler(int error) {
    clockDisplay.print(error, HEX);  // error code to be displayed
    clockDisplay.writeDisplay();     // now push out to the display

    delay(ONE_MINUTE);               // delay so the error code can be read
    Serial.flush();

    ESP.reset();                     // nothing can be done so restart
}


// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
    Serial.print("Sending NTP packet.  ");

    memset(packetBuffer, 0, NTP_PACKET_SIZE);   // set all bytes in the buffer to 0

    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;            // Stratum, or type of clock
    packetBuffer[2] = 6;            // Polling Interval
    packetBuffer[3] = 0xEC;         // Peer Clock Precision

    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a time stamp:
    udp.beginPacket(address, 123);      // ntp requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}


// query the ntp server and update the current time
// NOTE: you corrupting the time via the "delay" and other things
unsigned long getNTPTime() {
    TimeChangeRule *tcr;

    WiFi.hostByName(ntpServerName, timeServerIP);   //get a random server from the pool

    sendNTPpacket(timeServerIP);    // send an NTP packet to a time server
    delay(ONE_SECOND);              // wait one second before checking for reply

    int cb = udp.parsePacket();
    if (!cb) {
        Serial.println("No packet from NTP server.");

        if ((millis() - lastNTPResponse) > ONE_DAY) {
            Serial.println("\n\nMore than 24 hours since last NTP response.  Rebooting.");
            errorHandler(NOREFRESH);
        }
    } else {
        Serial.print("NTP packet received, length= ");
        Serial.println(cb);

        lastNTPResponse = millis();

        udp.read(packetBuffer, NTP_PACKET_SIZE);      // read the packet into the buffer

        //the time stamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

        // combine the four bytes (two words) into a long integer giving you the NTP time
        // which is the seconds since Jan 1 1900 at UTC
        unsigned long secsSince1900 = highWord << 16 | lowWord;  // this is time (seconds since Jan 1 1900)
        Serial.print("NTP time (seconds since Jan 1 1900 UTC) =  " );
        Serial.println(secsSince1900);

        // now convert NTP time into unix time
        // which is the seconds since Jan 1 1970 UTC
        Serial.print("Unix time (seconds since Jan 1 1970 UTC) = ");
        unsigned long epoch = secsSince1900 - SEVENTYYEARS;    // subtract seventy years
        Serial.println(epoch);                                 // print Unix time (seconds since Jan 1 1970)

        // now print the full time, date, timezone
        printTimeDateLoc(epoch, "UTC", "Universal Coordinated Time");
        printTimeDateLoc(usET.toLocal(epoch, &tcr), tcr->abbrev, "New York");

        return epoch;     // successful
    }

    return NULL;          // can't get updated time via ntp
}


//------------------------------------------------------------------------------


// check for clock accuracy
bool TimeDriftCheck() {
    bool rtn;
    time_t utc;
    int h, m, s, drift;
    unsigned long epoch;
    TimeChangeRule *tcr;

    // get the current ntp time of your time server (do first to avoid introducing delay)
    startUDP();             // use udp to reach ntp time server
    epoch = getNTPTime();   // get the unix epoch time from ntp time server
    utc = epoch;            // convert unix epoch time structure to universal coordinated time

    // estimated drift in the clock
    drift = (int)(epoch - oldUNIXepoch);
    drift = (int)ClockTicks - drift;

    // print the time as seen by this clock (aka ntp-clock)
    Serial.println("\nChecking for time drift:");
    Serial.print("\tntp-clock time = ");
    printTime(displayHours, displayMinutes, displaySeconds);
    Serial.println();

    if (utc != NULL) {
        // get hour:minutes:seconds time from universal coordinated time structure
        h = hour(usET.toLocal(utc, &tcr));
        m = minute(usET.toLocal(utc, &tcr));
        s = second(usET.toLocal(utc, &tcr));

        // convert from 24 to 12 hour clock
        if (h == 0) h = 12;
        if (h >= 13) {
            h -= 12;
        }

        Serial.print("\tNTP time =       ");
        printTime(h, m, s);
        Serial.println();

        Serial.print("\testimated drift =       ");
        Serial.print(drift);
        Serial.println(" seconds\n");
        Serial.println();

        MQTTPublishDrift(drift);
        rtn = true;
    } else {
        Serial.println("\nNTP server could not be reached. NTP Time unknown.");
        rtn = false;
    }

    stopUDP();                           // stop listening for UDP messages

    return rtn;
}


//------------------------------------------------------------------------------


void reconnect() {
    if (!mqtt_client.connected()) {
        Serial.println("Attempting broker reconnection.");

        // Create a random client ID
        String clientId = "MQTTClient-";
        Serial.print("MQTT Client ID = ");
        Serial.println(clientId);

        // Attempt to connect to mqtt broker
        if (mqtt_client.connect(clientId.c_str())) {
            Serial.println("reconnected to broker");

            // Once connected, publish an announcement
            mqtt_client.publish(ERRORTOPIC, "reconnected to broker", false);

            // and then resubscribe
            mqtt_client.subscribe(CMDTOPIC);
        } else {
            Serial.print("Reconnect failed,  return code = ");
            Serial.println(mqtt_client.state());
        }
    } else {
        Serial.println("Already connected to broker");
    }
}


void SubscriptionCallback(char* topic, byte* payload, unsigned int length) {
#if DEBUG
    Serial.print("Message arrived on topic \"");
    Serial.print(topic);
    Serial.print("\".  Message = ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
#endif

    switch((char)payload[0]) {
        case '1':         // toggle the nodemcu red LED to blinking or off
            if (blinkLED) {
                blinkLED = false;
                nodemcuLED = HIGH;
                digitalWrite(LED, nodemcuLED);   // make sure nodmcu red led is 'off'
            } else {
                blinkLED = true;
            }
            break;
        case '2':          // write to topic how much the clock has drifted from the ntp time server
            TimeDriftCheck();
            break;
        case '3':         // forced time refresh
            TimeRefresh(NULL);
            break;
        default:
            Serial.print("Message unknown.  No action taken.");
    }
}


bool MQTTPublishTime(void *) {

    // if not connect to mqtt broker, then reconnect
    if (!mqtt_client.connected()) {
        reconnect();
    }

    // format message for sending
    if (displayPM)
        snprintf(msg, MSGSIZE, "%02d:%02d:%02dpm", displayHours, displayMinutes, displaySeconds);
    else
        snprintf(msg, MSGSIZE, "%02d:%02d:%02dam", displayHours, displayMinutes, displaySeconds);

    // publish message
    mqtt_client.publish(TIMETOPIC, msg, true);
}


void MQTTPublishDrift(unsigned long drift) {

    // if not connect to mqtt broker, then reconnect
    if (!mqtt_client.connected()) {
        reconnect();
    }

    // format and publish message
    snprintf(msg, MSGSIZE, "%d", drift);
    mqtt_client.publish(DRIFTTOPIC, msg, true);
}


//------------------------------------------------------------------------------


// update nodemcu red led
bool UpdateLED(void *) {

    // if the led is not blink, then take no action and return
    if (!blinkLED)
        return true;                          // to repeat the timer, set to 'true'

    // toggle the led flag
    if (nodemcuLED == LOW)
        nodemcuLED = HIGH;
    else
        nodemcuLED = LOW;

    // toggle nodmcu red led
    digitalWrite(LED, nodemcuLED);

    return true;                              // to repeat the timer, set to 'true'
}


// push colon status to 7-segment display
bool UpdateColon(void *) {
    clockDisplay.drawColon(blinkColon);       // print the colon status
    clockDisplay.writeDisplay();              // now push out to the display

    blinkColon = !blinkColon;                 // flipping value to blink colon

    return true;                              // to repeat the timer, set to 'true'
}


// push time to 7-segment display
bool UpdateTime(void *) {

    ++ClockTicks;          // increment number of clock ticks (seconds) sinse last time set
    ++displaySeconds;      // increment clock display seconds

    if (displaySeconds >= 60) {
        displaySeconds = 0;
        displayMinutes += 1;
    }

    if (displayMinutes >= 60) {      // increment minutes
        displayMinutes = 0;
        displayHours += 1;           // increment hours
    }

    // convert from 24 to 12 hour clock
    if (displayHours == 0) {
        displayHours = 12;
        displayPM = false;
    }
    if (displayHours >= 13) {
        displayHours -= 12;
        displayPM = true;
    }

    // format value for HH:MM:SS display
    //displayValue = 10000 * displayHours + 100 * displayMinutes + displaySeconds;

    // format value for HH:MM display
    displayValue = 100 * displayHours + displayMinutes;

    clockDisplay.print(displayValue, DEC);    // print the time value
    clockDisplay.drawColon(blinkColon);       // print the colon status
    clockDisplay.writeDisplay();              // now push out to the display

    // print heart beat
    if (HEARTBEAT) {
        if (displaySeconds == 59)
            Serial.print(".\n\r");
        else
            Serial.print(".");
    }

    return true;                              // to repeat the timer, set to 'true'
}


// set the time of the clock using unix epoch time
bool SetClockTime(unsigned long time) {
    time_t utc;
    TimeChangeRule *tcr;

    if (time != NULL) {
        utc = time;

        // zero out accumulated ntp-clock ticks since last clock time set
        ClockTicks = 0UL;

        // new time based on ntp server
        displayHours = hour(usET.toLocal(utc, &tcr));
        displayMinutes = minute(usET.toLocal(utc, &tcr));
        displaySeconds = second(usET.toLocal(utc, &tcr));

        // convert from 24 to 12 hour clock
        if (displayHours == 0) {
            displayHours = 12;
            displayPM = false;
        }
        if (displayHours >= 13) {
            displayHours -= 12;
            displayPM = true;
        } else
            displayPM = false;

        Serial.print("Clock time set to = ");
        printTime(displayHours, displayMinutes, displaySeconds);
        if (displayPM)
            Serial.println("pm");
        else
            Serial.println("am");

        return true;
    }

    Serial.println("New time could not be set. Continuing with old time.");

    return false;
}


// used to periodically refresh the time with a query to ntp server
bool TimeRefresh(void *) {
    unsigned long epoch;

    // before you do a refresh, report how much the clock has drifted
    TimeDriftCheck();

    // get current time from ntp server and set the clock
    Serial.println("Calling NTP server for refresh of time set.");
    startUDP();
    epoch = getNTPTime();                // get current time from ntp server (unix epoch time)
    SetClockTime(epoch);                 // set the clock with ntp server time
    oldUNIXepoch = epoch;                // store this value for later use
    stopUDP();                           // stop listening for UDP messages

    return true;                         // to repeat the timer, set to 'true'
}



//------------------------------------------------------------------------------


void setup() {
    unsigned long epoch;

    // setup serial port to print debug output
    Serial.begin(9600);
    while (!Serial) {}                        // wait for serial port to connect

    Serial.println("\n\rStarting NTP-Clock!");

    // initialize the nodemcu red led so it blinks as the clock ticks
    pinMode(LED, OUTPUT);                     // set LED pin as output
    nodemcuLED = LOW;                         // initial setting of nodmcu red led is 'on'
    digitalWrite(LED, nodemcuLED);            // turn the red LED on

    // initialize the clock display
    clockDisplay.begin(DISPLAY_ADDRESS);      // Setup the display
    clockDisplay.setBrightness(Brightness);   // set brightness of the display
    clockDisplay.print(INITIALIZING, HEX);    // print initial value
    clockDisplay.writeDisplay();              // now push the above out to the display

    // scan for wifi access point and print what you find (useful for trouble shouting wifi)
    scanNetworks();                           // scan for wifi access points

    if (wifiConnect(ssid, pass, WIFITIME)) {  // connect to wifi
        startUDP();                           // start listening for UDP messages

        Serial.println("Calling NTP server for initial time set.");
        epoch = getNTPTime();                 // get current time from ntp server (unix epoch time)
        SetClockTime(epoch);                  // set the clock with ntp server time
        oldUNIXepoch = epoch;                 // store this value for later use

        // you need to stop if you can't get your first ntp time request
        if (epoch == NULL) {
            Serial.println("Can't go on without NTP time. Press reset to try again.");
            errorHandler(NONTPSERVER);
        }

        // set the timer callback functions for print time to display
        timer1.every(TICKTOCK, UpdateTime);            // timer to update clock display
        timer2.every(TICKTOCK, UpdateColon);           // timer to blink colon on clock display

        // set the timer callback functions for getting NIST time refresh
        timer3.every(REFRESH, TimeRefresh);            // timer to internal clock with ntp time

        // set the timer callback functions for publishing time to mqtt broker
        timer4.every(ONE_SECOND, MQTTPublishTime);     // timer for publishing display time to mqtt

        // set the timer callback functions for blinking the led on the nodemcu
        timer5.every(TICKTOCK, UpdateLED);             // timer for update nodemcu led

        mqtt_client.setServer(mqtt_server, MQTTPORT);  // set your mqtt broker to be used
        mqtt_client.setCallback(SubscriptionCallback); // set the callback for subscribe topic

        stopUDP();                           // stop listening for UDP messages
    } else {
        Serial.println("Can't go on without WiFi connection. Press reset twice to fix.");
        errorHandler(NOWIFI);
    }
}


void loop() {
    // to make sure you capture day light savings time changes
    // refresh the time at 2:01:01am with a query to ntp server
    if (displayHours == 2 && displayMinutes == 1 && displaySeconds == 1 && displayPM == false) {
        Serial.println("\nChecking for day light savings time changes.");
        TimeRefresh(NULL);
    }

    // tick the timers that drive the clock, update with ntp time, and other
    timer1.tick();
    timer2.tick();
    timer3.tick();
    timer4.tick();
    timer5.tick();

    // allow the client to process subscriptions, publish, and refresh connection
    mqtt_client.loop();
}
