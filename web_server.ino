// =============================================================================
// web_server.cpp - Web server management implementation
// =============================================================================
#include "web_server.h"

WebServerManager::WebServerManager() : server(WEB_SERVER_PORT) {
  sensorManager = nullptr;
  weatherManager = nullptr;
}

void WebServerManager::begin(SensorManager* sensors, WeatherManager* weather) {
  sensorManager = sensors;
  weatherManager = weather;
  
  Serial.println("Initializing web server...");
  
  setupRoutes();
  server.begin();
  
  Serial.println("Web server started on port " + String(WEB_SERVER_PORT));
  Serial.println("Available endpoints:");
  Serial.println("  GET / - API information");
  Serial.println("  GET /jsondata?key=API_KEY - JSON sensor data");
  Serial.println("  GET /jsondata?key=API_KEY&weather=true - JSON sensor data with weather");
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
  message += "  GET /jsondata?key=YOUR_API_KEY - Get sensor data in JSON format\n";
  message += "  GET /jsondata?key=YOUR_API_KEY&weather=true - Get sensor data with weather\n";
  message += "  GET /jsondata?key=YOUR_API_KEY&weather=true&forcerefresh=true - Force refresh weather data\n\n";
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
  
  // Check if weather data is requested
  bool includeWeather = server.hasArg("weather") && server.arg("weather") == "true";
  bool forceRefresh = server.hasArg("forcerefresh") && server.arg("forcerefresh") == "true";
  
  setCorsHeaders();
  server.sendHeader("Cache-Control", "max-age=" + String(JSON_CACHE_TIME));
  
  String jsonResponse = createJsonResponse(includeWeather, forceRefresh);
  server.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleNotFound() {
  String message = "{\"error\":\"Endpoint not found\",\"available_endpoints\":[\"/\",\"/jsondata\"]}";
  server.send(404, "application/json", message);
}

bool WebServerManager::validateAPIKey() {
  return server.hasArg("key") && server.arg("key") == API_KEY;
}

String WebServerManager::createJsonResponse(bool includeWeather, bool forceRefresh) {
  DynamicJsonDocument doc(1536);  // Increased size for weather data
  SensorData data = sensorManager->getData();
  
  // Sensor data
  JsonObject sensors = doc.createNestedObject("sensors");
  sensors["temperature_dht"] = data.temp_dht;
  sensors["temperature_bmp"] = data.temp_bmp;
  sensors["temperature_avg"] = data.temp_avg;
  sensors["humidity"] = data.humidity;
  sensors["pressure"] = data.pressure;
  sensors["air_quality_raw"] = data.air_quality_raw;
  sensors["air_quality_ppm"] = data.air_quality_ppm;
  sensors["air_quality_aqi"] = data.air_quality_aqi;
  sensors["air_quality_status"] = data.air_quality_status;
  sensors["is_valid"] = data.is_valid;
  
  // System information
  JsonObject system = doc.createNestedObject("system");
  addSystemInfo(system);
  
  // WiFi information
  JsonObject wifi = doc.createNestedObject("wifi");
  addWiFiInfo(wifi);
  
  // Weather information (if requested)
  if (includeWeather && weatherManager) {
    bool fetchResult;
    
    // Use force refresh if requested, otherwise use normal fetch with interval checking
    if (forceRefresh) {
      Serial.println("Force refreshing weather data via API request");
      fetchResult = weatherManager->forceRefreshWeatherData();
    } else {
      fetchResult = weatherManager->fetchWeatherData();
    }
    
    JsonObject weather = doc.createNestedObject("weather");
    addWeatherInfo(weather);
    
    // Add additional metadata about data freshness
    unsigned long lastUpdate = weatherManager->getLastUpdateTime();
    if (lastUpdate > 0) {
      unsigned long timeSinceUpdate = (millis() - lastUpdate) / 1000; // seconds
      weather["seconds_since_update"] = timeSinceUpdate;
      weather["can_fetch_new"] = weatherManager->canFetchNewData();
      weather["force_refreshed"] = forceRefresh;
    }
  }
  
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

void WebServerManager::addWeatherInfo(JsonObject& weather) {
  WeatherData data = weatherManager->getWeatherData();
  
  if (data.is_valid) {
    // Important weather data to include in jsondata
    weather["location"] = data.location_name + ", " + data.location_region;
    weather["temp_c"] = data.temp_c;
    weather["temp_f"] = data.temp_f;
    weather["condition"] = data.condition_text;
    weather["condition_icon"] = data.condition_icon;
    weather["pressure_mb"] = data.pressure_mb;
    weather["humidity"] = data.humidity;
    weather["feelslike_c"] = data.feelslike_c;
    weather["uv"] = data.uv;
    weather["last_update"] = data.last_update;
    weather["is_valid"] = true;
    // air quality object
    JsonObject airquality = weather.createNestedObject("airquality");
    airquality["co"] = data.co;
    airquality["no2"] = data.no2;
    airquality["o3"] = data.o3;
    airquality["so2"] = data.so2;
    airquality["pm2_5"] = data.pm2_5;
    airquality["pm10"] = data.pm10;
    airquality["us_epa_index"] = data.us_epa_index;
    airquality["gb_defra_index"] = data.gb_defra_index;
  } else {
    weather["is_valid"] = false;
    weather["error"] = data.error_message.isEmpty() ? "Weather data not available" : data.error_message;
  }
}