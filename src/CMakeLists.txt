add_executable(NESulator nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        utils/Utils.h utils/Utils.c mappers/CNROM.c mappers/CNROM.h utils/colour_palette.h gui/mainWindow.c gui/mainWindow.h gui/gui.c gui/gui.h gui/nametableViewer.c gui/nametableViewer.h gui/patterntableViewer.c gui/patterntableViewer.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c)

target_link_libraries(NESulator SDL2)