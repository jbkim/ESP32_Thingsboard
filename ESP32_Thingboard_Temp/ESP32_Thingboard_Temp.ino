#include <WiFi.h>
#include "ThingsBoard.h"    // ThingsBoard SDK
#include "DHT.h"
#include <Wire.h>

//all these libraries (from "Sketch->Include Library->Manage Libraries") are needed for compilation
//ArduinoJson
//PubSubClient
//ArduinoHttpClient

#define DHTPIN 18     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 

const char* ssid     = "xxxxx";
const char* password = "xxxxx";

const char* TOKEN     = "xxxxxxxxxxxxxxxxxxxx";
const char* THINGSBOARD_SERVER  = "cloud.thingsboard.io";

WiFiClient espClient;
ThingsBoard tb(espClient);
DHT dht(DHTPIN, DHTTYPE);

bool subscribed = false;
int status = WL_IDLE_STATUS;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());    

    dht.begin();
}

void loop()
{
  // Reconnect to WiFi, if needed
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
    return;
  }

  // Reconnect to ThingsBoard, if needed
  if (!tb.connected()) {
    subscribed = false;

    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
    
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");

  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C");

  tb.sendTelemetryFloat("temperature", t);
  tb.sendTelemetryFloat("humidity", h);  
  tb.loop();
  delay(5000);
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
