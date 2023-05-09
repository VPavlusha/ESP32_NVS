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
