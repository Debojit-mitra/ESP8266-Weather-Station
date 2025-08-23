// =============================================================================
// web_server.cpp - Web server management implementation
// =============================================================================
#include "web_server.h"

WebServerManager::WebServerManager() : server(WEB_SERVER_PORT) {
  sensorManager = nullptr;
}

void WebServerManager::begin(SensorManager* sensors) {
  sensorManager = sensors;
  
  Serial.println("Initializing web server...");
  
  setupRoutes();
  server.begin();
  
  Serial.println("Web server started on port " + String(WEB_SERVER_PORT));
  Serial.println("Available endpoints:");
  Serial.println("  GET / - API information");
  Serial.println("  GET /jsondata?key=API_KEY - JSON sensor data");
}

void WebServerManager::setupRoutes() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });
  server.on("/jsondata", HTTP_GET, [this]() { handleJsonData(); });
  server.onNotFound([this]() { handleNotFound(); });
}

void WebServerManager::handleClient() {
  server.handleClient();
}

void WebServerManager::handleRoot() {
  String message = "=== Mitra's Weather Station API ===\n\n";
  message += "Endpoints:\n";
  message += "  GET /jsondata?key=YOUR_API_KEY - Get sensor data in JSON format\n\n";
  message += "System Info:\n";
  message += "  IP Address: " + WiFi.localIP().toString() + "\n";
  message += "  Uptime: " + formatUptime(millis() / 1000) + "\n";
  message += "  Free Memory: " + String(ESP.getFreeHeap()) + " bytes\n";
  
  server.send(200, "text/plain", message);
}

void WebServerManager::handleJsonData() {
  if (!validateAPIKey()) {
    server.send(403, "application/json", "{\"error\":\"Unauthorized: Valid API Key Required\"}");
    return;
  }
  
  if (!sensorManager) {
    server.send(500, "application/json", "{\"error\":\"Sensor manager not initialized\"}");
    return;
  }
  
  setCorsHeaders();
  server.sendHeader("Cache-Control", "max-age=" + String(JSON_CACHE_TIME));
  
  String jsonResponse = createJsonResponse();
  server.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleNotFound() {
  String message = "{\"error\":\"Endpoint not found\",\"available_endpoints\":[\"/\",\"/jsondata\"]}";
  server.send(404, "application/json", message);
}

bool WebServerManager::validateAPIKey() {
  return server.hasArg("key") && server.arg("key") == API_KEY;
}

String WebServerManager::createJsonResponse() {
  StaticJsonDocument<768> doc;
  SensorData data = sensorManager->getData();
  
  // Metadata
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  doc["data_age"] = (millis() - data.last_update) / 1000;
  
  // Sensor data
  JsonObject sensors = doc.createNestedObject("sensors");
  sensors["temperature_dht"] = data.temp_dht;
  sensors["temperature_bmp"] = data.temp_bmp;
  sensors["temperature_avg"] = data.temp_avg;
  sensors["humidity"] = data.humidity;
  sensors["pressure"] = data.pressure;
  sensors["air_quality"] = data.air_quality;
  sensors["is_valid"] = data.is_valid;
  
  // System information
  JsonObject system = doc.createNestedObject("system");
  addSystemInfo(system);
  
  // WiFi information
  JsonObject wifi = doc.createNestedObject("wifi");
  addWiFiInfo(wifi);
  
  String json;
  serializeJson(doc, json);
  return json;
}

void WebServerManager::addSystemInfo(JsonObject& system) {
  system["free_heap"] = ESP.getFreeHeap();
  system["heap_fragmentation"] = ESP.getHeapFragmentation();
  system["max_free_block"] = ESP.getMaxFreeBlockSize();
  system["cpu_freq"] = ESP.getCpuFreqMHz();
  system["uptime"] = millis() / 1000;
  system["uptime_formatted"] = formatUptime(millis() / 1000);
  system["free_sketch_space"] = ESP.getFreeSketchSpace();
  system["flash_chip_size"] = ESP.getFlashChipSize();
  system["flash_chip_speed"] = ESP.getFlashChipSpeed() / 1000000;
}

void WebServerManager::addWiFiInfo(JsonObject& wifi) {
  wifi["ssid"] = WiFi.SSID();
  wifi["rssi"] = WiFi.RSSI();
  wifi["ip"] = WiFi.localIP().toString();
  wifi["mac"] = WiFi.macAddress();
}

void WebServerManager::setCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

String WebServerManager::formatUptime(unsigned long seconds) {
  unsigned long days = seconds / 86400;
  unsigned long hours = (seconds % 86400) / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  unsigned long secs = seconds % 60;
  
  String uptime = "";
  if (days > 0) uptime += String(days) + "d ";
  if (hours > 0) uptime += String(hours) + "h ";
  if (minutes > 0) uptime += String(minutes) + "m ";
  uptime += String(secs) + "s";
  
  return uptime;
}