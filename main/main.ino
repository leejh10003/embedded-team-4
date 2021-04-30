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

#ifndef LIMIT_DISTNACE_TO_OBJECT
#define LIMIT_DISTNACE_TO_OBJECT 5
#endif

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
      Serial.println(F("Disconnecting from server..."));
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
    const char endpoint[] PROGMEM = "http://ec2-13-125-229-44.ap-northeast-2.compute.amazonaws.com/qrcode";
    return ImageProcess::getInfoFromServer(endpoint);
  }
  static StaticJsonDocument<200> foodInfoGet(/*TODO: Need to determine arguments for foodInfoGet*/){
    const char endpoint[] PROGMEM = "http://ec2-13-125-229-44.ap-northeast-2.compute.amazonaws.com/food";
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

double duration, distance;
int index = 0;
int trigPin = 7;
int echoPin = A0;

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
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print(F("Signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}

/*
 * From here, authorized by Park, Jihun
 */
class LCDPrinter{
  private:
  static const int lcdColumns = 20;
  public:
  static LiquidCrystal_I2C LCDPrinter::lcd;
  static void string_print(int offset, int cursor_num, String line);
};
static LiquidCrystal_I2C LCDPrinter::lcd(0x27, LCDPrinter::lcdColumns, 2);
static void LCDPrinter::string_print(int offset, int cursor_num, String line){
  line += " ";
  lcd.setCursor(0,cursor_num);
  int ilen = line.length();
  lcd.print(line.substring(offset, min(ilen, offset + lcdColumns)));
} //offset - substring의 시작점 / cursor_num - LCD 디스플레이의 몇번째 줄에 출력할 것인지 / line - 출력하려는 문자열
/*
 * End of implementaion from Park, Jihun
 */ 

void setup()
{
  // 초음파 센서를 위한 setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, OUTPUT);
  // initialize serial for debugging
  Serial.begin(9600);

  // initialize serial for ESP module, softwear serial
  Serial1.begin(9600);

  // initialize ESP module
  WiFi.init(&Serial1);
  
  // initialize LCD
  LCDPrinter::lcd.init();
  LCDPrinter::lcd.clear();
  LCDPrinter::lcd.backlight();

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present"));

    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);

    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  
  // you're connected now, so print out the data
  Serial.println(F("You're connected to the network"));

  printWifiStatus();
  
  Serial.println();
  Serial.println(F("Starting connection to server..."));

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

class ObjectDetection {
  public:
  static void object_in_distance(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration / 29 / 2;
    return distance < LIMIT_DISTNACE_TO_OBJECT ? true : false;
  }
}

void loop()
{
  if (ObjectDetection::object_in_distance() == true){ // 초음파센서를 통해서 감지된 새로운 식품 등록, TODO: 거리 탐지
    client.println("food");
    StaticJsonDocument<200> foodInfo = ImageProcess::foodInfoGet(); //TODO: Image를 받아와야 함
    client.println("http://www.bizhows.com");
    StaticJsonDocument<200> qrCodeData = ImageProcess::decodeQrCode(); //TODO: Image를 받아와야 함
    time_t present;
    stock[index].lb = qrCodeData["data"]; //qr코드를 디코딩한 값.
    stock[index].ki = foodInfo["name"]; //재고의 종류 -> string으로 표현
    stock[index].registertime = time(&present); //등록일 -> datetime or timestamp interger사용.
    stock[index].maxavailable = foodInfo["max_availability"]; //저장가능시간, timediff or timestamp integer
  } 

  String arr = ''; // 기한이 얼마 남지 않은 음식 목록 string
  for(int i = 0; i < index; i++){
    time_t now;
    if(timecal(stock[i].registertime + stock[i].maxavailable, time(&now))){ //시간이 등록시간+저장가능시간 - 현재시간 <= n=2면 true    
      arr = arr + stock[i].ki + ', '; //arr에 그 index 재고의 종류 표시.
    }
  }

  // 재고정보 display 표시
  String line1 = F("The Expiration Date"); //맨 윗줄
  String line2 = "Expiration date of" + arr + "has remained 3 days. "; //두 번째 줄 -> 유통기한 임박 재고들 표시.
  LCDPrinter::string_print(0 ,0, line1); // line1의 길이가 20이상이 아닌 경우
  for(int i = 0; i < line2.length(); i++){
    LCDPrinter::string_print(i ,1, line2); // line2의 길이가 20이상인 경우
    delay(500);
  }   
}

