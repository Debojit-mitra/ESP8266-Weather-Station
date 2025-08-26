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
  currentData.air_quality_raw = 0;
  currentData.air_quality_ppm = 0;
  currentData.air_quality_aqi = 0;
  currentData.air_quality_status = "Unknown";
  currentData.last_update = 0;
  currentData.is_valid = false;
  currentData.dht_working = false;
  currentData.bmp_working = false;
  currentData.mq135_working = false;
  
  // Initialize MQ-135 calibration
  mq135_ro = 0;
}

void SensorManager::begin() {
  Serial.println("Initializing sensors...");
  
  statusManager.begin();
  initializeDHT();
  initializeBMP();
  
  // Calibrate MQ-135 using your baseline
  Serial.println("Calibrating MQ-135 with baseline...");
  mq135_ro = getMQ135Resistance(MQ135_CLEAN_AIR_VALUE) / MQ135_RO_CLEAN_AIR_FACTOR;
  Serial.println("MQ-135 R0 (clean air resistance): " + String(mq135_ro));
  
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
    currentData.air_quality_raw = reading;
    
    // Calculate resistance and ppm
    float resistance = getMQ135Resistance(reading);
    currentData.air_quality_ppm = getMQ135PPM(resistance);
    currentData.air_quality_aqi = calculateAQI(currentData.air_quality_ppm);
    currentData.air_quality_status = getAirQualityStatus(currentData.air_quality_aqi);
    
    currentData.mq135_working = true;
    
    // Enhanced debug output
    float ratio = (mq135_ro > 0) ? resistance / mq135_ro : 0;
    Serial.println("MQ-135 Debug:");
    Serial.println("  Raw: " + String(reading));
    Serial.println("  Voltage: " + String((reading / 1024.0) * 3.3, 2) + "V");
    Serial.println("  Resistance: " + String(resistance, 1) + " kΩ");
    Serial.println("  R0 (baseline): " + String(mq135_ro, 1) + " kΩ");
    Serial.println("  Ratio (Rs/R0): " + String(ratio, 2));
    Serial.println("  PPM: " + String(currentData.air_quality_ppm, 1));
    Serial.println("  AQI: " + String(currentData.air_quality_aqi));
    Serial.println("  Quality: " + currentData.air_quality_status);
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
  Serial.println("Air Quality Raw: " + String(currentData.air_quality_raw));
  Serial.println("Air Quality PPM: " + String(currentData.air_quality_ppm));
  Serial.println("Air Quality AQI: " + String(currentData.air_quality_aqi));
  Serial.println("Air Quality Status: " + currentData.air_quality_status);
  
  Serial.println("=== Sensor Health ===");
  Serial.println("DHT Working: " + String(currentData.dht_working ? "YES" : "NO"));
  Serial.println("BMP Working: " + String(currentData.bmp_working ? "YES" : "NO"));
  Serial.println("MQ135 Working: " + String(currentData.mq135_working ? "YES" : "NO"));
  Serial.println("=======================");
  
  // Print system status
  statusManager.printStatus();
}

// =============================================================================
// MQ-135 Calibration and Calculation Functions
// =============================================================================

float SensorManager::getMQ135Resistance(int analogValue) {
  // Convert analog reading to resistance
  // Formula: Rs = ((Vc * RL) / Vout) - RL
  // Where: Vc = 3.3V, RL = load resistance, Vout = analog voltage
  float voltage = (analogValue / 1024.0) * 3.3; // Convert to voltage
  if (voltage == 0) voltage = 0.001; // Prevent division by zero
  
  float resistance = ((3.3 * MQ135_RL_VALUE) / voltage) - MQ135_RL_VALUE;
  return resistance;
}

float SensorManager::getMQ135PPM(float resistance) {
  // Calculate gas concentration in ppm
  // Using simplified formula for CO2 estimation
  if (mq135_ro <= 0) return 0;
  
  float ratio = resistance / mq135_ro;
  
  // More realistic CO2 calculation with bounds checking
  // Formula based on MQ-135 datasheet characteristics
  float ppm;
  
  if (ratio > 3.0) {
    // Very clean air - lower than baseline
    ppm = 300 + (ratio - 3.0) * 50; // 300-400 ppm range
  } else if (ratio > 1.0) {
    // Normal to moderate air quality
    ppm = 400 + (3.0 - ratio) * 300; // 400-1000 ppm range
  } else {
    // Poor air quality
    ppm = 1000 + (1.0 - ratio) * 4000; // 1000+ ppm range
  }
  
  // Apply realistic bounds
  if (ppm < 300) ppm = 300;     // Minimum outdoor air
  if (ppm > 8000) ppm = 8000;   // Maximum reasonable indoor
  
  // Add some smoothing to reduce noise
  static float lastPPM = ppm;
  ppm = (lastPPM * 0.8) + (ppm * 0.2); // Simple low-pass filter
  lastPPM = ppm;
  
  return ppm;
}

int SensorManager::calculateAQI(float ppm) {
  // This is a simplified conversion - real AQI considers multiple pollutants
  
  // AQI breakpoints for CO2 (simplified scale)
  // Good: 0-50 AQI (300-400 ppm)
  // Moderate: 51-100 AQI (400-1000 ppm)  
  // Unhealthy for Sensitive: 101-150 AQI (1000-2000 ppm)
  // Unhealthy: 151-200 AQI (2000-5000 ppm)
  // Very Unhealthy: 201-300 AQI (5000+ ppm)
  
  int aqi;
  
  if (ppm <= 300) {
    // Below normal outdoor air - treat as excellent
    aqi = 25;
  }
  else if (ppm <= 400) {
    // Good air quality: 25-50 AQI for 300-400 ppm
    aqi = 25 + (ppm - 300) * (50 - 25) / (400 - 300);
  }
  else if (ppm <= 1000) {
    // Moderate air quality: 51-100 AQI for 400-1000 ppm
    aqi = 51 + (ppm - 400) * (100 - 51) / (1000 - 400);
  }
  else if (ppm <= 2000) {
    // Unhealthy for sensitive: 101-150 AQI for 1000-2000 ppm
    aqi = 101 + (ppm - 1000) * (150 - 101) / (2000 - 1000);
  }
  else if (ppm <= 5000) {
    // Unhealthy: 151-200 AQI for 2000-5000 ppm
    aqi = 151 + (ppm - 2000) * (200 - 151) / (5000 - 2000);
  }
  else {
    // Very unhealthy: 201+ AQI for 5000+ ppm
    aqi = 201 + (ppm - 5000) * (300 - 201) / (10000 - 5000);
    if (aqi > 500) aqi = 500; // Cap at hazardous level
  }
  
  return aqi;
}

String SensorManager::getAirQualityStatus(int aqi) {
  // Return descriptive air quality status based on AQI
  if (aqi <= 50) return "Good";
  else if (aqi <= 100) return "Moderate";
  else if (aqi <= 150) return "Unhealthy for Sensitive Groups";
  else if (aqi <= 200) return "Unhealthy";
  else if (aqi <= 300) return "Very Unhealthy";
  else return "Hazardous";
}
