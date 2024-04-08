#ifndef GLVGL_H
#define GLVGL_H
#include "lvgl/lvgl.h"

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};

#ifdef __cplusplus
extern "C" {
#endif
/*.$declare${AOs::Lvgl} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${AOs::Lvgl} ............................................................*/
typedef struct {
    /* private: */
    lv_obj_t* screen;

    /* public: */
    lv_obj_t* clock;

    /* private: */
    lv_obj_t* day;
    lv_obj_t* month;
    lv_obj_t* date;
    lv_obj_t* year;
    lv_obj_t* menu;
    lv_obj_t* cont;
    lv_obj_t* dropdwn_list;
    lv_obj_t* dropdwn_btn;
    lv_style_t style_clock;
    lv_style_t style_btn_normal;
    lv_style_t style_btn_clicked;
} Lvgl;

typedef struct
{
    uint8_t clock_hour;
    uint8_t clock_min;
    uint8_t clock_sec;
    uint8_t clock_format;
    uint8_t clock_mode;
    uint8_t alarm_status;
    uint8_t alarm_hour;
    uint8_t alarm_min;
    uint8_t alarm_sec;
    uint8_t alarm_format;
    uint8_t sound_status;
    uint8_t brightness;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint32_t year;
} lvgl_setting_page_data_t;

typedef struct
{
    uint8_t clock_hour;
    uint8_t clock_min;
    uint8_t clock_sec;
    uint8_t date;
    uint8_t month;
    uint32_t year;
} lvgl_alarm_page_data_t;

void Lvgl_ctor(Lvgl* const me);
void Lvgl_style_init(Lvgl* const me);
void Lvgl_btn_style_init(Lvgl* const me);
void Lvgl_create_main_page(Lvgl* const me);
void Lvgl_screen_cleanup(Lvgl* const me);
void Lvgl_create_setting_page(Lvgl* const me, lvgl_setting_page_data_t* data);
void Lvgl_create_message_box(Lvgl* const me, const char* title, const char* msg, const char* opts[], bool close_btn);
void Lvgl_main_page_set_style(Lvgl* const me);
void Lvgl_create_alarm_notif_page(Lvgl* const me, lvgl_alarm_page_data_t* data);
void Lvgl_display_clock(Lvgl* const me, char* const time_as_string);
void Lvgl_display_calendar_date(Lvgl* const me, uint8_t date, uint8_t month, uint32_t year);
void Lvgl_display_day(Lvgl* const me, const char* const day);
void Lvgl_display_month(Lvgl* const me, const char* const month);
void Lvgl_display_date(Lvgl* const me, uint8_t date);
void Lvgl_display_year(Lvgl* const me, uint32_t year);


#ifdef __cplusplus
}
#endif



#endif
