add_executable(NESulator nes.c utils/log.c utils/log.h nes.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        utils/Utils.h utils/Utils.c mappers/NROM.c mappers/NROM.h mappers/CNROM.c mappers/CNROM.h gui/mainWindow.c gui/mainWindow.h utils/colour_palette.h gui/gui.c gui/gui.h gui/nametableViewer.c gui/nametableViewer.h)

target_link_libraries(NESulator SDL2)