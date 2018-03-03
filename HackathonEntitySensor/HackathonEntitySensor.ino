
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "hackathongi";
const char* password = "tarla2018";
const char* fiwareHost = "84.89.60.4";

const char* eId = "daniTest2";
const char* eType = "sensor";
const char* eDescription = "My description here ...";
const char* eVar1Type = "Int";
int eVar1Value = 0;
int eVar1ValueTMP = 0;



void setup() {
  Serial.begin(115200);
  delay(100);
  connectWiFi();
  checkFiware();
  registerEntity();
  randomSeed(analogRead(0));
  delay(100);
}

void loop() {
  eVar1ValueTMP = random(2);
  if (eVar1ValueTMP != eVar1Value) {
    eVar1Value = eVar1ValueTMP;
    Serial.println("Change detected");
    postNewData();
  }
  delay(1000);
}

/* ======================================================= */

void postNewData() {
  String url = "http://" + String(fiwareHost) + "/v2/entities/" + String(eId) + "/attrs";
  patchJson(url, SensorData());
}

/* ======================================================= */

void connectWiFi(){
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println("---------------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

/* ======================================================= */

void checkFiware(){
  Serial.println();
  Serial.println("---------------------");
  String url = "http://" + String(fiwareHost) + "/version";
  JsonObject& fiwareVersion = fetchJson(url);
  Serial.println(F("Connected to Fiware Orion, version:"));
  Serial.println(fiwareVersion["orion"]["version"].as<char*>());
}

/* ======================================================= */

void registerEntity(){
  Serial.println();
  Serial.println("---------------------");
  String url = "http://" + String(fiwareHost) + "/v2/entities";
  JsonObject& entityData = jsonEntity();
  postJson(url, entityData);
}

/* ======================================================= */

JsonObject& fetchJson(String url){
  Serial.println("Fetching JSON data from:");
  Serial.println(url);
  String result = "{}";
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK) {
        result = http.getString();
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& jsonResult = jsonBuffer.parseObject(result);
  return jsonResult;
}

/* ======================================================= */

void postJson(String url, JsonObject& data){
  Serial.print("Posting JSON data to: ");
  Serial.print(String(url));
  Serial.print(String(url));

  char JSONmessageBuffer[512];
  data.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
    
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(JSONmessageBuffer);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
}

/* ======================================================= */

void patchJson(String url, JsonObject& data){
  Serial.print("Patching JSON data to: ");
  Serial.print(url);
  Serial.print(" -> ");

  char JSONmessageBuffer[512];
  data.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  
  Serial.print(JSONmessageBuffer);
    
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PATCH(JSONmessageBuffer);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
}

/* ======================================================= */

JsonObject& jsonEntity() {
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& entity = jsonBuffer.createObject();
  entity["id"] = eId;
  entity["type"] = eType;
  JsonObject& description = entity.createNestedObject("description");
  description["value"] = eDescription;
  description["type"] = "String";
  JsonObject& var1 = entity.createNestedObject("var1");
  var1["value"] = eVar1Value;
  var1["type"] = eVar1Type;
  return entity;
}

/* ======================================================= */

JsonObject& SensorData() {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& sData = jsonBuffer.createObject();
  JsonObject& var1 = sData.createNestedObject("var1");
  var1["value"] = eVar1Value;
  var1["type"] = eVar1Type;
  return sData;
}

