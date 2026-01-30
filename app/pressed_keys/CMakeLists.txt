zephyr_include_directories(include)

if(CONFIG_ZMK_DISPLAY)
  zephyr_library()
  zephyr_library_sources(src/pressed_keys.c)
  zephyr_library_include_directories(.
    include
  )
endif()
