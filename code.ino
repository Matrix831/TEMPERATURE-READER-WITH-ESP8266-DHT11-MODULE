#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

// --- Hardware Pins ---
#define DHTPIN 5        // NodeMCU pin D1
#define LED_PIN 14      // NodeMCU pin D5 (External Breathing LED)
#define ONBOARD_LED 2   // NodeMCU pin D4 (Built-in LED, usually active-low)
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// --- Calibration Offsets ---
// Adjust these values if you need to fine-tune further against your reference meter
const float TEMP_OFFSET = -1.3; 
const float HUM_OFFSET = 15.0;  

// --- Wi-Fi Access Point Credentials ---
const char* ssid = "ESP8266"; 
const char* password = "12345678"; 

ESP8266WebServer server(80);

// --- Global Variables ---
float t_global = NAN;
float h_global = NAN;

// --- Timers for Non-Blocking Tasks ---
unsigned long previousSensorMillis = 0;
const long sensorInterval = 2500; // Read sensor every 2.5 seconds

// Slower fade settings
unsigned long previousFadeMillis = 0;
const long fadeInterval = 4;      // Wait 4ms between steps
int ledBrightness = 0;
int fadeAmount = 1;               // Smallest step size for maximum smoothness

// Blink settings
unsigned long previousBlinkMillis = 0;
const long blinkInterval = 1000;  // Blink every 1000 milliseconds (1 second)
bool onboardLedState = HIGH;      // Start HIGH (which means OFF for the onboard LED)


// --- Function to build the Web Page ---
void handleRoot() {
  String temp = isnan(t_global) ? "--" : String(t_global, 1);
  String hum  = isnan(h_global) ? "--" : String(h_global, 0);

  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>Sensor Dashboard</title>";
  html += "<meta http-equiv=\"refresh\" content=\"5\">"; // Auto-refresh every 5s
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #121218; color: #f0f0f0; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; height: 90vh; }";
  html += ".card { background-color: #23232d; border-radius: 10px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 16px rgba(0,0,0,0.5); }";
  html += "h2 { color: #78788c; margin-top: 0; font-size: 1.5rem; text-align: center; margin-bottom: 25px; }";
  html += ".data-container { display: flex; justify-content: space-between; }";
  html += ".data-box { text-align: center; width: 45%; }";
  html += ".label { font-size: 1rem; font-weight: bold; }";
  html += ".temp-label { color: #ff5722; }"; 
  html += ".hum-label { color: #03a9f4; }";  
  html += ".value { font-size: 3rem; font-weight: bold; margin-top: 10px; }";
  html += ".unit { font-size: 1.2rem; font-weight: normal; }";
  html += "</style></head><body>";

  html += "<div class=\"card\"><h2>ENVIRONMENT</h2>";
  html += "<div class=\"data-container\">";
  html += "<div class=\"data-box\"><div class=\"label temp-label\">TEMP</div><div class=\"value\">" + temp + "<span class=\"unit\">&deg;C</span></div></div>";
  html += "<div class=\"data-box\"><div class=\"label hum-label\">HUMIDITY</div><div class=\"value\">" + hum + "<span class=\"unit\">%</span></div></div>";
  html += "</div></div>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Initialize External LED
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);

  // Initialize Onboard LED
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, onboardLedState);

  dht.begin();

  Serial.println("\nStarting Access Point...");
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // 1. Listen for web browsers
  server.handleClient();

  unsigned long currentMillis = millis();

  // 2. External LED Breathing Animation
  if (currentMillis - previousFadeMillis >= fadeInterval) {
    previousFadeMillis = currentMillis;

    analogWrite(LED_PIN, ledBrightness);
    ledBrightness = ledBrightness + fadeAmount;

    if (ledBrightness <= 0 || ledBrightness >= 1023) {
      fadeAmount = -fadeAmount;
    }
  }

  // 3. Onboard LED Blinking
  if (currentMillis - previousBlinkMillis >= blinkInterval) {
    previousBlinkMillis = currentMillis;
    
    // Toggle the state (если он был HIGH, сделать LOW, и наоборот)
    onboardLedState = !onboardLedState;
    digitalWrite(ONBOARD_LED, onboardLedState);
  }

  // 4. Read sensor cleanly in the background with calibration applied
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;

    float temp_t = dht.readTemperature();
    float temp_h = dht.readHumidity();

    if (!isnan(temp_t) && !isnan(temp_h)) {
      t_global = temp_t + TEMP_OFFSET;
      h_global = temp_h + HUM_OFFSET;
      
      Serial.print("Updated -> Temp: ");
      Serial.print(t_global);
      Serial.print("C, Hum: ");
      Serial.print(h_global);
      Serial.println("%");
    }
  }
}
