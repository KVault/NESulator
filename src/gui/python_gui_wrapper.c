#include "python_gui_wrapper.h"

static PyModuleDef EmbModule = {
		PyModuleDef_HEAD_INIT, "emb", NULL, -1, NULL,
		NULL, NULL, NULL, NULL
};

PyObject *hello_world() {
	Py_Initialize();
	log_info("Py_IsInitialized: %d\n", Py_IsInitialized());
	log_info("Py_GetPath: %s\n", Py_GetPath());
	PyObject *pyObject = PyModule_Create(&EmbModule);
	PyImport_AppendInittab("hello_world", pyObject);
	Py_Finalize();
}