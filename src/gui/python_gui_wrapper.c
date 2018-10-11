#include "python_gui_wrapper.h"
#include "../cpu.h"

static PyMethodDef NESulator_methods[] = {
		{"cycle_count",  get_cycle_count, METH_NOARGS, "Retrieves the current cycle count"},
		{NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef NESulator_module = {
		PyModuleDef_HEAD_INIT,
		"NESulator",    /* name of module */
		NULL,           /* module documentation, may be NULL */
		1,             /* size of per-interpreter state of the module,
                           or -1 if the module keeps state in global variables. */
		NESulator_methods
};


PyMODINIT_FUNC PyInit_NESulator(void)
{
	printf("Hey");
	return PyModule_Create(&NESulator_module);
}


static PyObject *get_cycle_count(PyObject *self, PyObject *args) {
	return PyLong_FromLong(cpu_cyclesThisSec);
}

void expose_buffers(){
	//callback_buffer_obj = create_memoryview(callback_buffer_obj, callback_buffer, 10 * sizeof(int), sizeof(int));
}
/*
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
}*/

