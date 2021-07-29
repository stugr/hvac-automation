#include <ESP8266WiFi.h> 
#include "config-wifi.h"

const int upPin = 5;
const int downPin = 4; // TODO: deal with weak pull up on reset

// wifi
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// TODO: read starting speed from EEPROM
int fanSpeed = 0;

// fan speed bounds
const int fanSpeedLowerBound = 0;
const int fanSpeedUpperBound = 3;

// TODO: change to use ESP8266WebServer
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10); 

  // setup pins
  pinMode(upPin, OUTPUT);
  digitalWrite(upPin, LOW);
  pinMode(downPin, OUTPUT);
  digitalWrite(downPin, LOW);
 
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); 
  server.begin();
  Serial.println("Server started");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

WiFiClient client;

void loop() {

  client = server.available();
  if (!client) {
    return;
  } else {
    //Serial.println("new client");
    while(!client.available()){
      delay(1);
    } 
    String request = client.readStringUntil('\r');

    // ignore favicon requests
    if (request.indexOf("/favicon.ico") != -1) {
      return;
    }
    
    Serial.println(request);
    client.flush(); 
    int value = LOW;
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); 
    client.println("");
    client.println("");

    debugPrint("Fan is currently set to: ");
    debugPrintln(fanSpeed);

    // fan changes
    if (request.indexOf("/FAN=UP") != -1)  {
      fanButton(1);
    }
    if (request.indexOf("/FAN=DOWN") != -1)  {
      fanButton(-1);
    }
  
    client.println(" ");
    delay(1);
    Serial.println("Client disconnected");
    Serial.println("");
  }
}

void fanButton(int direction) {
  // if already at upper or lower bounds (0-3) don't do anything
  int requestedFanSpeed = constrain(fanSpeed + direction, fanSpeedLowerBound, fanSpeedUpperBound);
  if (requestedFanSpeed == fanSpeed) {
    debugPrintln("Fan is already at min or max");
  } else {
    debugPrint("Adjusting fan: ");
    debugPrintln(direction);
    
    int thePin = upPin;
    if (direction == -1) {
      thePin = downPin;
    }

    digitalWrite(thePin, HIGH);
    delay(100); // TODO: test lower values (10 was too low)
    digitalWrite(thePin, LOW);

    fanSpeed = requestedFanSpeed;

    debugPrint("New speed: ");
    debugPrintln(fanSpeed);
  }
}

template<class T> void debugPrint(T param)
{
  Serial.print(param);
  client.print(param);
}

template<class T> void debugPrintln(T param)
{
  Serial.println(param);
  client.print(param);
  client.println("<br />");
}
