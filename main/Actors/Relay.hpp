#ifndef MY_RELAY_HPP
#define MY_RELAY_HPP

#include <driver/gpio.h>

namespace{
    constexpr bool k_lowLevel{0};
    constexpr bool k_highLevel{!k_lowLevel};
}

class Relay
{
private:
    gpio_num_t m_pin;
    bool m_state;

public:
    Relay(gpio_num_t pin) : m_pin{pin}, m_state{k_lowLevel}
    {

    }

    void setState(bool state)
    {
        m_state = state;
        gpio_set_level(m_pin, m_state ? k_highLevel : k_lowLevel);
    }

    bool getState()
    {
        return m_state;
    }

    void init()
    {
        gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(m_pin, k_lowLevel);
    }

    ~Relay() {}
};

#endif