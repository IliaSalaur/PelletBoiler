#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DynamicConfig.hpp"
#include "Listeners/AppCOnfigListener.hpp"
#include "Loaders/MockLoader.hpp"

#include "Display/LVGLDisplay.hpp"
#include "Display/ft5x06TouchDriver.hpp"
#include "Display/ili3941Driver.hpp"
#include "App.hpp"

#include "ui_handlers.hpp"

ESP_EVENT_DEFINE_BASE(CONTROL_EVENTS);

extern "C" void app_main(void)
{
    // auto nvsConfLoader{std::make_shared<NVSConfigLoader>("configs")};
    auto nvsConfLoader{std::make_shared<MockConfigLoader>(k_t{
        {"kLimitTemp","100"},
        {"kHysteresis", "5"},
        {"kTargetTemp", "60"},
        // {"kSmokeMaxTemp","0"},
        {"kActivePeriod","60"},
        {"kInterval", "60"},
        {"kPumpOnTemp", "15"},
        {"kPumpOffTemp","10"},
        {"kSafeModePower","20"},
        {"kPowerStep", "10"},
        {"kStepInterval","10"}
    })};
    // ESP_LOGI("main", "NVSConfigLoader init %s", nvsConfLoader->init() ? "success" : "failed");
    // nvsConfLoader->loadAllEntries();

    dynamicConfig.addConfig("kLimitTemp", nvsConfLoader);
    dynamicConfig.addConfig("kHysteresis", nvsConfLoader);
    dynamicConfig.addConfig("kTargetTemp", nvsConfLoader);
    // dynamicConfig.addConfig("kSmokeMaxTemp", nvsConfLoader);/

    dynamicConfig.addConfig("kActivePeriod", nvsConfLoader);      // For ScrewMotorManager
    dynamicConfig.addConfig("kInterval", nvsConfLoader);          // For ScrewMotorManager

    dynamicConfig.addConfig("kPumpOnTemp", nvsConfLoader);        // For App
    dynamicConfig.addConfig("kPumpOffTemp", nvsConfLoader);       // For App   
    dynamicConfig.addConfig("kSafeModePower", nvsConfLoader);       // For App   

    dynamicConfig.addConfig("kPowerStep", nvsConfLoader);         // For Fan
    dynamicConfig.addConfig("kStepInterval", nvsConfLoader);      // For Fan

    dynamicConfig.loadConfigs();

    App app{};
    dynamicConfig.attachListener(std::make_shared<AppConfigListener>(app));
    app.init();
    ui_registerHandler(&app);

    ILI3491Driver dispDriver{};
    ft5x06TouchDriver touchDriver{};

    dispDriver.init();
    touchDriver.init();

    LVGLDisplay display{&dispDriver, &touchDriver};
    display.init();

    app.start();

    while (1)
    {
        vTaskDelay(portMAX_DELAY);
    }
    
}