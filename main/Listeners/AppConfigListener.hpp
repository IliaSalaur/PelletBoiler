#ifndef MY_APP_CONFIG_LISTENER_HPP
#define MY_APP_CONFIG_LISTENER_HPP

#include "AbstractListener.hpp"
#include "../App.hpp"

// class App;

class AppConfigListener : public AbstractListener
{
private:
    App& m_app;
    static constexpr const char* tag{"appConfigListener"};

public:
    AppConfigListener(App& app)
        : AbstractListener{"kPumpOnTemp", "kPumpOffTemp", "kSafeModePower"}, m_app{app} {}

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        ESP_LOGI(tag, "Value updated, |%s|:|%s|", cfgName.c_str(), newValue.c_str());

        if (cfgName == "kPumpOnTemp")
        {
            m_app.setPumpOnTemp(std::stof(newValue));
        }
        else if (cfgName == "kPumpOffTemp")
        {
            m_app.setPumpOffTemp(std::stof(newValue));
        }
        else if (cfgName == "kSafeModePower")
        {
            m_app.setSafeModePower(std::stoul(newValue));
        }
    }
};

#endif
