# ESP32 expLORA Temperature Sensor

## Project Overview

This project implements a temperature sensor using an ESP32 microcontroller with LoRa communication capabilities. The sensor is designed to be compatible with the expLORA CLIMA protocol, allowing it to work seamlessly with existing expLORA gateway infrastructure without any modifications to the gateway code.

## Features

- **Real-time temperature monitoring** using a DS18B20 digital temperature sensor
- **Low power consumption** with deep sleep mode between measurements
- **LoRa wireless communication** for long-range data transmission
- **Fully compatible with expLORA CLIMA protocol**
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
3. **Packet construction**: Data is formatted according to the expLORA CLIMA protocol
   - The first byte contains a random value for improved encryption
   - The second byte identifies this as a CLIMA sensor (0x01)
   - The next three bytes contain the unique serial number
   - The next two bytes store battery voltage in mV
   - Byte 7 indicates there are 3 values to follow (temperature, pressure, humidity)
   - Bytes 8-9 store temperature in hundredths of degrees Celsius
   - Bytes 10-11 store pressure (set to 0 as not measured)
   - Bytes 12-13 store humidity (set to 0 as not measured)
   - The final byte contains a checksum
4. **Encryption**: The data packet is encrypted using the device key
5. **LoRa transmission**: The encrypted packet is sent via LoRa
6. **Deep sleep**: The ESP32 enters deep sleep for 15 minutes to conserve power

## Gateway Compatibility

This implementation uses the expLORA CLIMA protocol which is natively supported by expLORA gateways. Although this device only measures temperature (with pressure and humidity set to zero), it's identified as a CLIMA sensor type in the protocol. This means:

- **No changes needed to gateway code**
- **Complete compatibility** with existing infrastructure
- **Automatic decoding** of temperature data

## Configuration Options

The code includes several configurable parameters:

- `SENSOR_TYPE_CLIMA` (0x01): Identifies this as a CLIMA-compatible sensor
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