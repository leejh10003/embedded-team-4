/*
 * From here, authored by Yang, Yongjae
 */
#include <WiFiEsp.h> // Wifi(ESP8266 - ESP01)용 라이브러리 -> arduino 라이브러리에 있습니다.
#include <ArduinoJson.h> // Json parsing 용 라이브러리 -> arduino 라이브러리에 있습니다.
#include <time.h> // timestamp 용 라이브러리 -> 별도의 설치 필요 없음
#include <LiquidCrystal_I2C.h> // LCD 2004 I2C용 라이브러리
/*
 * https://github.com/johnrickman/LiquidCrystal_I2C - github에서 다운로드 받으시고 추가해주시면 됩니다.
 */
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#endif
#include "SoftwareSerial.h" // 서버 연결 라이브러리 -> arduino 라이브러리에 있습니다.

SoftwareSerial Serial1(2, 3); // RX, TX

char ssid[] = "Embedded4";            // your network SSID (name)
char pass[] = "12341234";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "http://ec2-13-125-229-44.ap-northeast-2.compute.amazonaws.com"; // 서버 API가 구현되면 채워 넣어야 함.
/*
 * End of implementation from Yang, Yongjae
 */

/*
 * From here, authored by Lee, JunHyuk
 */
class ImageProcess{ //서버에 요청을 보내고, 결과로 온 json해석
  private:
  static WiFiEspClient client;
  static StaticJsonDocument<200> getInfoFromServer(const char* endpoint/*TODO: Image를 위한 argument 추가*/){
    ImageProcess::client.connect(endpoint, 80);
    String d;
    while (client.available()) {
      char c = client.read();
      if(c == '\r') break; //끝났다면 while을 break
      Serial.write(c);
      d += c;  //Python에서 작업한 것으로 추정. C++에서는 이렇게 작업 불가 
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
          Serial.print(F("deserializeJson() failed with code ")); //F가 무엇인지??
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
/*
 * End of implementation from Lee, JunHyuk
 */

/*
 * From here, authorized by Yang, Yongjae
 */
struct {
  const char* lb;
  const char* ki;
  time_t registertime;
  time_t maxavailable;
} stock[20];

int index = 0;

// Initialize the Ethernet client object


int timecal(time_t a, time_t b){
  if(((a-b) / 60 / 60 / 24) <= 2){
    return 1;
  }
  else{
    return 0;
  }
}

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
/*
 * From here, authorized by Park, Jihun
 */
class LCDPrinter{
  public:
  static const int lcdAddress = 0x27;
  static const int lcdColumns = 20;
  static const int lcdRows = 4;
  private:
  static LiquidCrystal_I2C lcd;
  public:
  static void string_print(int cursor_num, String line);
};
static LiquidCrystal_I2C LCDPrinter::lcd(LCDPrinter::lcdAddress, LCDPrinter::lcdColumns, LCDPrinter::lcdRows);
static void LCDPrinter::string_print(int cursor_num, String line){
  lcd.setCursor(0,cursor_num);
  int ilen = line.length();
  String temp;
  if(ilen <= lcdColumns){
    temp = line;
    for(int i = 0; i< lcdColumns - ilen ; i++){
      temp += "";
    }
    lcd.print(temp);
    delay(5000);
  } 
  else{
    String linestringplus = "                    ";
    linestringplus += line;
    linestringplus += "                    ";
    ilen = linestringplus.length();
    for(int i = 1 ; i < ilen - lcdColumns ; i++){
      lcd.setCursor(0,1);
      int end;
      if(ilen < i + lcdColumns){
        end = ilen;
      }
      else{
        end = i + lcdColumns;
      }
    temp = linestringplus.substring(i,end);
    lcd.print(temp);
    delay(500);
    }
  }
}
/*
 * End of implementaion from Park, Jihun
 */ 

void loop()
{
  StaticJsonDocument<200> foodInfo = ImageProcess::foodInfoGet(); //TODO: Image를 받아와야 함
  StaticJsonDocument<200> qrCodeData = ImageProcess::decodeQrCode(); //TODO: Image를 받아와야 함
  time_t present;
  stock[index].lb = qrCodeData["data"];
  stock[index].ki = foodInfo["name"];
  stock[index].registertime = time(&present);
  stock[index].maxavailable = foodInfo["max_availability"];
  
  index++;

  for(int i = 0; i < index; i++){
    time_t now;
    if(timecal(time(&now),stock[i].registertime + stock[i].maxavailable)){
     /*
     * End of implementaion from Yang, Yongjae
     */ 

     /*
    * From here, authorized by Park, Jihun
    */
    String line1 = "The expiration date"; 
    String line2 = "Milk's Expiration date has passed 3 days.";
    // 임시 출력용 string 
    LCDPrinter::string_print(0, line1);
    LCDPrinter::string_print(1, line2);
      // 재고정보 display 표시
     /*
    * End of implementation from Park, Jihun
    */     
    }
  }
}

/*
* End of implementaion from Park, Jihun
 */
