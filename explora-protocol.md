# expLORA Sensor Protocol Documentation

This document describes the communication protocol used between expLORA sensors and the expLORA Gateway Lite.

## Table of Contents

1. [General Packet Structure](#general-packet-structure)
2. [Sensor Types](#sensor-types)
4. [Packet Format by Sensor Type](#packet-format-by-sensor-type)
   - [CLIMA (BME280)](#clima-bme280)
   - [CARBON (SCD40)](#carbon-scd40)
   - [METEO Weather Station](#meteo-weather-station)
   - [Light Sensor (VEML7700)](#light-sensor-veml7700)
   - [DIY Temperature Sensor (DS18B20)](#diy-temperature-sensor-ds18b20)
5. [Payload Examples](#payload-examples)
6. [LoRa Settings](#lora-settings)
6. [Gateway Processing Logic](#gateway-processing-logic)

## General Packet Structure

All LoRa packets follow a common structure:

| Byte(s) | Field            | Description                                        |
|---------|-----------------|----------------------------------------------------|
| 0       | Random Seed     | Random byte used as seed for encryption             |
| 1       | Device Type     | Type of sensor (see [Sensor Types](#sensor-types)) |
| 2-4     | Serial Number   | 3-byte unique identifier of the sensor             |
| 5-6     | Battery Voltage | 2-byte battery voltage in millivolts               |
| 7       | Value Count     | Number of sensor values in the payload              |
| 8+      | Sensor Data     | Sensor-specific data (variable length)             |
| Last    | Checksum        | XOR of all previous bytes                          |

The total length of the packet varies based on the sensor type and the number of values it transmits.

The Gateway must know the device key to decrypt the data received from sensors.

## Sensor Types

The system supports the following sensor types:

| Value | Name     | Description                                     |
|-------|----------|-------------------------------------------------|
| 0x00  | UNKNOWN  | Unknown or undefined sensor type                |
| 0x01  | CLIMA    | Temperature, humidity, pressure (BME280)        |
| 0x02  | CARBON   | Temperature, humidity, CO2 (SCD40)              |
| 0x03  | METEO    | Weather station (temp, hum, press, wind, rain)  |
| 0x04  | VEML7700 | Light intensity sensor                          |
| 0x51  | DIY_TEMP | DIY Temperature sensor (like DS18B20)           |

## Packet Format by Sensor Type

### CLIMA (BME280)

The CLIMA sensor sends temperature, humidity, and pressure data.

| Byte(s) | Field           | Description                                  | Conversion           |
|---------|----------------|----------------------------------------------|---------------------|
| 0-7     | Header         | Common packet header                         |                     |
| 8-9     | Temperature    | 2-byte signed integer (temperature × 100)    | Value ÷ 100 = °C    |
| 10-11   | Pressure       | 2-byte unsigned integer (pressure × 10)      | Value ÷ 10 = hPa    |
| 12-13   | Humidity       | 2-byte unsigned integer (humidity × 100)     | Value ÷ 100 = %     |
| 14      | Checksum       | XOR of all previous bytes                    |                     |

Total packet length: 15 bytes

### CARBON (SCD40)

The CARBON sensor sends temperature, CO2 concentration, and humidity data.

| Byte(s) | Field           | Description                                  | Conversion           |
|---------|----------------|----------------------------------------------|---------------------|
| 0-7     | Header         | Common packet header                         |                     |
| 8-9     | Temperature    | 2-byte signed integer (temperature × 100)    | Value ÷ 100 = °C    |
| 10-11   | CO2            | 2-byte unsigned integer (CO2 in ppm)         | Value = ppm         |
| 12-13   | Humidity       | 2-byte unsigned integer (humidity × 100)     | Value ÷ 100 = %     |
| 14      | Checksum       | XOR of all previous bytes                    |                     |

Total packet length: 15 bytes

### METEO Weather Station

The METEO weather station sends comprehensive weather data.

| Byte(s) | Field           | Description                                  | Conversion             |
|---------|----------------|----------------------------------------------|------------------------|
| 0-7     | Header         | Common packet header                         |                       |
| 8-9     | Temperature    | 2-byte signed integer (temperature × 100)    | Value ÷ 100 = °C      |
| 10-11   | Pressure       | 2-byte unsigned integer (pressure × 10)      | Value ÷ 10 = hPa      |
| 12-13   | Humidity       | 2-byte unsigned integer (humidity × 100)     | Value ÷ 100 = %       |
| 14-15   | Wind Speed     | 2-byte unsigned integer (wind speed × 10)    | Value ÷ 10 = m/s      |
| 16-17   | Wind Direction | 2-byte unsigned integer (0-359 degrees)      | Value = degrees       |
| 18-19   | Rain Amount    | 2-byte unsigned integer (rain amount × 1000) | Value ÷ 1000 = mm     |
| 20-21*  | Rain Rate*     | 2-byte unsigned integer (rain rate × 100)    | Value ÷ 100 = mm/h    |
| Last    | Checksum       | XOR of all previous bytes                    |                       |

\* Rain Rate field is optional, present only in extended packets (7 values instead of 6).

Total packet length: 21 bytes (basic) or 23 bytes (extended with rain rate)

### Light Sensor (VEML7700)

The Light sensor sends illuminance data.

| Byte(s) | Field           | Description                                  | Conversion           |
|---------|----------------|----------------------------------------------|---------------------|
| 0-7     | Header         | Common packet header                         |                     |
| 8-11    | Light Level    | 4-byte unsigned integer (lux × 100)          | Value ÷ 100 = lux   |
| 12      | Checksum       | XOR of all previous bytes                    |                     |

Total packet length: 13 bytes

### DIY Temperature Sensor (DS18B20)

The DIY_TEMP sensor sends only temperature data from sensors like DS18B20.

| Byte(s) | Field           | Description                                  | Conversion           |
|---------|----------------|----------------------------------------------|---------------------|
| 0-7     | Header         | Common packet header                         |                     |
| 8-9     | Temperature    | 2-byte signed integer (temperature × 100)    | Value ÷ 100 = °C    |
| 10      | Checksum       | XOR of all previous bytes                    |                     |

Total packet length: 11 bytes

## Payload Examples

### CLIMA (BME280) Example

```
Raw bytes (hexadecimal): 
3F 01 AB CD EF 0E DC 03 09 C4 27 16 1A 40 XX

Decoded:
- Random Seed: 0x3F (random)
- Device Type: 0x01 (CLIMA)
- Serial Number: 0xABCDEF
- Battery Voltage: 0x0EDC = 3804 mV = 3.804V
- Value Count: 0x03 (3 values)
- Temperature: 0x09C4 = 2500 → 25.00°C
- Pressure: 0x2716 = 10006 → 1000.6 hPa
- Humidity: 0x1A40 = 6720 → 67.20%
- Checksum: XX (calculated XOR of all previous bytes)
```

### CARBON (SCD40) Example

```
Raw bytes (hexadecimal): 
A2 02 12 34 56 0E 10 03 09 60 01 F4 1C 20 XX

Decoded:
- Random Seed: 0xA2 (random)
- Device Type: 0x02 (CARBON)
- Serial Number: 0x123456
- Battery Voltage: 0x0E10 = 3600 mV = 3.6V
- Value Count: 0x03 (3 values)
- Temperature: 0x0960 = 2400 → 24.00°C
- CO2: 0x01F4 = 500 ppm
- Humidity: 0x1C20 = 7200 → 72.00%
- Checksum: XX (calculated XOR of all previous bytes)
```

### METEO Example (Extended)

```
Raw bytes (hexadecimal): 
C7 03 AA BB CC 0E 5C 07 08 98 27 2C 1B 58 00 C8 01 0E 00 64 00 C8 XX

Decoded:
- Random Seed: 0xC7 (random)
- Device Type: 0x03 (METEO)
- Serial Number: 0xAABBCC
- Battery Voltage: 0x0E5C = 3676 mV = 3.676V
- Value Count: 0x07 (7 values)
- Temperature: 0x0898 = 2200 → 22.00°C
- Pressure: 0x272C = 10028 → 1002.8 hPa
- Humidity: 0x1B58 = 7000 → 70.00%
- Wind Speed: 0x00C8 = 200 → 20.0 m/s
- Wind Direction: 0x010E = 270 degrees
- Rain Amount: 0x0064 = 100 → 0.100 mm
- Rain Rate: 0x00C8 = 200 → 2.00 mm/h
- Checksum: XX (calculated XOR of all previous bytes)
```

### DIY Temperature Sensor (DS18B20) Example

```
Raw bytes (hexadecimal): 
7B 51 12 34 56 0C E4 01 09 C4 XX

Decoded:
- Random Seed: 0x7B (random)
- Device Type: 0x51 (DIY_TEMP)
- Serial Number: 0x123456
- Battery Voltage: 0x0CE4 = 3300 mV = 3.3V
- Value Count: 0x01 (1 value)
- Temperature: 0x09C4 = 2500 → 25.00°C
- Checksum: XX (calculated XOR of all previous bytes)
```

## LoRa Settings

All expLORA sensors use the same LoRa settings:

- Frequency: 868 MHz (European band)
- Transmit power: 17 dBm with PA_BOOST
- Spreading factor: 9
- Bandwidth: 125 kHz
- Coding rate: 4/5
- CRC: Enabled
- Preamble length: 16
- Sync word: 0x12

## Gateway Processing Logic

When the Gateway receives a LoRa packet, it processes it as follows:

1. **Detect packet reception** via interrupt (DIO0 pin of RFM95W)
2. **Read the packet** from the LoRa module's FIFO buffer
3. **Try to decrypt** using known keys from registered sensors 
4. **Validate checksum** to ensure data integrity
5. **Parse data** based on the device type
6. **Apply corrections** if configured for the sensor
7. **Update sensor data** in memory
8. **Forward data** to:
   - MQTT broker (if configured)
   - Custom HTTP endpoint (if configured)
   - Web interface (for real-time monitoring)
9. **Log the event** with original and corrected values

If a packet is received from an unknown sensor (serial number not found in the Gateway's database), the Gateway will log the detection but cannot process the data without the correct device key.

### Data Correction Example

If a temperature correction of +1.5°C is configured for a CLIMA sensor:

1. Original temperature: 0x09C4 = 2500 → 25.00°C
2. Correction applied: 25.00°C + 1.5°C = 26.50°C
3. The Gateway will log: "Corrections applied: Temp 25.00→26.50°C"
4. The corrected value (26.50°C) is stored and used for all subsequent operations

## Advanced Features

### Altitude Adjustment for Pressure Sensors

For pressure sensors (CLIMA and METEO), the Gateway can adjust reported pressure values based on the configured altitude. This converts relative pressure to absolute pressure using the following formula:

```
P_abs = P_rel / ((1 - (L * h) / T) ^ (g * M / (R * L)))

Where:
- P_abs: Absolute pressure (hPa)
- P_rel: Relative pressure (hPa)
- L: Temperature gradient (0.0065 K/m)
- h: Altitude (m)
- T: Temperature (K)
- g: Gravitational acceleration (9.80665 m/s²)
- M: Molar mass of air (0.0289644 kg/mol)
- R: Universal gas constant (8.3144598 J/(mol·K))
```

This adjustment is performed automatically when an altitude value is configured for the sensor.

### Daily Rain Total Reset

The METEO sensor reports individual rain amounts, which the Gateway accumulates to a daily total. This total is reset at midnight (local time). The Gateway stores:

- `rainAmount`: The most recently received rain amount measurement
- `dailyRainTotal`: The accumulated rain since midnight
- `lastRainReset`: The timestamp of the last reset

This allows tracking both current precipitation and daily accumulation.
