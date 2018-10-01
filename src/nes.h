#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include <Python.h>
#include "cpu.h"
#include "rom.h"

static PyObject *run(PyObject *self, PyObject *args);

void every_second();

#endif //NESULATOR_NES_H
