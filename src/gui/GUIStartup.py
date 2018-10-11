import importlib

from src.gui import MainWindow
from src.gui import NESulator

importlib.import_module("MainWindow")
importlib.import_module("NESulator")


def main():
    MainWindow.run_main_window()


if __name__ == '__main__':
    main()
