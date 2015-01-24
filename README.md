master
======

Master repository for ENGR3199 includes C and Python source code and supporting files

All instructions assume you're using a Debian derivative flavor of Linux, in our case Ubuntu. They have also been tested in Debian weasy. 

For this project, we will use a microcontroler based on the PIC24FJ128GB206 designed by [Dr. Minch](https://github.com/bminch) at Olin College for [Elecanisms](http://elecanisms.olin.edu/). 

##Compiling
In order to use our code, we must first compile it from our human readable code into something the microcontroler understands, in this case, a .hex file. The compiler we will use for this is a non-free fork of the GCC compiler. It can be obtained from [Microchip](http://www.microchip.com/pagehandler/en_us/devtools/mplabxc/). Download the ["MPLAB® XC16 Compiler v1.23"](http://www.microchip.com/mplabxc16linux). Newer is not always better, so don't grab v1.34! It will not work. Once the compiler has downloaded, open up a command prompt and type: 
```bash
cd ~/Downloads
sudo chmod +x ./xc16-v1.24-full-install-linux-installer.run
sudo ./xc16-v1.24-full-install-linux-installer.run
```
At this point, let's go ahead and get the bootloader. We won't need it right away, but having it now will make thing flow smoother. First download the bootloader with: 
```bash
cd 
wget elecanisms.olin.edu/internal/bootloader.zip
unzip bootloader.zip
```
Now let's go ahead and move the bootloader out of the Downloads folder so we don't accidetnly delete it during a purgue. This assumes you already have a bin folder in your home directory. If not, it's a nice, useful place to store programs and other important files. You can make one with: "mkdir bin" when in your home directory. 
```bash
mv bootloader ~/bin
```

Let's also go ahead and get and install [pyusb](http://sourceforge.net/projects/pyusb). This is needed to load code over a USB serial connection to the microcontroler board. Download it from Sourceforge. 
```bash
unzip pyusb-1.0.0b2.zip
mv pyusb-1.0.0b2 ~/bin
cd ~/bin/pyusb-1.0.0b2/
sudo python setup.py install
```

Now, since our board is not made by a vendor, we will need to grant the bootloader specific privledge to access it. We do this with:
```bash 
sudo nano /etc/udev/rules.d/usb_prototype_devices.rules
```
Insert the following lines into the file:
```
SUBSYSTEM=="usb", ATTRS{idVendor}=="6666", ATTRS{idProduct}=="4321", MODE=="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="6666", ATTRS{idProduct}=="0003", MODE=="0666"
```

Once we have the compiler, we need to edit the SConstruct file to reference this compiler. "cd" to the folder where you cloned the elecanisms repository, if you cloned it into your home directory, the below comands will work. 

```bash
cd
cd elecanisms/blink
nano SConstruct
```
Inside this file, change all instances of pic60 to x16. This change must be made on lines 3,9, and 13. Aditionally, add the flag "-omf=coff" to the command on line 13. Alternatively, you can use the SConstruct file provided in our repository. 
##Build System
To manage the compilation of multi-filed code, we will use Scons, a python based build system. This is a build system tool which will help automate compiation. To obtain Scons, type into a terminal: 
```bash
sudo apt-get install scons
```

##Cloning the Repository
Next, let's get some code to put on our microcontroler. You can write your own code in C or, (recomended) clone [this](https://github.com/ETumang/elecanisms) or the [parent repository](https://github.com/OlinElecanisms/elecanisms). The best way to do this is with SSH keys. These manage security in a very unubiquitous way and are significantly more secure than HTTPS cloning. Aditionlly, using SSH prevents you from needing to authenticate with your git-hub credentials with every commit. However, either method will work, and if you only plan to pull code from the upstream, parent repository, it doesn't make much difference which method you use. If you plan to use SSH and don't already have SSH keys setup, GitHub has fantastic guides on how to [generate](https://help.github.com/articles/generating-ssh-keys/) and [use](https://help.github.com/articles/which-remote-url-should-i-use/#cloning-with-ssh) SSH keys for authentication. No mater your authentication method, getting the code is more or less the same:
```bash
cd ~/__Folder_In_Which_you_Want_the_Repository__
git clone __URL_OR_REPOSITORY__
```
For using SSH and cloning into the home folder, this would look like: 
```bash
cd
git clone git@github.com:ETumang/elecanisms.git
```
And for HTTPS cloning into the home folder:
```bash
cd
git clone https://github.com/ETumang/elecanisms.git
```

Now that we have the code, let's look at the code file we will be using. Our first task will be to upload the compiled result of blink.c. Blinking an LED is the embedded equivelent to printing 'Hellow World!' in other languagues and is a good starting point for making sure that all components of your system are functioning properly. For this, I will use the text editor nano because it's included with most distrabutions. Feel free to use another editor. Also, if you didn't clone the repository into your home folder, you will need to cd into that folder.
```bash
cd 
cd elecanisms/blink
nano blink.c
```
This should open up some C code in the terminal text editor, Nano. To close this, use the key sequence ctl+X,Y. Look at this code and make sure everything makes sense. 


##Bootloader
Now that we can compile our code into something that won't seem wacky to our microcontroler, let's try putting it on the board! To do this, we will use a bootloader. This is a Python build system with both GUI functionality and a command line interface. To install Scons, type into a command prompt:

##Loading Code to the Board
First, grab the latest version of [pyusb](http://sourceforge.net/projects/pyusb/?source=typ_redirect) from Sourceforge. Once you have this, 
```bash
mv ~/Downloads/pyusb-1.0.0b2 ~/bin
cd ~/bin/pyusb-1.0.0b2
sudo python setup.py
```
