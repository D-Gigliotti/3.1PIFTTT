#include <WiFiNINA.h>
#include <BH1750.h>
#include <WiFiClient.h>

// WiFi Credentials
#define WIFI_SSID "AndroidAP6B99"
#define WIFI_PASSWORD "jnjm2364"

// IFTTT Webhooks URLs for two applets
#define IFTTT_URL_HIGH "https://maker.ifttt.com/trigger/light_level_high/with/key/bKH0ldacoLazd9mwUaR8Wq"
#define IFTTT_URL_LOW "https://maker.ifttt.com/trigger/light_level_low/with/key/bKH0ldacoLazd9mwUaR8Wq"

// Initialize BH1750 sensor
BH1750 lightMeter;

// WiFi Client
WiFiClient client;

// Light level threshold
#define LIGHT_THRESHOLD 100 

// To track the last state of the light level (high/low)
bool lastStateHigh = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize BH1750 sensor
  if (lightMeter.begin()) {
    Serial.println("BH1750 sensor initialized successfully.");
  } else {
    Serial.println("Error initializing BH1750 sensor.");
    while (1);  // Stop if sensor isn't found
  }

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read light level from BH1750 sensor
  float lightLevel = lightMeter.readLightLevel();
  
  // Print light level to Serial Monitor
  Serial.print("Light Level: ");
  Serial.print(lightLevel);
  Serial.println(" lx");

  // Check light level and send notification if changed
  if (lightLevel > LIGHT_THRESHOLD && !lastStateHigh) {
    // Light level has gone high
    sendToIFTTT("high");
    lastStateHigh = true;  // Update the state to high
  } 
  else if (lightLevel <= LIGHT_THRESHOLD && lastStateHigh) {
    // Light level has gone low
    sendToIFTTT("low");
    lastStateHigh = false;  // Update the state to low
  }

  delay(20000);  // Send data every 20 seconds
}

void sendToIFTTT(String level) {
  String url = "";

  // Select the appropriate IFTTT Webhook URL based on the light level
  if (level == "high") {
    url = IFTTT_URL_HIGH;  // Use the high light level Webhook URL
  } else if (level == "low") {
    url = IFTTT_URL_LOW;  // Use the low light level Webhook URL
  }

  if (client.connect("maker.ifttt.com", 80)) {  // Connect to IFTTT server
    // Convert the URL to a String before connecting
    String fullUrl = url + "?value1=" + level;

    client.print("GET " + fullUrl + " HTTP/1.1\r\n");
    client.print("Host: maker.ifttt.com\r\n");
    client.print("Connection: close\r\n\r\n");
    Serial.println("Sent data to IFTTT: " + level);
  } else {
    Serial.println("Connection to IFTTT failed");
  }
  client.stop();  // Close the connection
}

