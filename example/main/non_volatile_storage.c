#include "non_volatile_storage.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

#include "sdkconfig.h"

static const char *TAG = "non_volatile_storage";

esp_err_t nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

static esp_err_t esp32_nvs_open(const char *namespace, nvs_open_mode_t open_mode, nvs_handle_t *nvs_handle)
{
    esp_err_t err = nvs_open(namespace, open_mode, nvs_handle); 
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s(): Error opening NVS namespace %s: %d (%s)!", __func__, namespace, err, esp_err_to_name(err));
    }
    return err;
}

static char* value_to_string(nvs_type_t type_value, const void *value) {
    char *string_value = NULL;
    int16_t ret = -1;

    switch(type_value) {
        case NVS_TYPE_I8:
            ret = asprintf(&string_value, "%d", *((int8_t*)value));
            break;
        case NVS_TYPE_U8:
            ret = asprintf(&string_value, "%u", *((uint8_t*)value));
            break;
        case NVS_TYPE_I16:
            ret = asprintf(&string_value, "%d", *((int16_t*)value));
            break;
        case NVS_TYPE_U16:
            ret = asprintf(&string_value, "%u", *((uint16_t*)value));
            break;
        case NVS_TYPE_I32:
            ret = asprintf(&string_value, "%d", *((int32_t*)value));
            break;
        case NVS_TYPE_U32:
            ret = asprintf(&string_value, "%u", *((uint32_t*)value));
            break;
        case NVS_TYPE_I64:
            ret = asprintf(&string_value, "%lld", *((int64_t*)value));
            break;
        case NVS_TYPE_U64:
            ret = asprintf(&string_value, "%llu", *((uint64_t*)value));
            break;
        default:
            ret = asprintf(&string_value, "Unsupported type: %d", type_value);
            break;
    }

    if (ret == -1) {
        string_value = NULL;
        ESP_LOGE(TAG, "%s(): Failed to allocate memory", __func__);
    }
    return string_value;
}

static esp_err_t esp32_nvs_write(const char *namespace, const char *key, nvs_type_t type_value,
                                 const void *value, size_t length)
{
    if (namespace == NULL) {
        ESP_LOGE(TAG, "%s(): Failed to write value: namespace is NULL!", __func__);
        return ESP_ERR_INVALID_ARG;
    }
    if (key == NULL) {
        ESP_LOGE(TAG, "%s(): Failed to write value: key is NULL!", __func__);
        return ESP_ERR_INVALID_ARG;
    }
    if (value == NULL) {
        ESP_LOGE(TAG, "%s(): Failed to write NULL value!", __func__);
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = esp32_nvs_open(namespace, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    switch (type_value) {
        case NVS_TYPE_I8:
            err = nvs_set_i8(nvs_handle, key, *(int8_t*)value);
            break;
        case NVS_TYPE_U8:
            err = nvs_set_u8(nvs_handle, key, *(uint8_t*)value);
            break;
        case NVS_TYPE_I16:
            err = nvs_set_i16(nvs_handle, key, *(int16_t*)value);
            break;
        case NVS_TYPE_U16:
            err = nvs_set_u16(nvs_handle, key, *(uint16_t*)value);
            break;
        case NVS_TYPE_I32:
            err = nvs_set_i32(nvs_handle, key, *(int32_t*)value);
            break;
        case NVS_TYPE_U32:
            err = nvs_set_u32(nvs_handle, key, *(uint32_t*)value);
            break;
        case NVS_TYPE_I64:
            err = nvs_set_i64(nvs_handle, key, *(int64_t*)value);
            break;
        case NVS_TYPE_U64:
            err = nvs_set_u64(nvs_handle, key, *(uint64_t*)value);
            break;
        case NVS_TYPE_STR:
            err = nvs_set_str(nvs_handle, key, (char*)value);
            break;
        case NVS_TYPE_BLOB:
            err = nvs_set_blob(nvs_handle, key, value, length);
            break;
        default:
            err = ESP_ERR_NVS_TYPE_MISMATCH;
            break;
    }

    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
        if (err == ESP_OK) {
            switch (type_value) {
                case NVS_TYPE_STR:
                    ESP_LOGI(TAG, "Successfully write string to NVS %s.%s: %s", namespace, key, value);
                    break;
                case NVS_TYPE_BLOB:
                    ESP_LOGI(TAG, "Successfully write blob to NVS %s.%s", namespace, key);
                    break;
                default:
                    #if CONFIG_LOG_MAXIMUM_LEVEL >= CONFIG_LOG_DEFAULT_LEVEL_INFO
                        char *string_value = value_to_string(type_value, value);
                        if (string_value != NULL) {
                            ESP_LOGI(TAG, "Successfully write value to NVS %s.%s: %s", namespace, key, string_value);
                            free(string_value);
                        }
                    #endif  // CONFIG_LOG_MAXIMUM_LEVEL
                    break;
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to write to NVS %s.%s: %d (%s)!", namespace, key, err, esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
    return err;
}

esp_err_t nvs_write_int8(const char *namespace, const char *key, int8_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_I8, &value, 0);
}

esp_err_t nvs_write_uint8(const char *namespace, const char *key, uint8_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_U8, &value, 0);
}

esp_err_t nvs_write_int16(const char *namespace, const char *key, int16_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_I16, &value, 0);
}

esp_err_t nvs_write_uint16(const char *namespace, const char *key, uint16_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_U16, &value, 0);
}

esp_err_t nvs_write_int32(const char *namespace, const char *key, int32_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_I32, &value, 0);
}

esp_err_t nvs_write_uint32(const char *namespace, const char *key, uint32_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_U32, &value, 0);
}

esp_err_t nvs_write_int64(const char *namespace, const char *key, int64_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_I64, &value, 0);
}

esp_err_t nvs_write_uint64(const char *namespace, const char *key, uint64_t value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_U64, &value, 0);
}

esp_err_t nvs_write_string(const char *namespace, const char *key, const char *value)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_STR, value, 0);
}

#define MAX_STRING_LENGTH_FOR_FLOAT 32  // Maximum length of the float to string representation
esp_err_t nvs_write_float(const char *namespace, const char *key, float value)
{
    char string[MAX_STRING_LENGTH_FOR_FLOAT];
    int32_t result = snprintf(string, sizeof(string), "%f", value);

    esp_err_t err;
    if (result >= 0 && result < sizeof(string)) {
        err = esp32_nvs_write(namespace, key, NVS_TYPE_STR, &string, 0);
    } else {
        err = ESP_FAIL;
        ESP_LOGE(TAG, "%s(): Error occurred during formatting", __func__);
    }
    return err;
}

#define MAX_STRING_LENGTH_FOR_DOUBLE 64  // Maximum length of the double to string representation
esp_err_t nvs_write_double(const char *namespace, const char *key, double value)
{
    char string[MAX_STRING_LENGTH_FOR_DOUBLE];
    int32_t result = snprintf(string, sizeof(string), "%lf", value);

    esp_err_t err;
    if (result >= 0 && result < sizeof(string)) {
        err = esp32_nvs_write(namespace, key, NVS_TYPE_STR, &string, 0);
    } else {
        err = ESP_FAIL;
        ESP_LOGE(TAG, "%s(): Error occurred during formatting", __func__);
    }
    return err;
}

esp_err_t nvs_write_blob(const char *namespace, const char *key, const void *value, size_t length)
{
    return esp32_nvs_write(namespace, key, NVS_TYPE_BLOB, value, length);
}

static esp_err_t esp32_nvs_read(const char *namespace, const char *key, nvs_type_t type_value,
                                void *value, size_t length)
{
    if (namespace == NULL) {
        ESP_LOGE(TAG, "%s(): Failed to read value: namespace is NULL!", __func__);
        return ESP_ERR_INVALID_ARG;
    }
    if (key == NULL) {
        ESP_LOGE(TAG, "%s(): Failed to read value: key is NULL!", __func__);
        return ESP_ERR_INVALID_ARG;
    }
    if (type_value != NVS_TYPE_STR) {
        if (value == NULL) {
            ESP_LOGE(TAG, "%s(): Failed to read NULL value!", __func__);
            return ESP_ERR_INVALID_ARG;
        }
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = esp32_nvs_open(namespace, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    switch (type_value) {
        case NVS_TYPE_I8:
            err = nvs_get_i8(nvs_handle, key, (int8_t*)value);
            break;
        case NVS_TYPE_U8:
            err = nvs_get_u8(nvs_handle, key, (uint8_t*)value);
            break;
        case NVS_TYPE_I16:
            err = nvs_get_i16(nvs_handle, key, (int16_t*)value);
            break;
        case NVS_TYPE_U16:
            err = nvs_get_u16(nvs_handle, key, (uint16_t*)value);
            break;
        case NVS_TYPE_I32:
            err = nvs_get_i32(nvs_handle, key, (int32_t*)value);
            break;
        case NVS_TYPE_U32:
            err = nvs_get_u32(nvs_handle, key, (uint32_t*)value);
            break;
        case NVS_TYPE_I64:
            err = nvs_get_i64(nvs_handle, key, (int64_t*)value);
            break;
        case NVS_TYPE_U64:
            err = nvs_get_u64(nvs_handle, key, (uint64_t*)value);
            break;
        case NVS_TYPE_STR:
            size_t required_string_size = 0;
            err = nvs_get_str(nvs_handle, key, NULL, &required_string_size);

            if (err == ESP_OK) {
                *(char**)value = malloc(required_string_size);
                if (*(char**)value != NULL) {
                    err = nvs_get_str(nvs_handle, key, *(char**)value, &required_string_size);
                } else {
                    err = ESP_ERR_NO_MEM;
                    ESP_LOGE(TAG, "%s(): Failed to allocate memory", __func__);
                }
            }
            break;
        case NVS_TYPE_BLOB:
            size_t required_blob_size = length;
            err = nvs_get_blob(nvs_handle, key, value, &required_blob_size);
            break;
        default:
            err = ESP_ERR_NVS_TYPE_MISMATCH;
            break;
    }

    switch (err) {
        case ESP_OK:
            switch (type_value) {
                case NVS_TYPE_STR:
                    ESP_LOGI(TAG, "Successfully read string from NVS %s.%s: %s", namespace, key, *(char**)value);
                    break;
                case NVS_TYPE_BLOB:
                    ESP_LOGI(TAG, "Successfully read blob from NVS %s.%s", namespace, key);
                    break;
                default:
                    #if CONFIG_LOG_MAXIMUM_LEVEL >= CONFIG_LOG_DEFAULT_LEVEL_INFO
                        char *string_value = value_to_string(type_value, value);
                        if (string_value != NULL) {
                            ESP_LOGI(TAG, "Successfully read value from NVS %s.%s: %s", namespace, key, string_value);
                            free(string_value);
                        }
                    #endif  // CONFIG_LOG_MAXIMUM_LEVEL
                    break;
            }
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(TAG, "Value %s.%s is not initialized yet", namespace, key);
            break;
        default:
            ESP_LOGE(TAG, "Failed to read from NVS %s.%s: %d (%s)", namespace, key, err, esp_err_to_name(err));
            break;
    }
    
    nvs_close(nvs_handle);
    return err;
}

esp_err_t nvs_read_int8(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_I8, out_value, 0);
}

esp_err_t nvs_read_uint8(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_U8, out_value, 0);
}

esp_err_t nvs_read_int16(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_I16, out_value, 0);
}

esp_err_t nvs_read_uint16(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_U16, out_value, 0);
}

esp_err_t nvs_read_int32(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_I32, out_value, 0);
}

esp_err_t nvs_read_uint32(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_U32, out_value, 0);
}

esp_err_t nvs_read_int64(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_I64, out_value, 0);
}

esp_err_t nvs_read_uint64(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_U64, out_value, 0);
}

esp_err_t nvs_read_string(const char *namespace, const char *key, void *out_value)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_STR, out_value, 0);
}

esp_err_t nvs_read_float(const char *namespace, const char *key, void *out_value)
{
    char *string = NULL;
    esp_err_t err = esp32_nvs_read(namespace, key, NVS_TYPE_STR, &string, 0);
    if (err == ESP_OK) {
        *(float*)out_value = strtof(string, NULL);
        free(string);
    }
    return err;
}

esp_err_t nvs_read_double(const char *namespace, const char *key, void *out_value)
{
    char *string = NULL;
    esp_err_t err = esp32_nvs_read(namespace, key, NVS_TYPE_STR, &string, 0);
    if (err == ESP_OK) {
        *(double*)out_value = strtod(string, NULL);
        free(string);
    }
    return err;
}

esp_err_t nvs_read_blob(const char *namespace, const char *key, void *out_value, size_t length)
{
    return esp32_nvs_read(namespace, key, NVS_TYPE_BLOB, out_value, length);
}
