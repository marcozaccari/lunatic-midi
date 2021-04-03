cmake_minimum_required (VERSION 3.0)

set (project_name "midi-controller")
set (default_flags -Wall; -pthread)
set (debug_flags -g; -Wextra; -Wno-unused-parameter; -Wno-unused-function)
set (release_flags )

set (linker_libraries "-lasound -lpthread -lrt")


project (${project_name})

# Check for ALSA
find_path (ALSA_INCLUDE_DIR NAMES alsa/asoundlib.h)
if (ALSA_INCLUDE_DIR)
    message ("ALSA: OK")
else ()
    message (FATAL_ERROR "ALSA: not found; please install libasound2-dev")
endif ()


add_executable (
    ${project_name}
    lib/exceptions.c
    lib/gpio.c
    lib/log.c
    lib/utils.c
    lib/i2c.c
    lib/ipc.cpp
    lib/zini.cpp
    lib/midi.c
    src/adc.c
    src/buttons.c
    src/keyboard.c
    src/led_monitor.c
    src/threads.c
    src/threads.devices.c
    src/threads.midi.c
    src/console.c
    src/main.settings.c
    src/main.c
)

target_compile_options (
    ${project_name}
    PRIVATE ${default_flags}
    PRIVATE "$<$<CONFIG:debug>:${debug_flags}>"
    PRIVATE "$<$<CONFIG:release>:${release_flags}>"
)

target_link_libraries (
    ${project_name}
    ${linker_libraries}
)


#include_directories (src lib)
