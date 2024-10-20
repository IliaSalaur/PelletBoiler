#ifndef MY_LVGL_DISPLAY_HPP
#define MY_LVGL_DISPLAY_HPP

#include <functional>
#include "ui.h"
#include "ui_handlers.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_common.hpp"

#include "lvgl.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

#include "IDisplayDriver.hpp"
#include "ITouchDriver.hpp"
#include "display_common.hpp"

#define ENABLE_LVGL_LOG
#ifdef ENABLE_LVGL_LOG
#define LOG_LV(x...) ESP_LOGI(lvdispTAG, x)
#else
    #define LOG_LV(x) 
#endif

namespace
{
    constexpr size_t k_v_res_size{20};
    constexpr size_t k_tick_interval{2};        ///> in ms    
};

class LVGLDisplay
{
private:
    IDisplayDriver* m_displayDriver;
    ITouchDriver* m_inputDriver;
    lv_disp_drv_t m_dispDrvDesc;
    static LVGLDisplay* m_instance;

    static constexpr const char* lvdispTAG{"lvgl"};

    static void _flushDone()
    {    
        // LOG_LV("ending flush...");    
        lv_disp_flush_ready(&m_instance->m_dispDrvDesc);
        // LOG_LV("flush done");
    }

    static void _lvTick(void* arg)
    {
        // LOG_LV("tick");
        lv_tick_inc(k_tick_interval);
    }

    static void _lvglTask(void* pvParam)
    {
        using namespace DisplayCommon;
        lv_color_t* m_imgBuf;
        lv_color_t* m_flushBuf;
        lv_disp_draw_buf_t m_drawBuf;

        LVGLDisplay& inst = *static_cast<LVGLDisplay*>(pvParam);

        lv_indev_drv_t m_indevDrvDesc;

        LOG_LV("Init LVGL");
        lv_init();
        const auto bufSize{k_Hres * k_v_res_size};
        m_imgBuf = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_DMA);
        m_flushBuf = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_DMA);
        
        if(!m_imgBuf || !m_flushBuf)
        {
            LOG_LV("Alloc failed");
            return;
        }

        lv_disp_draw_buf_init(&m_drawBuf, m_imgBuf, m_flushBuf, bufSize);

        LOG_LV("Register Display driver");
        lv_disp_drv_init(&inst.m_dispDrvDesc);

        inst.m_dispDrvDesc.hor_res = k_Hres;
        inst.m_dispDrvDesc.ver_res = k_Vres;
        inst.m_dispDrvDesc.draw_buf = &m_drawBuf;
        inst.m_dispDrvDesc.flush_cb = IDisplayDriver::flush;
        inst.m_dispDrvDesc.drv_update_cb = IDisplayDriver::update;
    
        lv_disp_t* disp{lv_disp_drv_register(&inst.m_dispDrvDesc)};

        LOG_LV("Install Tick Timer");
        const esp_timer_create_args_t lvgl_tick_timer_args = {
            .callback = &_lvTick,
            .name = "lvgl_tick"
        };
        esp_timer_handle_t lvgl_tick_timer = NULL;
        ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, k_tick_interval * 1000));

        LOG_LV("Register Input driver");
        lv_indev_drv_init(&m_indevDrvDesc);
        m_indevDrvDesc.type = LV_INDEV_TYPE_POINTER;
        m_indevDrvDesc.disp = disp;
        m_indevDrvDesc.read_cb = ITouchDriver::read;
        lv_indev_drv_register(&m_indevDrvDesc);

        LOG_LV("Last steps");
        lv_disp_set_rotation(disp, LV_DISP_ROT_90);       

        lv_disp_set_default(disp);
        ui_init();
        LOG_LV("inited");
        init_ui_sync();


        while(1)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            lv_timer_handler();
        }        
    }

public:
    LVGLDisplay(IDisplayDriver* dispDriver, ITouchDriver* touchDriver)
      :
      m_displayDriver{dispDriver},
      m_inputDriver{touchDriver},
      m_dispDrvDesc{} 
    {
        m_instance = this;
    }

    void init()
    {
        m_displayDriver->setOnFlushDoneCB(LVGLDisplay::_flushDone);
        using namespace TaskCommon;
        xTaskCreatePinnedToCore(LVGLDisplay::_lvglTask, "lvglTask", k_lvglTaskStackSize, this, k_lvglTaskPriority, nullptr, k_lvglTaskCoreID);
    }

    ~LVGLDisplay() {}
};

LVGLDisplay* LVGLDisplay::m_instance{nullptr};

#endif
