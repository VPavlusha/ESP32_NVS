[![GitHub](https://img.shields.io/github/license/VPavlusha/ESP32_NVS?color=blue&label=License&logo=github)](LICENSE)
[![cpp-linter](https://github.com/VPavlusha/ESP32_NVS/actions/workflows/cpp-linter.yml/badge.svg)](https://github.com/VPavlusha/ESP32_NVS/actions/workflows/cpp-linter.yml)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/VPavlusha/ESP32_NVS?label=Release&logo=github)](https://github.com/VPavlusha/ESP32_NVS/releases)
[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/badges/StandWithUkraine.svg)](https://stand-with-ukraine.pp.ua)
[![Made in Ukraine](https://img.shields.io/badge/Made_in-Ukraine-ffd700.svg?labelColor=0057b7)](https://stand-with-ukraine.pp.ua)
---

# ESP32 IDF Non-Volatile Storage Library
The ESP32 IDF [Non-Volatile Storage (NVS) library](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32/api-reference/storage/nvs_flash.html) is designed to store key-value pairs in flash.

#### Table of Contents
&emsp;[1. Features](#1-features)  
&emsp;[2. ESP32 NVS](#2-esp32-nvs)  
&emsp;[3. Basic Usage](#3-basic-usage)  
&emsp;[4. Getting Started](#4-getting-started)  
&emsp;[5. Example](#5-example)  
&emsp;[6. Contributing](#6-contributing)  
&emsp;[7. License](#7-license)  

## 1. Features
  - ESP-IDF v5.0.2
  - Support **float** and **double** types.
  - Written in C language.
  - MIT License.

## 2. ESP32 NVS
NVS operates on key-value pairs. Keys are ASCII strings, the maximum key length is currently 15 characters.
Values can have one of the following types:
  - Integer types: **uint8_t**, **int8_t**, **uint16_t**, **int16_t**, **uint32_t**, **int32_t**, **uint64_t**, **int64_t**
  - Floating-point types: **float**, **double**
  - Zero-terminated **string**
  - Variable length binary data (**blob**)

NVS assigns each key-value pair to one of namespaces. Namespace names follow the same rules as key names, i.e., the maximum length is 15 characters. Furthermore, there can be no more than 254 different namespaces in one NVS partition.

## 3. Basic Usage
```C
#include <stdlib.h>

#include "esp_check.h"
#include "esp_log.h"

#include "non_volatile_storage.h"

static const char *TAG = "app_main";

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_init());

    // Float

    float write_float = 123456.789;
    if (nvs_write_float("namespace_1", "key_1", write_float) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully write float to NVS");
    } else {
        ESP_LOGE(TAG, "Failed to write float to NVS");
    }

    float read_float;
    if (nvs_read_float("namespace_1", "key_1", &read_float) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully read float from NVS: %f", read_float);
    } else {
        ESP_LOGE(TAG, "Failed to read float from NVS");
    }

    // String

    char *write_string = "Hello, World!";
    if (nvs_write_string("namespace_1", "key_2", write_string) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully write string to NVS");
    } else {
        ESP_LOGE(TAG, "Failed to write string to NVS");
    }

    char *read_string = NULL;
    if (nvs_read_string("namespace_1", "key_2", &read_string) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully read string from NVS: %s", read_string);
    } else {
        ESP_LOGE(TAG, "Failed to read string from NVS");
    }
    // IMPORTANT NOTE!: This applies ONLY to strings. Remember to delete the pointer to avoid a memory leak.
    free(read_string);

    // Blob

    #define MAX_STRING_LENGTH 10 
    typedef struct {
        char name[MAX_STRING_LENGTH];
        uint32_t id;
    } person_t;

    const person_t write_blob = {
        .name = "Joe",
        .id = 123,
    };

    if (nvs_write_blob("namespace_1", "key_3", &write_blob, sizeof(person_t)) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully write blob to NVS");
    } else {
        ESP_LOGE(TAG, "Failed to write blob to NVS");
    }

    person_t read_blob = {};
    if (nvs_read_blob("namespace_1", "key_3", &read_blob, sizeof(person_t)) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully read blob from NVS: name:%s, id:%d", read_blob.name, read_blob.id);
    } else {
        ESP_LOGE(TAG, "Failed to read blob from NVS");
    }
}
```
## 4. Getting Started
To get started with the ESP32 Task Monitor project, you'll need an ESP32 microcontroller and a host computer running Python. You'll also need to install the ESP-IDF development framework and the required Python packages.

### 4.1 Clone the project repository:
```C
    git clone git@github.com:VPavlusha/ESP32_NVS.git
```
### 4.2 Build the project:
```C
    cd ESP32_NVS/example
    idf.py build
```
### 4.3 Flash onto your ESP32 microcontroller:
```C
    idf.py -p PORT [-b BAUD] flash
```
Replace PORT with your ESP32 board’s serial port name.
You can also change the flasher baud rate by replacing BAUD with the baud rate you need. The default baud rate is 460800.<br/>
### 4.4 Monitor the output:
```C
    idf.py -p <PORT> monitor
```
Do not forget to replace PORT with your serial port name.

More information how to build project: [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32/get-started/start-project.html).

## 5. Example
This project includes an [example](https://github.com/VPavlusha/ESP32_NVS/tree/main/example) that showcases the functionality of the Task Monitor library. This example provides a practical demonstration of how to use the NVS API to write/read data to/from NVS in your own applications.

## 6. Contributing
Contributions to the ESP32 NVS project are welcome. If you find a bug or have a feature request, please submit an issue on the project's GitHub page. If you'd like to contribute code, please submit a pull request.

## 7. License
The ESP32 NVS project is licensed under the MIT License. See the [MIT license] file for more information.
  
  [MIT license]: http://www.opensource.org/licenses/mit-license.html
