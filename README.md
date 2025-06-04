nrsc5_gui
=================

A qt gui that receives and displays hd radio from a rtl-sdr (want to support more soon) using nrsc5.

### Problems with the original gui version
It was made in Python with dependencies that are linux only or are hard to install on Windows.

### Dependencies
- CMake
- Qt6
- nrsc5
- libusb (from [PortSDR](https://github.com/TheDaChicken/PortSDR/))
- librtlsdr (from [PortSDR](https://github.com/TheDaChicken/PortSDR/))
- PortAudio (Cmake automatically downloads)
- fmt (used with spdlog) (Cmake automatically downloads)
- spdlog (used with fmt) (Cmake automatically downloads)
- nlohmann/json (Cmake automatically downloads)
- SQLite (embedded into the project)

The build instructions assumes that libnrsc5 is compiled and able to be found by CMake. 

### Building on Ubuntu

```bash
sudo apt-get install cmake qt6-base-dev librtlsdr-dev
mkdir build
cd build
cmake ..
make
sudo make install
```

### Building on Windows using MSVC

Make sure to add nrsc5's dependencies to the PATH.

```bash
sudo pacman -S mingw-w64-x86_64-qt6-base
mkdir build
cd build 
cmake ..
make
make install
```