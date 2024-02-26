add_library(scan_kbd
        scan_kbd.c
        scan_kbd.h
        )

target_link_libraries(scan_kbd
        hardware_i2c
	hardware_gpio
        pico_stdlib
        )