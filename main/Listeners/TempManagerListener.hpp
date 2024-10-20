#ifndef MY_TEMPMANAGER_CONFIG_LISTENER_HPP
#define MY_TEMPMANAGER_CONFIG_LISTENER_HPP

#include "AbstractListener.hpp"
#include "Sensors/TempSensorManager.hpp"

class TempManagerConfigListener : public AbstractListener
{
private:
    TempSensorManager& m_man;
    static constexpr const char* tag{"tempManagerListener"};
public:
    TempManagerConfigListener(TempSensorManager& man) : AbstractListener{"kLimitTemp", "kHysteresis", "kTargetTemp", "kSmokeMaxTemp"}, m_man{man}{}

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        ESP_LOGI(tag, "val updated, |%s|:|%s|", cfgName.c_str(), newValue.c_str());
        if(cfgName == "kLimitTemp")
        {
            m_man.setLimitTemp(std::stof(newValue));
        }
        else if(cfgName == "kHysteresis")
        {
            m_man.setHysteresisTemp(std::stof(newValue));
        }
        else if(cfgName == "kTargetTemp")
        {
            m_man.setTargetTemp(std::stof(newValue));
        }
        else if(cfgName == "kSmokeMaxTemp")
        {
            m_man.setSmokeMaxTemp(std::stof(newValue));
        }
    }
};

#endif