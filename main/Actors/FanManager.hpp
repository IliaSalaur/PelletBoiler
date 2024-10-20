#ifndef MY_FAN_Manager_HPP
#define MY_FAN_Manager_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Sensors/AbstractManager.hpp"
#include "PWMControl.hpp"
#include "task_common.hpp"

namespace{
    constexpr auto k_powerStep{5};
    constexpr auto k_stepInterval{5};    ///> in seconds
    // constexpr auto k_safeModePower{20};
};
enum class FanModeEnum{
    RISING = 0,
    FALLING,
    CONSTANT
};

class FanManager : public AbstractManager
{
private:
    using enum FanModeEnum;
    uint8_t m_powerPct;
    bool m_enabled;
    PWMControl m_pwmControl;
    uint32_t m_lastTickCount;
    uint8_t m_powerStep;
    size_t m_stepInterval;
    FanModeEnum m_opMode;

    void doManage() override
    {
        static uint8_t m_lastPower{};
        if(m_lastPower != m_powerPct)
        {
            m_lastPower = m_powerPct;
            event_data_t dutyData{float(m_powerPct)};
            m_eventDispatcher.dispatch({CONTROL_EVENTS, EventsEnum::FAN_DUTY_CHANGE}, dutyData);
        }        

        if(m_enabled && m_opMode != CONSTANT && xTaskGetTickCount() - m_lastTickCount > pdMS_TO_TICKS(m_stepInterval * 1000))
        {
            m_lastTickCount = xTaskGetTickCount();
            if(m_opMode == RISING)
                m_powerPct = m_powerStep + m_powerPct > 100 ? 100 : m_powerPct + m_powerStep;
            else if(m_opMode == FALLING)
                m_powerPct = m_powerStep < m_powerPct ? m_powerPct - m_powerStep : 0;
            m_pwmControl.setDuty(m_powerPct);            
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }

    void doTaskInit() override
    {
        m_pwmControl.init();
    }

public: 

    FanManager(gpio_num_t pin, const EventDispatcher& evDispatcher)
     : AbstractManager{evDispatcher},
     m_powerPct{},
     m_enabled{false},
     m_pwmControl{pin},
     m_lastTickCount{},
     m_powerStep{k_powerStep},
     m_stepInterval{k_stepInterval},
     m_opMode{}
    {

    }

    void setEnable(bool enabled)
    {
        m_enabled = enabled;

        m_pwmControl.setDuty(enabled ? m_powerPct : 0);      
        event_data_t data{enabled};
        m_eventDispatcher.dispatch({CONTROL_EVENTS, EventsEnum::FAN_ENABLE_CHANGE}, data);
    }

    void setPower(uint8_t power)
    {
        m_powerPct = power;
        m_pwmControl.setDuty(m_enabled ? m_powerPct : 0);
    }

    void setConstantPower(uint8_t power)
    {
        m_opMode = CONSTANT;
        setPower(power);
    }

    void setMode(FanModeEnum mode)
    {
        assert(mode != CONSTANT);
        m_opMode = mode;
        // m_powerPct = 0;
        m_lastTickCount = 0;
    }

    void setPowerStep(uint8_t powerStep)
    {
        m_powerStep = powerStep;
    }

    void setStepInterval(size_t stepInterval)
    {
        m_stepInterval = stepInterval;        
    }

    uint8_t getPower()
    {
        return m_powerPct;
    }

    bool isEnabled()
    {
        return m_enabled;
    }

    ~FanManager() {}
};

#endif