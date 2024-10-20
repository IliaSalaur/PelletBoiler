// #include "App.hpp"

// #include "DynamicConfig.hpp"
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

// // Static member function to handle events
// void App::_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* dataPtr) {
//     auto& app = *static_cast<App*>(event_handler_arg);
//     event_data_t evData = dataPtr ? *static_cast<event_data_t*>(dataPtr) : event_data_t{};
//     ESP_LOGI("event", "Event: %s; temp: %f", EventMap[event_id], evData.getType() != EvDataTypesEnum::FLOAT ? -228.0f : evData.num);

//     using enum EventsEnum;
//     switch (EventsEnum(event_id)) {
//         case TEMP_CHANGED:
//             if (evData.num > app.m_pumpOnTemp)
//                 app.m_pump.setState(true);
//             else if (evData.num < app.m_pumpOffTemp)
//                 app.m_pump.setState(false);
//             break;

//         case TEMP_BELOW_MIN:
//             app.m_fanManager.setMode(FanModeEnum::RISING);
//             break;

//         case TEMP_OVER_MAX:
//             app.m_fanManager.setMode(FanModeEnum::FALLING);
//             break;

//         case TEMP_OVER_LIMIT:
//         case SMOKE_TEMP_OVER_LIMIT:
//             app.m_fanManager.setConstantPower(app.m_safeModePower);
//             app.m_screwMotorManager.setEnable(false);
//             app.m_screwMotorManager.setState(false);
//             break;

//         default:
//             break;
//     }
// }

// // Constructor
// App::App()
//     : m_evLoop{},
//       m_tempSensorManager{k_boilerTempADC, k_smokeTempADC, EventDispatcher{m_evLoop}},
//       m_fireDetectorManager{k_ldrADC, EventDispatcher{m_evLoop}},
//       m_screwMotorManager{k_screwMotorPin, EventDispatcher{m_evLoop}},
//       m_fanManager{k_fanPin, EventDispatcher{m_evLoop}},
//       m_pump{k_pumpPin},
//       m_pumpOffTemp{k_pumpOffTemp},
//       m_pumpOnTemp{k_pumpOnTemp},
//       m_safeModePower{k_safeModePower} {}

// // Initialize the App
// void App::init() {
//     m_evLoop.registerHandler({CONTROL_EVENTS, ESP_EVENT_ANY_ID}, App::_handler, this);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     m_tempSensorManager.init();
//     m_fireDetectorManager.init();
//     m_screwMotorManager.init();
//     m_fanManager.init();
//     m_pump.init();

//     m_fanManager.setEnable(true);
//     m_screwMotorManager.setEnable(true);

//     // Attach configuration listeners
//     dynamicConfig.attachListener(std::make_shared<FanManagerConfigListener>(m_fanManager));
//     dynamicConfig.attachListener(std::make_shared<ScrewMotorManagerConfigListener>(m_screwMotorManager));
//     dynamicConfig.attachListener(std::make_shared<TempManagerConfigListener>(m_tempSensorManager));
// }

// // Set the safe mode power
// void App::setSafeModePower(uint8_t safeModePower) {
//     m_safeModePower = safeModePower;
// }

// // Set the pump off temperature
// void App::setPumpOffTemp(float temp) {
//     m_pumpOffTemp = temp;
// }

// // Set the pump on temperature
// void App::setPumpOnTemp(float temp) {
//     m_pumpOnTemp = temp;
// }

// // Destructor
// App::~App() {}
