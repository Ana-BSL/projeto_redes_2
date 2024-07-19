#include <Arduino.h>
#include "BluetoothSerial.h"
//#include <WiFi.h>

/*#define WIFI_SSID "nome da rede"
#define WIFI_PASSWORD "senha da rede"
#define SERVER_IP "colocar ip do dispositivo"  
#define SERVER_PORT 12345  

WiFiClient client;*/
BluetoothSerial SerialBT;

char dados;
unsigned long dataMillis = 0;
int count = 0;

void getDeviceId(char *deviceId) {
  uint64_t chipid = ESP.getEfuseMac();
  snprintf(deviceId, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
           (uint8_t)(chipid >> 40), (uint8_t)(chipid >> 32),
           (uint8_t)(chipid >> 24), (uint8_t)(chipid >> 16),
           (uint8_t)(chipid >> 8), (uint8_t)chipid);
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BLUETOOTH");
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);

  //mantém os 2 leds desligados no começo
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);

  /*WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  if (!client.connect(SERVER_IP, SERVER_PORT)) {
    Serial.println("Connection to server failed");
  } else {
    Serial.println("Connected to server");
  }*/
}

void loop() {
  if (SerialBT.connected()) {
    Serial.println("Conectado via Bluetooth");
  } else {
    Serial.println("Desconectado");
  }

  if (SerialBT.available()) {
    dados = SerialBT.read();
    char deviceId[18];
    getDeviceId(deviceId);

    //lê o comando
    String command = SerialBT.readStringUntil('\n');
    command.trim();
    
    char msg[100];

    //adicionei outra condição pra ativar os leds
    if (dados == 'a') {
      digitalWrite(33, HIGH);
      digitalWrite(32, LOW);
      snprintf(msg, sizeof(msg), "ID: %s, Status: presente", deviceId);
      SerialBT.println(msg);
      Serial.println(msg);

    } else if (dados == 'A') {
      digitalWrite(33, LOW);
      digitalWrite(32, HIGH);
      snprintf(msg, sizeof(msg), "ID: %s, Status: falta justificada", deviceId);
      SerialBT.println(msg);
      Serial.println(msg);

    } else if (dados == 'd') {
      digitalWrite(32, LOW);
      digitalWrite(33, LOW);
      snprintf(msg, sizeof(msg), "Desligado!");
      SerialBT.println(msg);
      Serial.println(msg);
    }

    if (command == "presente"){
      digitalWrite(33, HIGH);
      digitalWrite(32, LOW);
      snprintf(msg, sizeof(msg), "ID: %s, Status: presente", deviceId);
      SerialBT.println(msg);
      Serial.println(msg);
    }
    else if (command == "faltou"){
      digitalWrite(33, LOW);
      digitalWrite(32, HIGH);
      snprintf(msg, sizeof(msg), "ID: %s, Status: falta justificada", deviceId);
      SerialBT.println(msg);
      Serial.println(msg);
    }
  }

  if (millis() - dataMillis > 30000) { 
    dataMillis = millis();
    char msg[50];
    snprintf(msg, sizeof(msg), "Count: %d", count++);
    Serial.println(msg);  
  }

  delay(100);
}