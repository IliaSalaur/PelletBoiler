#ifndef MY_I_TOUCH_DRIVER_HPP
#define MY_I_TOUCH_DRIVER_HPP

#include "lvgl.h"

class ITouchDriver
{
protected:
    static ITouchDriver* m_instance;
    virtual void doRead(lv_indev_drv_t* drv, lv_indev_data_t* data) = 0;
public:
    ITouchDriver() 
    {
        m_instance = this;
    }

    virtual esp_err_t init() = 0;    
    static void read(_lv_indev_drv_t* drv, lv_indev_data_t* data)
    {
        m_instance->doRead(drv, data);
    }

    ~ITouchDriver() {}
};

ITouchDriver* ITouchDriver::m_instance = nullptr;

#endif