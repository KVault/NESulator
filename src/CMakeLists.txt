find_package(Lua REQUIRED)

include_directories(${LUA_INCLUDE_DIR})

add_library(NESulator nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        utils/Utils.h utils/Utils.c mappers/CNROM.c mappers/CNROM.h utils/colour_palette.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c)

add_library(GUItulator MODULE "gui/Lua_wrapper/gui.c" "gui/Lua_wrapper/gui.h")

add_executable(LUAtulator gui/lua_gui_main.c)

target_link_libraries(LUAtulator ${CMAKE_CURRENT_BINARY_DIR}/libNESulator.a ${CMAKE_CURRENT_BINARY_DIR}/libGUItulator.dll)
target_link_libraries(GUItulator ${CMAKE_CURRENT_BINARY_DIR}/libNESulator.a)
target_link_libraries(LUAtulator ${LUA_LIBRARIES})
target_link_libraries(GUItulator ${LUA_LIBRARIES})

add_custom_command(TARGET LUAtulator PRE_BUILD COMMAND ${CMAKE_COMMAND}
        -E copy_directory ${CMAKE_SOURCE_DIR}/src/gui/lua ${CMAKE_BINARY_DIR}/src/lua/)
