Using the PIC24 Microcontroller
======

Uploading C code to the PIC24 based microcontroller designed for Olin's Elecanisms course is fun and easy. The following guide will take you through how to set up your environment and upload example code onto the board. This was written by Halie Murray-Davis and Emily Tumang as our deliverable for Mini-project 0. 

All instructions assume you're using a Debian derivative flavor of Linux, in our case Ubuntu. They have also been tested in Debian wheezy. 

For this project, we will use a microcontroller based on the PIC24FJ128GB206 designed by [Dr. Minch](https://github.com/bminch) at Olin College for [Elecanisms](http://elecanisms.olin.edu/). 

##Setting up Our Environment
In order to use our code, we must first compile it from our human readable C code into something the microcontroller understands, in this case, a .hex file. Finally, we will load the compiled file onto the board. 

#Compiler
The compiler we will use for this is a non-free fork of the GCC compiler. It can be obtained from [Microchip](http://www.microchip.com/pagehandler/en_us/devtools/mplabxc/). Download the ["MPLAB® XC16 Compiler v1.23"](http://www.microchip.com/mplabxc16linux). Newer is not always better, so don't grab v1.34! It will not work. Once the compiler has downloaded, open up a command prompt and type: 
```bash
cd ~/Downloads
sudo chmod +x ./xc16-v1.24-full-install-linux-installer.run
sudo ./xc16-v1.24-full-install-linux-installer.run
```
#Bootloader
At this point, let's go ahead and get the bootloader. We won't need it right away, but having it now will make thing flow smoother. First download the bootloader with: 
```bash
cd 
wget elecanisms.olin.edu/internal/bootloader.zip
unzip bootloader.zip
```
Now let's go ahead and move the bootloader out of the Downloads folder so we don't accidentally delete it during a purgue of the Downloads folder. This assumes you already have a bin folder in your home directory. If not, it's a nice, useful place to store programs and other important files. You can make one with: "mkdir bin" when in your home directory. 
```bash
mv bootloader ~/bin
```

#USB Serial Connection
Let's also go ahead and get and install [pyusb](http://sourceforge.net/projects/pyusb). This is needed to load code over a USB serial connection to the microcontroller board. Download it from Sourceforge. 
```bash
cd ~/Downloads
unzip pyusb-1.0.0b2.zip
mv pyusb-1.0.0b2 ~/bin
cd ~/bin/pyusb-1.0.0b2/
sudo python setup.py install
```

#Graphical Bootloader Interface
Now let's see if the bootloader will work, at this point, it should, especially if you're using Ubuntu. If you're using Debian, you might still need to install a few extra Python modules. In this guide, we will be using the graphical bootloader. To test and see if it works:
```bash
cd ~/bootloader/software
sudo python bootloadergui.py 
```
If everything worked well and was successfully installed, you should see your terminal “think” for a few seconds and then a graphical user interface should pop up. It should look like:
![Image of the GUI Bootloader on startup](https://github.com/hmurraydavis/elecanisms/blob/master/blink/GUI_Bootloader_opens.jpg)
If this worked for you, fantastic. If it gave you mean error messages, perhaps you'll need to install some of the python modules yourself. In my case python-tk was missing, so I installed it with: 
```bash
sudo apt-get install python-tk
```

#Grant Permissions
Now, since our board is not made by a vendor, we will need to grant the bootloader specific privileges to access it. We do this with:
```bash 
sudo nano /etc/udev/rules.d/usb_prototype_devices.rules
```
Insert the following lines into the file:
```
SUBSYSTEM=="usb", ATTRS{idVendor}=="6666", ATTRS{idProduct}=="4321", MODE=="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="6666", ATTRS{idProduct}=="0003", MODE=="0666"
```
##Cloning the Repository
Next, let's get some code to put on our microcontroller. You can write your own code in C or, (recommended) clone [this](https://github.com/ETumang/elecanisms) or the [parent repository](https://github.com/OlinElecanisms/elecanisms). It's best to confirm everything works and your environment is setup correctly with known working code (like blink.c in the elecanisms repository) before writing your own code. The best way to do this is with SSH keys. These manage security in a very unubiquitous way and are significantly more secure than HTTPS cloning. Additionally, using SSH prevents you from needing to authenticate with your Git-Hub credentials with every commit. However, either method will work, and if you only plan to pull code from the upstream, parent repository, it doesn't make much difference which method you use. If you plan to use SSH and don't already have SSH keys set up, GitHub has fantastic guides on how to [generate](https://help.github.com/articles/generating-ssh-keys/) and [use](https://help.github.com/articles/which-remote-url-should-i-use/#cloning-with-ssh) SSH keys for authentication. No mater your authentication method, getting the code is more or less the same:
```bash
cd ~/__Folder_In_Which_you_Want_the_Repository__
git clone __URL_OF_REPOSITORY__
```
For using SSH and cloning into the home folder (recommended), this would look like: 
```bash
cd
git clone git@github.com:ETumang/elecanisms.git
```
And for HTTPS cloning into the home folder:
```bash
cd
git clone https://github.com/ETumang/elecanisms.git
```

Now that we have the code, let's look at the code file we will be using. Our first task will be to upload the compiled result of blink.c. Blinking an LED is the embedded equivalent to printing 'Hello World!' in other programing languages and is a good starting point for making sure that all components of your system are functioning properly. For this, I will use the text editor nano because it's included with most distributions. Feel free to use another editor. Also, if you didn't clone the repository into your home folder, you will need to cd into that folder.
```bash
cd ~/elecanisms/blink
nano blink.c
```
This should open up some C code in the terminal text editor, Nano. To close this, use the key sequence ctl+X,Y. Look at this code and make sure everything makes sense. 

##Compiling Code
We downloaded the compiler previously in the guide. However, we aren't ready to use it just yet. We first need to edit the SConstruct file to reference this, specific compiler. "cd" to the folder where you cloned the elecanisms repository. If you cloned it into your home directory, the below commands will work. 

```bash
cd ~/elecanisms/blink
nano SConstruct
```
Inside this file, change all instances of pic60 to xc16. This change must be made on lines 3,9, and 13. Additionally, add the flag "-omf=coff" to the command on line 13. Finally, you need to change the path listed in line 8 to the correct path for where your compiler installed so Scons, our build system, can find the compiler. This path will most likely be “/opt/microchip/xc16/v1.24/bin,” however it is wise to confirm this or check it if you are experiencing problems. Alternatively, you can use the [SConstruct file](https://github.com/ETumang/elecanisms/blob/master/blink/SConstruct) provided in our repository. 

##Build System
To manage the compilation of multi-filed code, we will use Scons, a python based build system. This is a tool which will help automate compilation. To obtain Scons, type into a terminal: 
```bash
sudo apt-get install scons
```

To compile code with scons, you need to be in the folder of the files you want to compile. To compile the blink code in the elecanisms repository (assuming you cloned the repository into your home directory):
```bash
cd ~/elecanisms/blink
scons
ls
```
The output of this should be some messages from the compiler. Then you should see a file named blink.hex. This is a representation of the C code in blink.c as machine code in hexadecimal numbers. This the microcontroller can understand and run once we load this file onto the board. 


##Loading Code to the Board
Now that we can compile our code into something that won't seem wacky to our microcontroller, let's try putting it on the board! Before beginning this section, you should be able to generate a .hex file from a C code source file using the build system scons. To do this, we will use a bootloader. This is a Python tool with both GUI functionality and a command line interface, and as such takes the form of a Python script in the bootloader folder you unzipped earlier. In this guide, we're using the graphical interface. 

#Opening the Bootloader
Assuming you followed the previous instructions, you should be able to run the bootloader from your /bin/bootloader/software folder, or wherever you unzipped it to.
 ```bash
 cd
 cd bin/bootloader/software
 python bootloadergui.py
 ```
 This should open the familiar window:
 
 ![Image of the GUI Bootloader on startup](https://github.com/hmurraydavis/elecanisms/blob/master/blink/GUI_Bootloader_opens.jpg)
 
*Replacing "bootloadergui" with "bootloadercmd" in the above code opens a command-line version of the bootlaoder with the same functionality. This document will assume you are using the GUI.
 
#Connecting to the Board
If you plugged the board in before opening the bootloader, your bootloader will display the message "Could not connect...". Reset the board by holding switch 1 (the one next to the red reset button) and pressing said red button,. Release the red, reset button while holding down switch one. Hold switch one down for a few seconds as the board powers up. These switches can be seen in the diagram below: 
![Board's Switches Diagram](https://github.com/hmurraydavis/elecanisms/blob/master/blink/Pic24_board_diagram.jpg)

Click "Connect" in the bootloader window. If you did not plug in the board before launching the bootloader, do so now and click "Connect". You should see the screen below.

![GUI bootloader on connection](https://github.com/hmurraydavis/elecanisms/blob/master/blink/Connected_to_Elecanisms_Board_GUI_Bootloader.jpg)

#Uploading a .hex File  
The .hex file to upload seems oddly uniform. That is because the hex file representing the blink code hasn't been loaded into the bootloader yet- the bootloader can be used for any compiled C file (.hex), and doesn't know which one to load automatically. To choose the file to load, click on "File" in the top left corner and choose the "Import Hex" option. A small window should pop up. Navigate to the "blink" folder and select blink.hex. Hit the "Open" button, closing the popup and loading blink.hex as the file to be uploaded to the board. 

Finally, click "Write" to actually upload the blink code. It should take a few seconds, and then display the confirmation message below.

![Gui after code write](https://github.com/hmurraydavis/elecanisms/blob/master/blink/Write_Successful_GUI_Bootloader.jpg)

#Run and Test Code
In order to test that the correct code was uploaded, you can either hit the board's red Reset button again, or click "Disconnect/Run" in the bootloader. You should see the three small LEDs on the board flashing in sequence from blue to green to red and then back to blue.  If you do, congratulations! 
