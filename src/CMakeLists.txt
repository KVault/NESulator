add_library(NESulator nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h
        cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h utils/Utils.h utils/Utils.c mappers/CNROM.c
        mappers/CNROM.h utils/colour_palette.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c)

add_executable(GUItulator gui/gui.c gui/gui.h)

target_link_libraries(GUItulator ${CMAKE_CURRENT_BINARY_DIR}/libNESulator.a)

