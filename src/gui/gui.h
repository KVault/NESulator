#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#include "../utils/Utils.h"
#include "../utils/ppu_utils.h"

#define PATTERNTABLE_HEIGHT 240
#define PATTERNTABLE_WIDTH 512

enum PatterntableSide {LeftPatterntable, RightPatterntable};

typedef struct{
	uint size; //In bytes
	byte *buffer;
}patterntable;

void render_tiles(tile *tiles, patterntable *patterntable, enum PatterntableSide side);

patterntable *fill_patterntable();

#endif //NESULATOR_GUI_H
