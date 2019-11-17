/* -----------------------------------------------------------------------------
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.1.0
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  10/2/2018
  Author: Makerbro
  Platforms: ESP8266
  Language: C++/Arduino
  File: wifi_modes_switch.ino
  ------------------------------------------------------------------------------
  Description:
  Code for YouTube video demonstrating how  to switch the ESP8266 between Station
  (STA) and Access Point (AP) modes, and how to store network credentials in
  Flash memory:
  https://youtu.be/lyoBWH92svk

  Do you like my videos? You can support the channel:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use.

  https://acrobotic.com/
  https://amazon.com/acrobotic
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/

#include <FS.h>                       // SPIFF file system
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server;

uint8_t pin_led = 2;
char* ssid = "<your-ssid>";               // not used
char* password = "password";
char* mySsid = "STORE-CRED-AP";

const char* filepath = "/config.json";

IPAddress local_ip(192, 168, 11, 4);
IPAddress gateway(192, 168, 11, 1);
IPAddress netmask(255, 255, 255, 0);

char webpage[] PROGMEM = R"=====(
<html>
<head>
</head>
<body>
    <form>
        <fieldset>
            <div>
                <label for="ssid">SSID</label>
                <input value="" id="ssid" placeholder="SSID">
            </div>
            <div>
                <label for="password">PASSWORD</label>
                <input type="password" value="" id="password" placeholder="PASSWORD">
            </div>
            <div>
                <button class="primary" id="savebtn" type="button" onclick="myFunction()">SAVE</button>
            </div>
        </fieldset>
    </form>
</body>
<script>
function myFunction() {
    console.log("button was clicked!");

    var ssid = document.getElementById("ssid").value;
    var password = document.getElementById("password").value;
    var data = {ssid:ssid, password:password};

    var xhr = new XMLHttpRequest();
    var url = "/settings";

    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            // Typical action to be performed when the document is ready:
            if(xhr.responseText != null){
                console.log(xhr.responseText);
            }
        }
    };

    xhr.open("POST", url, true);
    xhr.send(JSON.stringify(data));
};
</script>
</html>
)=====";


//------------------------------------------------------------------------------


void wifiConnect() {

    // reset networking
    WiFi.softAPdisconnect(true);
    WiFi.disconnect();
    delay(1000);

    // check for previously stored credentials
    if(SPIFFS.exists(filepath)) {
        const char * _ssid = "", *_pass = "";

        Serial.print("File \""); Serial.print(filepath); Serial.println("\" exists on the ESP8266.");
        File configFile = SPIFFS.open(filepath, "r");

        if(configFile) {
            size_t size = configFile.size();
            std::unique_ptr<char[]> buf(new char[size]);
            configFile.readBytes(buf.get(), size);
            configFile.close();
            //DynamicJsonBuffer jsonBuffer;
            DynamicJsonDocument jBuffer(1024);
            //JsonObject& jObject = jsonBuffer.parseObject(buf.get());
            deserializeJson(jBuffer, buf.get());
            //if(jObject.success()) {
            if(jBuffer.isNull()) {
                //_ssid = jObject["ssid"];
                //_pass = jObject["password"];
                _ssid = jBuffer["ssid"];
                _pass = jBuffer["password"];
                WiFi.mode(WIFI_STA);
                WiFi.begin(_ssid, _pass);
                unsigned long startTime = millis();

                while (WiFi.status() != WL_CONNECTED) {
                    delay(500);
                    Serial.print(".");
                    digitalWrite(pin_led,!digitalRead(pin_led));
                    if ((unsigned long)(millis() - startTime) >= 5000) break;
                }
            }
        }
    } else
        Serial.print("File \""); Serial.print(filepath); Serial.println("\" DOES NOT exists on the ESP8266.");

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Successfully connected to WiFi using stored credentials.  ");
        Serial.print("IP address is ");
        Serial.println(WiFi.localIP());
        Serial.print("WiFi status exit code is ");
        Serial.println(WiFi.status());

        digitalWrite(pin_led, HIGH);
    } else {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(local_ip, gateway, netmask);
        WiFi.softAP(mySsid, password);

        Serial.println("Successfully set up access point so you can enter WiFi credentials.  ");
        Serial.print("IP address is ");
        Serial.println(WiFi.softAPIP());

        digitalWrite(pin_led, LOW);
    }

    Serial.println("\nWiFi diagnostic information:");
    Serial.setDebugOutput(true);     // by default the diagnostic output is disabled
    WiFi.printDiag(Serial);
    Serial.println("");
}


void handleSettingsUpdate() {
    String data = server.arg("plain");

    //DynamicJsonBuffer jBuffer;
    DynamicJsonDocument jBuffer(1024);
    //JsonObject& jObject = jBuffer.parseObject(data);
    deserializeJson(jBuffer, data);

    File configFile = SPIFFS.open(filepath, "w");
    //jObject.printTo(configFile);
    serializeJson(jBuffer, configFile);
    configFile.close();

    server.send(200, "application/json", "{\"status\" : \"ok\"}");
    delay(500);

    wifiConnect();
}


void toggleLED() {
    digitalWrite(pin_led, !digitalRead(pin_led));
    server.send(204, "");
}

//------------------------------------------------------------------------------


void setup() {

    pinMode(pin_led, OUTPUT);

    // setup serial port to print debug output
    Serial.begin(9600);
    while (!Serial) continue;              // wait for serial port to connect

    Serial.println("\n\rStarting test program for storing WiFi credentials within flash file system on a NodeMCU.");

    // start the SPI flash files system
    Serial.print("\n\rMounting file system ... ");
    if (!SPIFFS.begin()) {
        Serial.println("Error has occurred while mounting SPIFFS!");
        //return;
    }
    Serial.println("Success!");

    // true if a file with given path exists, false otherwise
    if (SPIFFS.exists(filepath))
        Serial.println("file exists!");
    else
        Serial.println("file DOES NOT exist!");

    wifiConnect();

    // prepare you web page
    server.on("/", [](){server.send_P(200,"text/html", webpage);});
    server.on("/toggle", toggleLED);
    server.on("/settings", HTTP_POST, handleSettingsUpdate);

    // begin serving a web page
    Serial.println("Starting Web Server.");
    server.begin();

    // Fills FSInfo structure with information about the file system. Returns true is successful, false otherwise.
    //FSInfo fs_info;
    //SPIFFS.info(fs_info);

    // list all the files in the SPIFFS file system
    File root = SPIFFS.open("/", "r");
    File file = root.openNextFile();
    while(file) {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
}


void loop() {
    server.handleClient();
}
