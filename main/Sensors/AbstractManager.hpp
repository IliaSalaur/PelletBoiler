#ifndef MY_ABSTRACT_MANAGER_HPP
#define MY_ABSTRACT_MANAGER_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Events/Events.hpp"
#include "task_common.hpp"

class AbstractManager
{
private:
    TaskHandle_t m_handle;     

    static void _task(void* pvParam)
    {
        auto& instance = *static_cast<AbstractManager*>(pvParam);
        instance.doTaskInit();

        vTaskSuspend(nullptr);

        while(1)
            instance.doManage();
    }

protected:
    const EventDispatcher m_eventDispatcher;   
    virtual void doManage() = 0;

    virtual void doTaskInit() = 0;
    
public:
    AbstractManager(const EventDispatcher evDispatcher)
     : m_eventDispatcher{evDispatcher}
    {

    }

    void init()
    {
        using namespace TaskCommon;
        xTaskCreatePinnedToCore(AbstractManager::_task, "manager", k_managerTaskStackSize, this, k_managerTaskPriority, &m_handle, k_managerTaskCoreID);
    }

    void start()
    {
        vTaskResume(m_handle);
    }

    ~AbstractManager() {
        vTaskDelete(m_handle);
    }
};

#endif