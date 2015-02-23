import usb.core

class usbStates:

    def __init__(self):
        self.SPRING = 0
        self.DAMPER = 1
        self.TEXTURE = 2
        self.WALL = 3

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None
    def setMode(self, number):
        try:
            self.dev.ctrl_transfer(0x40, 0,number)
            print(number)
        except usb.core.USBError:
            print "Could not set board mode."

    def updateVal(self, newVal,selector):
        try:
            self.dev.ctrl_transfer(0x40, 1, newVal,selector)
            print(newVal)
        except usb.core.USBError:
            print "Could not set parameter."






