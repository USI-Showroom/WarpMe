## What is this repository for?
Hardware accelerated image morphing with barycentric coordinates, developed by Teseo Schneider.


## How do I get set up?
You need:

* C++ compiler
* cmake
* Qt5
* OpenCV (optional)

### Linux
Use your favorite package manager (e.g. yum, apt-get, etc) to install the three dependencies. 

### Mac
* Install XCode from the Apple Store
* Install [homebrew](https://brew.sh/)
* Install cmake with homebrew: `brew install cmake`
* Install qt5 with homebrew: `brew install qt`
* Install opencv with homebrew: `brew install opencv` (optional)
* Add OpenCV to PATH: `echo 'export PATH="/usr/local/opt/qt/bin:$PATH"' >> ~/.zshrc` (optional)
 
### Windows
* Install Visual Studio (the express or community version are free) to obtain the C++ compiler. Use the advanced installation and, under languages, select visual C++
* Download and install cmake from its [official website](https://cmake.org/download/)
* Download Qt5.* from the [official website](https://www.qt.io/download/) and install in C:\\. Careful: install the 32bit version compatible with your C++ compiler
* Set the enviromement variable QT_PATH to the installation of Qt (e.g. C:\\Qt\\5.7\\msvc2015)


## Compiling
Use cmake to generate a makefile (on Linux and Mac), or a visual studio project (on windows).

### Mac/Linux
* `mkdir bin`
* `cd bin`
* `cmake -S .. -B .` (or `cmake -S .. -B . -DTOUCH_SCREEN_MODE=ON` for the touch mode, requires OpenCV)
* `make`
* `./BarycentricMapping`

### Windows
* Open the cmake gui
* Under "Where is the source code", select the root folder of the project
* Under "Where to build the binaries" select the bin folder in the root directory
* Click configure and choose you version of Visual Studio
* Click generate to generate the Visual Studio Solution
* In the bin folder open "barycentricMapping.sln"
* In Visual Studio right click on the project "barycentricMapping" within the solution, and select "Set as startup project"
* Press the play button to compile and run
* If it complains about missing dll's, open the bin/Debug or bin/Release folder and copy all the necessary dll's from QT_PATH/bin (e.g. C:\\Qt\\5.7\\msvc2015\\bin)


## Run @ USI
See [pdf file in /docs](https://github.com/USI-Showroom/WarpMe/blob/master/docs/WarpMe_manual_ITA.pdf)
