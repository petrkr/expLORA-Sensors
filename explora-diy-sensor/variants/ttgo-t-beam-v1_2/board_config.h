#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

// Define pins for LoRa module
#define LORA_SCK     5    // SCK pin
#define LORA_MISO    19   // MISO pin
#define LORA_MOSI    27   // MOSI pin
#define LORA_CS      18   // NSS pin
#define LORA_RST     23   // RESET pin
#define LORA_DIO0    26   // DIO0 pin (interrupt)

// Define pin for DS18B20 temperature sensor
#define ONE_WIRE_BUS 2   // GPIO2 - DS18B20 data pin

#endif