#ifndef MY_NTC_SENSOR_HPP
#define MY_NTC_SENSOR_HPP

#include <cmath>
#include "ADC.hpp"
#include "esp_log.h"

namespace{
    constexpr float k_beta{3977.0};
    constexpr float k_T0_ref{298.15};
    constexpr float k_R0_ref{10000.0};
    constexpr float k_RD{10000.0};
};

class NTCSensor
{
private:
    ADC m_adc;

    int _rawToMV(int raw)
    {
        return raw * k_maxADCVoltage / (1 << k_adcBitwidth);
    }

    float _getResistance(int mV)
    {
        //u1 / u2 = r1 / r2;
        //r1 = r2 * u1 / u2; u2 = k_maxVoltage - u1;
        //

        //uges / rges = u1 / r1;

        //r1 = rges * u1 / uges      
        return (float)(k_RD * mV) / (float)(k_supplyVoltage - mV);
    }

public:
    NTCSensor(adc_channel_t adcChannel)
        : 
        m_adc{adcChannel}
    {
        
    }

    void init()
    {
        m_adc.init();
    }

    float readTemp()
    {        
        int u1{_rawToMV(m_adc.read())};

        float r = _getResistance(u1);

        float temperature = 1.0 / ((1.0 / k_T0_ref) + (1.0 / k_beta) * log(r / k_R0_ref)) - 273.15;

        return temperature;
    }
};

#endif