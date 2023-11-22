add_library(kbd
        kbd.c
        kbd.h
        )

target_link_libraries(kbd
        hardware_i2c
	pico_binary_info
        pico_stdlib
        )