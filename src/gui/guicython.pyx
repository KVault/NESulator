cdef extern from "impl/lib/gui.h":
    int get_cycle_count()
    void open_rom(char *filename)

def py_cycles() -> int:
    return get_cycle_count()

def open_rom(filename: bytes) -> None:
    open_rom(filename)
