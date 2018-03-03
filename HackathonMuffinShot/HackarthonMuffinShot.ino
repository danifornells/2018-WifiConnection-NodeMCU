#include <Servo.h> 
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

static const uint8_t pin01   = 5;
static const uint8_t moute = 99;
static const uint8_t atura = 93;

const char* ssid     = "hackathongi";
const char* password = "tarla2018";
const char* fiwareHost = "84.89.60.4";

const int IP = 200;

const char* eId = "muffin_shot";
const char* eAttrName = "shot";

String eAttrValue = "off";

Servo myservo;  
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);
  connectWiFi();
  checkFiware();
  delay(100);
  server.on("/", HTTP_POST, handleBody);
  server.begin();
}
 
void dispara() {
  myservo.attach(pin01);    
  Serial.begin(9600);
  Serial.print("Comencem"); 
  myservo.write(moute);              // tell servo to go to position in variable 'pos'   
  delay(1700);
  myservo.write(atura);              
  myservo.detach();

}


void loop() {
  server.handleClient();
}

/* ======================================================= */

void handleBody() { //Handler for the body path
  String response = server.arg("plain");
  String message = "Body received:\n";
         message += response;
         message += "\n";
  Serial.println(message);
  server.send(200, "text/plain", message);
  DynamicJsonBuffer jsonBuffer(256);
  JsonObject& jsonResponse = jsonBuffer.parseObject(response);
  if (jsonResponse["data"][0]["id"] == eId) {
    Serial.println("received action for " + String(eId));
    if (jsonResponse["data"][0][eAttrName]["value"] == "on") {
      Serial.println("received ON action for " + String(eId));
      dispara();
    } else if (jsonResponse["data"][0][eAttrName]["value"] == "off") {
      Serial.println("received OFF action for " + String(eId));
    }
  }
}

/* ======================================================= */

void connectWiFi(){
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println("---------------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Configure static IP
  IPAddress ip(192, 168, 4, IP);
  IPAddress gateway(192, 168, 4, 254); // set gateway to match your network
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  
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

