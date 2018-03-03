
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "hackathongi";
const char* password = "tarla2018";
const char* fiwareHost = "84.89.60.4";

const char* eId = "daniTest2";
const char* eType = "sensor";
const char* eDescription = "My description here ...";
const char* eVar1Type = "Float";
float eVar1Value = 2.25;



void setup() {
  Serial.begin(115200);
  delay(100);
  connectWiFi();
  checkFiware();
  registerEntity();
  delay(100);
}

void loop() {
  delay(5000);
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
  Serial.println("Posting JSON data to:");
  Serial.println(url);

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

/*JsonObject& fetchJson (String endpoint) {

  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(8) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);

  // Use WiFiClient class to create TCP connections
  WiFiClient client = fiwareConnection();
  
  Serial.print("Requesting URL: ");
  Serial.print(fiwareHost);
  Serial.println(endpoint);
  
  // This will send the request to the server
  client.print(String("GET ") + endpoint + " HTTP/1.1\r\n" +
               "Host: " + fiwareHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
    return jsonBuffer.parseObject("{}");
    }
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return jsonBuffer.parseObject("{}");
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return jsonBuffer.parseObject("{}");
  }

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    return jsonBuffer.parseObject("{}");
  }
  // Disconnect & return
  client.stop();
  return root;
  
}*/

/* ======================================================= */

/*WiFiClient fiwareConnection() {
  
  Serial.print("connecting to ");
  Serial.println(fiwareHost);
  
   // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(fiwareHost, httpPort)) {
    Serial.println("connection failed");
  }
  return client;
  
}*/

/* ======================================================= */

/*void registerEntity() {
  
  WiFiClient client = fiwareConnection();
  
}*/

/* ======================================================= */

/*JsonObject& jsonEntity() {
  
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
}*/
