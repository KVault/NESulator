import sys
import importlib

from src.gui import MainWindow

importlib.import_module("MainWindow")

sys.argv = ['']  # Apparently if being called from C, we need to do this


def main():
    MainWindow.run_main_window()


if __name__ == '__main__':
    main()
