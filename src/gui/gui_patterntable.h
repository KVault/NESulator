#ifndef NESULATOR_GUI_PATTERNTABLE_H
#define NESULATOR_GUI_PATTERNTABLE_H

#include "../utils/ppu_utils.h"
#include "gui.h"

#define PATTERNTABLE_TEXTURE_WIDTH 512
#define PATTERNTABLE_TEXTURE_HEIGHT 240

/**
 * Custom implementation for the patterntable. It's in its own file because oh boy this is big and messy....
 * returns the size of the buffer
 */
FrameInfo patterntable_frame();

#endif //NESULATOR_GUI_PATTERNTABLE_H
