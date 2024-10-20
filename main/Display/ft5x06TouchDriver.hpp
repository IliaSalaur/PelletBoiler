#ifndef MY_FT5X06_TOUCH_DRIVER_HPP
#define MY_FT5X06_TOUCH_DRIVER_HPP

#include "lvgl.h"
#include "ITouchDriver.hpp"
#include "display_common.hpp"
#include "driver/i2c.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch_ft5x06.h"

class ft5x06TouchDriver : public ITouchDriver
{
private:
    esp_lcd_panel_io_handle_t m_tp_io_handle;
    esp_lcd_touch_handle_t m_tp_handle;

    void doRead(lv_indev_drv_t* drv, lv_indev_data_t* data) override
    {
        uint16_t touchpad_x[1] = {0};
        uint16_t touchpad_y[1] = {0};
        uint8_t touchpad_cnt = 0;

        /* Read touch controller data */
        esp_lcd_touch_read_data(m_tp_handle);
        // esp_lcd_touch_set_mirror_y(m_tp_handle, true);
        // esp_lcd_touch_set_mirror_x(m_tp_handle, true);

        /* Get coordinates */
        bool touchpad_pressed = esp_lcd_touch_get_coordinates(m_tp_handle, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);
        

        if (touchpad_pressed && touchpad_cnt > 0) {
            data->point.x = touchpad_x[0];
            data->point.y = touchpad_y[0];
            data->state = LV_INDEV_STATE_PRESSED;
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }

        // ESP_LOGI("touch", "touch cb: %u, %u", touchpad_x[0], touchpad_y[0]);
    }

public:
    ft5x06TouchDriver(/* args */) {}

    esp_err_t init() override
    {
        using namespace DisplayCommon;
        i2c_driver_install(0, I2C_MODE_MASTER, 1024, 1024, 0);
        i2c_set_pin(0, 10, 8, 0, 0, I2C_MODE_MASTER);
        esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(0, &tp_io_config, &m_tp_io_handle));

        esp_lcd_touch_config_t tp_cfg = {
            .x_max = k_Hres,
            .y_max = k_Vres,
            .rst_gpio_num = gpio_num_t::GPIO_NUM_NC,
            .int_gpio_num = gpio_num_t::GPIO_NUM_NC,
            .flags = {
                .swap_xy = 0,
                .mirror_x = 1,
                .mirror_y = 1,
            },
        };

        // ESP_LOGI(TAG, "Initialize touch controller FT5x06");
        ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(m_tp_io_handle, &tp_cfg, &m_tp_handle));
        return ESP_OK;
    }

    ~ft5x06TouchDriver() {}
};

#endif