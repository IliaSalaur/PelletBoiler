#ifndef MY_EVENTS_HPP
#define MY_EVENTS_HPP

#include <cstring>
#include "EspEvent.hpp"
#include <variant>
#include <array>
#include <string_view>

#define LOG_EVENT_DEF_ENABLE
#ifdef LOG_EVENT_DEF_ENABLE
#include "esp_log.h"
#define LOG_EVENTDEF(x...) ESP_LOGI("event-def", x)
#else
#define LOG_EVENTDEF(x...)
#endif

ESP_EVENT_DECLARE_BASE(CONTROL_EVENTS);

/* Events:
    TEMP_CHANGED -> on every temp change, for LVGL
    TEMP_OVER_MAX -> temp >= t0 + hysteresis
    TEMP_OVER_MIN -> temp <= t0 - hysteresis
    TEMP_OVER_LIMIT -> temp dangerously high
    
    
*/

enum class EventsEnum : int32_t
{
    TEMP_CHANGED = 0,
    TEMP_OVER_MAX,
    TEMP_BELOW_MIN,
    TEMP_OVER_LIMIT,
    SMOKE_TEMP_OVER_LIMIT,
    FIRE_DETECTED,
    FAN_DUTY_CHANGE,
    FAN_ENABLE_CHANGE,
    PUMP_STATE_CHANGE,
    MOTOR_STATE_CHANGE
};

const char* EventMap[]{
    "TEMP_CHANGED",
    "TEMP_OVER_MAX",
    "TEMP_BELOW_MIN",
    "TEMP_OVER_LIMIT",
    "SMOKE_TEMP_OVER_LIMIT",
    "FIRE_DETECTED",
    "FAN_DUTY_CHANGE",
    "FAN_ENABLE_CHANGE",
    "PUMP_STATE_CHANGE",
    "MOTOR_STATE_CHANGE"
};

enum class EvDataTypesEnum
{
    NO_TYPE = 0,
    FLOAT,
    BOOL,
    STR
};

// using event_data_t = std::variant<int, char[32]>;

class event_data_t
{
private:
    EvDataTypesEnum type{};

public:
    union
    {
        bool boolean;
        float num{};
        char str[8];
    };

    event_data_t() : type{EvDataTypesEnum::NO_TYPE}, num{0} {}

    event_data_t(std::string_view strView) : type{EvDataTypesEnum::STR}, str{}
    {
        strView.copy(str, 8);
    }

    event_data_t(bool boolean_) : type{EvDataTypesEnum::BOOL}, boolean{boolean_}
    {

    }

    event_data_t(float numData) : type{EvDataTypesEnum::FLOAT}, num{numData}
    {
    }



    EvDataTypesEnum getType() const
    {
        return type;
    }
};

// struct EventDefinition
// {
//     time_t timestamp{};
//     int general_id{};
//     EventsEnum event_type{};
//     event_data_t result{};
// };

// namespace EventUtils
// {
//     std::array<std::pair<EventsEnum, std::string_view>, 13> EventTypeMaping{
//         std::pair{EventsEnum::ACK, "ACK"},
//         std::pair{EventsEnum::MOTION, "MOTION"},
//         std::pair{EventsEnum::WEIGHT_DETECTED, "WEIGHT_DETECTED"},
//         std::pair{EventsEnum::CAT_IN, "CAT_IN"},
//         std::pair{EventsEnum::WEIGHT, "WEIGHT"},
//         std::pair{EventsEnum::PHOTO, "PHOTO"},
//         std::pair{EventsEnum::CAT_OUT, "CAT_OUT"},
//         std::pair{EventsEnum::FDX, "FDX"},
//         std::pair{EventsEnum::FDX_IN_DB, "FDX_IN_DB"},
//         std::pair{EventsEnum::CATCH_REQUESTED, "CATCH_REQUESTED"},
//         std::pair{EventsEnum::CATCH_ABORTED, "CATCH_ABORTED"},
//         std::pair{EventsEnum::CATCH_ALLOWED, "CATCH_ALLOWED"},
//         std::pair{EventsEnum::DOOR_CLOSED, "DOOR_CLOSED"}};
// };

// NLOHMANN_JSON_NAMESPACE_BEGIN
// template <>
// struct adl_serializer<EventDefinition>
// {
//     static void to_json(json &j, const EventDefinition &ed)
//     {
//         j = json{
//             {"timestamp", ed.timestamp},
//             {"general-id", ed.general_id},
//             {"event-type", ed.event_type}};
//         switch (ed.result.getType())
//         {
//         case EvDataTypesEnum::NO_TYPE:
//             j["result"] = nullptr;
//             break;
//         case EvDataTypesEnum::INT:
//             j["result"] = ed.result.num;
//             break;
//         case EvDataTypesEnum::STR:
//             j["result"] = ed.result.str;
//             break;
//         }
//     }
//     static void from_json(const json &j, EventDefinition &ed)
//     {
//         // using namespace EventUtils;
//         j.at("timestamp").get_to(ed.timestamp);
//         j.at("general-id").get_to(ed.general_id);
//         j.at("event-type").get_to(ed.event_type);
//         auto res = j.at("result");
//         switch (res.type())
//         {
//         case json::value_t::string:
//             ed.result = event_data_t(j.value("result", std::string{}));
//             break;
//         case json::value_t::number_integer:
//             ed.result = event_data_t(res.get<int>());
//             break;
//         default:
//             break;
//         }
//     }
// };
// NLOHMANN_JSON_NAMESPACE_END

// NLOHMANN_JSON_NAMESPACE_BEGIN
// template <>
// struct adl_serializer<EventsEnum>
// {
//     static void to_json(nlohmann::json &j, const EventsEnum &eventType)
//     {
//         using namespace EventUtils;
//         const auto it = std::find_if(std::begin(EventTypeMaping), std::end(EventTypeMaping), [&](const auto &el)
//                                      { return el.first == eventType; });
//         if (it != std::end(EventTypeMaping))
//         {
//             j = it->second;
//         }
//     }

//     static void from_json(const nlohmann::json &j, EventsEnum &eventType)
//     {
//         using namespace EventUtils;
//         const auto it = std::find_if(std::begin(EventTypeMaping), std::end(EventTypeMaping), [&](const auto &el)
//                                      { return el.second == j.get<std::string_view>(); });
//         if (it != std::end(EventTypeMaping))
//         {
//             eventType = it->first;
//         }
//     }
// };
// NLOHMANN_JSON_NAMESPACE_END
#endif