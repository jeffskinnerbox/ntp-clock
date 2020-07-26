
/*------------------------------------------------------------------------------
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.2.0

DESCRIPTION:

REFERENCE MATERIALS:

CREATED BY:
    jeffskinnerbox@yahoo.com
------------------------------------------------------------------------------*/


// ESP8266 libraries (~/.arduino15/packages/esp8266)
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

// Arduino libraries (~/src/arduino/libraries)
#include <Arduino.h>

// Arduino Sketchbooks libraries (~/src/arduino/sketchbooks/libraries)

// simple-display project's include files (~/src/scrolling-display/test/simple-display)
#include "debug.h"
#include "WiFiHandler.h"

#define BUF 25

// ------------------------ Constructors & Destructors -------------------------

// Constructor to create WiFiHandler
WiFiHandler::WiFiHandler(void) {

    ssid = new char[BUF];
    password = new char[BUF];
    timeout = 10000;              // time out for wifi access request

}


// Destructor to delete WiFiHandler
WiFiHandler::~WiFiHandler(void) {

    delete [] ssid;
    delete [] password;

}



//-------------------------------- WiFi Methods --------------------------------

// connect to wifi
bool WiFiHandler::wifiConnect(char *id, char *pass, unsigned long tout) {

    ssid = id;
    password = pass;
    timeout = tout;

    // attempt first connect to a WiFi network
    INFOS("Attempting connection to WiFi SSID ", ssid);
    WiFi.begin(ssid, password);

    // make subsequent connection attempts to wifi
    tout = timeout + millis();                // milliseconds of time given to making connection attempt
    while(WiFi.status() != WL_CONNECTED) {
        PRT(".");
        if (millis() > tout) {
            ERRORD("Failed to connect to WiFi!  WiFi status exit code is ", WiFi.status());
            ERRORD("\tTimed out after (milliseconds): ", timeout);
            return false;
        }
        delay(500);
    }

    PRT(".\n\r");
    INFOS("Successfully connected to WiFi!  IP address is ", WiFi.localIP());
    INFOD("WiFi status exit code is ", WiFi.status());

    return true;
}


// terminate the wifi connect
void WiFiHandler::wifiTerminate() {
    INFOS("Disconnecting from WiFi with SSID ", WiFi.SSID());

    WiFi.disconnect();

    PRINT("--------------------------------------------------------------------------------");
}


// scan for nearby networks
void WiFiHandler::wifiScan() {
    INFO("Starting Network Scan");
    byte numSsid = WiFi.scanNetworks();

    // print the list of networks seen
    INFOD("Total number of SSIDs found: ", numSsid);

    // print the name of each network found
    for (int thisNet = 0; thisNet<numSsid; thisNet++) {
        INFOS("   ", WiFi.SSID(thisNet));
    }

    INFO("Network Scan Completed");
    PRINT("--------------------------------------------------------------------------------");

}



//--------------------------------- UDP Methods --------------------------------

void WiFiHandler::udpSetPort(unsigned int port) {
    UDPport = port;
}


// start listening for UDP messages on port UDPport
bool WiFiHandler::udpStart() {

    if (udp.begin(UDPport)) {
        INFOD("Starting UDP for NTP connection.  Using local port ", UDPport);
        return true;
    } else {
        ERROR("Failed to start UDP listener.");
        return false;
    }

}


// stop listening for UDP messages on port UDPport
void WiFiHandler::udpStop() {

    INFOD("Stopping UDP on local port ", UDPport);
    udp.stop();

}


bool WiFiHandler::udpCheck(void) {
    unsigned int rtn;

    rtn = udp.parsePacket();

    if (!rtn)
        WARNING("No UDP packets are available");

    return rtn;

}


bool WiFiHandler::udpRequest(IPAddress& address, unsigned int port, byte *packetbuffer, unsigned int len) {
    unsigned int rtn1, rtn2, bytes_rtn;

    rtn1 = udp.beginPacket(address, port);      // request IP address and port
    bytes_rtn = udp.write(packetbuffer, len);   // buffer containing the request return
    rtn2 = udp.endPacket();

    if (rtn1 == 0 || rtn2 == 0 || bytes_rtn == 0) {
        WARNING("UDP request failed");
        WARNINGD("\trtn1 = ", rtn1);
        WARNINGD("\trtn2 = ", rtn2);
        WARNINGD("\tbytes_rtn = ", bytes_rtn);
        return false;
    }

    return true;

}


int WiFiHandler::udpRead(byte *packetbuffer, unsigned int len) {
    int rtn;

    // This function can only be successfully called after WiFiUDP.parsePacket()
    rtn = udp.read(packetbuffer, len);      // read the packet into the buffer

    return rtn;

}



/*
void handleWiFi(void)
{
  static enum { S_IDLE, S_WAIT_CONN, S_READ, S_EXTRACT, S_RESPONSE, S_DISCONN } state = S_IDLE;
  static char szBuf[1024];
  static uint16_t idxBuf = 0;
  static WiFiClient client;
  static uint32_t timeStart;

  switch (state)
  {
  case S_IDLE:   // initialise
    INFO("S_IDLE");
    idxBuf = 0;
    state = S_WAIT_CONN;
    break;

  case S_WAIT_CONN:   // waiting for connection
  {
    client = server.available();
    if (!client) break;
    if (!client.connected()) break;

#if DEBUG
    char szTxt[20];
    sprintf(szTxt, "%03d.%03d.%03d.%03d", client.remoteIP()[0], client.remoteIP()[1], client.remoteIP()[2], client.remoteIP()[3]);
    INFOS("New client @ ", szTxt);
#endif

    timeStart = millis();
    state = S_READ;
  }
  break;

  case S_READ: // get the first line of data
    INFO("S_READ ");

    while (client.available())
    {
      char c = client.read();

      if ((c == '\r') || (c == '\n'))
      {
        szBuf[idxBuf] = '\0';
        client.flush();
        INFOS("Recv: ", szBuf);
        state = S_EXTRACT;
      }
      else
        szBuf[idxBuf++] = (char)c;
    }
    if (millis() - timeStart > 1000)
    {
      INFO("Wait timeout");
      state = S_DISCONN;
    }
    break;

  case S_EXTRACT: // extract data
    INFO("S_EXTRACT");
    // Extract the string from the message if there is one
    getData(szBuf, BUF_SIZE);
    state = S_RESPONSE;
    break;

  case S_RESPONSE: // send the response to the client
    INFO("S_RESPONSE");
    // Return the response to the client (web page)
    client.print(WebResponse);
    client.print(WebPage);
    state = S_DISCONN;
    break;

  case S_DISCONN: // disconnect client
    INFO("S_DISCONN");
    client.flush();
    client.stop();
    state = S_IDLE;
    break;

  default:  state = S_IDLE;
  }
}
*/
