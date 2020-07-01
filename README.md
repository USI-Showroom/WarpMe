## What is this repository for?
Hardware accelerated image morphing with barycentric coordinates, developed by Teseo Schneider.


## How do I get set up?
You need:

* C++ compiler
* cmake
* Qt5
* OpenCV

### Linux
* `sudo apt install cmake`
* `sudo apt install qt5-default`
* Download OpenCV from their [Sourceforge page](https://sourceforge.net/projects/opencvlibrary/)

### Mac
* Install XCode from the Apple Store
* Install [homebrew](https://brew.sh/)
* Install cmake with homebrew: `brew install cmake`
* Install Qt5 with homebrew: `brew install qt`
* Install OpenCV with homebrew: `brew install opencv`
* Add Qt to PATH: `echo 'export PATH="/usr/local/opt/qt/bin:$PATH"' >> ~/.bashrc` or `echo 'export PATH="/usr/local/opt/qt/bin:$PATH"' >> ~/.zshrc` depending on your shell
 
### Windows x64
* Install Visual Studio (the community version is free) to obtain the C++ compiler. Select "Desktop development"
* Download and install cmake from its [official website](https://cmake.org/download/)
* Download Qt5 from the [official website](https://www.qt.io/download/). Choose the Open Source version, install it in `C:\` and be 
sure to check the OpenSSL toolkit under "Developer and Designer Tools"
* Download OpenCV from their [Sourceforge page](https://sourceforge.net/projects/opencvlibrary/) and extract in `C:\`
* Set the enviromement variable `QT_PATH` to the installation of Qt (e.g. `C:\Qt\5.15.0\msvc2019_64`)
* Add the Qt, OpenSSL and OpenCV bin folders (e.g. `C:\Qt\5.15.0\msvc2019_64\bin`, `C:\Qt\Tools\OpenSSL\Win_x64\bin` and `C:\opencv\build\x64\vc15\bin`) to PATH and reboot your machine


## Compiling
Use cmake to generate a makefile (on Linux and Mac), or a Visual Studio project (on Windows).

### Linux/Mac
* `mkdir bin`
* `cd bin`
* `cmake ..` (or `cmake .. -DTOUCH_SCREEN_MODE=ON` for the touch mode, requires OpenCV)
* `make`
* `./BarycentricMapping`

### Windows x64
* Open the cmake GUI
* Under "Where is the source code", select the root folder of the project
* Under "Where to build the binaries" select the bin folder in the root directory
* Click "Configure" and choose your version of Visual Studio
* Tick the `TOUCH_SCREEN_MODE` checkbox to enable touch mode
* Click "Generate" until there are no more red lines to generate the Visual Studio Solution
* Click "Open Project" to open it
* In Visual Studio, choose "Release" instead of "Debug"
* Press F7 to compile
* Run `bin\Release\BarycentricMapping.exe` or click the green play button

Note: if it complains about missing DLLs, check that you added the correct folders to PATH


## Run @ USI
See [these instructions](https://github.com/USI-Showroom/WarpMe/blob/master/docs/WarpMe_manual_ITA.pdf)
