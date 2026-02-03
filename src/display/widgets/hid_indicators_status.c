/*
 * Copyright (c) 2026 The ZMK Contributors
 * 
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <./hid_indicators_status.h>
#include <zmk/hid_indicators.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators_types.h>

#define ZMK_LED_NUMLOCK_BIT BIT(0)
#define ZMK_LED_CAPSLOCK_BIT BIT(1)
#define ZMK_LED_SCROLLLOCK_BIT BIT(2)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct hid_indicators_status_state {
    zmk_hid_indicators_t flags;  // HID Indicator Status Bit Mask
};

/**
 * @brief 设置HID指示器的显示符号
 * 
 * 根据HID指示器状态（Caps Lock、Num Lock、Scroll Lock）生成对应的显示符号，
 * 并将其设置到LVGL标签对象中。
 * 
 * @param label LVGL标签对象指针，用于显示HID指示器状态
 * @param state HID指示器状态结构体，包含当前锁定键的状态
 */
static void set_hid_indicators_symbol(lv_obj_t *label, struct hid_indicators_status_state state) {
    // 用于存储生成的显示文本，最大长度为10个字符
    char text[10];
    
    // 根据当前HID指示器状态生成显示文本
    // 如果Caps Lock开启，添加"C"；否则添加空字符串
    // 如果Num Lock开启，添加"N"；否则添加空字符串
    // 如果Scroll Lock开启，添加"S"；否则添加空字符串
    snprintf(text, sizeof(text), "%s%s%s",
             (state.flags & ZMK_LED_CAPSLOCK_BIT) ? "C" : "",  // Caps Lock指示器
             (state.flags & ZMK_LED_NUMLOCK_BIT) ? "N" : "",   // Num Lock指示器
             (state.flags & ZMK_LED_SCROLLLOCK_BIT) ? "S" : ""); // Scroll Lock指示器
    
    // 如果没有任何锁定键开启，显示"---"
    if (text[0] == '\0') {
        strcpy(text, "---");
    } 
    // 如果文本末尾有空格，移除空格
    else if (text[strlen(text) - 1] == ' ') {
        text[strlen(text) - 1] = '\0';
    }
    
    // 将生成的文本设置到LVGL标签对象中
    lv_label_set_text(label, text);
}


static void hid_indicators_status_update_cb(struct hid_indicators_status_state state) {
    struct zmk_widget_hid_indicators_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_hid_indicators_symbol(widget->obj, state);
    }
}

static struct hid_indicators_status_state hid_indicators_status_get_state(const zmk_event_t *eh) {
    return (struct hid_indicators_status_state){
        .flags = zmk_hid_indicators_get_current_profile()
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_hid_indicators_status, struct hid_indicators_status_state,
                            hid_indicators_status_update_cb, hid_indicators_status_get_state)

ZMK_SUBSCRIPTION(widget_hid_indicators_status, zmk_hid_indicators_changed);

int zmk_widget_hid_indicators_status_init(struct zmk_widget_hid_indicators_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    sys_slist_append(&widgets, &widget->node);
    widget_hid_indicators_status_init();
    return 0;
}

lv_obj_t *zmk_widget_hid_indicators_status_obj(struct zmk_widget_hid_indicators_status *widget) {
    return widget->obj;
}