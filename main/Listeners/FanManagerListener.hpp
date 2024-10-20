#ifndef MY_FAN_MANAGER_CONFIG_LISTENER_HPP
#define MY_FAN_MANAGER_CONFIG_LISTENER_HPP

#include "AbstractListener.hpp"
#include "Actors/FanManager.hpp"

class FanManagerConfigListener : public AbstractListener
{
private:
    FanManager& m_fanMan;
    static constexpr const char* tag{"fanManagerListener"};

public:
    FanManagerConfigListener(FanManager& fanMan)
        : AbstractListener{"kPowerStep", "kStepInterval"}, m_fanMan{fanMan} {}

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        ESP_LOGI(tag, "Value updated, |%s|:|%s|", cfgName.c_str(), newValue.c_str());

        if(cfgName == "kPowerStep")
        {
            m_fanMan.setPowerStep(static_cast<uint8_t>(std::stoi(newValue)));
        }
        else if(cfgName == "kStepInterval")
        {
            m_fanMan.setStepInterval(static_cast<size_t>(std::stoul(newValue)));
        }
    }
};

#endif
