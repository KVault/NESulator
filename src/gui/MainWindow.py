from tkinter import *
from tkinter import filedialog


def greet(callback_memoryview : memoryview):
    print("At least I got here")
    print(callback_memoryview[0])


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

    def open_rom(self):
        self.master.rom_dialog = filedialog.askopenfilename(initialdir="/", title="Select file", filetypes=(
            ("NES files", "*.nes"), ("all files", "*.*")))


def run_main_window():
    root = Tk()
    MainWindow(root)
    root.mainloop()
