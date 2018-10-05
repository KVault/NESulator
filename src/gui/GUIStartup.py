import sys

sys.argv = ['']

import tkinter


def greet():
    print("Hello from fucking Python!")
    return 42


class MainWindow:
    def __init__(self, master):
        self.master = master
        master.title("NESulator")

        self.label = tkinter.Label(master, text="Suck my GTK!")
        self.label.pack()

        self.greet_button = tkinter.Button(master, text="Beat me", command=greet)
        self.greet_button.pack()

        self.close_button = tkinter.Button(master, text="Close", command=master.quit)
        self.close_button.pack()


