// =============================================================================
// sensor_manager.cpp - Sensor management implementation
// =============================================================================
#include "sensor_manager.h"

SensorManager::SensorManager() : dht(DHT_PIN, DHT_TYPE), lastReadTime(0) {
  // Initialize data structure
  currentData.temp_dht = 0;
  currentData.temp_bmp = 0;
  currentData.temp_avg = 0;
  currentData.humidity = 0;
  currentData.pressure = 0;
  currentData.air_quality = 0;
  currentData.last_update = 0;
  currentData.is_valid = false;
  currentData.dht_working = false;
  currentData.bmp_working = false;
  currentData.mq135_working = false;
}

void SensorManager::begin() {
  Serial.println("Initializing sensors...");
  
  statusManager.begin();
  initializeDHT();
  initializeBMP();
  
  // Initial sensor reading
  update();
  
  Serial.println("Sensors initialized successfully!");
}

void SensorManager::initializeDHT() {
  dht.begin();
  Serial.println("DHT22 sensor initialized");
}

void SensorManager::initializeBMP() {
  if (!bmp.begin(BMP_ADDRESS)) {
    Serial.println("ERROR: BMP280 initialization failed!");
    currentData.bmp_working = false;
  } else {
    Serial.println("BMP280 sensor initialized");
    currentData.bmp_working = true;
  }
}

void SensorManager::update() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastReadTime >= SENSOR_READ_INTERVAL) {
    Serial.println("Reading sensors...");
    
    readDHTSensor();
    readBMPSensor();
    readAirQuality();
    calculateAverageTemperature();
    
    currentData.last_update = currentMillis;
    currentData.is_valid = true;
    lastReadTime = currentMillis;
    
    printReadings();
  }
  
  // Update system status with current sensor health
  statusManager.update(getSensorHealth());
}

void SensorManager::readDHTSensor() {
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();
  
  if (!isnan(newTemp) && validateTemperature(newTemp)) {
    currentData.temp_dht = roundToDecimal(newTemp);
    currentData.dht_working = true;
  } else {
    currentData.dht_working = false;
  }
  
  if (!isnan(newHumidity)) {
    currentData.humidity = roundToDecimal(newHumidity);
  }
}

void SensorManager::readBMPSensor() {
  if (!currentData.bmp_working) return; // Skip if BMP is not working
  
  float newTemp = bmp.readTemperature();
  float newPressure = bmp.readPressure() / 100.0F; // Convert Pa to hPa
  
  if (validateTemperature(newTemp)) {
    currentData.temp_bmp = roundToDecimal(newTemp);
  }
  
  if (validatePressure(newPressure)) {
    currentData.pressure = roundToDecimal(newPressure);
  }
}

void SensorManager::readAirQuality() {
  int reading = analogRead(AIR_QUALITY_PIN);
  if (reading > 0) {
    currentData.air_quality = reading;
    currentData.mq135_working = true;
  } else {
    currentData.mq135_working = false;
  }
}

void SensorManager::calculateAverageTemperature() {
  // Only calculate average if both sensors have valid readings
  if (currentData.temp_dht != 0 && currentData.temp_bmp != 0) {
    float average = (currentData.temp_dht + currentData.temp_bmp) / 2.0;
    currentData.temp_avg = roundToDecimal(average);
  } else if (currentData.temp_dht != 0) {
    // If only DHT has valid reading, use that
    currentData.temp_avg = currentData.temp_dht;
  } else if (currentData.temp_bmp != 0) {
    // If only BMP has valid reading, use that
    currentData.temp_avg = currentData.temp_bmp;
  } else {
    // No valid readings
    currentData.temp_avg = 0;
  }
}

bool SensorManager::validateTemperature(float temp) {
  return (temp >= TEMP_MIN && temp <= TEMP_MAX);
}

bool SensorManager::validatePressure(float pressure) {
  return (pressure >= PRESSURE_MIN && pressure <= PRESSURE_MAX);
}

float SensorManager::roundToDecimal(float value, int decimals) {
  float multiplier = pow(10, decimals);
  return round(value * multiplier) / multiplier;
}

SensorData SensorManager::getData() {
  return currentData;
}

SystemStatus SensorManager::getSystemStatus() {
  return statusManager.getStatus();
}

SensorHealth SensorManager::getSensorHealth() {
  SensorHealth health;
  health.dht_working = currentData.dht_working;
  health.bmp_working = currentData.bmp_working;
  health.mq135_working = currentData.mq135_working;
  return health;
}

void SensorManager::printReadings() {
  Serial.println("=== Sensor Readings ===");
  Serial.println("DHT Temperature: " + String(currentData.temp_dht) + "°C");
  Serial.println("BMP Temperature: " + String(currentData.temp_bmp) + "°C");
  Serial.println("Average Temperature: " + String(currentData.temp_avg) + "°C");
  Serial.println("Humidity: " + String(currentData.humidity) + "%");
  Serial.println("Pressure: " + String(currentData.pressure) + " hPa");
  Serial.println("Air Quality: " + String(currentData.air_quality));
  
  Serial.println("=== Sensor Health ===");
  Serial.println("DHT Working: " + String(currentData.dht_working ? "YES" : "NO"));
  Serial.println("BMP Working: " + String(currentData.bmp_working ? "YES" : "NO"));
  Serial.println("MQ135 Working: " + String(currentData.mq135_working ? "YES" : "NO"));
  Serial.println("=======================");
  
  // Print system status
  statusManager.printStatus();
}
