// =============================================================================
// main - Main application file
// =============================================================================
#include "config.h"
#include "wifi_manager.h"
#include "sensor_manager.h" 
#include "weather_manager.h"
#include "web_server.h"
#include "system_status.h"

// Global instances
WiFiManager wifiManager;
SensorManager sensorManager;
WeatherManager weatherManager;
WebServerManager webServer;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Weather Station...");
  
  // Initialize WiFi
  wifiManager.begin();
  
  // Initialize sensors
  sensorManager.begin();
  
  // Initialize weather manager
  weatherManager.begin();
  
  // Initialize web server
  webServer.begin(&sensorManager, &weatherManager);
  
  Serial.println("Weather Station initialized successfully!");
}

void loop() {
  // Update sensors periodically (includes LED status updates)
  sensorManager.update();
  
  // Handle web server requests
  webServer.handleClient();
}
