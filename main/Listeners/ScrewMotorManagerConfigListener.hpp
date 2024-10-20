#ifndef MY_SCREW_MOTOR_MANAGER_CONFIG_LISTENER_HPP
#define MY_SCREW_MOTOR_MANAGER_CONFIG_LISTENER_HPP

#include "AbstractListener.hpp"
#include "Actors/ScrewMotorManager.hpp"

class ScrewMotorManagerConfigListener : public AbstractListener
{
private:
    ScrewMotorManager& m_screwMotorMan;
    static constexpr const char* tag{"screwMotorManagerListener"};

public:
    ScrewMotorManagerConfigListener(ScrewMotorManager& screwMotorMan)
        : AbstractListener{"kActivePeriod", "kInterval"}, m_screwMotorMan{screwMotorMan} {}

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        ESP_LOGI(tag, "Value updated, |%s|:|%s|", cfgName.c_str(), newValue.c_str());

        if(cfgName == "kActivePeriod")
        {
            m_screwMotorMan.setActivePeriod(static_cast<size_t>(std::stoul(newValue)));
        }
        else if(cfgName == "kInterval")
        {
            m_screwMotorMan.setInterval(static_cast<size_t>(std::stoul(newValue)));
        }
    }
};

#endif