#ifndef MY_ADC_HPP
#define MY_ADC_HPP

#include "esp_adc/adc_oneshot.h"

namespace{
    constexpr auto k_adcBitwidth{ADC_BITWIDTH_12};
    constexpr auto k_adcUnitID{ADC_UNIT_1};
    constexpr auto k_maxADCVoltage{3300};          //> in mV
    constexpr auto k_supplyVoltage{3300};                   //> inmV
};

class ADC
{
private:
    static constexpr adc_unit_t m_unit{ADC_UNIT_1};
    static adc_oneshot_unit_handle_t m_adcHandle;
    adc_channel_t m_adcChannel;

    static void _install()
    {
        static bool installed = false;
        if(!installed)
        {
            adc_oneshot_unit_init_cfg_t unit_cfg{
            .unit_id = m_unit,
            .ulp_mode = ADC_ULP_MODE_DISABLE
            };
            ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &m_adcHandle));
            installed = true;
        }
    }

public:
    ADC(adc_channel_t channel) : m_adcChannel{channel}
    {  
        
    }

    void init()
    {
        _install();

        adc_oneshot_chan_cfg_t init_cfg{
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = k_adcBitwidth
        };

        ESP_ERROR_CHECK(adc_oneshot_config_channel(m_adcHandle, m_adcChannel, &init_cfg));
    }

    int32_t read()
    {
        int raw;
        ESP_ERROR_CHECK(adc_oneshot_read(m_adcHandle, m_adcChannel, &raw));
        return raw;
    }
};

adc_oneshot_unit_handle_t ADC::m_adcHandle{nullptr};

#endif
