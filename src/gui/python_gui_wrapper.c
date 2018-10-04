#include "python_gui_wrapper.h"

static PyModuleDef EmbModule = {
		PyModuleDef_HEAD_INIT, "hello_world", NULL, -1, NULL,
		NULL, NULL, NULL, NULL
};

PyObject *hello_world() {
	Py_Initialize();
	PyObject *obj = Py_BuildValue("s", "hello_world.py");
	FILE *file = _Py_fopen_obj(obj, "r+");
	if(file != NULL) {
		PyRun_SimpleFile(file, "hello_world.py");
	}
	Py_Finalize();
}
