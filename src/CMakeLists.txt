file(GLOB_RECURSE SRC_FILES "*.h" "*.c")
add_library(NESulator MODULE ${SRC_FILES})

add_custom_command(TARGET NESulator PRE_BUILD
        COMMAND powershell ../../scripts/build_guitulator.ps1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

add_custom_command(TARGET NESulator PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ../../GUItulator/bin/Debug/netcoreapp2.0
        ${CMAKE_BINARY_DIR}/gui_runtime
        )

add_custom_command(TARGET NESulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/libNESulator.dll
        ${CMAKE_BINARY_DIR}/gui_runtime
        )