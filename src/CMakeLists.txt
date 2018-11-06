file(GLOB_RECURSE SRC_FILES "*.h" "*.c")
add_library(NESulator MODULE ${SRC_FILES})


add_custom_command(TARGET NESulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/libNESulator.dll
        ../../GUItulator/bin/Debug/netcoreapp2.0
        )