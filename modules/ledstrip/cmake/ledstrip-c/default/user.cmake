# Increase the level of optimizations of xc8-cc, 
# in order to generate few gotos as possible for the leds update functions.

# automatic patch for .generated/rule.cmake: "-O0" -> "-O1",
# executed at every reconfiguration (included before .generated/main.cmake)
set(_gen_dir "${CMAKE_CURRENT_LIST_DIR}/.generated")
set(_rule_file "${_gen_dir}/rule.cmake")

if(EXISTS "${_rule_file}")
    file(READ "${_rule_file}" _orig_content)

    set(_patched "${_orig_content}")

    # safe substitutions
    string(REPLACE "\"-O0\"" "\"-O1\"" _patched "${_patched}")
    string(REPLACE "'-O0'" "'-O1'" _patched "${_patched}")
    string(REPLACE " -O0 " " -O1 " _patched "${_patched}")
    string(REPLACE " -O0\n" " -O1\n" _patched "${_patched}")
    string(REPLACE "\n-O0 " "\n-O1 " _patched "${_patched}")

    if(NOT "${_patched}" STREQUAL "${_orig_content}")
        # backup
        file(WRITE "${_rule_file}.bak" "${_orig_content}")
        file(WRITE "${_rule_file}" "${_patched}")
        message(STATUS "Patched ${_rule_file}: -O0 -> -O1")
    endif()
endif()
