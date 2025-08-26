// =============================================================================
// weather_manager.ino - Weather API management implementation
// =============================================================================
#include "weather_manager.h"

WeatherManager::WeatherManager() {
  clearWeatherData();
}

void WeatherManager::begin() {
  Serial.println("Initializing Weather Manager...");
  clearWeatherData();
  Serial.println("Weather Manager initialized");
}

bool WeatherManager::fetchWeatherData() {
  return fetchWeatherData(WEATHER_API_LOCATION);
}

bool WeatherManager::forceRefreshWeatherData() {
  return forceRefreshWeatherData(WEATHER_API_LOCATION);
}

bool WeatherManager::forceRefreshWeatherData(const String& location) {
  if (WiFi.status() != WL_CONNECTED) {
    weatherData.error_message = "WiFi not connected";
    weatherData.is_valid = false;
    return false;
  }
  
  Serial.println("Force fetching fresh weather data for: " + location);
  
  // Construct API URL
  String url = buildAPIUrl(location);
  
  String response;
  if (!makeAPIRequest(url, response)) {
    return false;
  }
  
  return parseWeatherResponse(response);
}

bool WeatherManager::fetchWeatherData(const String& location) {
  if (WiFi.status() != WL_CONNECTED) {
    weatherData.error_message = "WiFi not connected";
    weatherData.is_valid = false;
    return false;
  }
  
  // Check if enough time has passed since last API call
  if (!canFetchNewData()) {
    Serial.println("Weather data is still fresh, using cached data");
    Serial.print("Time since last update: ");
    Serial.print((millis() - weatherData.last_update) / 1000);
    Serial.println(" seconds");
    Serial.print("Next update allowed in: ");
    Serial.print(WEATHER_API_INTERVAL - ((millis() - weatherData.last_update) / 1000));
    Serial.println(" seconds");
    return weatherData.is_valid;  // Return current validity status
  }
  
  Serial.println("Fetching fresh weather data for: " + location);
  
  // Construct API URL
  String url = buildAPIUrl(location);
  
  String response;
  if (!makeAPIRequest(url, response)) {
    return false;
  }
  
  return parseWeatherResponse(response);
}

bool WeatherManager::makeAPIRequest(const String& url, String& response) {
  HTTPClient http;
  http.begin(wifiClient, url);
  http.setTimeout(WEATHER_API_TIMEOUT);
  
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      response = http.getString();
      http.end();
      return true;
    } else {
      weatherData.error_message = "HTTP Error: " + String(httpCode);
      Serial.println("HTTP Error: " + String(httpCode));
    }
  } else {
    weatherData.error_message = "Connection failed: " + http.errorToString(httpCode);
    Serial.println("Connection failed: " + http.errorToString(httpCode));
  }
  
  http.end();
  weatherData.is_valid = false;
  return false;
}

bool WeatherManager::parseWeatherResponse(const String& response) {
  Serial.println("Parsing weather response...");
  
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, response);
  
  if (error) {
    weatherData.error_message = "JSON parsing failed: " + String(error.c_str());
    weatherData.is_valid = false;
    Serial.println("JSON parsing failed: " + String(error.c_str()));
    return false;
  }
  
  // Check for API error
  if (doc.containsKey("error")) {
    weatherData.error_message = doc["error"]["message"].as<String>();
    weatherData.is_valid = false;
    Serial.println("API Error: " + weatherData.error_message);
    return false;
  }
  
  // Parse location data
  JsonObject location = doc["location"];
  weatherData.location_name = location["name"].as<String>();
  weatherData.location_region = location["region"].as<String>();
  weatherData.location_country = location["country"].as<String>();
  weatherData.lat = location["lat"];
  weatherData.lon = location["lon"];
  weatherData.local_time = location["localtime"].as<String>();
  
  // Parse current weather data
  JsonObject current = doc["current"];
  weatherData.temp_c = current["temp_c"];
  weatherData.temp_f = current["temp_f"];
  weatherData.condition_text = current["condition"]["text"].as<String>();
  weatherData.condition_icon = current["condition"]["icon"].as<String>();
  weatherData.pressure_mb = current["pressure_mb"];
  weatherData.humidity = current["humidity"];
  weatherData.cloud = current["cloud"];
  weatherData.feelslike_c = current["feelslike_c"];
  weatherData.feelslike_f = current["feelslike_f"];
  weatherData.uv = current["uv"];
  
  // Parse air quality data
  JsonObject airQuality = current["air_quality"];
  weatherData.co = airQuality["co"];
  weatherData.no2 = airQuality["no2"];
  weatherData.o3 = airQuality["o3"];
  weatherData.so2 = airQuality["so2"];
  weatherData.pm2_5 = airQuality["pm2_5"];
  weatherData.pm10 = airQuality["pm10"];
  weatherData.us_epa_index = airQuality["us-epa-index"];
  weatherData.gb_defra_index = airQuality["gb-defra-index"];
  
  weatherData.is_valid = true;
  weatherData.last_update = millis();
  weatherData.error_message = "";
  
  Serial.println("Weather data parsed successfully");
  Serial.println("Location: " + weatherData.location_name + ", " + weatherData.location_region);
  Serial.println("Temperature: " + String(weatherData.temp_c) + "°C");
  Serial.println("Condition: " + weatherData.condition_text);
  
  return true;
}

WeatherData WeatherManager::getWeatherData() {
  return weatherData;
}

bool WeatherManager::isDataValid() {
  return weatherData.is_valid;
}

unsigned long WeatherManager::getLastUpdateTime() {
  return weatherData.last_update;
}

bool WeatherManager::canFetchNewData() {
  // If we don't have any data yet, always allow fetching
  if (weatherData.last_update == 0) {
    return true;
  }
  
  // Check if enough time has passed since last update
  unsigned long currentTime = millis();
  unsigned long timeSinceLastUpdate = currentTime - weatherData.last_update;
  
  // Convert WEATHER_API_INTERVAL from seconds to milliseconds
  unsigned long intervalMs = WEATHER_API_INTERVAL * 1000UL;
  
  return timeSinceLastUpdate >= intervalMs;
}

String WeatherManager::buildAPIUrl(const String& location) {
  String url = "http://api.weatherapi.com/v1/current.json?key=";
  url += WEATHER_API_KEY;
  url += "&q=";
  url += location;
  url += "&aqi=yes";
  return url;
}

void WeatherManager::clearWeatherData() {
  weatherData.location_name = "";
  weatherData.location_region = "";
  weatherData.location_country = "";
  weatherData.lat = 0.0;
  weatherData.lon = 0.0;
  weatherData.local_time = "";
  
  weatherData.temp_c = 0.0;
  weatherData.temp_f = 0.0;
  weatherData.condition_text = "";
  weatherData.condition_icon = "";
  weatherData.pressure_mb = 0.0;
  weatherData.humidity = 0;
  weatherData.cloud = 0;
  weatherData.feelslike_c = 0.0;
  weatherData.feelslike_f = 0.0;
  weatherData.uv = 0.0;
  
  weatherData.co = 0.0;
  weatherData.no2 = 0.0;
  weatherData.o3 = 0.0;
  weatherData.so2 = 0.0;
  weatherData.pm2_5 = 0.0;
  weatherData.pm10 = 0.0;
  weatherData.us_epa_index = 0;
  weatherData.gb_defra_index = 0;
  
  weatherData.is_valid = false;
  weatherData.error_message = "";
  weatherData.last_update = 0;
}
