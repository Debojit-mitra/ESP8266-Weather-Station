// =============================================================================
// weather_manager.h - Weather API management header
// =============================================================================
#ifndef WEATHER_MANAGER_H
#define WEATHER_MANAGER_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "config.h"

struct WeatherData {
  // Location data
  String location_name;
  String location_region;
  String location_country;
  float lat;
  float lon;
  String local_time;
  
  // Weather data
  float temp_c;
  float temp_f;
  String condition_text;
  String condition_icon;
  float pressure_mb;
  int humidity;
  int cloud;
  float feelslike_c;
  float feelslike_f;
  float uv;
  
  // Air quality data
  float co;
  float no2;
  float o3;
  float so2;
  float pm2_5;
  float pm10;
  int us_epa_index;
  int gb_defra_index;
  
  // Status
  bool is_valid;
  String error_message;
  unsigned long last_update;
};

class WeatherManager {
public:
  WeatherManager();
  void begin();
  bool fetchWeatherData();
  bool fetchWeatherData(const String& location);
  bool forceRefreshWeatherData();  // Force fetch ignoring interval
  bool forceRefreshWeatherData(const String& location);  // Force fetch with location
  WeatherData getWeatherData();
  bool isDataValid();
  unsigned long getLastUpdateTime();
  bool canFetchNewData();  // Check if enough time has passed for new API call
  
private:
  WeatherData weatherData;
  WiFiClient wifiClient;
  
  bool makeAPIRequest(const String& url, String& response);
  bool parseWeatherResponse(const String& response);
  void clearWeatherData();
  String buildAPIUrl(const String& location);  // Helper method to construct API URL
};

#endif
