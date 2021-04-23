#include <ArduinoJson.h>
#include <WiFi101.h>
#include <SPI.h>

char ssid[] = "TURTLE";
char pass[] = "yesican1";
int keyIndex = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  WiFi.setPins(8, 7, 4, 2);

  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println("WiFi shield not present");
    while (true);
  }
}

void loop() {
  return;
}
