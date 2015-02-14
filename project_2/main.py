from gi.repository import Gtk
import usbComm


SPRING = 0
DAMPER = 1
TEXTURE = 2
WALL = 3

usb = usbComm.usbStates();

def onButton(button, number):
	if number == 0:
		usb.spring()
	elif number == 1:
		usb.damper()
	elif number == 2:
		usb.texture()
	elif number == 3:
		usb.wall()
	

# damper_button = Gtk.GtkRadioButton(spring_button, "Damper Mode")
# damper_button.show

win = Gtk.Window(title = "Joystick Control")

grid = Gtk.Grid()
win.add(grid)
hbox = Gtk.Box(spacing=6)
grid.add(hbox)


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
Gtk.main()

