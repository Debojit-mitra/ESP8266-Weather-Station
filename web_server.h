// =============================================================================
// web_server.h - Web server management header
// =============================================================================
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "sensor_manager.h"
#include "weather_manager.h"
#include "config.h"

class WebServerManager {
public:
  WebServerManager();  // Declare constructor
  void begin(SensorManager* sensors, WeatherManager* weather);
  void handleClient();
  
private:
  ESP8266WebServer server;
  SensorManager* sensorManager;
  WeatherManager* weatherManager;
  
  void setupRoutes();
  void handleRoot();
  void handleJsonData();
  void handleNotFound();
  
  bool validateAPIKey();
  String createJsonResponse(bool includeWeather = false, bool forceRefresh = false);
  void addSystemInfo(JsonObject& system);
  void addWiFiInfo(JsonObject& wifi);
  void addWeatherInfo(JsonObject& weather);
  void setCorsHeaders();
  
  String formatUptime(unsigned long seconds);
};

#endif