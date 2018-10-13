from src.gui.impl import MainWindow
from src.gui.guicython import py_cycles


def main():
    print(py_cycles())
    MainWindow.run_main_window()


if __name__ == '__main__':
    main()
