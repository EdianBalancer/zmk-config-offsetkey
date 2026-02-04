# Copyright (c) 2026 The ZMK Contributors
# SPDX-License-Identifier: MIT

# 暴露本模块的 include 目录给 app（应用）和其他模块使用
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# 根据配置选项添加源文件到 app（应用）模块
target_sources_ifdef(CONFIG_ZMK_WIDGET_INDICATORS_STATUS app PRIVATE src/offsetkey_display/widgets/hid_indicators_status.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_BUILT_IN app PRIVATE src/offsetkey_display/hid_indicators_screen.c)

