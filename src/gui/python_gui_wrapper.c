#include "python_gui_wrapper.h"
#include "../utils/Utils.h"

int callback_buffer[10];
PyObject *callback_buffer_obj;

void *start_gui(void *arg){
	hello_world();
	expose_buffers();

	return NULL;
}

void expose_buffers(){
	callback_buffer_obj = create_memoryview(callback_buffer_obj, callback_buffer, 10 * sizeof(int), sizeof(int));
}

PyObject *create_memoryview(PyObject *memoryview,void *buffer, long size, long item_size){
	Py_buffer py_buffer = {};
	py_buffer.len = size;
	py_buffer.buf = buffer;
	py_buffer.itemsize = item_size;
	py_buffer.readonly = FALSE;
	py_buffer.format = NULL; //Unsigned bytes assumed (For now)
	py_buffer.ndim = 1;//1 Dimension array. Do we need more? Maybe not right now
	py_buffer.obj = NULL;//This one can be null if used from PyMemoryView_FromBuffer. this should be the case

	memoryview = PyMemoryView_FromBuffer(&py_buffer);
	return memoryview;
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
