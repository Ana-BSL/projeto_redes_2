#include <Arduino.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID "UPE-Estudante(UnL)"
#define WIFI_PASSWORD "OrgulhodeserUPE"
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient client(espClient);
BluetoothSerial SerialBT;

char dados;
unsigned long dataMillis = 0;
int count = 0;

String getDeviceId() {
  uint64_t chipid = ESP.getEfuseMac();
  String id = String((uint16_t)(chipid >> 32), HEX);
  id += String((uint32_t)chipid, HEX);
  id.toUpperCase();
  return id;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BLUETOOTH");
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }
  
  client.setServer(MQTT_SERVER, MQTT_PORT);
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker");
    } else {
      delay(5000);
    }
  }
}

void Conexao() {
  if (SerialBT.connected())
    Serial.println("connected");
  else
    Serial.println("disconnected");
}

void loop() {
  Conexao();
  if (SerialBT.available()) {
    dados = SerialBT.read();
    char deviceId[17]; 
    getDeviceId().toCharArray(deviceId, 17); 
    
    if (dados == 'a') {
      digitalWrite(32, HIGH);
      digitalWrite(33, LOW);
      SerialBT.println("presente");
      String msg = String("ID: ") + deviceId + ", Status: presente";
      client.publish("esp/attendance", msg.c_str());

    } else if (dados == 'A') {
      digitalWrite(32, LOW);
      digitalWrite(33, HIGH);
      SerialBT.println("falta justificada");
      String msg = String("ID: ") + deviceId + ", Status: falta justificada";
      client.publish("esp/attendance", msg.c_str());
    }
  }

  if (millis() - dataMillis > 30000) { 
    dataMillis = millis();
    String msg = String("Count: ") + count++;
    client.publish("esp/test", msg.c_str());
  }

  client.loop();
  delay(100); 
}
