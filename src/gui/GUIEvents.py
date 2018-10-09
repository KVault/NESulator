from enum import Enum


class NES_event(Enum):
    ROM_OPEN = 1
    KEY_PRESSED = 2
    APP_QUIT = 3
