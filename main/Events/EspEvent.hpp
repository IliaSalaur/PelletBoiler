#ifndef MY_ESP_EVENT_WRAPPER_HPP
#define MY_ESP_EVENT_WRAPPER_HPP

#include <functional>
#include <memory>
#include <vector>
#include <utility>
#include "esp_event.h"
#include "esp_log.h"
#include "Events.hpp"

struct Event{
    esp_event_base_t base{};
    int32_t id;

    Event(auto _base, auto _id) : base{_base}, id{int32_t(_id)}{}
};

extern const char*EventMap[];

class EventLoop{
private:    
    // using evt_cb = std::function<void(Event ev, void* data)>;  
    using evt_cb = esp_event_handler_t;  
    esp_event_loop_handle_t m_loopHandle;    

public:
    EventLoop() : m_loopHandle{}
    {
        esp_event_loop_args_t loop_args = {
            .queue_size = 10,
            .task_name = "eventloop",
            .task_priority = 5,
            .task_stack_size = 8192,
            .task_core_id = 0
        };    

        ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &m_loopHandle));
    }

    bool post(Event ev, void* data, size_t size, TickType_t timeout = 1) const
    {
        return esp_event_post_to(m_loopHandle, ev.base, ev.id, data, size, pdMS_TO_TICKS(100)) == ESP_OK;
    }

    void registerHandler(Event ev, evt_cb cb, void* handlerArg)
    {     
        esp_event_handler_register_with(m_loopHandle, ev.base, ev.id, cb, handlerArg);
    }

    esp_event_loop_handle_t getHandle() const
    {
        return m_loopHandle;
    }
};

class EventDispatcher
{
private:
    // const EventLoop& m_eventLoop;
    esp_event_loop_handle_t m_handle;    

public:
    EventDispatcher(EventLoop& evLoop) : m_handle{evLoop.getHandle()}
    // EventDispatcher(EventLoop& evLoop) : m_eventLoop{evLoop}
    {

    }

    template <typename T>
    bool dispatch(Event ev, T data) const
    {
        ESP_LOGI("event", "Event: %s; size: %u", EventMap[ev.id], sizeof(T));
        esp_err_t err = esp_event_post_to(m_handle, ev.base, ev.id, &data, sizeof(T), pdMS_TO_TICKS(100));
        ESP_ERROR_CHECK(err);
        return  err == ESP_OK;
        // return m_eventLoop.post(ev, data, size);
    }

    // bool dispatch(Event ev, void* data, size_t size) const
    // {
    //     ESP_ERROR_CHECK(esp_event_post_to(m_handle, ev.base, ev.id, data, size, pdMS_TO_TICKS(100)));
    //     return  0 == ESP_OK;
    //     // return m_eventLoop.post(ev, data, size);
    // }
};

#endif