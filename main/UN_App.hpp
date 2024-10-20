// #ifndef MY_APP_HPP
// #define MY_APP_HPP

// #include "Events/Events.hpp"
// #include "Sensors/TempSensorManager.hpp"
// #include "Sensors/FireDetectorManager.hpp"
// #include "Actors/FanManager.hpp"
// #include "Actors/ScrewMotorManager.hpp"
// #include "Actors/Relay.hpp"

// #include "Listeners/AppConfigListener.hpp"
// #include "Listeners/FanManagerListener.hpp"
// #include "Listeners/ScrewMotorManagerConfigListener.hpp"
// #include "Listeners/TempManagerListener.hpp"

// namespace {
//     constexpr auto k_safeModePower{20};
//     constexpr auto k_boilerTempADC{ADC_CHANNEL_4};
//     constexpr auto k_smokeTempADC{ADC_CHANNEL_8};
//     constexpr auto k_ldrADC{ADC_CHANNEL_6};
//     constexpr auto k_fanPin{gpio_num_t::GPIO_NUM_39};
//     constexpr auto k_pumpPin{gpio_num_t::GPIO_NUM_40};
//     constexpr auto k_screwMotorPin{gpio_num_t::GPIO_NUM_41};
//     constexpr auto k_pumpOnTemp{15.0f};
//     constexpr auto k_pumpOffTemp{5.0f};
// }

// class App {
// private:
//     EventLoop m_evLoop;
//     TempSensorManager m_tempSensorManager;
//     FireDetectorManager m_fireDetectorManager;
//     ScrewMotorManager m_screwMotorManager;
//     FanManager m_fanManager;
//     Relay m_pump;
//     float m_pumpOffTemp;
//     float m_pumpOnTemp;
//     uint8_t m_safeModePower;

//     static void _handler(void* event_handler_arg,
//                          esp_event_base_t event_base,
//                          int32_t event_id,
//                          void* dataPtr);

// public:
//     App();
//     void init();
//     void setSafeModePower(uint8_t safeModePower);
//     void setPumpOffTemp(float temp);
//     void setPumpOnTemp(float temp);
//     ~App();
// };

// #endif // MY_APP_HPP
