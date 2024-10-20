#include <memory>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "nvs_handle.hpp"

#include "NVSConfigLoader.hpp"

const char* TAG = "nsv_conf_loader";
static constexpr size_t k_nvs_config_loader_buffer_size = 2048;

NVSConfigLoader::NVSConfigLoader(const std::string& ns) : m_localCfgs{}, m_nsName{ns}, m_hasChanges{false}
{
}

bool NVSConfigLoader::init()
{
    // Init NVS
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    return ret == ESP_OK;
}

bool NVSConfigLoader::loadAllEntries()
{
    esp_err_t err{ESP_OK};
    // Open the nvs handle and perform some error checking
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(m_nsName.c_str(), NVS_READONLY, &err);

    if(err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
        return false;
    }

    nvs_iterator_t it = NULL;
    err = nvs_entry_find("nvs", m_nsName.c_str(), NVS_TYPE_ANY, &it);
    ESP_LOGI(TAG, "Iterator opening: (%s)", esp_err_to_name(err));
    while(err == ESP_OK)
    {
        nvs_entry_info_t info{};
        // Get the entry' info and log
        err = nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL                
        
        // Get the item from nvs and then emplace in the configs map
        char buf[k_nvs_config_loader_buffer_size]{};
        handle->get_string(info.key, buf, k_nvs_config_loader_buffer_size);  

        ESP_LOGI(TAG, "key '%s', type '%d', value '%s' \n", info.key, info.type, buf);
        m_localCfgs.emplace(info.key, buf);

        err = nvs_entry_next(&it);
    }

    if(err != ESP_ERR_INVALID_ARG)
    {
        nvs_release_iterator(it);
        return true;
    }  

    return false;
}

std::optional<std::string> NVSConfigLoader::loadConfig(const std::string &cfgName)
{
    if(const auto it = m_localCfgs.find(cfgName); it != std::end(m_localCfgs))
    {
        return it->second;
    }

    return std::nullopt;
}

bool NVSConfigLoader::saveConfig(const std::pair<std::string, std::string> &config)
{    
    const auto&[confName, confValue] = config;
    ESP_LOGI(TAG, "saving config: |%s|:|%s|", confName.c_str(), confValue.c_str());

    m_hasChanges = true;
    
    if(auto it = m_localCfgs.find(confName); it != m_localCfgs.end())
    {
        it->second = confValue;
        return true;
    }

    m_localCfgs.insert(config);
    return true;
}

bool NVSConfigLoader::commit()
{
    if(!m_hasChanges)
        return ESP_OK;
    
    m_hasChanges = false;
    esp_err_t err{ESP_OK};
    // Open the nvs handle and perform some error checking
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(m_nsName.c_str(), NVS_READWRITE, &err);

    if(err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
        return err;
    }

    // Iterate through the map and save configs
    for (const auto&[confName, confValue] : m_localCfgs)
    {
        err = handle->set_string(confName.c_str(), confValue.c_str());
        if(err != ESP_OK)
        {
            ESP_LOGW(TAG, "Error (%s) saving the data, '%s':'%s'", esp_err_to_name(err), confName.c_str(), confValue.c_str());
            return err;
        }
    }

    err = handle->commit();
    if(err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) commiting the data", esp_err_to_name(err));
    }

    return err == ESP_OK;
}

NVSConfigLoader::~NVSConfigLoader() = default;