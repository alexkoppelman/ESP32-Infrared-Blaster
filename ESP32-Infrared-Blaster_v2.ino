#include <WiFiManager.h> 
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t kIrLed = 4;  
IRsend irsend(kIrLed);  

bool enableHeater = false;
uint8_t loopCnt = 0;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

const char* mqtt_server = "alexkoppelman.es";
const char* mqttUser = "happyplants";
const char* mqttPassword = "Pa55word";

long lastMsg = 0;
char msg[50];
#define STATUS_TOPIC "hostafrancs/status"
#define TEMP_TOPIC "hostafrancs/0temperature"
#define HUM_TOPIC "hostafrancs/0/humidity"
#define JSON_TOPIC "hostafrancs/0/json"
#define LED_TOPIC "hostafrancs/0/irblaster"
#define LED 2

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
  
  if ((char)payload[0] == '4') {
    irsend.sendNEC(0x807F807F, 32);
    client.publish(STATUS_TOPIC, "PELLETS ON OFF Executed");
  } else if ((char)payload[0] == '5') {
    irsend.sendNEC(0x807F28D7, 32);
    client.publish(STATUS_TOPIC, "PELLETS TEMP DOWN Executed");
  } else if ((char)payload[0] == '6') {
    irsend.sendNEC(0x807F6897, 32);
    client.publish(STATUS_TOPIC, "PELLETS TEMP UP Executed");
  } else if ((char)payload[0] == '7') {
    irsend.sendNEC(0x807F906F, 32);
    client.publish(STATUS_TOPIC, "PELLETS BUTTON AUTO Executed");
  } else if ((char)payload[0] == '8') {
    irsend.sendNEC(0x807F8877, 32);
    client.publish(STATUS_TOPIC, "PELLETS BUTTON GOTAS Executed");
  } else {
    client.publish(STATUS_TOPIC, "Test Call Recieved on ESP32");
  }
   delay(1000);
   blinkLed();
}


void blinkLed() {
  for (int i = 1; i <= 5; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}

void setup() {

    Serial.begin(115200);
    WiFiManager wm;

    bool res;

    res = wm.autoConnect("ESP32_IRBLASTER","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect WiFi");
        
    } 
    else {
        Serial.println("WiFi connected...yeey :)");
    }

  // setup IR
  irsend.begin();
  
  
  // setup SHT
  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
    Serial.println("SHT31 loaded fine");

  // setup status led
  pinMode(LED, OUTPUT);


  // set up MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("mqtt set up");
  

}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Mqtt Connected");
      client.subscribe(LED_TOPIC); // Subscribe to relevant topics
    } else {
      Serial.print("Mqtt failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}


void loop() {
  
if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  long now = millis();
  if (now - lastMsg > 120000) {
    lastMsg = now;

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(t);
  } else { 
    Serial.println("Failed to read temperature");
  }
  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Humidity = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read humdity");
  }
    char buffer[10];
    dtostrf(t, 1, 2, buffer); //  
    //String s = dtostrf(newValues.t, 1, 4, buffer);
    client.publish(STATUS_TOPIC, buffer);
    client.publish(TEMP_TOPIC, buffer);

      // Create a JSON object
      StaticJsonDocument<200> doc;
      doc["device"] = "ir-blaster";
      doc["temperature"] = t; // Add temperature
      doc["humidity"] = h;    // Add humidity

      // Serialize JSON to a string
      char jsonBuffer[200];
      serializeJson(doc, jsonBuffer);

      // Publish the JSON object
      client.publish(JSON_TOPIC, jsonBuffer);

    
    }
}
