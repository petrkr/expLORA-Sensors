#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

// Define pins for LoRa module (matching your gateway configuration)
#define LORA_SCK     18   // SCK pin
#define LORA_MISO    35   // MISO pin
#define LORA_MOSI    37   // MOSI pin
#define LORA_CS      5    // NSS pin
#define LORA_RST     14   // RESET pin
#define LORA_DIO0    36   // DIO0 pin (interrupt)

// Define pin for DS18B20 temperature sensor
#define ONE_WIRE_BUS 12   // GPIO12 - DS18B20 data pin

#endif
