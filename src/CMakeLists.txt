find_package(PythonLibs REQUIRED)
add_executable(NESulator nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        utils/Utils.h utils/Utils.c mappers/CNROM.c mappers/CNROM.h utils/colour_palette.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c gui/python_gui_wrapper.c gui/python_gui_wrapper.h)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gui/hello_world.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(NESulator ${PYTHON_LIBRARIES})