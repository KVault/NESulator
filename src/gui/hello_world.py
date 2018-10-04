import sys

sys.argv = ['']

import tkinter


class MainWindow:
    def __init__(self, master):
        self.master = master
        master.title("NESulator")

        self.label = tkinter.Label(master, text="Suck my GTK!")
        self.label.pack()

        self.greet_button = tkinter.Button(master, text="Beat me", command=self.greet)
        self.greet_button.pack()

        self.close_button = tkinter.Button(master, text="Close", command=master.quit)
        self.close_button.pack()

    def greet(self):
        print("Hello from fucking Python!")


if __name__ == '__main__':
    root = tkinter.Tk()
    my_gui = MainWindow(root)
    root.mainloop()
