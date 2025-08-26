// =============================================================================
// config_example.h - Example configuration file
// =============================================================================
#ifndef CONFIG_H
#define CONFIG_H

#include <IPAddress.h>

// WiFi Configuration - Replace with your actual credentials
#define WIFI_SSID "Your_WiFi_Name"
#define WIFI_PASSWORD "Your_WiFi_Password"

// Static IP Configuration (optional) - Adjust to your network
#define STATIC_IP IPAddress(192, 168, 1, 223)
#define GATEWAY_IP IPAddress(192, 168, 1, 1)
#define SUBNET_MASK IPAddress(255, 255, 255, 0)
#define PRIMARY_DNS IPAddress(8, 8, 8, 8)
#define SECONDARY_DNS IPAddress(8, 8, 4, 4)

// API Configuration - Replace with your custom API key
#define API_KEY "your_custom_api_key"
#define WEB_SERVER_PORT 80

// Weather API Configuration - Get your free API key from weatherapi.com
#define WEATHER_API_KEY "your_weather_api_key_here"
#define WEATHER_API_LOCATION "YourCity"  // Default location for weather data
#define WEATHER_API_TIMEOUT 10000  // 10 seconds timeout
#define WEATHER_API_INTERVAL 300  // 300 seconds (5 minutes) between API calls to prevent rate limit exhaustion

// Sensor Configuration
#define DHT_PIN D5
#define DHT_TYPE DHT22
#define BMP_ADDRESS 0x76
#define AIR_QUALITY_PIN A0

// LED Status Configuration
#define GREEN_LED_PIN D6  // GPIO12
#define RED_LED_PIN D7    // GPIO13

// Timing Configuration
#define SENSOR_READ_INTERVAL 5000  // 5 seconds
#define JSON_CACHE_TIME 5         // 5 seconds
#define LED_UPDATE_INTERVAL 100   // LED update interval in ms
#define LED_SLOW_BLINK_RATE 1000  // Slow blink rate in ms
#define LED_FAST_BLINK_RATE 250   // Fast blink rate in ms
#define LED_DOUBLE_BLINK_PAUSE 1500 // Pause between double blinks in ms

// Validation Ranges
#define TEMP_MIN -100.0
#define TEMP_MAX 100.0
#define PRESSURE_MIN 800.0
#define PRESSURE_MAX 1200.0

// MQ-135 Calibration Configuration
// IMPORTANT: These values MUST be calibrated for your specific sensor!
// See README.md for detailed calibration instructions
#define MQ135_CLEAN_AIR_VALUE 200  // Your baseline reading in clean air (CUSTOMIZE THIS!)
#define MQ135_RL_VALUE 1.0        // Load resistance (1kΩ in my case)
#define MQ135_RO_CLEAN_AIR_FACTOR 3.6  // From datasheet for 100ppm CO2
#define MQ135_CALIBRATION_SAMPLE_TIMES 50    // Number of samples for calibration
#define MQ135_CALIBRATION_SAMPLE_INTERVAL 500 // Time between samples (ms)

#endif
