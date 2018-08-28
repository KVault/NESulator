#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "gui/mainWindow.h"
#include "log.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "apu.h"
#include <stdio.h>

void stopEmulation();

//So that SDL stops complaining
#undef main

int main();

#endif //NESULATOR_NES_H
