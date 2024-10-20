#ifndef MY_EVENT_SESSION_HPP
#define MY_EVENT_SESSION_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Events.hpp"
#include <vector>
#include <tuple>
#include <algorithm>
#include <execution>

class EventSession
{
private:
    const int m_generalId;
    std::vector<std::pair<EventsEnum, TickType_t>> m_eventQueue;
public:
    EventSession(int generalId)
     : m_generalId{generalId}, m_eventQueue{}
    {
        m_eventQueue.reserve(10);
    }

    void pushEvent(auto event, TickType_t _now = xTaskGetTickCount())
    {
        m_eventQueue.emplace_back(EventsEnum(event), _now);
    }

    bool hasEvent(EventsEnum event)
    {
        return std::find_if(m_eventQueue.begin(), m_eventQueue.end(), [=](auto& el){return el.first == event;}) != m_eventQueue.end();
    }

    bool isExpired(EventsEnum event, TickType_t timeout, TickType_t _now = xTaskGetTickCount())
    {
        auto const it = std::find_if(m_eventQueue.begin(), m_eventQueue.end(), [=](auto& el){return el.first == event;});
        if(it == std::end(m_eventQueue))
            return true;

        return it->second + timeout > _now;
    }

    bool hasActiveSequence(const std::vector<EventsEnum> sequence, TickType_t timeout)
    {
        auto now = xTaskGetTickCount();
        auto const it = std::search(
            std::execution::seq,
            std::begin(m_eventQueue),
            std::end(m_eventQueue),
            std::begin(sequence),
            std::end(sequence),
            [&](const auto& el1, const auto& el2)
            {
                return el1.first == el2 && now <= timeout + el1.second;
            }
        );

        return it != std::end(m_eventQueue);
    }

    bool hasSequence(const std::vector<EventsEnum> sequence)
    {
        auto const it = std::search(
            std::execution::seq,
            std::begin(m_eventQueue),
            std::end(m_eventQueue),
            std::begin(sequence),
            std::end(sequence),
            [&](const auto& el1, const auto& el2)
            {
                return el1.first == el2;
            }
        );

        return it != std::end(m_eventQueue);
    }

    void clearSequence()
    {
        m_eventQueue.clear();
    }

    int getGeneralId() const
    {
        return m_generalId;
    }

    ~EventSession()
    {

    }
};

#endif