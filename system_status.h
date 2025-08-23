// =============================================================================
// system_status.h - System status and LED management header
// =============================================================================
#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include <ESP8266WiFi.h>
#include "config.h"

// System Status Definitions
enum SystemStatus {
  STATUS_ALL_OK,           // Green Solid: All systems operational
  STATUS_MINOR_ISSUES,     // Green Slow Blink: Minor sensor issues
  STATUS_WIFI_DISCONNECTED,// Green Double Blink: WiFi disconnected, sensors OK
  STATUS_MULTIPLE_FAILURES,// Red Fast Blink: Multiple sensor failures
  STATUS_CRITICAL_FAILURE  // Red Solid: Critical system failure
};

struct SensorHealth {
  bool dht_working;
  bool bmp_working;
  bool mq135_working;
};

class SystemStatusManager {
public:
  SystemStatusManager();
  void begin();
  void update(SensorHealth sensorHealth);
  SystemStatus getStatus();
  void printStatus();
  
private:
  SystemStatus currentStatus;
  unsigned long lastLEDUpdate;
  unsigned long lastBlinkTime;
  bool ledState;
  int blinkCount;
  
  void initializeLEDs();
  void updateSystemStatus(SensorHealth sensorHealth);
  void updateLEDs();
  void handleLEDPattern();
  void setLEDState(bool greenState, bool redState);
  bool checkWiFiConnection();
  int countSensorFailures(SensorHealth sensorHealth);
  String getStatusText();
};

#endif
