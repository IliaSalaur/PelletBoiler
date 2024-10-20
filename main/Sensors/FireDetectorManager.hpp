#ifndef MY_FIRE_DETECTOR_MANAGER_HPP
#define MY_FIRE_DETECTOR_MANAGER_HPP

#include "AbstractManager.hpp"
#include "LDRSensor.hpp"


class FireDetectorManager : public AbstractManager
{
private:
    LDRSensor m_ldr;
    bool m_lastState{};

    void doManage() override
    {
        vTaskDelay(pdMS_TO_TICKS(200));
        bool fireDetected = m_ldr.read();
        if(fireDetected == m_lastState)
            return;

        using enum EventsEnum;
        event_data_t fireData{fireDetected}; 
        m_lastState = fireDetected;
        m_eventDispatcher.dispatch({CONTROL_EVENTS, FIRE_DETECTED}, fireData);
    }

    void doTaskInit() override
    {
        m_ldr.init();
    }

public:
    FireDetectorManager(adc_channel_t ldrSensorADC, const EventDispatcher& evDispatcher) 
        : AbstractManager{evDispatcher}, m_ldr{ldrSensorADC}, m_lastState{false}
    {

    }

    ~FireDetectorManager() = default;
};

#endif