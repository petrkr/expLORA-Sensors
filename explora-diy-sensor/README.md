# ESP32 expLORA DIY DS18B20 Temperature Sensor

## Project Overview

This project implements a custom temperature sensor using an ESP32 microcontroller with LoRa communication capabilities. Unlike the CLIMA-compatible version, this implementation uses a custom DS18B20 sensor type that requires corresponding changes to the expLORA gateway code.

## Features

- **Real-time temperature monitoring** using a DS18B20 digital temperature sensor
- **Low power consumption** with deep sleep mode between measurements
- **LoRa wireless communication** for long-range data transmission
- **Custom sensor type implementation** (0x51 - DIY DS18B20)
- **15-minute measurement interval**
- **Data encryption** for secure transmission
- **Battery voltage monitoring**

## Hardware Requirements

- ESP32 development board
- RFM95W LoRa module
- DS18B20 temperature sensor
- Power source (battery or USB)
- Jumper wires for connections

## Pin Configuration

| Component | ESP32 Pin |
|-----------|-----------|
| **LoRa Module** |
| SCK       | GPIO18    |
| MISO      | GPIO35    |
| MOSI      | GPIO37    |
| CS (NSS)  | GPIO5     |
| RESET     | GPIO14    |
| DIO0      | GPIO36    |
| **DS18B20** |
| Data      | GPIO12    |

## Software Dependencies

- Arduino IDE
- SPI Library
- LoRa Library
- OneWire Library
- DallasTemperature Library

## How It Works

1. **Wake from deep sleep**: ESP32 boots up from deep sleep (or initial power-on)
2. **Sensor reading**: DS18B20 temperature sensor is initialized and read
3. **Packet construction**: Data is formatted according to a custom protocol:
   - The first byte contains a random value for improved encryption
   - The second byte identifies this as a DIY DS18B20 sensor (0x51)
   - The next three bytes contain the unique serial number
   - The next two bytes store battery voltage in mV
   - Byte 7 indicates there is 1 value to follow (temperature only)
   - Bytes 8-9 store temperature in hundredths of degrees Celsius
   - The final byte contains a checksum
4. **Encryption**: The data packet is encrypted using the device key
5. **LoRa transmission**: The encrypted packet is sent via LoRa
6. **Deep sleep**: The ESP32 enters deep sleep for 15 minutes to conserve power

## Gateway Compatibility

This implementation uses a custom sensor type (0x51) which is not part of the standard expLORA protocol. For the gateway to properly decode and process data from this sensor, changes to the gateway code are required.

### Required Gateway Changes

The expLORA gateway must be updated to recognize and process the custom DIY DS18B20 sensor type. These changes are included in the following commit:
[https://github.com/Pajenicko/expLORA-Gateway-Lite/commit/ab84ba3c6fc0f48b05f633b6888df16e3517fa95](https://github.com/Pajenicko/expLORA-Gateway-Lite/commit/ab84ba3c6fc0f48b05f633b6888df16e3517fa95)

The key modifications to the gateway include:

1. Adding recognition for sensor type 0x51 (DIY DS18B20)
2. Implementing proper decoding of the temperature value from the custom packet format
3. Updating the data processing pipeline to handle the single temperature value format
4. Adding appropriate visualization or storage of the DIY DS18B20 sensor data

To use this implementation, you must ensure your gateway is updated with these changes.

## Key Differences from CLIMA Implementation

| Feature | DIY DS18B20 (This Version) | CLIMA Implementation |
|---------|----------------------------|----------------------|
| Sensor Type ID | 0x51 | 0x01 |
| Values Transmitted | 1 (temperature only) | 3 (temperature, pressure, humidity) |
| Packet Size | 11 bytes | 15 bytes |
| Gateway Compatibility | Requires gateway update | Works with standard gateway |

## Configuration Options

The code includes several configurable parameters:

- `SENSOR_TYPE_DS18B20` (0x51): Identifies this as a DIY DS18B20 sensor
- `SERIAL_NUMBER` (0x123456): Unique identifier for this sensor
- `DEVICE_KEY` (0xABCD1234): Encryption key for secure transmission
- `SLEEP_TIME_US` (900000000): Sleep duration between measurements (15 minutes)

## LoRa Settings

The LoRa radio is configured with the following parameters:

- Frequency: 868 MHz (European band)
- Transmit power: 17 dBm with PA_BOOST
- Spreading factor: 9
- Bandwidth: 125 kHz
- Coding rate: 4/5
- CRC: Enabled
- Preamble length: 16
- Sync word: 0x12

## Power Consumption

The device is optimized for low power operation:
- Deep sleep between measurements
- LoRa module put to sleep when not transmitting
- Minimal active time for sensor readings and transmissions

## Implementation Notes

This implementation is optimized for simplicity and minimal data transmission, as it only sends temperature values. The custom sensor type allows for specific handling and display on the gateway side but requires gateway modifications to function properly.
