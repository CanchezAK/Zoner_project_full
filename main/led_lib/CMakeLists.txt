add_library(led_lib
        led.c
        led.h
        )

target_link_libraries(led_lib
        hardware_spi
        pico_stdlib
        )