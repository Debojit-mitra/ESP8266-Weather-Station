# 🌤️ ESP8266 Weather Station

Weather monitoring system built with ESP8266 NodeMCU, featuring multiple sensors for temperature, humidity, atmospheric pressure, and air quality monitoring with real-time web interface.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP8266-green.svg)
![Arduino](https://img.shields.io/badge/Arduino-IDE-blue.svg)

## 📋 Features

- **Multi-sensor monitoring**: DHT11, BMP280, and MQ-135 sensors
- **Dual temperature sensing**: Average calculation from DHT11 and BMP280 for accuracy
- **Real-time JSON data**: Access sensor data via web browser
- **Visual status indicators**: LED-based system health monitoring
- **WiFi connectivity**: Static IP configuration with automatic reconnection
- **Data validation**: Built-in sensor reading validation and error handling

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
   ```

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
GET /json?api_key=your_api_key
```

**Response Example:**

```json
{
  "sensors": {
    "temperature": {
      "dht11": 24.5,
      "bmp280": 24.3,
      "average": 24.4,
      "unit": "°C"
    },
    "humidity": {
      "value": 65.2,
      "unit": "%"
    },
    "pressure": {
      "value": 1013.25,
      "unit": "hPa"
    },
    "air_quality": {
      "value": 145,
      "unit": "ppm"
    }
  },
  "system": {
    "status": "ALL_OK",
    "uptime": 120000,
    "free_heap": 45320,
    "wifi_strength": -45
  },
  "timestamp": 1634567890
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

**Web Interface Not Loading:**

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

- [ ] Properly configure AQI sensor calibration and thresholds
- [ ] Add data logging to SD card
- [ ] Add weather forecast integration
- [ ] Mobile app development (almost done)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Arduino community for extensive documentation
- Adafruit for excellent sensor libraries
- ESP8266 community for board support and examples
- Contributors to the open-source libraries used in this project

---

Made with 💖 by **Debojit**

⭐ **Star this repository if you found it helpful!**
