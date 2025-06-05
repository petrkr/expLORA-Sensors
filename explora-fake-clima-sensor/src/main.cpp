/**
 * ESP32 expLORA Temperature Sensor Example
 *
 * This example shows how to implement an expLORA temperature sensor
 * using ESP32, a LoRa module (RFM95W), and a DS18B20 temperature sensor.
 *
 * Hardware configuration:
 * - ESP32 board
 * - LoRa RFM95W module (connected to SPI pins)
 * - DS18B20 temperature sensor on GPIO12
 *
 * The sensor reads temperature from DS18B20, transmits it via LoRa using
 * the expLORA CLIMA protocol with zero values for pressure and humidity,
 * and then enters deep sleep for 15 minutes
 *
 * Author: Igor Sverma, Pajenicko.cz
 * Date: May 16, 2025
 */

#include <Arduino.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// LoRa library
#include <LoRa.h>

// Board config
 #include "board_config.h"

// Define LoRa registers
#define REG_FIFO 0x00
#define REG_OP_MODE 0x01
#define REG_FRF_MSB 0x06
#define REG_FRF_MID 0x07
#define REG_FRF_LSB 0x08
#define REG_PA_CONFIG 0x09
#define REG_OCP 0x0B
#define REG_LNA 0x0C
#define REG_FIFO_ADDR_PTR 0x0D
#define REG_FIFO_TX_BASE_ADDR 0x0E
#define REG_FIFO_RX_BASE_ADDR 0x0F
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS 0x12
#define REG_RX_NB_BYTES 0x13
#define REG_PKT_SNR_VALUE 0x19
#define REG_PKT_RSSI_VALUE 0x1A
#define REG_MODEM_CONFIG_1 0x1D
#define REG_MODEM_CONFIG_2 0x1E
#define REG_SYMB_TIMEOUT_LSB 0x1F
#define REG_PREAMBLE_MSB 0x20
#define REG_PREAMBLE_LSB 0x21
#define REG_PAYLOAD_LENGTH 0x22
#define REG_MODEM_CONFIG_3 0x26
#define REG_FREQ_ERROR_MSB 0x28
#define REG_FREQ_ERROR_MID 0x29
#define REG_FREQ_ERROR_LSB 0x2A
#define REG_RSSI_WIDEBAND 0x2C
#define REG_DETECTION_OPTIMIZE 0x31
#define REG_INVERTIQ 0x33
#define REG_DETECTION_THRESHOLD 0x37
#define REG_SYNC_WORD 0x39
#define REG_INVERTIQ2 0x3B
#define REG_DIO_MAPPING_1 0x40
#define REG_VERSION 0x42
#define REG_PA_DAC 0x4D

// LoRa modes
#define MODE_LONG_RANGE_MODE 0x80
#define MODE_SLEEP 0x00
#define MODE_STDBY 0x01
#define MODE_TX 0x03
#define MODE_RX_CONTINUOUS 0x05
#define MODE_RX_SINGLE 0x06
#define MODE_CAD 0x07

// Sensor constants
#define SENSOR_TYPE_CLIMA 0x01 // CLIMA sensor ID
#define SERIAL_NUMBER 0x123456 // Unique serial number for this sensor
#define DEVICE_KEY 0xABCD1234  // Encryption key (device-specific)

// Setup a oneWire instance to communicate with DS18B20
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature
DallasTemperature sensors(&oneWire);

// Sleep time in microseconds (15 minutes = 900 seconds)
#define SLEEP_TIME_US 900000000 // 15 minutes in microseconds

// Function prototypes
void initLoRa();
void readSensorData(int16_t *temperature);
void buildPacket(uint8_t *packet, int16_t temperature);
uint8_t calculateChecksum(const uint8_t *data, uint8_t length);
void encrypt_data(uint8_t *data, uint8_t data_len, uint32_t key);
uint8_t generateRandomByte();
void sendData();

void setup()
{
  // Initialize serial communication for debugging
  Serial.begin(115200);
  delay(2000); // Give serial monitor time to open
  Serial.println("\nESP32 LoRa DS18B20 Temperature Sensor");

  // Setup will execute once, then sleep, then wake up and execute again
  try
  {
    sendData();
  }
  catch (const std::exception &e)
  {
    Serial.print("Exception in sendData(): ");
    Serial.println(e.what());
  }
  catch (...)
  {
    Serial.println("Unknown exception in sendData()!");
  }

  // Enter deep sleep
  Serial.println("Entering deep sleep for 15 minutes...");
  Serial.flush();

  // Configure wake up from deep sleep using timer
  esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);

  // Enter deep sleep
  esp_deep_sleep_start();
}

void loop()
{
  // This function will never run because we enter deep sleep at the end of setup()
  // After wakeup, ESP32 will restart and execute setup() again
}

/**
 * Initialize LoRa module with settings matching the TI example
 */
void initLoRa()
{
  Serial.println("Initializing LoRa module...");

  // Set SPI pins
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  // Initialize LoRa
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  // Try to initialize LoRa with retries
  int attempts = 0;
  bool initialized = false;

  while (!initialized && attempts < 5)
  {
    if (LoRa.begin(868E6))
    { // 868 MHz for Europe
      initialized = true;
    }
    else
    {
      Serial.println("LoRa initialization failed, retrying...");
      delay(500);
      attempts++;
    }
  }

  if (!initialized)
  {
    Serial.println("Failed to initialize LoRa after multiple attempts!");
    return;
  }

  LoRa.sleep(); // Put in sleep mode first to allow configuration

  // Configure radio using public methods

  // Set frequency to 868 MHz
  LoRa.setFrequency(868E6);

  // Set power to 17dBm using PA_BOOST
  LoRa.setTxPower(17, 1); // 17dBm with PA_BOOST enabled

  // Set spreading factor to 9
  LoRa.setSpreadingFactor(9);

  // Set bandwidth to 125kHz
  LoRa.setSignalBandwidth(125E3);

  // Set coding rate to 4/5
  LoRa.setCodingRate4(5);

  // Enable CRC
  LoRa.enableCrc();

  // Set preamble length to 16
  LoRa.setPreambleLength(16);

  // Set the sync word
  LoRa.setSyncWord(0x12);

  // Use built-in routine to ensure all registers are properly set
  LoRa.idle();
  delay(10);

  Serial.println("LoRa module initialized successfully");
}

/**
 * Read temperature data from DS18B20 sensor
 */
void readSensorData(int16_t *temperature)
{
  // Default value in case of failure
  *temperature = 2500; // 25.00 °C

  Serial.println("Initializing DS18B20 sensor...");

  // Initialize DS18B20
  sensors.begin();
  delay(250); // Increased delay for initialization

  // Count connected devices
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(deviceCount);
  Serial.println(" DS18B20 devices");

  if (deviceCount == 0)
  {
    Serial.println("No DS18B20 sensors found! Using default value.");
    return;
  }

  // Set resolution to 12 bits (0.0625°C)
  sensors.setResolution(12);

  // Request temperature from all sensors
  Serial.println("Requesting temperature...");
  sensors.requestTemperatures();
  delay(750); // delay to give time for conversion (up to 750ms for 12-bit resolution)

  // Get temperature from first sensor in Celsius
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC == DEVICE_DISCONNECTED_C || tempC == 85.0)
  {
    Serial.println("Error: Failed to read temperature or got power-on value! Using default value.");
    return;
  }

  // Convert to hundredths of degrees
  *temperature = static_cast<int16_t>(tempC * 100);

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
}

/**
 * Build packet with sensor data in the same format as TI example
 */
void buildPacket(uint8_t *packet, int16_t temperature)
{
  // Add random byte at the beginning
  packet[0] = generateRandomByte();

  // Device type (CLIMA instead of DS18B20)
  packet[1] = SENSOR_TYPE_CLIMA; // Změněno na CLIMA

  // Serial number (3 bytes)
  packet[2] = (SERIAL_NUMBER >> 16) & 0xFF;
  packet[3] = (SERIAL_NUMBER >> 8) & 0xFF;
  packet[4] = SERIAL_NUMBER & 0xFF;

  // Battery voltage (2 bytes) in mV
  uint16_t batteryVoltage = 3300; // 3.3V as an example, could read from ADC
  packet[5] = (batteryVoltage >> 8) & 0xFF;
  packet[6] = batteryVoltage & 0xFF;

  // Number of values (změněno z 1 na 3 pro CLIMA - teplota, tlak, vlhkost)
  packet[7] = 3;

  // Temperature value (2 bytes - signed integer in hundredths of degrees)
  packet[8] = (temperature >> 8) & 0xFF;
  packet[9] = temperature & 0xFF;

  // Pressure value (2 bytes - unsigned integer in tenths of hPa) - nastaveno na 0
  uint16_t pressure = 10000; // 0 hPa
  packet[10] = (pressure >> 8) & 0xFF;
  packet[11] = pressure & 0xFF;

  // Humidity value (2 bytes - unsigned integer in hundredths of percent) - nastaveno na 0
  uint16_t humidity = 0; // 0.00 %
  packet[12] = (humidity >> 8) & 0xFF;
  packet[13] = humidity & 0xFF;

  // Calculate checksum (simple XOR of all previous bytes)
  packet[14] = calculateChecksum(packet, 14);
}

/**
 * Calculate checksum using simple XOR of all bytes
 */
uint8_t calculateChecksum(const uint8_t *data, uint8_t length)
{
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < length; i++)
  {
    checksum ^= data[i];
  }
  return checksum;
}

/**
 * Encrypt data using a simple XOR-based algorithm
 */
void encrypt_data(uint8_t *data, uint8_t data_len, uint32_t key)
{
  // Use key_bytes to access the four bytes of the key
  uint8_t *key_bytes = (uint8_t *)&key;
  uint8_t prev_byte = 0; // Start from zero

  for (uint8_t i = 0; i < data_len; i++)
  {
    uint8_t key_byte = key_bytes[i & 0x03];
    // Encrypt
    data[i] = data[i] ^ key_byte ^ (prev_byte >> 1);
    // Save encrypted byte for next round
    prev_byte = data[i];
  }
}

/**
 * Generate random byte for packet header
 */
uint8_t generateRandomByte()
{
  // Use ESP32's hardware random number generator
  return random(256);
}

/**
 * Read sensor data and send it via LoRa
 */
void sendData()
{
  Serial.println("Preparing to send data...");

  // Read battery voltage - in a real application, we would read from ADC
  uint16_t batteryVoltage = 3300; // 3.3V as an example

  // Read temperature from DS18B20
  int16_t temperature;
  readSensorData(&temperature);

  // Create packet
  uint8_t packet[15]; // 14 bytes + checksum
  buildPacket(packet, temperature);

  // Debug - print unencrypted packet
  Serial.println("Unencrypted packet:");
  for (int i = 0; i < sizeof(packet); i++)
  {
    Serial.print("0x");
    if (packet[i] < 16)
      Serial.print("0");
    Serial.print(packet[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Encrypt the packet
  encrypt_data(packet, sizeof(packet), DEVICE_KEY);

  // Debug - print encrypted packet
  Serial.println("Encrypted packet:");
  for (int i = 0; i < sizeof(packet); i++)
  {
    Serial.print("0x");
    if (packet[i] < 16)
      Serial.print("0");
    Serial.print(packet[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Initialize and configure LoRa module
  initLoRa();

  // Send packet via LoRa
  Serial.println("Sending LoRa packet...");

  // Begin packet
  LoRa.beginPacket();

  // Write packet data
  for (uint8_t i = 0; i < sizeof(packet); i++)
  {
    LoRa.write(packet[i]);
  }

  // End packet and send
  bool success = LoRa.endPacket();

  if (success)
  {
    Serial.println("Packet sent successfully!");
  }
  else
  {
    Serial.println("Failed to send packet!");
  }

  // Put LoRa module to sleep to save power
  LoRa.sleep();
}