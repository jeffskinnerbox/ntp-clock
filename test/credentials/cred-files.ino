/* -----------------------------------------------------------------------------
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.4.0

DESCRIPTION
    Test program for storing JSON configuration file in flash file system on a NodeMCU.

PHYSICAL DESIGN
    Hardware
        HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E - https://www.amazon.com/gp/product/B010O1G1ES

    Wiring
        None required

MONITOR
    To monitor activities while operating, power the NodeMCU via
    USB connection to a Linux computer and execut the following:
        screen /dev/ttyUSB0 9600,cs8
    To terminate monitoring, enter:
        CNTR-a :quit

TESTING

USAGE

SOURCES
    Uses ArduinoJson library by Benoit Blanchon - https://github.com/bblanchon/ArduinoJson

DOCUMENTATION
    https://arduinojson.org/v6/doc/upgrade/
    https://arduinojson.org/v6/example/

MODIFICATION
    Port to ArduinoJason library version 6.10.1

CREATED BY
    Created Aug 10, 2015 by Ivan Grokhotkov and modified by jeffskinnerbox@yahoo.com
------------------------------------------------------------------------------ */

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>

const char *filename = "/data/credentials.json";   // file within SPIFFS where data is stored

/*
// the info structure containing the information you wish to store in SPIFFS
struct info {
    struct personal {
        char first-name[20];
        char last-name[20];
        char blog[50];
        char email[20];
        char github[50];
        char gravatar[50];
    },
    struct device {
        char model[50];
        char mac-address[20];
    },
    struct home-wifi {
        char ssid[20];
        char password[30];
    },
    struct jetpack-wifi {
        char ssid[20];
        char password[30];
};
*/
/*
    strcpy(info.personal.first-name, "Jeff");
    strcpy(info.personal.last-name, "Irland");
    strcpy(info.personal.blog, "http://jeffskinnerbox.me");
    strcpy(info.personal.email, "jeffskinnerbox@yahoo.com");
    strcpy(info.personal.github, "https://github.com/jeffskinnerbox");
    strcpy(info.personal.gravatar, "http://1.gravatar.com/avatar/d6386d1bc80e871a5a4b6d0e28d5f7da.png");

    strcpy(info.device.model, "HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E");
    strcpy(info.device.mac-address, WiFi.macAddress());

    strcpy(info.home-wifi.ssid, "<wifi-ssid>");
    strcpy(info.home-wifi.password, "<wifi-password>");

    strcpy(info.jetpack-wifi.ssid, "<wifissid>");
    strcpy(info.jetpack-wifi.password, "<wifi-password>");
*/

// JSON object that will be stored with SPIFFS
DynamicJsonDocument doc(200);
//StaticJsonDocument<200> doc;


//------------------------------------------------------------------------------


// Initialize info structure with the information you wish to store in SPIFFS
void initialize() {
    //JsonObject obj;
    JsonArray obj;

    /* https://arduinojson.org/v6/api/jsondocument/createnestedobject/ */
    JsonArray personal = doc.createNestedObject("personal");
    personal["first-name"] = "Jeff";
    personal["last-name"] = "Irland";
    personal["blog"] = "http://jeffskinnerbox.me";
    personal["email"] = "jeffskinnerbox@yahoo.com";
    personal["github"] = "https://github.com/jeffskinnerbox";
    personal["gravatar"] = "http://1.gravatar.com/avatar/d6386d1bc80e871a5a4b6d0e28d5f7da.png";

    obj = doc.createNestedObject("device");
    obj["model"] = "HiLetgo New Version ESP8266 NodeMCU LUA CP2102 ESP-12E";
    obj["mac-address"] = WiFi.macAddress();

    obj = doc.createNestedObject("home-wifi");
    obj["ssid"] = "<wifi-ssid>";
    obj["password"] = "<wifi-password>";

    obj = doc.createNestedObject("jetpack-wifi");
    obj["ssid"] = "<wifi-ssid>";
    obj["password"] = "<wifi-password>";

/*
    // print the generated json
    Serial.println("JSON object ...");
    serializeJson(doc, Serial);
*/
}

/* -----------------------------------------------------------------------------
// Populate the JSON object with data from info structure and save in programs SPIFFS file.
bool saveConfig() {
    // allocate a temporary JsonDocument
    // NOTE: don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<200> doc;

    // set the values in the document
    doc.add("personal");
    doc.add("device");
    doc.add("home-wifi");
    doc.add("jetpack-wifi");

    doc["personal"]
    doc["device"]
    doc["home-wifi"]
    doc["jetpack-wifi"]

    JsonObject& json = jsonBuffer.createObject();
    json["serverName"] = "api.example.com";
    json["accessToken"] = "128du9as8du12eoue8da98h123ueh9h98";

    File configFile = SPIFFS.open(filename, "w");
    if (!configFile) {
        Serial.println("Failed to open credentials.json file for writing!");
        return false;
    }

    // print print the generated json
    json.printTo(configFile);

    return true;
}


// Load the JSON object stored in SPIFFS into the programs memory.
bool loadConfig() {
    File configFile = SPIFFS.open(filename, "r");

    if (!configFile) {
        Serial.println("Failed to open config file");
        return false;
    }

    // retrieve the size fo the configuration file
    size_t size = configFile.size();
    Serial.print("Config file size (bytes??): "); Serial.println(size);
    if (size > 1024) {
        Serial.println("Config file size is too large");
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);

    // Arduino JSON uses a preallocated memory pool to store the JsonObject tree
    // you can use ArduinoJson Assistant to compute the exact buffer size needed
    // see here - https://arduinojson.org/v6/assistant/
    //StaticJsonBuffer<1000> jsonBuffer;
    StaticJsonDocument<200> jsonBuffer;

    // use parseObject() to decode/parse the JSON string to a JsonObject called json
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    // check if the decoding/parsing was successful
    if (!json.success()) {
        Serial.println("parseObject() failed to parse config file!");
        Serial.println("The JSON string may have invalid syntax; the JSON string may not represent an object; the StaticJsonBuffer might be too small.");
        return false;
    }

    // print out a portion of the json object
    // real world application would store these values in some variables for later use
    DynamicJsonDocument doc(size);
    deserializeJson(doc, json);
    //deserializeJsonPretty(doc, json);
    JsonObject personal = doc["personal"];
    const char* personal_first_name = personal["first-name"];   // "Jeff"
    const char* personal_last_name = personal["last-name"];     // "Irland"
    Serial.print("\"first-name\": ");  Serial.println(personal_first_name);
    Serial.print("\"last-name\": ");  Serial.println(personal_last_name);

    // print the generated json
    json.printTo(json);

    return true;
}
---------------------------------------------------------------------         */



//------------------------------------------------------------------------------


void setup() {
    // setup serial port to print debug output
    Serial.begin(9600);
    while (!Serial) continue;              // wait for serial port to connect

    Serial.println("\n\rStarting test program for storing JSON configuration file in flash file system on a NodeMCU.");

    initialize();
/* -----------------------------------------------------------------------------
    Serial.println("\n\rMounting FS...");

    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system!");
        return;
    }

    if (!saveConfig()) {
        Serial.println("Failed to save config");
    } else {
        Serial.println("Config saved");
    }

    if (!loadConfig()) {
        Serial.println("Failed to load config");
    } else {
        Serial.println("Config loaded");
    }
--------------------------------------------------------------------          */
}


void loop() {
}
