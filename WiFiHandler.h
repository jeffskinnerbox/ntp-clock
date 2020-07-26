
/*------------------------------------------------------------------------------
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.2.0

DESCRIPTION:

REFERENCE MATERIALS:

CREATED BY:
    jeffskinnerbox@yahoo.com
------------------------------------------------------------------------------*/


#pragma once                        // compiler to skip subsequent includes of this file

// ESP8266 libraries (~/.arduino15/packages/esp8266)
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>


class WiFiHandler {
  private:
    // wifi parameters
    char *ssid = NULL;               // memory array used to store wifi ssid
    char *password = NULL;           // memory array used to store wifi password
    unsigned long timeout;           // time out for wifi access request

    // udp parameters
    unsigned int UDPport = 2390;     // local port to listen for UDP packets
    WiFiUDP udp;                     // A UDP object to send/receive packets

  public:
    // constructors & destructors for the class
    WiFiHandler(void);
    ~WiFiHandler(void);

    // public methods for wifi
    bool wifiConnect(char *, char *, unsigned long);
    void wifiTerminate(void);
    void wifiScan(void);

    // public methods for udp
    void udpSetPort(unsigned int);
    bool udpStart(void);
    void udpStop(void);
    bool udpCheck(void);
    bool udpRequest(IPAddress&, unsigned int, byte *, unsigned int);
    int udpRead(byte *, unsigned int);
};



