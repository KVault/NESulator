#include "python_gui_wrapper.h"

static PyModuleDef EmbModule = {
		PyModuleDef_HEAD_INIT, "hello_world", NULL, -1, 0,
		NULL, NULL, NULL, NULL
};

void *start_gui(void *arg){
	hello_world();
}

PyObject *hello_world() {
	Py_Initialize();
	Py_SetProgramName(L"Hello_world.py");
	PyObject *obj = Py_BuildValue("s", "hello_world.py");
	FILE *file = _Py_fopen_obj(obj, "r+");
	if(file != NULL) {
		PyRun_SimpleFileEx(file, "hello_world.py", 0);
	}
	Py_Finalize();
}
