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

const int LEDpin = 2; //on board blueLED

const char* ssid     = "xxxxxx";
const char* password = "xxxxxxxxxxxx";

const char* TOKEN     = "xxxxxxxxxxxxxxxxxx";
const char* THINGSBOARD_SERVER  = "cloud.thingsboard.io";

WiFiClient espClient;
ThingsBoard tb(espClient);
DHT dht(DHTPIN, DHTTYPE);

bool subscribed = false;
int status = WL_IDLE_STATUS;

boolean ledstate = HIGH;
const size_t callbacks_size = 1;

RPC_Response processSetLED(const RPC_Data &data)
{
  int tmp = data;

  Serial.print("Recv data: ");
  Serial.println(tmp);

  digitalWrite(LEDpin,!(digitalRead(LEDpin)));

  return RPC_Response("toggleGpio", digitalRead(LEDpin));
}

RPC_Callback callbacks[callbacks_size] = {
  { "setValue", processSetLED },
};

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());    

    dht.begin();
    pinMode(LEDpin, OUTPUT);

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

  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, callbacks_size)) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }
  tb.loop();
  delay(1000);
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

