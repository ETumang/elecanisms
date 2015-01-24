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

##Bootloader
Now that we can compile our code into something that won't seem wacky to our microcontroler, let's try putting it on the board! To do this, we will use a bootloader. This is a Python build system with both GUI functionality and a command line interface. To install Scons, type into a command prompt:

##Loading Code to the Board
First, grab the latest version of [pyusb](http://sourceforge.net/projects/pyusb/?source=typ_redirect) from Sourceforge. Once you have this, 
```bash
mv ~/Downloads/pyusb-1.0.0b2 ~/bin
cd ~/bin/pyusb-1.0.0b2
sudo python setup.py
```
