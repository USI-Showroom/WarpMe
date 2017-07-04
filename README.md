# README #

## What is this repository for? ##

Hardware accelerated image morphing with barycentric coordinates

## How do I get set up? ##

You need:

* C++ compiler
* cmake
* Qt5

### Linux

Use your favorite package manager (e.g., yum, apt-get, etc...) to install the three dependencies. 


### Mac

* Install XCode from the apple store.
* Install macports (https://www.macports.org/).
* Install cmake with macports, on the terminal type sudo port install cmake.
* Install qt5 with macports, on the terminal type sudo port install qt5.
 

### Windows

* Install Visual Studio, the express or community version are free, to obtain the C++ compiler. Use the advanced installation and, under languages, select visual C++.
* Download and install cmake from its official website (https://cmake.org/download/).
* Download Qt5.* from http://www.qt.io/download/ and install in C:\\. Carefull: install the 32bit version compatible with your C++ compiler.
* Set the enviromement variable QT_PATH to the installation of Qt (e.g., C:\\Qt\\5.7\\msvc2015).

## Compiling
Use cmake to generate make files (on unix), xcode project (on mac), or visual studio project (on windows).

### Mac/Linux
* Navigate with the terminal to the bin directory.
* Type cmake .. (or on mac to generate the xcode project type cmake -GXcode ..).
* Build the project from the makefile or the generated project.

###Windows
* Open the cmake gui
* Under "Where is the source code", select the root folder of the project (e.g., barycentricMapping).
* Under "Where to build the binaries" select the bin folder in the root directory.
* Click configure and choose you version of visual studio
* Click generate to generate the Visual Studio Solution
* In the bin folder open "barycentricMapping.sln"
* In Visual Studio right click on the project "barycentricMapping" within the solution, and select "set as startup project"
* Press the play button to compile and run
* If it complains about missing dll's, open the bin/Debug or bin/Release folder and copy all the necessary dll's from QT_PATH/bin (e.g., C:\\Qt\\5.7\\msvc2015\\bin).

# Run @ USI #

* Turn the screen in portrait mode (buttons on the bottom)
* Attach the webcam on the velcro (right)
* Plug the printer
* Use the user warpme, and run the shortcut on desktop