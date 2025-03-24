#include <WiFiNINA.h>
#include <BH1750.h>
#include <WiFiClient.h>

// WiFi Credentials
#define WIFI_SSID "AndroidAP6B99"
#define WIFI_PASSWORD "jnjm2364"

// IFTTT Webhooks URL 
#define IFTTT_URL "https://maker.ifttt.com/trigger/light_level_event/with/key/bKH0ldacoLazd9mwUaR8Wq"

// Initialize BH1750 sensor
BH1750 lightMeter;

// WiFi Client
WiFiClient client;

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

  // Send light level to IFTTT
  sendToIFTTT(lightLevel);

  delay(20000);  // Send data every 20 seconds
}

void sendToIFTTT(float lightLevel) {
  if (client.connect("maker.ifttt.com", 80)) {  // Connect to IFTTT server
    // Convert the URL to a String before connecting
    String url = String(IFTTT_URL) + "?value1=" + String(lightLevel);

    client.print("GET " + url + " HTTP/1.1\r\n");
    client.print("Host: maker.ifttt.com\r\n");
    client.print("Connection: close\r\n\r\n");
    Serial.println("Sent data to IFTTT");
  } else {
    Serial.println("Connection to IFTTT failed");
  }
  client.stop();  // Close the connection
}
