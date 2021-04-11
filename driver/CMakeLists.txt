cmake_minimum_required (VERSION 3.0)

set (project_name "lunatic-driver")
set (default_flags -Wall; -Werror; -pthread)
set (debug_flags -g; -Wextra; -Wno-unused-parameter; -Wno-unused-function; -Wfatal-errors; -Werror)
set (release_flags )

set (linker_libraries "-lpthread -lrt")


project (${project_name})

add_executable (
	${project_name}
	src/libs/exceptions.c
	src/libs/gpio.c
	src/libs/log.c
	src/libs/utils.c
	src/libs/i2c.c
	src/libs/ini.c
	src/devices/devices.c
	src/devices/analog.c
	src/devices/buttons.c
	src/devices/keyboard.c
	src/devices/ledstrip.c
	src/threads.c
	src/scheduler.c
	src/api.c
	src/console.c
	src/debug.c
	src/settings.c
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
