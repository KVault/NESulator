#include "python_gui_wrapper.h"

static PyModuleDef EmbModule = {
		PyModuleDef_HEAD_INIT, "hello_world", NULL, -1, 0,
		NULL, NULL, NULL, NULL
};

void *start_gui(void *arg){
	hello_world();
}

PyObject *hello_world() {

	//TODO Turn this into a reusable function. I would hate to be doing this every single time 
	Py_Initialize();
	PyObject *module, *function, *dictionary, *result;

	PyObject *sys = PyImport_ImportModule("sys");
	PyObject *path = PyObject_GetAttrString(sys, "path");
	PyList_Append(path, PyUnicode_FromString("."));
	module = PyImport_ImportModule("GUIStartup");
	dictionary = PyModule_GetDict(module);
	function = PyDict_GetItemString(dictionary, "greet");

	result = PyObject_CallObject(function, NULL);
	long c_result = PyLong_AsLong(result);
	log_info("The result from the method call is %i:\n",c_result);

	Py_Finalize();
	return Py_None;
}
