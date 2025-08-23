// =============================================================================
// wifi_manager.cpp - WiFi management implementation
// =============================================================================
#include "wifi_manager.h"

void WiFiManager::begin() {
  Serial.println("Initializing WiFi...");
  
  configureStaticIP();
  connectToWiFi();
  
  Serial.println("WiFi initialization complete!");
  printStatus();
}

void WiFiManager::configureStaticIP() {
  if (!WiFi.config(STATIC_IP, GATEWAY_IP, SUBNET_MASK, PRIMARY_DNS, SECONDARY_DNS)) {
    Serial.println("ERROR: Failed to configure static IP");
  } else {
    Serial.println("Static IP configured successfully");
  }
}

void WiFiManager::connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected successfully!");
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIP() {
  return WiFi.localIP().toString();
}

String WiFiManager::getSSID() {
  return WiFi.SSID();
}

int WiFiManager::getRSSI() {
  return WiFi.RSSI();
}

String WiFiManager::getMacAddress() {
  return WiFi.macAddress();
}

void WiFiManager::printStatus() {
  Serial.println("=== WiFi Status ===");
  Serial.println("SSID: " + getSSID());
  Serial.println("IP Address: " + getIP());
  Serial.println("RSSI: " + String(getRSSI()) + " dBm");
  Serial.println("MAC: " + getMacAddress());
  Serial.println("==================");
}
