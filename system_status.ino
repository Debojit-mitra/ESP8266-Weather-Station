// =============================================================================
// system_status.cpp - System status and LED management implementation
// =============================================================================
#include "system_status.h"

SystemStatusManager::SystemStatusManager() : currentStatus(STATUS_CRITICAL_FAILURE), 
                                             lastLEDUpdate(0), lastBlinkTime(0), 
                                             ledState(false), blinkCount(0) {
}

void SystemStatusManager::begin() {
  Serial.println("Initializing system status manager...");
  initializeLEDs();
  Serial.println("System status manager initialized!");
}

void SystemStatusManager::initializeLEDs() {
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  
  // Turn off both LEDs initially
  setLEDState(false, false);
  
  Serial.println("Status LEDs initialized");
}

void SystemStatusManager::update(SensorHealth sensorHealth) {
  updateSystemStatus(sensorHealth);
  updateLEDs();
}

void SystemStatusManager::updateSystemStatus(SensorHealth sensorHealth) {
  int sensorFailures = countSensorFailures(sensorHealth);
  bool wifiConnected = checkWiFiConnection();
  
  // Critical failure: BMP280 is essential (provides pressure + temperature backup)
  if (!sensorHealth.bmp_working) {
    currentStatus = STATUS_CRITICAL_FAILURE;
  } else if (sensorFailures >= 2) {
    currentStatus = STATUS_MULTIPLE_FAILURES;
  } else if (!wifiConnected && sensorFailures == 0) {
    currentStatus = STATUS_WIFI_DISCONNECTED;
  } else if (sensorFailures == 1) {
    currentStatus = STATUS_MINOR_ISSUES;
  } else {
    currentStatus = STATUS_ALL_OK;
  }
}

bool SystemStatusManager::checkWiFiConnection() {
  return WiFi.status() == WL_CONNECTED;
}

int SystemStatusManager::countSensorFailures(SensorHealth sensorHealth) {
  int failures = 0;
  if (!sensorHealth.dht_working) failures++;
  if (!sensorHealth.bmp_working) failures++;
  if (!sensorHealth.mq135_working) failures++;
  return failures;
}

void SystemStatusManager::setLEDState(bool greenState, bool redState) {
  digitalWrite(GREEN_LED_PIN, greenState ? HIGH : LOW);
  digitalWrite(RED_LED_PIN, redState ? HIGH : LOW);
}

void SystemStatusManager::updateLEDs() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastLEDUpdate >= LED_UPDATE_INTERVAL) {
    handleLEDPattern();
    lastLEDUpdate = currentMillis;
  }
}

void SystemStatusManager::handleLEDPattern() {
  unsigned long currentMillis = millis();
  
  switch (currentStatus) {
    case STATUS_ALL_OK:
      // Green Solid
      setLEDState(true, false);
      break;
      
    case STATUS_MINOR_ISSUES:
      // Green Slow Blink
      if (currentMillis - lastBlinkTime >= LED_SLOW_BLINK_RATE) {
        ledState = !ledState;
        setLEDState(ledState, false);
        lastBlinkTime = currentMillis;
      }
      break;
      
    case STATUS_WIFI_DISCONNECTED:
      // Green Double Blink
      if (currentMillis - lastBlinkTime >= LED_SLOW_BLINK_RATE) {
        if (blinkCount < 4) { // Two complete blinks (on-off-on-off)
          ledState = !ledState;
          setLEDState(ledState, false);
          blinkCount++;
          lastBlinkTime = currentMillis;
        } else {
          // Pause between double blinks
          setLEDState(false, false);
          if (currentMillis - lastBlinkTime >= LED_DOUBLE_BLINK_PAUSE) {
            blinkCount = 0;
            lastBlinkTime = currentMillis;
          }
        }
      }
      break;
      
    case STATUS_MULTIPLE_FAILURES:
      // Red Fast Blink
      if (currentMillis - lastBlinkTime >= LED_FAST_BLINK_RATE) {
        ledState = !ledState;
        setLEDState(false, ledState);
        lastBlinkTime = currentMillis;
      }
      break;
      
    case STATUS_CRITICAL_FAILURE:
      // Red Solid
      setLEDState(false, true);
      break;
  }
}

SystemStatus SystemStatusManager::getStatus() {
  return currentStatus;
}

String SystemStatusManager::getStatusText() {
  switch (currentStatus) {
    case STATUS_ALL_OK: return "ALL OK (Green Solid)";
    case STATUS_MINOR_ISSUES: return "MINOR ISSUES (Green Slow Blink)";
    case STATUS_WIFI_DISCONNECTED: return "WIFI DISCONNECTED (Green Double Blink)";
    case STATUS_MULTIPLE_FAILURES: return "MULTIPLE FAILURES (Red Fast Blink)";
    case STATUS_CRITICAL_FAILURE: return "CRITICAL FAILURE (Red Solid)";
    default: return "UNKNOWN STATUS";
  }
}

void SystemStatusManager::printStatus() {
  Serial.println("=== System Status ===");
  Serial.println("WiFi Connected: " + String(checkWiFiConnection() ? "YES" : "NO"));
  Serial.println("System Status: " + getStatusText());
  Serial.println("=====================");
}
