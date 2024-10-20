#ifndef MY_NTC_SENSOR_MANAGER_HPP
#define MY_NTC_SENSOR_MANAGER_HPP

#include "AbstractManager.hpp"
#include "NTCSensor.hpp"

namespace{
    constexpr float k_limitTemp{78.0};
    constexpr float k_hysteresis{5.0};
    constexpr float k_targetTemp{50.0};
    constexpr float k_smokeMaxTemp{78.0};
};

class TempSensorManager : public AbstractManager
{
private:
    NTCSensor m_boilerNtc;
    NTCSensor m_smokeNtc;
    float m_lastTemp;
    float m_hysteresis;
    float m_targetTemp;
    float m_limitTemp;
    float m_smokeMaxTemp;
    bool m_targetTempAchieved;

    void doManage() override
    {
        m_boilerNtc.readTemp();
        vTaskDelay(pdMS_TO_TICKS(500));
        float temp = m_boilerNtc.readTemp();
        float smokeTemp = m_smokeNtc.readTemp();

        event_data_t smokeTempData{smokeTemp};
        if(smokeTemp >= m_smokeMaxTemp)
        {
            // ESP_LOGI("temp", "Smoke: %f", smokeTemp);
            m_eventDispatcher.dispatch({CONTROL_EVENTS, EventsEnum::SMOKE_TEMP_OVER_LIMIT}, smokeTempData);
        }
            

        if(temp == m_lastTemp)
            return;

        m_lastTemp = temp;
        using enum EventsEnum;
        event_data_t tempData{temp};

        if(temp <= m_targetTemp - m_hysteresis && m_targetTempAchieved)
        {
            // ESP_LOGI("ntcMan", "temp below min");
            m_targetTempAchieved = false;
            m_eventDispatcher.dispatch({CONTROL_EVENTS, TEMP_BELOW_MIN}, tempData);
        }
        else if(temp >= m_limitTemp)
        {
            // ESP_LOGI("ntcMan", "temp above limit");
            m_eventDispatcher.dispatch({CONTROL_EVENTS, TEMP_OVER_LIMIT}, tempData);
        }
        else if(temp >= m_targetTemp + m_hysteresis)
        {
            // ESP_LOGI("ntcMan", "temp above max");
            m_targetTempAchieved = true;
            m_eventDispatcher.dispatch({CONTROL_EVENTS, TEMP_OVER_MAX}, tempData);
        }        

        m_eventDispatcher.dispatch({CONTROL_EVENTS, TEMP_CHANGED}, tempData);
    }

    void doTaskInit() override
    {
        m_boilerNtc.init();
        m_smokeNtc.init();
    }

public:
    TempSensorManager(adc_channel_t boilerNtcAdc, adc_channel_t smokeNtcAdc, const EventDispatcher& evDispatcher) 
        : AbstractManager{evDispatcher}, m_boilerNtc{boilerNtcAdc}, m_smokeNtc{smokeNtcAdc}, m_lastTemp{0}, m_hysteresis{k_hysteresis}, m_targetTemp{k_targetTemp}, m_limitTemp{k_limitTemp}, m_smokeMaxTemp{k_smokeMaxTemp}, m_targetTempAchieved{false}
    {

    }

    void setTargetTemp(float targetTemp)
    {
        m_targetTemp = targetTemp;
    }

    void setLimitTemp(float limitTemp)
    {
        m_limitTemp = limitTemp;
    }

    void setHysteresisTemp(float hysteresis)
    {
        m_hysteresis = hysteresis;
    }

    void setSmokeMaxTemp(float smokeMaxTemp)
    {
        m_smokeMaxTemp = smokeMaxTemp;
    }

    ~TempSensorManager() {}
};

#endif