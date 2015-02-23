from gi.repository import Gtk
import usbComm
import numpy


SPRING = 0
DAMPER = 1
TEXTURE = 2
WALL = 3

getData = False
pos = []


#usb = usbComm.usbStates();

def onButton(button, number):
	#usb.setMode(number)
	if number == 0:
		dial.set_adjustment(spring_val)
		sbox.hide()
		main_buffer.set_text("Spring Constant")
	elif number == 1:
		dial.set_adjustment(damper_val)
		sbox.hide()
		main_buffer.set_text("Damping Constant")
	elif number == 2:
		print("texture")
	elif number == 3:
		main_buffer.set_text("Wall Position")
		dial.set_adjustment(wall_loc)
		sec_buffer.set_text("Wall Hardness")
		sbox.show()


def updateValue(button,which):
	#usb.updateVal(dial.get_adjustment().get_value(),which)
	print dial.get_adjustment().get_value()

def dataController(button):
	getData = not getData
	if not getData:
		numpy.save('output',pos)

# damper_button = Gtk.GtkRadioButton(spring_button, "Damper Mode")
# damper_button.show

win = Gtk.Window(title = "Joystick Control")

grid = Gtk.Grid()
win.add(grid)
hbox = Gtk.Box(spacing=6)
grid.add(hbox)
sbox = Gtk.Grid()

spring_val= Gtk.Adjustment(100, 0, 128,1)
damper_val = Gtk.Adjustment(100, 0, 128,1)
wall_loc = Gtk.Adjustment(0, 0,10,1)
wallhardness = Gtk.Adjustment(0,0,10,1)

main_buffer = Gtk.TextBuffer()
main_buffer.set_text("Spring Constant")
mainView = Gtk.TextView()
mainView.set_buffer(main_buffer)
grid.attach(mainView,0,1,1,1)


sec_buffer = Gtk.TextBuffer()
sec_buffer.set_text("Wall Hardness")
secView = Gtk.TextView()
secView.set_buffer(sec_buffer)

dial2 = Gtk.HScale()
dial2.set_adjustment(wallhardness)
dial2.connect("value_changed", updateValue,1)


sbox.add(secView)
sbox.attach(dial2,0,1,1,1)

dial = Gtk.HScale()
dial.set_adjustment(spring_val)
dial.connect("value_changed", updateValue,0)

grid.attach(dial, 0,2,1,1)

grid.attach(sbox,0,3,10,2)
sbox.hide()

data_button = Gtk.Button(label = "Collect Data")
data_button.connect("clicked", dataController)
grid.attach(data_button,0,4,1,1)



spring_button = Gtk.RadioButton.new_with_label_from_widget(None, "Spring Mode")
spring_button.connect("toggled",onButton, 0)
hbox.pack_start(spring_button, False, False, 0)

damper_button = Gtk.RadioButton.new_with_label_from_widget(spring_button, "Damper Mode")
damper_button.connect("toggled", onButton, 1)
hbox.pack_start(damper_button, False, False, 0)


texture_button = Gtk.RadioButton.new_with_label_from_widget(spring_button, "Texture Mode")
texture_button.connect("toggled", onButton, 2)
hbox.pack_start(texture_button, False, False, 0)

wall_button = Gtk.RadioButton.new_with_label_from_widget(spring_button, "Wall Mode")
wall_button.connect("toggled", onButton, 3)
hbox.pack_start(wall_button, False, False, 0)

win.connect("delete-event", Gtk.main_quit)
win.show_all()


while True:
    if (getData):
    	pos.append(usbComm.getVal()[0])


    while Gtk.events_pending():
        Gtk.main_iteration()

