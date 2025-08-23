// =============================================================================
// sensor_manager.h - Sensor management header
// =============================================================================
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include "config.h"
#include "system_status.h"

struct SensorData {
  float temp_dht; // dht22
  float temp_bmp; // bmp280
  float temp_avg; // average of both temperature sensors
  float humidity; // dht22
  float pressure; // bmp280
  int air_quality; // MQ-135
  unsigned long last_update;
  bool is_valid;
  bool dht_working;
  bool bmp_working;
  bool mq135_working;
};

class SensorManager {
public:
  SensorManager();  // Declare constructor
  void begin();
  void update();
  SensorData getData();
  SystemStatus getSystemStatus();
  void printReadings();
  
private:
  DHT dht;
  Adafruit_BMP280 bmp;
  SensorData currentData;
  unsigned long lastReadTime;
  SystemStatusManager statusManager;
  
  void initializeDHT();
  void initializeBMP();
  void readDHTSensor();
  void readBMPSensor();
  void readAirQuality();
  void calculateAverageTemperature();
  bool validateTemperature(float temp);
  bool validatePressure(float pressure);
  float roundToDecimal(float value, int decimals = 1);
  SensorHealth getSensorHealth();
};

#endif