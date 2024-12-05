
from bluetooth import btfpy

def init_bluetooth_and_pair(devices_file):
    if btfpy.Init_blue(devices_file) == 0:
        exit(0)

    random_addr = [0xD3, 0x56, 0xDB, 0x24, 0x32, 0xA0]
    btfpy.Set_le_random_address(random_addr)
    btfpy.Set_le_wait(5000)
    btfpy.Le_pair(btfpy.Localnode(), btfpy.JUST_WORKS, 0)