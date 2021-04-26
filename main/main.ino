#include <WiFiEsp.h>
#include <ArduinoJson.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#endif
#include "SoftwareSerial.h"

SoftwareSerial Serial1(2, 3); // RX, TX

char ssid[] = "Embedded4";            // your network SSID (name)
char pass[] = "12341234";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = ""; // 서버 API가 구현되면 채워 넣어야 함.

struct {
  const char* lb;
  const char* ki;
  long registertime;
  long maxavailable;
} stock[20];

int index = 0;

// Initialize the Ethernet client object
WiFiEspClient client;

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);

  // initialize serial for ESP module
  Serial1.begin(9600);

  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");

    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  
  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  printWifiStatus();
  
  Serial.println();
  Serial.println("Starting connection to server...");

  // if you get a connection, report back via serial
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");

    // Make a HTTP request
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.cc");
    client.println("Connection: close");
    client.println();
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    const char* c = client.read();
    Serial.write(c);

    StaticJsonDocument<200> doc;
    auto error = deserializeJson(doc, c);

    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    
    stock[index].lb = doc["label"];
    stock[index].ki = doc["kind"];
    stock[index].registertime = doc["registered_at"];
    stock[index].maxavailable = doc["max_availability"];

    index++;
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}
