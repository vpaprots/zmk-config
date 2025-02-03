zephyr_library()
zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/charger.h)
zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/led_strip.h)

zephyr_library_sources_ifdef(CONFIG_CHARGER_BQ25895 charger/charger_bq25895.c)
zephyr_library_sources_ifdef(CONFIG_SK6812_STRIP_I2S led_strip/sk6812_i2s.c)
# zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)