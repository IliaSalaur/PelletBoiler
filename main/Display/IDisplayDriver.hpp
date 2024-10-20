#ifndef MY_I_DISPLAY_DRIVER_HPP
#define MY_I_DISPLAY_DRIVER_HPP

#include "lvgl.h"

class IDisplayDriver
{
public:
    using flush_done_cb = void(*)();
    IDisplayDriver() : m_flushDoneCb{}
    {
        m_instance = this;
    }

    virtual esp_err_t init() = 0;
    void setOnFlushDoneCB(flush_done_cb cb)
    {
        m_flushDoneCb = cb;
    }

    static void update(lv_disp_drv_t* drv)
    {
        // ESP_LOGI("disp", "update");
        m_instance->doUpdate(drv);
    }

    static void flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t *color_map)
    {
        // ESP_LOGI("disp", "flush");
        m_instance->doFlush(drv, area, color_map);        
        // ESP_LOGI("disp", "flushed");
    }

    ~IDisplayDriver() {}

protected:
    flush_done_cb m_flushDoneCb;
    static IDisplayDriver* m_instance;
    virtual void doFlush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t *color_map) = 0;
    virtual void doUpdate(lv_disp_drv_t* drv) = 0;
};

IDisplayDriver* IDisplayDriver::m_instance = nullptr;

#endif