#ifndef MY_UI_HANDLERS_HPP
#define MY_UI_HANDLERS_HPP

#include "DynamicConfig.hpp"
#include "lvgl.h"
#include "ui.h"
#include "esp_event.h"
#include "Events/Events.hpp"
#include "esp_log.h"
#include "App.hpp"

static App* appPtr;

#include <functional>

#define CFG(x, y) dynamicConfig.getConfig(x, y).c_str()
#define SET_TA(x, y, z) \
    lv_textarea_set_text(x, CFG(y##_str, z)); \
    lv_obj_set_user_data(x, (void*)y##_str);

#define lTAG(y) static const char* y##_str = #y;

lTAG(kTargetTemp);
lTAG(kHysteresis);
lTAG(kSafeModePower);
lTAG(kLimitTemp);
lTAG(kActivePeriod);
lTAG(kInterval);
lTAG(kPowerStep);
lTAG(kStepInterval);
lTAG(kPumpOnTemp);
lTAG(kPumpOffTemp);

lv_obj_t** screens[] = {&ui_Main, &ui_Manual, &ui_Settings};
void(*screenInits[])() = {ui_Main_screen_init, ui_Manual_screen_init, ui_Settings_screen_init};

void ui_event_MenuDropdown(lv_event_t *e)
{
      lv_event_code_t event_code = lv_event_get_code(e);
      lv_obj_t *target = lv_event_get_target(e);
      if (event_code == LV_EVENT_VALUE_CHANGED)
      {
            uint16_t selected = lv_dropdown_get_selected(target);
            ESP_LOGI("ui", "selected:%u", selected);
            _ui_screen_change(screens[selected], LV_SCR_LOAD_ANIM_NONE, 0, 0, screenInits[selected]);
            // switch (selected)
            // {
            // case 0:
            //     _ui_screen_change(screens[selected], LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_Main_screen_init);
            //     break;

            // case 1:
            //     _ui_screen_change(screens[selected], LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_Manual_screen_init);
            //     break;

            // case 2:
            //     _ui_screen_change(screens[selected], LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_Settings_screen_init);
            //     break;
            
            // default:
            //     break;
            // }            
      }
}

void ui_event_OnInput(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t* ta = lv_keyboard_get_textarea(target);

    if (event_code == LV_EVENT_READY)
    {
        ESP_LOGI("ui_handler", "%p", ta);
        ESP_LOGI("ui_handler", "getting confname");
        const char* confName = (const char*)lv_obj_get_user_data(ta);    
        ESP_LOGI("ui_handler", "%p", confName);
        ESP_LOGI("ui_handler", "confname:%s", confName);
        dynamicConfig.updateConfig(confName, lv_textarea_get_text(ta));
      _ui_flag_modify(ui_Keyboard1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);      
    }
}

void ui_roller_event(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_VALUE_CHANGED)
    {        
        lv_obj_t *target = lv_event_get_target(e);
        auto selected = lv_roller_get_selected(target);
        ESP_LOGI("ui_handler", "tempValue:%u", selected*5);
        dynamicConfig.updateConfig(kTargetTemp_str, std::to_string(5 * selected));
    }
}

void ui_init_menuDropdown(lv_obj_t* dropdown)
{
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_LIST);
    lv_obj_add_event_cb(dropdown, ui_event_MenuDropdown, LV_EVENT_ALL, nullptr);
}

void ui_set_img_enable(lv_obj_t* obj, bool enabled)
{
    ESP_LOGI("ui", "image set enable %d", int(enabled));
    if(!obj)
        return;
    
    static const auto opa_disabled = 80;
    lv_obj_set_style_opa(obj, enabled ? 255 : opa_disabled, LV_PART_MAIN| LV_STATE_DEFAULT);
    ESP_LOGI("ui", "set");
}

void ui_set_switch_state(lv_obj_t* sw, bool state)
{
    if(!sw)
        return;
    
    if(state)
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    else 
        lv_obj_clear_state(sw, LV_STATE_CHECKED);
}

void ui_event_PumpSwitch(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);

    appPtr->getPump().setState(lv_obj_has_state(target, LV_STATE_CHECKED));
}

void ui_event_FanSwitch(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);

    appPtr->getFanManager().setEnable(lv_obj_has_state(target, LV_STATE_CHECKED));
}

void ui_event_MotorSwitch(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);

    appPtr->getScrewMotorManager().setState(lv_obj_has_state(target, LV_STATE_CHECKED));
}

void init_ui_sync()
{
    // SET_TA(ui_temp)
    lv_roller_set_selected(ui_TempRoller, std::stof(dynamicConfig.getConfig(kTargetTemp_str, "0.0")) / 5, LV_ANIM_OFF);
    SET_TA(ui_HysteresisInp, kHysteresis, "5");
    SET_TA(ui_SafeModePowerInp, kSafeModePower, "20");
    SET_TA(ui_SafeModeTempInp, kLimitTemp, "78.0");
    SET_TA(ui_FuelDispIntervalInp, kInterval, "10");
    SET_TA(ui_ScrewInp, kActivePeriod, "5");
    SET_TA(ui_FanTickPercentInp, kPowerStep, "5");
    SET_TA(ui_FanTickIntervalInp, kStepInterval, "5");
    SET_TA(ui_PumpOnTempInp, kPumpOnTemp, "15.0");
    SET_TA(ui_PumpOffTempInp, kPumpOffTemp, "5.0");

    lv_obj_add_event_cb(ui_Keyboard1, ui_event_OnInput, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_TempRoller, ui_roller_event, LV_EVENT_ALL, nullptr);
    lv_obj_add_event_cb(ui_PumpSwitch, ui_event_PumpSwitch, LV_EVENT_VALUE_CHANGED, nullptr);
    lv_obj_add_event_cb(ui_ScrewSwitch, ui_event_MotorSwitch, LV_EVENT_VALUE_CHANGED, nullptr);
    lv_obj_add_event_cb(ui_FanSwitch, ui_event_FanSwitch, LV_EVENT_VALUE_CHANGED, nullptr);

    lv_obj_set_style_anim_time(ui_TempRoller, 0, LV_STYLE_ANIM_TIME);
    ui_init_menuDropdown(ui_Dropdown1);
    ui_init_menuDropdown(ui_Dropdown2);
    ui_init_menuDropdown(ui_Dropdown3);

    lv_obj_set_style_blend_mode(ui_ChimneyImg, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_blend_mode(ui_FireImg, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_DEFAULT);

    ui_set_img_enable(ui_ChimneyImg, false);
    ui_set_img_enable(ui_FireImg, false);
}

void ui_handler(void* event_handler_arg,
                                        esp_event_base_t event_base,
                                        int32_t event_id,
                                        void* data)
{
    event_data_t evData = data ? *static_cast<event_data_t*>(data) :event_data_t{};
    using enum EventsEnum;

    ESP_LOGI("HANDLER","\t\t\t\tEvent: %s;", EventMap[event_id]);
    switch (EventsEnum{event_id})
    {
    case TEMP_CHANGED:
        if(ui_TempLabel)
        {
            lv_label_set_text_fmt(ui_TempLabel, "%d°", int(evData.num));
        }
        break;

    case FIRE_DETECTED:
        ui_set_img_enable(ui_FireImg, evData.boolean);
        break;

    case SMOKE_TEMP_OVER_LIMIT:
        ui_set_img_enable(ui_ChimneyImg, evData.boolean);
        break;
    
    case MOTOR_STATE_CHANGE:
        ui_set_switch_state(ui_ScrewSwitch, evData.boolean);
        break;

    case PUMP_STATE_CHANGE:
        ui_set_switch_state(ui_PumpSwitch, evData.boolean);
        break;

    case FAN_ENABLE_CHANGE:
        ui_set_switch_state(ui_FanSwitch, evData.boolean);
        break;

    case FAN_DUTY_CHANGE:
        if(ui_FanSlider && ui_FanLabel)
        {
            lv_slider_set_value(ui_FanSlider, int(evData.num), LV_ANIM_OFF);
            lv_label_set_text_fmt(ui_FanLabel, "Обдув: %d%%", int(evData.num));
        }
        break;

    default:
        break;
    }
}

void ui_registerHandler(App* app)
{
    appPtr = app;
    app->getEventLoop().registerHandler({CONTROL_EVENTS, ESP_EVENT_ANY_ID}, ui_handler, app);
}

#endif