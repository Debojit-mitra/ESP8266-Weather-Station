// =============================================================================
// wifi_manager.h - WiFi management header
// =============================================================================
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include "config.h"

class WiFiManager {
public:
  void begin();
  bool isConnected();
  String getIP();
  String getSSID();
  int getRSSI();
  String getMacAddress();
  void printStatus();

private:
  void configureStaticIP();
  void connectToWiFi();
};

#endif