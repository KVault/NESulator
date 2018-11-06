find_package(Python REQUIRED)

add_executable(Startup startup.c)

add_custom_command(TARGET Startup PRE_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/gui_runtime/Startup.py
        ${CMAKE_BINARY_DIR}/gui_runtime)
