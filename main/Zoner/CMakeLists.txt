add_library(zoner
        zoner.c
        zoner.h
        )

target_link_libraries(zoner
        hardware_uart
	pico_binary_info
        pico_stdlib
        )