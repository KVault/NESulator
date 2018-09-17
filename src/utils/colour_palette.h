#ifndef NESULATOR_COLOUR_PALETTE_H
#define NESULATOR_COLOUR_PALETTE_H

#include "Utils.h"

typedef struct pixel{
	byte R;
	byte G;
	byte B;
}pixel;

/**
 * The Colour palette for the NES. The interpretation of the colors may change, but not its positions on it
 *
 * This might seem a bit random but it can be found here (https://wiki.nesdev.com/w/images/5/59/Savtool-swatches.png)
 */
extern const pixel COLOUR_PALETTE[];


#endif //NESULATOR_COLOUR_PALETTE_H
