import kivy

kivy.require('1.0.7')

from kivy.app import App
from kivy.uix.button import Button


class MainWindow(App):
    def build(self):
        return Button(text='Hello from fucking Python!')


def main():
    MainWindow().run()


main()
