#ifndef MY_SCREW_MOTOR_MANAGER_HPP
#define MY_SCREW_MOTOR_MANAGER_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Sensors/AbstractManager.hpp"
#include "Relay.hpp"
#include "task_common.hpp"

namespace{
    constexpr auto k_activePeriod{5}; ///> in seconds
    constexpr auto k_interval{60};    ///> in seconds
    // constexpr auto k_safeModePower{20};
};

class ScrewMotorManager : public AbstractManager
{
private:
    bool m_enabled;
    bool m_state;
    Relay m_relay;
    int32_t m_lastTickCount;
    size_t m_activePeriod;
    size_t m_interval;

    void doManage() override
    {
        // ESP_LOGI("motor", "");
        if(m_enabled && (int32_t)xTaskGetTickCount() - m_lastTickCount > (!m_state ? pdMS_TO_TICKS(m_interval * 1000) : pdMS_TO_TICKS(m_activePeriod * 1000)))
        {
            // ESP_LOGI("motor", "state change %d", (int)m_state);
            m_lastTickCount = xTaskGetTickCount();
            m_state = !m_state;
            m_relay.setState(m_state);
            event_data_t data{m_state};
            m_eventDispatcher.dispatch({CONTROL_EVENTS, EventsEnum::MOTOR_STATE_CHANGE}, data);
        }        

        vTaskDelay(pdMS_TO_TICKS(200));
    }

    void doTaskInit() override
    {
        m_relay.init();
    }

public: 

    ScrewMotorManager(gpio_num_t pin, const EventDispatcher& evDispatcher)
     : AbstractManager{evDispatcher},
     m_enabled{false},
     m_state{true},
     m_relay{pin},
     m_lastTickCount{},
     m_activePeriod{k_activePeriod},
     m_interval{k_interval}
    {

    }

    void setInterval(size_t interval)
    {
        m_interval = interval;
    }

    void setActivePeriod(size_t activePeriod)
    {
        m_activePeriod = activePeriod;
    }

    void setEnable(bool enabled)
    {
        m_enabled = enabled;
        m_relay.setState(m_state);  
        event_data_t data{m_state};
        m_eventDispatcher.dispatch({CONTROL_EVENTS, EventsEnum::MOTOR_STATE_CHANGE}, data);
    }

    void setState(bool state)
    {
        m_state = state;
        m_relay.setState(state);
    }

    bool isEnabled()
    {
        return m_enabled;
    }

    ~ScrewMotorManager() {}
};

#endif