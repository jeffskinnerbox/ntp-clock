/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic OUTTOPIC every two seconds
  - subscribes to the topic INTOPIC, printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic INTOPIC is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

 SOURCE:
    https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// topics used by mqtt client for publishing and subscribing
#define OUTTOPIC "outTopic"
#define INTOPIC "inTopic"

// Update these with values suitable for your network
const char* ssid = "<my-wifi-ssid>";
const char* password = "<my-wifi-password>";
const char* mqtt_server = "broker.mqtt-dashboard.com";   // "<my-mqtt-broker>"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;



// We start by connecting to a WiFi network so we can connect with thmqtt broker
// We also initializes the pseudo-random number generator causing it to start
// at an arbitrary point in its random sequence.
void setup_wifi() {
    delay(10);

    Serial.print("\nConnecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

    randomSeed(micros());

    Serial.print("\nWiFi connected ... ");
    Serial.println("IP address = ");
    Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
        digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
    } else {
        digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
}



void reconnect() {
    // Loop until we're reconnected
    while (!mqtt_client.connected()) {
        Serial.println("Attempting MQTT connection...");

        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        Serial.print("MQTT Client ID = ");
        Serial.println(clientId);

        // Attempt to connect to mqtt broker
        if (mqtt_client.connect(clientId.c_str())) {
            Serial.println("connected");

            // Once connected, publish an announcement...
            mqtt_client.publish(OUTTOPIC, "hello world");

            // ... and resubscribe
            mqtt_client.subscribe(INTOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}



void setup() {
    pinMode(BUILTIN_LED, OUTPUT);     // initialize the BUILTIN_LED pin as an output

    Serial.begin(9600);               // initialize the serial communications
    setup_wifi();                     // initialize your wifi communications

    mqtt_client.setServer(mqtt_server, 1883);  // set your mqtt broker to be used
    mqtt_client.setCallback(callback);
}



void loop() {
    if (!mqtt_client.connected()) {
        reconnect();
    }
    mqtt_client.loop();

    long now = millis();
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf(msg, 50, "Hello World #%ld", value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        mqtt_client.publish(OUTTOPIC, msg);
    }
}
