#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

gpio_num_t LED = GPIO_NUM_2;
gpio_num_t BUTTON = GPIO_NUM_4;

const char* ssid = "ESP32_Access_Point";
const char* password = "12345678";

WebServer server(80);
DNSServer dnsServer;

void handleRoot();
void handleAbout();
void handleNotFound();
void handleNetworks();

bool ledState = false;
int brightness = 0;
int fadeAmount = 5;

void setup() {
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);
  gpio_set_direction(BUTTON, GPIO_MODE_INPUT);

  Serial.begin(115200);
  Serial.println("Rozpoczynanie konfiguracji...");

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Adres IP Access Point: ");
  Serial.println(IP);

  dnsServer.start(53, "*", IP);

  if (!MDNS.begin("esp32")) {
    Serial.println("Błąd podczas konfiguracji mDNS!");
  }

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/about", HTTP_GET, handleAbout);
  server.on("/networks", HTTP_GET, handleNetworks);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Serwer HTTP uruchomiony");

  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED, 0);
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  analogWrite(LED, brightness);
  ledcWrite(0, brightness);
  brightness = brightness + fadeAmount;
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  if (digitalRead(BUTTON) == LOW) {
    ledState = !ledState;
    digitalWrite(LED, ledState);
    delay(300); // Debounce delay
  }
}

void handleRoot() {
  Serial.println("Obsługa żądania root");
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Nie udało się otworzyć index.html");
    server.send(404, "text/plain", "Plik nie znaleziony");
    return;
  }

  String html = file.readString();
  server.send(200, "text/html", html);
  file.close();
}

void handleAbout() {
  Serial.println("Obsługa żądania about");
  File file = SPIFFS.open("/about.html", "r");
  if (!file) {
    Serial.println("Nie udało się otworzyć about.html");
    server.send(404, "text/plain", "Plik nie znaleziony");
    return;
  }

  String html = file.readString();
  Serial.println("Wysyłanie zawartości about.html:");
  Serial.println(html);
  server.send(200, "text/html", html);
  file.close();
}

void handleNotFound() {
  Serial.println("Obsługa żądania nie znaleziono");
  server.send(404, "text/plain", "404: Nie znaleziono");
}
void handleNetworks() {
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    DynamicJsonDocument doc(1024);
    JsonArray networks = doc.to<JsonArray>();
    for (int i = 0; i < n; ++i) {
      JsonObject network = networks.add<JsonObject>();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
    }
    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  }
}
/*#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

gpio_num_t LED = GPIO_NUM_2;

const char* ssid = "ESP32_Access_Point";
const char* password = "12345678";

WebServer server(80);
DNSServer dnsServer;

void handleRoot();
void handleAbout();
void handleNotFound();
void handleNetworks();

bool ledState = false;
int brightness = 0;
int fadeAmount = 5;

void setup() {
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  Serial.begin(115200);
  Serial.println("Rozpoczynanie konfiguracji...");

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Adres IP Access Point: ");
  Serial.println(IP);

  dnsServer.start(53, "*", IP);

  if (!MDNS.begin("esp32")) {
    Serial.println("Błąd podczas konfiguracji mDNS!");
  }

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/about", HTTP_GET, handleAbout);
  server.on("/networks", HTTP_GET, handleNetworks);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Serwer HTTP uruchomiony");

  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED, 0);
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  analogWrite(LED, brightness);
  ledcWrite(0, brightness);
  brightness = brightness + fadeAmount;
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
}

void handleRoot() {
  Serial.println("Obsługa żądania root");
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Nie udało się otworzyć index.html");
    server.send(404, "text/plain", "Plik nie znaleziony");
    return;
  }

  String html = file.readString();
  server.send(200, "text/html", html);
  file.close();
}

void handleAbout() {
  File file = SPIFFS.open("/about.html", "r");
  if (!file) {
    Serial.println("Nie udało się otworzyć about.html");
    server.send(404, "text/plain", "Plik nie znaleziony");
    return;
  }

  String html = file.readString();
  Serial.println("Wysyłanie zawartości about.html:");
  Serial.println(html);
  server.send(200, "text/html", html);
  file.close();
}

void handleNotFound() {
  Serial.println("Obsługa żądania nie znaleziono");
  server.send(404, "text/plain", "404: Nie znaleziono");
}

void handleNetworks() {
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    DynamicJsonDocument doc(1024);
    JsonArray networks = doc.to<JsonArray>();
    for (int i = 0; i < n; ++i) {
      JsonObject network = networks.add<JsonObject>();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
    }
    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  }
}*/