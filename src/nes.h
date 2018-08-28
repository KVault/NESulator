#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "gui/mainWindow.h"
#include "log.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "apu.h"
#include <stdio.h>


#define SDL_main main

void stopEmulation();

int main();

#endif //NESULATOR_NES_H
