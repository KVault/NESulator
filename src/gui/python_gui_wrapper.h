#ifndef NESULATOR_PYTHON_GUI_WRAPPER_H
#define NESULATOR_PYTHON_GUI_WRAPPER_H

#include <Python.h>
#include <limits.h>
#include <stdio.h>
#include "../utils/log.h"

void *start_gui(void *arg);

PyObject *hello_world();

#endif //NESULATOR_PYTHON_GUI_WRAPPER_H
