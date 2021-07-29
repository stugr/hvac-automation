#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>
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

String serverMsg = "";

ESP8266WebServer server(80);

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
  
  Serial.println("Server started");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  server.on("/", HTTP_GET, handleRoot);
  server.on(UriBraces("/FAN{}"), HTTP_GET, handleFan);
  //, fanButton(1));
  //server.on("/FANDOWN", HTTP_GET, fanButton(-1));
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  debugPrint("Fan is currently set to: ");
  debugPrintln(fanSpeed);
  fanForm();
  serverRespond();
}

void handleNotFound() {
  server.send(404, "text/html", "404 mate");
}

void handleFan() {
  String req = server.pathArg(0);
  if (req == "UP") {
    fanButton(1);
  } else if (req == "DOWN") {
    fanButton(-1);
  } else {
    handleNotFound();
  }
}

void fanForm() {
  addServerMsg("<br /><form action=\"/FANUP\" method=\"GET\"><input type=\"submit\" value=\"FAN UP\"></form><form action=\"/FANDOWN\" method=\"GET\"><input type=\"submit\" value=\"FAN DOWN\"></form>");
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

  fanForm();
  serverRespond();
}

template<class T> void debugPrint(T param)
{
  Serial.print(param);
  addServerMsg(String(param));
}

template<class T> void debugPrintln(T param)
{
  Serial.println(param);
  addServerMsg(String(param));
  addServerMsg("<br />");
}

void addServerMsg(String msg) {
  serverMsg += msg;
}

void serverRespond() {
  server.send(200, "text/html", serverMsg);
  serverMsg = "";
}
