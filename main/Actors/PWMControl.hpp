#ifndef MY_PWM_CONTROL_HPP
#define MY_PWM_CONTROL_HPP

#include <driver/ledc.h>

namespace 
{
    constexpr uint8_t k_pwm_frequency = 50; //Hz
    constexpr ledc_timer_bit_t k_tmrResolution = LEDC_TIMER_14_BIT;
    constexpr uint32_t k_pwmMaxDuty = 1 << LEDC_TIMER_14_BIT;
    constexpr ledc_mode_t k_ledcSpeed = LEDC_LOW_SPEED_MODE;
    constexpr uint32_t k_period = 1000000 / k_pwm_frequency;
};


class PWMControl
{
private:
    ledc_timer_t m_tmr;
    ledc_channel_t m_channel;
    gpio_num_t m_pin;
    uint8_t m_dutyPct;


public:
    PWMControl(gpio_num_t pin, ledc_timer_t tmr=LEDC_TIMER_0, ledc_channel_t channel=LEDC_CHANNEL_0) : m_tmr{tmr}, m_channel{channel}, m_pin{pin}
    {

    }

    esp_err_t init()
    {
        // ESP_LOGI("servoREAL", "INITING");
        gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);

        ledc_timer_config_t timer_conf = {};
        timer_conf.duty_resolution 	= k_tmrResolution;
        timer_conf.freq_hz    		= k_pwm_frequency;
        timer_conf.speed_mode 		= k_ledcSpeed;
        timer_conf.timer_num  		= m_tmr;
        timer_conf.clk_cfg          = LEDC_AUTO_CLK;
        esp_err_t err = ledc_timer_config(&timer_conf);

        if(err != ESP_OK)
            return err;

        ledc_channel_config_t ledc_conf = {};
        ledc_conf.channel		= m_channel;
        ledc_conf.duty			= 0;
        ledc_conf.gpio_num		= (int)m_pin;
        ledc_conf.intr_type		= LEDC_INTR_DISABLE;
        ledc_conf.speed_mode	= k_ledcSpeed;
        ledc_conf.timer_sel		= m_tmr;
        ledc_conf.hpoint        = 0;

        return ledc_channel_config(&ledc_conf);

    }

    void setDuty(uint8_t pct)
    {        
        ESP_LOGI("pwm", "setDuty: %u", pct);
        m_dutyPct = pct * k_pwmMaxDuty / 100;
        ESP_ERROR_CHECK(ledc_set_duty(k_ledcSpeed, m_channel, m_dutyPct));
	    ESP_ERROR_CHECK(ledc_update_duty(k_ledcSpeed, m_channel));
    }

    uint8_t getDuty()
    {
        return m_dutyPct;
    }

    ~PWMControl()
    {
        ledc_stop(k_ledcSpeed, m_channel, 0);
    }
};

#endif