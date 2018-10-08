#include "python_gui_wrapper.h"
#include "../utils/Utils.h"

int callback_buffer[10];
PyObject *callback_buffer_obj;

void *start_gui(void *arg){
	hello_world();
	expose_buffers();
}

void expose_buffers(){
	Py_buffer py_callback_buffer = {};
	py_callback_buffer.buf = callback_buffer;
	py_callback_buffer.len = 10 * sizeof(int);
	py_callback_buffer.itemsize = sizeof(int);
	py_callback_buffer.readonly = FALSE;
	py_callback_buffer.format = NULL;//Unsigned bytes assumed
	py_callback_buffer.ndim = 1;
	//As a special case, for temporary buffers used by PyMemoryView_FromBuffer() this field is NULL.
	py_callback_buffer.obj = NULL;

	callback_buffer_obj = PyMemoryView_FromBuffer(&py_callback_buffer);


}

PyObject *hello_world() {
	//TODO Turn this into a reusable function. I would hate to be doing this every single time
	Py_Initialize();
	expose_buffers();

	//Get the test data in there
	callback_buffer[0] = 23;

	PyObject *module, *function, *dictionary, *result, *args;

	module = PyImport_ImportModule("MainWindow");
	if(module == NULL){
		log_error("Module not found:");
	}
	dictionary = PyModule_GetDict(module);
	function = PyDict_GetItemString(dictionary, "greet");
	args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, callback_buffer_obj);

	if(!PyCallable_Check(function)){
		log_error("Function not callable:");
	}

	result = PyObject_CallObject(function, args);

	Py_Finalize();
	return Py_None;
}
