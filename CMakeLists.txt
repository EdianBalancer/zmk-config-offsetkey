# Copyright (c) 2026 The ZMK Contributors
# SPDX-License-Identifier: MIT


target_sources_ifdef(CONFIG_ZMK_WIDGET_INDICATORS_STATUS app PRIVATE src/display/widgets/hid_indicators_status.c)
