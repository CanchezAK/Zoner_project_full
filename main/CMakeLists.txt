add_executable(zoner_full
         main.c
         )
pico_enable_stdio_usb(zoner_full 1)
pico_enable_stdio_uart(zoner_full 0)

pico_generate_pio_header(zoner_full ${CMAKE_CURRENT_LIST_DIR}/uart_pio.pio)

add_subdirectory(DFPlayerMini)
add_subdirectory(Zoner)
#add_subdirectory(KBD)
add_subdirectory(scan_kbd)
add_subdirectory(led_lib)
 # Import pico_stdlib which imports commonly used features
 target_link_libraries(zoner_full pico_stdlib hardware_uart pico_time pico_binary_info hardware_gpio hardware_irq hardware_pio hardware_clocks df_player scan_kbd zoner hardware_timer hardware_i2c hardware_flash led_lib)

 # Create map/bin/hex file etc.
 pico_add_extra_outputs(zoner_full)

 # Add path via pico_set_program_url CMAKE
 example_auto_set_url(zoner_full)