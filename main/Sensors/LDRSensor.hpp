#ifndef MY_LDR_SENSOR_HPP
#define MY_LDR_SENSOR_HPP

#include "ADC.hpp"

namespace{
    constexpr auto k_thereshold{30};               //> in mV
}

class LDRSensor
{
private:
    ADC m_adc;

    int _rawToMV(int raw)
    {
        return raw * k_maxADCVoltage / (1 << k_adcBitwidth);
    }

public:
    LDRSensor(adc_channel_t adcChannel)
        : 
        m_adc{adcChannel}
    {
        
    }

    void init()
    {
        m_adc.init();
    }

    bool read()
    {
        const auto mv{_rawToMV(m_adc.read())};
        // ESP_LOGI("adc", "%d", mv);
        return mv < k_thereshold;
    }
};

#endif