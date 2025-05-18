# expLORA Sensors

![expLORA Logo](https://pajenicko.cz/image/catalog/explora/explora-logo-sm.png)

## What is expLORA?

expLORA is a complete IoT ecosystem for measuring and transmitting environmental data using LoRa technology on the 868 MHz frequency. The system consists of two main components:

1. **expLORA Gateway** - a central device that receives data from sensors and provides visualization, storage, and further processing
2. **expLORA Sensors** - various types of measuring devices that capture data and send it via LoRa to the gateway

The system is designed for:
- **Long-range communication** (up to several kilometers in open terrain)
- **Low power consumption** (battery-powered sensors with a lifetime of months to years)
- **Simple installation and use** (no complex configuration required)
- **Openness and extensibility** (ability to create your own DIY sensors)

expLORA uses the license-free 868 MHz frequency band, which is permitted for IoT devices in Europe, and the LoRa protocol, which enables energy-efficient long-distance communication.

## Where to buy expLORA devices

### expLORA Gateway

The expLORA Gateway is available in two versions:

- **expLORA Gateway Lite** - basic version for smaller installations
  - Available at: [https://pajenicko.cz/bezdratova-brana-explora-gateway-lite](https://pajenicko.cz/bezdratova-brana-explora-gateway-lite)

- **expLORA CLIMA** - temperature, humidity and air pressure sensor
  - Available at: [https://pajenicko.cz/bezdratovy-senzor-explora-clima](https://pajenicko.cz/bezdratovy-senzor-explora-clima)

- **expLORA CLIMA OUTDOOR** - temperature, humidity and air pressure sensor in radiation shield
  - Available at: [https://pajenicko.cz/bezdratovy-senzor-explora-clima-outdoor](https://pajenicko.cz/bezdratovy-senzor-explora-clima-outdoor)

- **expLORA CARBON** - temperature, humidity and CO2 sensor
  - Available at: [https://pajenicko.cz/bezdratovy-senzor-explora-carbon](https://pajenicko.cz/bezdratovy-senzor-explora-carbon)

- **expLORA METEO** - temperature, humidity, atmospheric pressure, wind speed and direction, precipitation amount and intensity
  - Available at: [https://pajenicko.cz/meteorologicka-stanice-explora-meteo](https://pajenicko.cz/meteorologicka-stanice-explora-meteo)

## 3D Models for Enclosures

3D models of enclosures for expLORA sensors are available for 3D printing:

[expLORA 3D models on Printables](https://www.printables.com/search/models?q=tag%3Aexplora)

## DIY expLORA Sensors

The expLORA system allows you to create your own sensors. Below are two examples of implementing custom sensors using ESP32 and a LoRa module.

### Example 1: ESP32 expLORA CLIMA Sensor (explora-fake-clima-sensor)

This example shows how to implement a custom sensor that is compatible with the CLIMA protocol and doesn't require any changes to the gateway.

#### Key Features:
- Compatible with the expLORA CLIMA protocol (sensor type 0x01)
- Measures temperature using a DS18B20 sensor
- Sends data in CLIMA format (temperature, pressure, humidity - with fake values for pressure and humidity)
- Works with any expLORA Gateway without modifications

#### Hardware Requirements:
- ESP32 development board
- RFM95W LoRa module
- DS18B20 temperature sensor
- Power source (battery or USB)
- Easiest to start with [Gateway Lite](https://pajenicko.cz/bezdratova-brana-explora-gateway-lite) hardware

#### Source Code:
Complete [source code for ESP32 expLORA CLIMA Sensor](/explora-fake-clima-sensor)

### Example 2: ESP32 expLORA DIY DS18B20 Sensor (explora-diy-sensor)

This example shows how to implement a custom sensor with its own sensor type, which requires a gateway modification.

#### Key Features:
- Custom sensor type (0x51 - DIY DS18B20)
- Measures only temperature using a DS18B20 sensor
- Smaller packet size (11 bytes compared to 15 bytes for CLIMA)
- Requires gateway update for decoding

#### Hardware Requirements:
- ESP32 development board
- RFM95W LoRa module
- DS18B20 temperature sensor
- Power source (battery or USB)
- Easiest to start with [Gateway Lite](https://pajenicko.cz/bezdratova-brana-explora-gateway-lite) hardware

#### Source Code:
Complete [source code for ESP32 expLORA DIY DS18B20 Sensor](/explora-diy-sensor)

#### Required Gateway Changes:
For this sensor to work properly, the gateway needs to be updated with support for the custom sensor type. The changes are available in this commit:
[https://github.com/Pajenicko/expLORA-Gateway-Lite/commit/ab84ba3c6fc0f48b05f633b6888df16e3517fa95](https://github.com/Pajenicko/expLORA-Gateway-Lite/commit/ab84ba3c6fc0f48b05f633b6888df16e3517fa95)

## Implementation Details

### Power Consumption

The sensors are optimized for low power operation:
- Deep sleep between measurements (15 minutes)
- LoRa module in sleep mode when not transmitting
- Minimal active time for sensor readings and transmissions

### Development Tips

1. For maximum battery life, consider using ESP32's deep sleep mode
2. Use a quality antenna for the LoRa module for maximum range
3. When developing custom sensors, stick to the existing [expLORA protocol](/explora-protocol.md) or prepare appropriate modifications for the gateway
4. For outdoor use, ensure a waterproof enclosure

