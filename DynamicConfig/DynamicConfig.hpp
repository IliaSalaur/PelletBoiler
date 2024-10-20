#ifndef DYNAMIC_CONFIG_H
#define DYNAMIC_CONFIG_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <memory>

#include "Loaders/IConfigLoader.hpp"
#include "AbstractListener.hpp"
#include "esp_log.h"

class DynamicConfig
{
private:
    std::map<const std::string, std::string> m_cfgs;
    std::map<const std::string, std::shared_ptr<IConfigLoader>> m_loaders;

    std::vector<std::shared_ptr<AbstractListener>> m_listeners;

    // IConfigLoader* _loader;

    void _yieldListeners()
    {
        for(const auto& listener : m_listeners)
        {
            for(const auto& cfgName : listener->getListenedConfigs())
            {
                if(const auto& it = m_cfgs.find(cfgName); it != m_cfgs.end())
                {
                    listener->valueUpdated(cfgName, it->second);
                }
            }
        }
    }
public:
    DynamicConfig() : m_cfgs{}, m_loaders{}, m_listeners{}{}    

    void loadConfigs()
    {
        for(auto&[confName, confValue] : m_cfgs)
        {
            auto loadedVal = m_loaders[confName]->loadConfig(confName);
            if(loadedVal == std::nullopt)
            {
                continue;
            }

            confValue = *loadedVal;
        }

        this->_yieldListeners();
    }

    const std::string getConfig(const std::string& cfgName, const std::string& defaultValue="")
    {
        if(const auto it = m_cfgs.find(cfgName); it != m_cfgs.end()) 
            return it->second;

        return defaultValue;
    }

    /**
     * @brief Updates an exsiting config
     * 
     * @param cfgName config name
     * @param cfgValue value to be stored
     */
    void updateConfig(const std::string& cfgName, const std::string&& cfgValue)
    {
        // Return if value not updated
        if(const auto& it = m_cfgs.find(cfgName); it == m_cfgs.end() || it->second == cfgValue)
        {
            ESP_LOGI("config", "Unknown config / value not updated-> |%s|:|%s|", cfgName.c_str(), cfgValue.c_str());
            return;
        }            

        // yield listeners
        for(const auto& listener : m_listeners)
        {
            const auto& listenSet = listener->getListenedConfigs();
            if(listenSet.find(cfgName) != listenSet.end())  listener->valueUpdated(cfgName, cfgValue);
        }        

        // save config
        m_loaders[cfgName]->saveConfig({cfgName, cfgValue});

        // move cfgValue to internal dynamic config storage
        m_cfgs[cfgName] = std::move(cfgValue);
                         
    }

    bool addConfig(const std::string& cfgName, std::shared_ptr<IConfigLoader> loader, bool tryLoad = true)
    {
        if(const auto it = m_cfgs.find(cfgName); it != m_cfgs.end())
            return false;
                
        if(tryLoad)
        {
            auto val = loader->loadConfig(cfgName);
            m_cfgs.emplace(cfgName, val == std::nullopt ? "" : *val);
        } else{
            m_cfgs.emplace(cfgName, "");
        }

        m_loaders.emplace(std::move(cfgName), std::move(loader));

        return true;
    }

    const std::string& operator[](const std::string& cfgName){
        return this->getConfig(cfgName);
    }

    const std::map<const std::string, std::string>& getConfigs()
    {
        return this->m_cfgs;
    }

    void attachListener(std::shared_ptr<AbstractListener> listenerPtr)
    {
        if(!listenerPtr) return;
        
        m_listeners.push_back(listenerPtr);

        for(const auto& cfgName : listenerPtr->getListenedConfigs())
            {
                if(const auto& it = m_cfgs.find(cfgName); it != m_cfgs.end())
                {
                    listenerPtr->valueUpdated(cfgName, it->second);
                }
            }
    }

    void detachListener(std::shared_ptr<AbstractListener> listenerPtr)
    {
        m_listeners.erase(std::remove(std::begin(m_listeners), std::end(m_listeners), listenerPtr), std::end(m_listeners));
    }
} dynamicConfig;



#endif