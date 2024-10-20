#ifndef MY_APP_HPP
#define MY_APP_HPP

#include "Events/Events.hpp"
#include "Sensors/TempSensorManager.hpp"
#include "Sensors/FireDetectorManager.hpp"
#include "Actors/FanManager.hpp"
#include "Actors/ScrewMotorManager.hpp"
#include "Actors/Relay.hpp"

#include "Listeners/FanManagerListener.hpp"
#include "Listeners/ScrewMotorManagerConfigListener.hpp"
#include "Listeners/TempManagerListener.hpp"

namespace {
    constexpr auto k_safeModePower{20};
    constexpr auto k_boilerTempADC{ADC_CHANNEL_4};
    constexpr auto k_smokeTempADC{ADC_CHANNEL_8};
    constexpr auto k_ldrADC{ADC_CHANNEL_6};
    constexpr auto k_fanPin{gpio_num_t::GPIO_NUM_39};
    constexpr auto k_pumpPin{gpio_num_t::GPIO_NUM_40};
    constexpr auto k_screwMotorPin{gpio_num_t::GPIO_NUM_41};
    constexpr auto k_pumpOnTemp{15.0f};
    constexpr auto k_pumpOffTemp{5.0f};

};

class App
{
private:
    EventLoop m_evLoop;
    TempSensorManager m_tempSensorManager;
    FireDetectorManager m_fireDetectorManager;
    ScrewMotorManager m_screwMotorManager;
    FanManager m_fanManager;
    Relay m_pump;
    float m_pumpOffTemp;
    float m_pumpOnTemp;
    uint8_t m_safeModePower;

    static void _handler(void* event_handler_arg,
                                        esp_event_base_t event_base,
                                        int32_t event_id,
                                        void* dataPtr)
    {
        

        auto& app = *static_cast<App*>(event_handler_arg);
        event_data_t evData = dataPtr ? *static_cast<event_data_t*>(dataPtr) :event_data_t{};
        ESP_LOGI("event", "Event: %s; temp: %f", EventMap[event_id], evData.getType() != EvDataTypesEnum::FLOAT ? -228.0f : evData.num);

        using enum EventsEnum;
        switch (EventsEnum(event_id))
        {
        case TEMP_CHANGED:
            if(evData.num > k_pumpOnTemp)
                app.m_pump.setState(true);
            else if(evData.num < k_pumpOffTemp)
                app.m_pump.setState(false);

            {
                event_data_t pumpState{app.m_pump.getState()};
                app.m_evLoop.post({CONTROL_EVENTS, PUMP_STATE_CHANGE}, &pumpState, sizeof(event_data_t));
            }
            break;

        case TEMP_BELOW_MIN:
            app.m_fanManager.setMode(FanModeEnum::RISING);
            break;

        case TEMP_OVER_MAX:
            app.m_fanManager.setMode(FanModeEnum::FALLING);
            break;

        case TEMP_OVER_LIMIT:
            app.m_fanManager.setConstantPower(app.m_safeModePower);
            app.m_screwMotorManager.setEnable(false);
            app.m_screwMotorManager.setState(false);
            break;

        case SMOKE_TEMP_OVER_LIMIT:
            app.m_fanManager.setConstantPower(app.m_safeModePower);
            app.m_screwMotorManager.setEnable(false);
            app.m_screwMotorManager.setState(false);
            break;
        
        default:
            break;
        }
    }
public:
    App() 
      : m_evLoop{},
      m_tempSensorManager{k_boilerTempADC, k_smokeTempADC, EventDispatcher{m_evLoop}},
      m_fireDetectorManager{k_ldrADC, EventDispatcher{m_evLoop}},
      m_screwMotorManager{k_screwMotorPin, EventDispatcher{m_evLoop}},
      m_fanManager{k_fanPin, EventDispatcher{m_evLoop}},
      m_pump{k_pumpPin},
      m_pumpOffTemp{k_pumpOffTemp},
      m_pumpOnTemp{k_pumpOnTemp},
      m_safeModePower{k_safeModePower}
    {
        
    }

    void init()
    {
        m_evLoop.registerHandler({CONTROL_EVENTS, ESP_EVENT_ANY_ID}, App::_handler, this);
        vTaskDelay(pdMS_TO_TICKS(100));
        m_tempSensorManager.init();
        m_fireDetectorManager.init();
        m_screwMotorManager.init();
        m_fanManager.init();
        m_pump.init();
                
        dynamicConfig.attachListener(std::make_shared<FanManagerConfigListener>(m_fanManager));
        dynamicConfig.attachListener(std::make_shared<ScrewMotorManagerConfigListener>(m_screwMotorManager));
        dynamicConfig.attachListener(std::make_shared<TempManagerConfigListener>(m_tempSensorManager));
    }

    void start()
    {
        m_tempSensorManager.start();
        m_fireDetectorManager.start();
        m_fanManager.start();
        m_screwMotorManager.start();

        m_fanManager.setEnable(true);
        m_screwMotorManager.setEnable(true);
    }

    void setSafeModePower(uint8_t safeModePower)
    {
        m_safeModePower = safeModePower;
    }

    void setPumpOffTemp(float temp)
    {
        m_pumpOffTemp = temp;
    }

    void setPumpOnTemp(float temp)
    {
        m_pumpOnTemp = temp;
    }

    EventLoop getEventLoop()
    {
        return m_evLoop;
    }

    Relay& getPump()
    {
        return m_pump;
    }

    ScrewMotorManager& getScrewMotorManager()
    {
        return m_screwMotorManager;
    }

    FanManager& getFanManager()
    {
        return m_fanManager;
    }

    ~App() {}
};

#endif