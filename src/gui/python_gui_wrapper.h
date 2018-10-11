#ifndef NESULATOR_PYTHON_GUI_WRAPPER_H
#define NESULATOR_PYTHON_GUI_WRAPPER_H

#include <Python.h> //This header MUST be the first one.
#include <stdio.h>
#include "../utils/log.h"

PyObject *create_memoryview(PyObject *memoryview,void *buffer, long size, long item_size);

void *start_gui(void *arg);

static PyObject *get_cycle_count(PyObject *self, PyObject *args);

/**
 * Calls the necessary functions ant stuff to expose all the buffers to the Python interpreter
 */
void expose_buffers();

#endif //NESULATOR_PYTHON_GUI_WRAPPER_H
