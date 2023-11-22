add_library(df_player
        df.c
        df.h
        )

target_link_libraries(df_player
        hardware_pio
        pico_stdlib
        )