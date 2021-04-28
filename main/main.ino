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

char server[] = "http://ec2-13-125-229-44.ap-northeast-2.compute.amazonaws.com"; // 서버 API가 구현되면 채워 넣어야 함.
class ImageProcess{
  private:
  static WiFiEspClient client;
  static StaticJsonDocument<200> getInfoFromServer(const char* endpoint/*TODO: Image를 위한 argument 추가*/){
    client.connect(server, 80);
    ImageProcess::client.connect(server, 80);
    const char* d;
    while (client.available()) {
      char c = client.read();
      if(c == '\r') break;
      Serial.write(c);
      d += c;   
    }
    if (!client.connected()) {
      Serial.println();
      Serial.println("Disconnecting from server...");
      client.stop();
    }
    if(d != NULL){
      StaticJsonDocument<200> doc;
    
      auto error = deserializeJson(doc, d);
      if (error) {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(error.c_str());
      } else {
        return doc;
      }
    }
  }
  public:
  static StaticJsonDocument<200> decodeQrCode/*TODO: Need to determine arguments for decodeQrCode*/(){
    const char endpoint[] = "http://ec2-13-125-229-44.ap-northeast-2.compute.amazonaws.com/qrcode";
    return ImageProcess::getInfoFromServer(endpoint);
  }
  static StaticJsonDocument<200> foodInfoGet(/*TODO: Need to determine arguments for foodInfoGet*/){
    const char endpoint[] = "http://ec2-13-125-229-44.ap-northeast-2.compute.amazonaws.com/food";
    return ImageProcess::getInfoFromServer(endpoint);
  }
};
static WiFiEspClient ImageProcess::client;

struct {
  const char* lb;
  const char* ki;
  long registertime;
  long maxavailable;
} stock[20];

int index = 0;

// Initialize the Ethernet client object


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
  /*if (client.connect(server, 80)) {
    Serial.println("Connected to server");

    // Make a HTTP request
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.cc");
    client.println("Connection: close");
    client.println();
  }*/
}

void loop()
{
  StaticJsonDocument<200> foodInfo = ImageProcess::foodInfoGet(); //TODO: Image를 받아와야 함
  StaticJsonDocument<200> qrCodeData = ImageProcess::decodeQrCode(); //TODO: Image를 받아와야 함
  stock[index].lb = qrCodeData["data"];
  stock[index].ki = foodInfo["name"];
  //stock[index].registertime = doc["registered_at"]; //서버에서 반환하지 않습니다. client에서 결정해야 합니다.
  stock[index].maxavailable = foodInfo["max_availability"]; //조만간 서버에서 결정된 값으로 반환할 예정입니다. 현재는 psuedo하게 값을 10으로 반환합니다.

  /*index++;
  }
  else{
    index = 0;
  }*/

  for(int i = 0; i < index; i++){
    if(true){//TODO: 남은 시간이 얼마 안 남았을 때){
      // 재고정보 display 표시
    }
  }
}
