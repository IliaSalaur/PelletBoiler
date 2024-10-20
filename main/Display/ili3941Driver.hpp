#ifndef ILI3491_DRIVER_HPP
#define ILI3491_DRIVER_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lcd_ili9341.h"

#include "IDisplayDriver.hpp"
#include "display_common.hpp"

namespace
{
    // SPI bus configuration
    // const int kPixelClockHz = 20 * 1000 * 1000;           // EXAMPLE_LCD_PIXEL_CLOCK_HZ
    const int kPixelClockHz = 60 * 1000 * 1000;           // EXAMPLE_LCD_PIXEL_CLOCK_HZ
    const int kBkLightOnLevel = 1;                        // EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
    const int kBkLightOffLevel = !kBkLightOnLevel;        // EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL

    // Pin numbers
    const int kPinNumSclk = 12;                           // EXAMPLE_PIN_NUM_SCLK
    const int kPinNumMosi = 11;                           // EXAMPLE_PIN_NUM_MOSI
    const int kPinNumMiso = 13;                           // EXAMPLE_PIN_NUM_MISO
    const int kPinNumLcdDc = 2;                           // EXAMPLE_PIN_NUM_LCD_DC
    const int kPinNumLcdRst = 4;                          // EXAMPLE_PIN_NUM_LCD_RST
    const int kPinNumLcdCs = 15;                          // EXAMPLE_PIN_NUM_LCD_CS
    const int kPinNumBkLight = 19;                        // EXAMPLE_PIN_NUM_BK_LIGHT

    // LCD resolution
    // const int kLcdHRes = 240;                             // EXAMPLE_LCD_H_RES
    // const int kLcdVRes = 320;                             // EXAMPLE_LCD_V_RES

    // Command and parameter bits
    const int kLcdCmdBits = 8;                            // EXAMPLE_LCD_CMD_BITS
    const int kLcdParamBits = 8;                          // EXAMPLE_LCD_PARAM_BITS

    // LVGL tick period
    const int kLvglTickPeriodMs = 2;                      // EXAMPLE_LVGL_TICK_PERIOD_MS

    //SPI HOST
    constexpr auto kLCD_HOST{SPI2_HOST};
}


class ILI3491Driver : public IDisplayDriver
{
public:
    ILI3491Driver() 
        : m_io_handle(nullptr), m_panel_handle(nullptr) {
        }

    esp_err_t init() override
    {
        // Initialize SPI bus
        spi_bus_config_t buscfg = {
            .mosi_io_num = kPinNumMosi,
            .miso_io_num = kPinNumMiso,
            .sclk_io_num = kPinNumSclk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = DisplayCommon::k_Hres * 80 * sizeof(uint16_t),
        };
        ESP_ERROR_CHECK(spi_bus_initialize(kLCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

        // Install panel IO
        esp_lcd_panel_io_spi_config_t io_config = {
            .cs_gpio_num = kPinNumLcdCs,
            .dc_gpio_num = kPinNumLcdDc,
            .spi_mode = 0,
            .pclk_hz = kPixelClockHz,
            .trans_queue_depth = 10,
            .on_color_trans_done = ILI3491Driver::flush_ready,
            .user_ctx = this,
            .lcd_cmd_bits = kLcdCmdBits,
            .lcd_param_bits = kLcdParamBits,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)kLCD_HOST, &io_config, &m_io_handle));

        // Install ILI9341 panel driver
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = kPinNumLcdRst,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
            .bits_per_pixel = 16,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(m_io_handle, &panel_config, &m_panel_handle));

        ESP_ERROR_CHECK(esp_lcd_panel_reset(m_panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_init(m_panel_handle));

        // Turn on the LCD backlight
        gpio_set_level((gpio_num_t)kPinNumBkLight, kBkLightOnLevel);

        return ESP_OK;
    }

protected:
    void doFlush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t *color_map) override
    {
        int offsetx1 = area->x1;
        int offsetx2 = area->x2;
        int offsety1 = area->y1;
        int offsety2 = area->y2;
        esp_lcd_panel_draw_bitmap(m_panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
        // ESP_LOGI("panel", "flushed");
    }

    void doUpdate(lv_disp_drv_t* drv) override
    {
        esp_lcd_panel_swap_xy(m_panel_handle, true);
        esp_lcd_panel_mirror(m_panel_handle, false, false);
        // ESP_LOGI("panel", "updated");
    }

private:
    static bool flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
    {
        // ESP_LOGI("panel", "flushready");
        ILI3491Driver& instance = *static_cast<ILI3491Driver*>(user_ctx);
        if(instance.m_flushDoneCb)
            instance.m_flushDoneCb();
            
        return false;
    }

    esp_lcd_panel_io_handle_t m_io_handle;
    esp_lcd_panel_handle_t m_panel_handle;
};

#endif // ILI3491_DRIVER_HPP
