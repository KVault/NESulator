find_package(PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})

add_executable(NESulator nes.c utils/log.c utils/log.h nes.h mappers/NROM.c mappers/NROM.h rom.c rom.h cpu.c cpu.h ppu.c ppu.h apu.c apu.h memory.c memory.h
        utils/Utils.h utils/Utils.c mappers/CNROM.c mappers/CNROM.h utils/colour_palette.h utils/ppu_utils.c utils/ppu_utils.h utils/colour_palette.c gui/python_gui_wrapper.c gui/python_gui_wrapper.h gui/gui_events.h gui/gui.c gui/gui.h)


target_link_libraries(NESulator ${PYTHON_LIBRARIES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gui/GUIStartup.py ${CMAKE_CURRENT_BINARY_DIR}/)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gui/MainWindow.py ${CMAKE_CURRENT_BINARY_DIR}/)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gui/GUIEvents.py ${CMAKE_CURRENT_BINARY_DIR}/)