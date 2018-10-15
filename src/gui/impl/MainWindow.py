from tkinter import *
from tkinter import filedialog
from src.gui.guicython import py_cycles
import threading


class MainWindow:
    def __init__(self, master):
        self.master = master
        master.title("NESulator")

        # Now for the menu bar
        self.menu = Menu(master)
        self.filemenu = Menu(self.menu, tearoff=0)
        self.filemenu.add_command(label="Open", command=self.open_rom)
        self.menu.add_cascade(label="File", menu=self.filemenu)

        master.config(menu=self.menu)

        # CLock label
        self.clock_speed = Label()

    def open_rom(self):
        self.master.rom_filename = filedialog.askopenfilename(initialdir="/", title="Select file", filetypes=(
            ("NES files", "*.nes"), ("all files", "*.*")))

        # Now with the filename, start the emulator!!!
        # self.master.rom_filename

    def update_clock_speeds(self):
        # Use this to schedule another run in 60 seconds
        threading.Timer(60.0, self.update_clock_speeds).start()
        speed = py_cycles()
        self.clock_speed.text = "CPU Speed: " + format(speed/1000000)



def run_main_window():
    root = Tk()
    MainWindow(root)
    root.mainloop()
