#ifndef NESULATOR_PYTHON_GUI_WRAPPER_H
#define NESULATOR_PYTHON_GUI_WRAPPER_H

#include <Python.h>
#include <limits.h>
#include <stdio.h>
#include "../utils/log.h"

extern int callback_buffer[];

void *start_gui(void *arg);

PyObject *hello_world();

/**
 * Calls the necessary functions ant stuff to expose all the buffers to the Python interpreter
 */
void expose_buffers();

#endif //NESULATOR_PYTHON_GUI_WRAPPER_H
