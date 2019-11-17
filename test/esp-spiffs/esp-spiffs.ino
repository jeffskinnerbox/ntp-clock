/*
ESP-SPIFFS-INO
DELOARTS Research

Date        2018.05.15
Creator     Philip Delorenzo
License     GNU GPLv3

Description
    This code opens a requested file that is located
    on the SPIFFS. For a more detailed explanation
    feel free to read the dedicated tuorial:

    http://deloarts.com/de/scripts/esp8266/spiffs/

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>

// create a webserver object that listens for HTTP request on port 80
ESP8266WebServer server(80);

/*
    Note: In this example you have to hard code the
    SSID and the password of your network here.

    This is pretty boring, but I'm lazy and the
    overall code is much shorter and easier to
    understand.

    For a better way check out the esp-http-request
    tutorial or the (upcomming) esp-axios tutorial.
*/
static char SSID[] = "ESP8266-SPIFFS";
static char password[] = "password";

unsigned long requestCounter = 0;


//------------------------------------------------------------------------------


/*
// https://github.com/pellepl/spiffs/wiki/Using-spiffs
// create a file, delete previous if it already exists, and open it for reading and writing
void checkspiffs() {
    char buf[12];

    spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    if (fd < 0) {
        Serial.println("errno %i\n", SPIFFS_errno(&fs));
        return;
    }

    // write to it
    if (SPIFFS_write(&fs, fd, (u8_t *)"Hello world", 12) < 0) {
        Serial.println("errno %i\n", SPIFFS_errno(&fs));
        return;
    }

    // close it
    if (SPIFFS_close(&fs, fd) < 0) {
        Serial.println("errno %i\n", SPIFFS_errno(&fs));
        return;
    }

    // open it
    fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
    if (fd < 0) {
        Serial.println("errno %i\n", SPIFFS_errno(&fs));
        return;
    }

    // read it
    if (SPIFFS_read(&fs, fd, (u8_t *)buf, 12) < 0) {
        Serial.println("errno %i\n", SPIFFS_errno(&fs));
        return;
    }

    // close it
    if (SPIFFS_close(&fs, fd) < 0) {
        Serial.println("errno %i\n", SPIFFS_errno(&fs));
        return;
    }

    // check it
    Serial.println("--> %s <--\n", buf);
}
*/


// https://www.instructables.com/id/Using-ESP8266-SPIFFS/
// https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/CheckFlashConfig/CheckFlashConfig.ino
// tests if the EEPROM settings of the IDE match to the Hardware
void checkflashconfig() {
    uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();

    Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
    Serial.printf("Flash real size: %u bytes\n", realSize);

    Serial.printf("Flash ide  size: %u bytes\n", ideSize);
    Serial.printf("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
    Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

    if (ideSize != realSize)
        Serial.println("Flash Chip configuration wrong!\n");
    else
        Serial.println("Flash Chip configuration ok.\n");
}


// https://techtutorialsx.com/2019/02/23/esp32-arduino-list-all-files-in-the-spiffs-file-system/
void listAllFiles() {

  Serial.println("Listing all the files in SPIFFS filesystem");

  File root = SPIFFS.open("/", "r");
  File file = root.openNextFile();

  while(file) {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
  }
}

//------------------------------------------------------------------------------


// convert the file extension to the correct content type
String getContentType(String filename) {

    if(filename.endsWith(".htm")) return "text/html";
    else if(filename.endsWith(".html")) return "text/html";
    else if(filename.endsWith(".css")) return "text/css";
    else if(filename.endsWith(".js")) return "application/javascript";
    else if(filename.endsWith(".png")) return "image/png";
    else if(filename.endsWith(".gif")) return "image/gif";
    else if(filename.endsWith(".jpg")) return "image/jpeg";
    else if(filename.endsWith(".ico")) return "image/x-icon";
    else if(filename.endsWith(".xml")) return "text/xml";
    else if(filename.endsWith(".pdf")) return "application/x-pdf";
    else if(filename.endsWith(".zip")) return "application/x-zip";
    else if(filename.endsWith(".gz")) return "application/x-gzip";
    else return "text/plain";

}


bool handleRequest(String requestUrl) {
    //String requestUrl = server.uri;

    // Do some UI stuff ...
    Serial.println("  Request number " + String(requestCounter) + ":  " + requestUrl);
    requestCounter++;

    /*
        If the client requests a folder, we want to send him the
        index-file of that folder. So we first check on that.
    */
    if (requestUrl.endsWith("/")) {
        requestUrl += "index.html";
    }

    /*
        And now we have check if the file is available on the file
        system. Then we send the data back.

        But wait! The client doesn't know the content type. So we
        set it up if the file exists. This shall be done with the
        getContentType()-function.
    */
    if (SPIFFS.exists(requestUrl)) {
        File requestFile = SPIFFS.open(requestUrl, "r");
        size_t sent = server.streamFile(requestFile, getContentType(requestUrl));
        requestFile.close();
        return true;
    } else {
        return false;
    }
}


void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found.");
}


String getIpAddress() {
    IPAddress ip = WiFi.softAPIP();
    return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}


String getMacAddress() {
    byte mac[6];
    String buffer = "";

    WiFi.macAddress(mac);
    for (byte i = 0; i < 6; i++)
        i < 5 ? buffer += String(int(mac[i]), HEX) + ":" : buffer += String(int(mac[i]), HEX);
    return buffer;
}


//------------------------------------------------------------------------------


void setup() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
    }
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, password);

    /*
        Somehow my esp12 has problems with the init-serial-com.
        So I use a delay of 100ms after the Serial.begin() function.
        I haven't had the time to fix this issue. Maybe you can
        solve this problem!
    */
    Serial.begin(9600);
    delay(10);
    Serial.println("\n\r- - - - - - - - - - - - - - - - - - -");
    Serial.println("  WiFi SSID:    " + String(SSID));
    Serial.println("  IP address:   " + getIpAddress());
    Serial.println("  MAC address:  " + getMacAddress());
    Serial.println("- - - - - - - - - - - - - - - - - - -");

    checkflashconfig();

    // Start the file server with SPIFFS.begin()
    Serial.print("  Starting file server .... ");
    if (SPIFFS.begin()) {
        Serial.println("OK.");
        //FSInfo fs_info;
        //SPIFFS.info(fs_info);
        //Serial.println("SPIFFS: %lu of %lu bytes used.\n", fs_info.usedBytes, fs_info.totalBytes);
    } else {
        Serial.println("Failed.");
        // delay(3000);
        return;
    }

    Serial.println("\n\n----Listing files before format----");
    listAllFiles();

    // format the spiffs file system
    bool formatted = SPIFFS.format();

    if (formatted)
        Serial.println("\n\nSuccess formatting");
    else
        Serial.println("\n\nError formatting");


    Serial.println("\n\n----Listing files after format----");
    listAllFiles();

    /*
        So, here's the deal: Normally you would setup the links to
        all specific web pages here. As an example:

        server.on("/", handleRoot);
        server.on("/verfication", HTTP_POST, handleVerification);
            ->  Note: We did use callback functions here, where we passed
                handleRoot and handleVerification as parameters.

        With handleRoot and handleVerfication being independend functions
        that do things and stuff with the clients request. You can view
        a working code on my esp-http-requests tutorial.

        BUT:
        We won't do this here. Instead we always call the not-found-function,
        which I named handleRequest.
            ->  We do not set a specific path, so every call to the server will
                lead to an unknown destination.
            ->  And we write it as inline function and pass the server-uri directly
                to the handleRequest function.
        And inside that function the file server comes into play.
    */
    server.onNotFound([]() {
        if (handleRequest(server.uri())) {
            Serial.println("  OK.");
        } else {
            handleNotFound();
            Serial.println("  Failed.");
        }
    });

    Serial.println("  Starting web server.");
    // Actually start the server with server.begin()
    server.begin();

    Serial.println("   OK.");
    Serial.println("- - - - - - - - - - - - - - - - - - -");
    Serial.println("  Initialized.");
    delay(1000);
    Serial.println("- - - - - - - - - - - - - - - - - - -\n");
}


void loop() {
    server.handleClient();
}
