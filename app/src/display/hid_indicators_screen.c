/*
 * Copyright (c) 2026 The ZMK Contributors
 * Author: Edian Balancer
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/logging/log.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/hid_indicators_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/wpm_status.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_INDICATORS_STATUS)
static struct zmk_widget_hid_indicators_status indicators_status_widget;
zmk_widget_hid_indicators_status_init(&indicators_status_widget, screen);
lv_obj_align(zmk_widget_hid_indicators_status_obj(&indicators_status_widget),
             LV_ALIGN_BOTTOM_RIGH, 0, 0);
#endif