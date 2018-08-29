add_executable(NESulator nes.c log.c log.h nes.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        Utils.h Utils.c mappers/NROM.c mappers/NROM.h mappers/CNROM.c mappers/CNROM.h gui/mainWindow.c gui/mainWindow.h)

target_link_libraries(NESulator SDL2)