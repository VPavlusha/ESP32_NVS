/*
 * MIT License
 * 
 * Copyright (c) 2023 Volodymyr Pavlusha
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef NON_VOLATILE_STORAGE_H_
#define NON_VOLATILE_STORAGE_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the default NVS partition
 *
 * @return
 *         - ESP_OK if storage was successfully initialized.
 *         - ESP_ERR_NVS_NO_FREE_PAGES if the NVS storage contains no empty pages (which may happen if NVS partition was truncated).
 *         - ESP_ERR_NOT_FOUND if no partition with label “nvs” is found in the partition table.
 *         - ESP_ERR_NO_MEM in case memory could not be allocated for the internal structures.
 *         - One of the error codes from the underlying flash storage driver.
 *         - Error codes from nvs_flash_read_security_cfg API (when “NVS_ENCRYPTION” is enabled).
 *         - Error codes from nvs_flash_generate_keys API (when “NVS_ENCRYPTION” is enabled).
 *         - Error codes from nvs_flash_secure_init_partition API (when “NVS_ENCRYPTION” is enabled).
 */
esp_err_t nvs_init(void);

/**
 * @brief Write int8_t, uint8, int16... value for given key
 *
 * @param[in] namespace Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn’t be empty.
 * @param[in] key Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn’t be empty.
 * @param[in] value The value to set.
 * @return
 *         - ESP_OK if value was set successfully.
 *         - ESP_FAIL if there is an internal error; most likely due to corrupted NVS partition (only if NVS assertion checks are disabled).
 *         - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL.
 *         - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only.
 *         - ESP_ERR_NVS_INVALID_NAME if key name doesn’t satisfy constraints.
 *         - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the underlying storage to save the value.
 *         - ESP_ERR_NVS_REMOVE_FAILED if the value wasn’t updated because flash write operation has failed. The value was written however, and update will be finished after re-initialization of nvs, provided that flash operation doesn’t fail again.
 */
esp_err_t nvs_write_int8(const char *namespace, const char *key, int8_t value);
esp_err_t nvs_write_uint8(const char *namespace, const char *key, uint8_t value);
esp_err_t nvs_write_int16(const char *namespace, const char *key, int16_t value);
esp_err_t nvs_write_uint16(const char *namespace, const char *key, uint16_t value);
esp_err_t nvs_write_int32(const char *namespace, const char *key, int32_t value);
esp_err_t nvs_write_uint32(const char *namespace, const char *key, uint32_t value);
esp_err_t nvs_write_int64(const char *namespace, const char *key, int64_t value);
esp_err_t nvs_write_uint64(const char *namespace, const char *key, uint64_t value);
esp_err_t nvs_write_string(const char *namespace, const char *key, const char *value);
esp_err_t nvs_write_float(const char *namespace, const char *key, float value);
esp_err_t nvs_write_double(const char *namespace, const char *key, double value);
esp_err_t nvs_write_blob(const char *namespace, const char *key, const void *value, size_t length);

/**
 * @brief Read int8_t, uint8, int16... value for given key
 *
 * @param[in]  namespace Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn’t be empty.
 * @param[in]  key Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn’t be empty.
 * @param[out] out_value Pointer to the output value. May be NULL for nvs_get_str and nvs_get_blob, in this case required length will be returned in length argument.
 * @return
 *         - ESP_OK if the value was retrieved successfully.
 *         - ESP_FAIL if there is an internal error; most likely due to corrupted NVS partition (only if NVS assertion checks are disabled).
 *         - ESP_ERR_NVS_NOT_FOUND if the requested key doesn’t exist.
 *         - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL.
 *         - ESP_ERR_NVS_INVALID_NAME if key name doesn’t satisfy constraints.
 *         - ESP_ERR_NVS_INVALID_LENGTH if length is not sufficient to store data.
 */
esp_err_t nvs_read_int8(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_uint8(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_int16(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_uint16(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_int32(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_uint32(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_int64(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_uint64(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_string(const char *namespace, const char *key, void *out_value);  // Please see an example below.
esp_err_t nvs_read_float(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_double(const char *namespace, const char *key, void *out_value);
esp_err_t nvs_read_blob(const char *namespace, const char *key, void *out_value, size_t length);

// IMPORTANT NOTE!: This applies ONLY to strings. Remember to delete the pointer to avoid a memory leak.
/* For example:

    char *read_string = NULL;
    if (nvs_read_string("namespace_1", "key_2", &read_string) == ESP_OK) {
        ESP_LOGI(TAG, "Successfully read string from NVS: %s", read_string);
    } else {
        ESP_LOGE(TAG, "Failed to read string from NVS");
    }
    // IMPORTANT NOTE!: This applies ONLY to strings. Remember to delete the pointer to avoid a memory leak.
    free(read_string);

*/

#ifdef __cplusplus
}
#endif

#endif  // NON_VOLATILE_STORAGE_H_
