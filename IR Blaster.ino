#include <WiFi.h>
#include <PubSubClient.h>
#include <IRLibSendBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLib_P02_Sony.h>
#include <IRLibCombo.h>
#include <Adafruit_Sensor.h>
#include "DHTesp.h"

DHTesp dht;
int dhtPin = 14;
IRsend mySender;

const char* ssid = "";
const char* password = "";
const char* mqtt_server = ".es";
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];

#define STATUS_TOPIC "hostafrancs/status"
#define TEMP_TOPIC "hostafrancs/temperature"
#define LED_TOPIC "hostafrancs/irblaster"
#define LED 2

void setup_wifi() {
  delay(1000);
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.enableSTA(true);
  delay(1000);
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if ((++i % 16) == 0) {
      Serial.println(F(" still trying to connect"));
    }
  }
  Serial.println("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  blinkLed();
  
  if ((char)payload[0] == '1') {
    delay(500);
    mySender.send(NEC, 0xFF31CE, 32);
    client.publish(STATUS_TOPIC, "ESTUFA ORBEGOZO - ON-OFF");
  } else if ((char)payload[0] == '2') {
    delay(500);
    mySender.send(NEC, 0xFF19E6, 32);
    client.publish(STATUS_TOPIC, "ESTUFA ORBEGOZO - Temp UP Executed");
  } else if ((char)payload[0] == '3') {
    delay(500);
    mySender.send(NEC, 0xFF29D6, 32);
    client.publish(STATUS_TOPIC, "ESTUFA ORBEGOZO - Temp DOWN Executed");
  } else if ((char)payload[0] == '4') {
    delay(500);
    mySender.send(NEC, 0x807F807F, 32);
    client.publish(STATUS_TOPIC, "PELLETS ON OFF Executed");
  } else if ((char)payload[0] == '5') {
    delay(500);
    mySender.send(NEC, 0x807F28D7, 32);
    client.publish(STATUS_TOPIC, "PELLETS TEMP DOWN Executed");
  } else if ((char)payload[0] == '6') {
    delay(500);
    mySender.send(NEC, 0x807F6897, 32);
    client.publish(STATUS_TOPIC, "PELLETS TEMP UP Executed");
  } else if ((char)payload[0] == '7') {
    delay(500);
    mySender.send(NEC, 0x807F906F, 32);
    client.publish(STATUS_TOPIC, "PELLETS BUTTON AUTO Executed");
  } else if ((char)payload[0] == '8') {
    delay(500);
    mySender.send(NEC, 0x807F8877, 32);
    client.publish(STATUS_TOPIC, "PELLETS BUTTON GOTAS Executed");
  } else if ((char)payload[0] == '9') {
    delay(500);
    mySender.send(NEC, 0xFF807F, 32);
    client.publish(STATUS_TOPIC, "ESPELMAS ON Executed");
  } else if ((char)payload[0] == '0') {
    delay(500);
    mySender.send(NEC, 0xFF906F, 32);
    client.publish(STATUS_TOPIC, "ESPELMAS OFF Executed");
  } else {
    delay(500);
    client.publish(STATUS_TOPIC, "Test Call Recieved on ESP32");
  }
}

void reconnect() {
  while (!client.connected()) {
    setup_wifi();
    Serial.print("Attempting MQTT connection...");
    if (client.connect("hfrancsESP32", mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.subscribe(LED_TOPIC);
      client.publish(STATUS_TOPIC, "EstufaControl Online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void blinkLed() {
  for (int i = 1; i <= 10; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  dht.setup(dhtPin, DHTesp::DHT11);
  pinMode(LED, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 120000) {
    lastMsg = now;

    TempAndHumidity newValues = dht.getTempAndHumidity();
    if (dht.getStatus() != 0) {
      Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    }

    Serial.println(" T:" + String(newValues.temperature));
    char buffer[5];
    String s = dtostrf(newValues.temperature, 1, 4, buffer);
    client.publish(STATUS_TOPIC, dht.getStatusString());
    client.publish(TEMP_TOPIC, (char*)s.c_str());
  }
}
