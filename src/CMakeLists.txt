find_package(PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})

add_library(NESulator SHARED nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        utils/Utils.h utils/Utils.c mappers/CNROM.c mappers/CNROM.h utils/colour_palette.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c gui/impl/lib/gui.c gui/impl/lib/gui.h)

add_library(GUI_Interface gui/impl/lib/gui.c gui/impl/lib/gui.h)

# target_link_libraries(NESulator ${PYTHON_LIBRARIES})
# set_target_properties(NESulator PROPERTIES COMPILE_FLAGS "-municode" )
