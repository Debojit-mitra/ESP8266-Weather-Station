# 🌤️ ESP8266 Weather Station

Weather monitoring system built with ESP8266 NodeMCU, featuring multiple sensors for temperature, humidity, atmospheric pressure, and air quality monitoring with real-time web interface.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP8266-green.svg)
![Arduino](https://img.shields.io/badge/Arduino-IDE-blue.svg)

## 📋 Features

- **Multi-sensor monitoring**: DHT11, BMP280, and MQ-135 sensors
- **Real-time JSON data**: Access sensor data via web browser
- **Visual status indicators**: LED-based system health monitoring
- **WiFi connectivity**: Static IP configuration with automatic reconnection
- **Data validation**: Sensor reading validation and error handling
- **External weather data**: Integration with WeatherAPI.com

## 🛠️ Hardware Components

### Main Components

- **ESP8266 NodeMCU** - Main microcontroller
- **DHT11** - Temperature and humidity sensor
- **BMP280** - Barometric pressure and temperature sensor (I²C)
- **MQ-135** - Air quality sensor
- **2x LEDs** (Green & Red) - Status indicators
- **2x 220Ω Resistors** - For LED current limiting
- **1x 10kΩ Resistor** - DHT11 pull-up resistor
- **Breadboard and jumper wires**

## 🔌 Circuit Connections

### DHT11 Sensor

```
DHT11          NodeMCU
VCC     →      3.3V (3V3 pin)
GND     →      GND
DATA    →      D5 (GPIO14)

Note: Connect 10kΩ resistor between VCC and DATA pins
```

### BMP280 Sensor (I²C, Address: 0x76)

```
BMP280         NodeMCU
VIN     →      3.3V (3V3 pin)
GND     →      GND
SDA     →      D2 (GPIO4)
SCL     →      D1 (GPIO5)
```

### MQ-135 Air Quality Sensor

```
MQ-135         NodeMCU
VCC     →      Vin (5V)
GND     →      GND
A0      →      A0 (ADC0 pin)

Note: D0 pin is not used in this project
```

### Status LEDs

```
Green LED:
Anode (+)  →   D6 (GPIO12) through 220Ω resistor
Cathode(-) →   GND

Red LED:
Anode (+)  →   D7 (GPIO13) through 220Ω resistor
Cathode(-) →   GND
```

## 📊 LED Status Indicators

The system uses two LEDs to indicate operational status:

| LED Pattern         | Status            | Description                           |
| ------------------- | ----------------- | ------------------------------------- |
| 🟢 **Solid Green**  | All OK            | All sensors working, WiFi connected   |
| 🟢 **Slow Blink**   | Minor Issues      | One sensor failure, system functional |
| 🟢 **Double Blink** | WiFi Disconnected | Sensors OK, WiFi connection lost      |
| 🔴 **Fast Blink**   | Multiple Failures | Multiple sensor failures              |
| 🔴 **Solid Red**    | Critical Failure  | System-wide failure                   |

## 🚀 Installation & Setup

### Prerequisites

- Arduino IDE with ESP8266 board package
- Required libraries (install via Library Manager):
  ```
  ESP8266WiFi
  ESP8266WebServer
  Adafruit_BMP280
  DHT sensor library
  ArduinoJson
  ```

### Configuration

> [!WARNING]
> **Arduino IDE File Structure Requirement**
>
> All `.ino` and `.h` files must be placed inside a folder named `weather_station` for Arduino IDE to compile properly. The main file `weather_station.ino` must have the same name as the containing folder. Failure to follow this structure will result in compilation errors.

> [!IMPORTANT]
> **Configuration Setup**
>
> Before compiling, you must create your own `config.h` file from the example:
>
> 1. Copy `config_example.h` to `config.h`
> 2. Update your WiFi credentials, API key, and network settings
> 3. The `config.h` file is ignored by git to protect your sensitive information

1. Clone this repository
2. Copy `config_example.h` to `config.h`:
   ```bash
   cp config_example.h config.h
   ```
3. Open `config.h` and update your settings:

   ```cpp
   // WiFi Configuration
   #define WIFI_SSID "Your_WiFi_Name"
   #define WIFI_PASSWORD "Your_WiFi_Password"

   // Static IP Configuration (optional)
   #define STATIC_IP IPAddress(192, 168, 1, 223)
   #define GATEWAY_IP IPAddress(192, 168, 1, 1)

   // API Security
   #define API_KEY "your_custom_api_key"

   // Weather API Configuration (optional)
   #define WEATHER_API_KEY "your_weather_api_key"
   #define WEATHER_API_LOCATION "YourCity"
   #define WEATHER_API_INTERVAL 300  // Seconds between API calls (300 = 5 minutes)

   // MQ135 Calibration (REQUIRED for accurate readings)
   #define MQ135_CLEAN_AIR_VALUE 200  // Update with your sensor's baseline
   ```

4. **Weather API Setup (Optional)**: To enable weather data features, sign up for a free API key at [WeatherAPI.com](https://www.weatherapi.com/) and update your `config.h`:
   - Replace `"your_weather_api_key"` with your actual API key
   - Replace `"YourCity"` with your desired location (e.g., "Guwahati", "London", "New York")
   - Adjust `WEATHER_API_INTERVAL` to control how often fresh data is fetched (300 seconds = 5 minutes)
   - If you don't configure this, the weather endpoints will return error messages

> [!CAUTION]
> **MQ135 Calibration Required**
>
> The MQ135 air quality sensor requires individual calibration as each sensor has different baseline values. You **must** update the `MQ135_CLEAN_AIR_VALUE` in your `config.h` file with your sensor's specific baseline reading. See the [MQ135 Calibration section](#-mq135-air-quality-sensor-calibration) for detailed instructions.

### Upload Process

1. Connect your NodeMCU to your computer
2. Select the correct board: `Tools > Board > ESP8266 Boards > NodeMCU 1.0`
3. Select the correct port: `Tools > Port > [Your COM Port]`
4. Upload the code: `Sketch > Upload`

## 🌐 Web Interface

### Accessing the Weather Station

After successful connection, access your weather station at:

```
http://192.168.1.223/  (or your configured IP)
```

### API Endpoints

#### Get Sensor Data (JSON)

```
GET /jsondata?key=your_api_key
```

#### Get Sensor Data with Weather (JSON)

```
GET /jsondata?key=your_api_key&weather=true
```

**Sensor Data with Response:**

> [!NOTE]
> The weather field is included only when weather=true is requested.

```json
{
  "sensors": {
    "temperature_dht": 30.3,
    "temperature_bmp": 29.6,
    "temperature_avg": 30,
    "humidity": 57.4,
    "pressure": 991.5,
    "air_quality_raw": 270,
    "air_quality_ppm": 554.0875,
    "air_quality_aqi": 63,
    "air_quality_status": "Moderate",
    "is_valid": true
  },
  "system": {
    "free_heap": 43368,
    "heap_fragmentation": 1,
    "max_free_block": 43264,
    "cpu_freq": 80,
    "uptime": 16,
    "uptime_formatted": "16s",
    "free_sketch_space": 1740800,
    "flash_chip_size": 4194304,
    "flash_chip_speed": 40
  },
  "wifi": {
    "ssid": "Mitra`s",
    "rssi": -30,
    "ip": "192.168.1.223",
    "mac": "8C:AA:B5:FB:FF:82"
  },
  "weather": {
    "location": "Guwahati, Assam",
    "temp_c": 27.2,
    "temp_f": 81,
    "condition": "Mist",
    "condition_icon": "//cdn.weatherapi.com/weather/64x64/night/143.png",
    "pressure_mb": 1004,
    "humidity": 89,
    "feelslike_c": 31.6,
    "uv": 0,
    "last_update": 16364,
    "is_valid": true,
    "airquality": {
      "co": 468.05,
      "no2": 3.515,
      "o3": 106,
      "so2": 2.96,
      "pm2_5": 20.35,
      "pm10": 20.905,
      "us_epa_index": 2,
      "gb_defra_index": 2
    },
    "seconds_since_update": 0,
    "can_fetch_new": false,
    "force_refreshed": false
  }
}
```

## 🔧 Customization

### Sensor Reading Intervals

Modify timing in `config.h`:

```cpp
#define SENSOR_READ_INTERVAL 5000  // 5 seconds
#define LED_UPDATE_INTERVAL 100    // LED update rate
```

### Validation Ranges

Adjust sensor validation ranges:

```cpp
#define TEMP_MIN -100.0
#define TEMP_MAX 100.0
#define PRESSURE_MIN 800.0
#define PRESSURE_MAX 1200.0
```

## 🌫 MQ135 Air Quality Sensor Calibration

The MQ135 sensor requires proper calibration for accurate air quality measurements. Each MQ135 sensor has unique characteristics and baseline values, making individual calibration essential.

### Calibration Notes

> [!WARNING]
> **Sensor Warm-up Required**
>
> MQ135 sensors require 24-48 hours of initial warm-up for stable readings. For best results, allow the sensor to run continuously for at least 24 hours before calibrating.

> [!TIP]
> **Calibration Tips**
>
> - Perform calibration in stable weather conditions
> - Avoid calibrating during high humidity or temperature changes
> - Take multiple readings and average them for better accuracy
> - Re-calibrate if you move the sensor to a different environment

### How Calibration Works

The calibration process determines the sensor's resistance in clean air (R0), which serves as a baseline for calculating gas concentrations:

1. **Clean Air Reading**: The sensor reads analog values in known clean air conditions
2. **Resistance Calculation**: Convert analog reading to resistance using the load resistor value
3. **R0 Determination**: Calculate baseline resistance using the clean air factor from the datasheet
4. **PPM Calculation**: Use the Rs/R0 ratio to determine gas concentration in parts per million

### Calibration Parameters in `config.h`

You **must** modify these values in your `config.h` file based on your specific sensor:

```cpp
// MQ-135 Calibration Configuration
#define MQ135_CLEAN_AIR_VALUE 200  // Your baseline reading in clean air
#define MQ135_RL_VALUE 1.0        // Load resistance (1kΩ in my case))
#define MQ135_RO_CLEAN_AIR_FACTOR 3.6  // From datasheet for 100ppm CO2
#define MQ135_CALIBRATION_SAMPLE_TIMES 50    // Number of samples for calibration
#define MQ135_CALIBRATION_SAMPLE_INTERVAL 500 // Time between samples (ms)
```

### Setting Up Your MQ135 Calibration

#### Step 1: Find Your Clean Air Baseline

1. Place the sensor in clean outdoor air (away from pollution sources)
2. Power on the weather station and monitor the serial output
3. Look for the "MQ-135 Debug" section in serial monitor
4. Record the "Raw" analog reading after the sensor has warmed up (atleast 30 minutes)
5. This raw value becomes your `MQ135_CLEAN_AIR_VALUE`

#### Step 2: Verify Load Resistance

- Most MQ135 breakout boards use a 1kΩ or 10kΩ load resistor
- Check your board's schematic or documentation
- Update `MQ135_RL_VALUE` if different (value in kΩ)

#### Step 3: Adjust Clean Air Factor (Optional)

- `MQ135_RO_CLEAN_AIR_FACTOR` is typically 3.6 from the datasheet
- This represents the Rs/R0 ratio at 100ppm CO2 in clean air
- Usually doesn't need modification unless using a different reference gas

#### Step 4: Test and Validate

```cpp
// Example calibration for different environments:

// Urban environment (slightly higher baseline):
#define MQ135_CLEAN_AIR_VALUE 250

// Rural/countryside (cleaner air):
#define MQ135_CLEAN_AIR_VALUE 180

// Indoor environment (after ventilation):
#define MQ135_CLEAN_AIR_VALUE 220
```

### Understanding the Output

The sensor provides multiple air quality metrics:

- **Raw Value**: Direct analog reading (0-1024)
- **Resistance**: Calculated sensor resistance in kΩ
- **PPM**: Gas concentration in parts per million (CO2 equivalent)
- **AQI**: Air Quality Index (simplified scale 0-500)
- **Status**: Descriptive quality level (Good, Moderate, Unhealthy, etc.)

### Accuracy Limitations
> [!NOTE]
>This implementation provides a **simplified air quality estimation**:

- Real AQI considers multiple pollutants (PM2.5, PM10, O3, NO2, SO2, CO)
- MQ135 primarily responds to CO2, ammonia, and alcohol vapors
- Values are estimates suitable for general indoor air quality monitoring
- For precise measurements, use calibrated professional equipment

## 🐛 Troubleshooting

### Common Issues

**WiFi Connection Problems:**

- Check SSID and password in `config.h`
- Verify network is 2.4GHz (ESP8266 doesn't support 5GHz)
- Check LED status patterns

**Sensor Reading Issues:**

- Verify wiring connections
- Check power supply (sensors need stable 3.3V/5V)
- Monitor serial output for error messages

**Web Json Data Not Loading:**

- Confirm IP address in serial monitor
- Check firewall settings
- Verify the device is on the same network

### Serial Monitor Debug

Enable detailed debugging by monitoring serial output at 9600 baud rate.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📋 TODO

- [x] Properly configure AQI sensor calibration and thresholds
- [x] Add weather forecast integration
- [ ] Mobile app development (almost done)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Arduino community for extensive documentation
- Adafruit for excellent sensor libraries
- ESP8266 community for board support and examples
- Contributors to the open-source libraries used in this project

---

Crafted with 💖 by **Debojit**

⭐ **Star this repository if you found it helpful!**
