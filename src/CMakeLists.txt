# Add all the files to compile
file(GLOB_RECURSE SRC_FILES "*.h" "*.c")

# Make a DLL with all the files
add_library(NESulator SHARED ${SRC_FILES})

# Run this script that calls the dotnet compiler to build the GUItulator
add_custom_command(TARGET NESulator PRE_BUILD
        COMMAND powershell ../../scripts/build_guitulator.ps1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Copy the contents of GUItulator to the bin directory
add_custom_command(TARGET NESulator PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ../../GUItulator/bin/Debug/netcoreapp2.0
        ${CMAKE_BINARY_DIR}/gui_runtime)

# And finally copy the NESulator dll to the bin directory
add_custom_command(TARGET NESulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/libNESulator.dll
        ${CMAKE_BINARY_DIR}/gui_runtime)