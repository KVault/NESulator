#include "python_gui_wrapper.h"

static PyModuleDef EmbModule = {
		PyModuleDef_HEAD_INIT, "emb", NULL, -1, NULL,
		NULL, NULL, NULL, NULL
};

PyObject *hello_world() {
	Py_Initialize();
	PyRun_SimpleFile("hello_world.py");
	Py_Finalize();
}