add_library(NESulator nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h
        cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h utils/Utils.h utils/Utils.c mappers/CNROM.c
        mappers/CNROM.h utils/colour_palette.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c)

file(GLOB IMGUI_SRC gui/imgui/*.h gui/imgui/*.cpp)

add_executable(GUItulator gui/gui.cpp gui/gui.h ${IMGUI_SRC} gui/imgui/imgui_impl_opengl2.cpp gui/imgui/imgui_impl_sdl.cpp)

target_link_libraries(GUItulator SDL2)
target_link_libraries(GUItulator GL)
target_link_libraries(GUItulator ${CMAKE_CURRENT_BINARY_DIR}/libNESulator.a)

