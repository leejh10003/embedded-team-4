#include <ArduinoJson.h>

using namespace std;
void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

  StaticJsonDocument<200> doc;

  char json[] = "{\"food\":\'\',\"shelf_life\":\'\'}";

  auto error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed with cod "));
    Serial.println(error.c_str());
    return;
  }
  const char* food = doc["food"];
  const char* sh_life = doc["shelf_life"];

  Serial.println(food);
  Serial.println(sh_life);
}

void loop() {
  return;
}
