#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
#include <ArduinoJson.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6,7);
#endif

char ssid[] = "Embedded";
char pass[] = "12341234";
int keyIndex = 0;
int status = WL_IDLE_STATUS;

WiFiEspServer server(80);

void setup(){
  Serial.begin(115200);
  Serial1.begin(9600);
  WiFi.init(&Serial1);

  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println("WiFi shield not present");
    while (true);
  }

  while(status!=WL_CONNECTED){
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();

  server.begin();
}

void loop(){
  WiFiEspClient client = server.available();
}

void printWifiStatus(){
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}
