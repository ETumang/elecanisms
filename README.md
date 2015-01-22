master
======

Master repository for ENGR3199 includes C and Python source code and supporting files

All instructions assume you're using a Debian derivative flavor of Linux, in our case Ubuntu. 

##Compiling
The compiler is a non-free fork of the GCC compiler. It can be obtained from [Microchip](http://www.microchip.com/pagehandler/en_us/devtools/mplabxc/). Download the ["MPLAB® XC16 Compiler v1.23"](http://www.microchip.com/mplabxc16linux). Newer is not always better, so don't grab v1.34! It will not work. Once the compiler has downloaded, open up a command prompt and type: 
```bash
cd ~/Downloads
sudo chmod +x ./xc16-v1.24-full-install-linux-installer.run
sudo ./xc16-v1.24-full-install-linux-installer.run
```


##Loading Code to the Board
First, grab the latest version of [pyusb](http://sourceforge.net/projects/pyusb/?source=typ_redirect) from Sourceforge. Once you have this, 
```bash
mv ~/Downloads/pyusb-1.0.0b2 ~/bin
cd ~/bin/pyusb-1.0.0b2
sudo python setup.py
```
