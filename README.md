nrsc5_gui
=================

A qt gui that receives and displays hd radio from a rtl-sdr (want to support more soon) using nrsc5.

### Problems with the original gui version
It was made in Python with dependencies that are linux only or are hard to install on Windows.

### Dependencies
- CMake
- Qt6 
- Qt6Sql (for sqlite3)
  - I want to remove this in the future. This is a cry for help. 
- nrsc5 (my fork of the original nrsc5)
- libusb (from [PortSDR](https://github.com/TheDaChicken/PortSDR/))
- librtlsdr (from [PortSDR](https://github.com/TheDaChicken/PortSDR/))
- PortAudio (Cmake automatically downloads)
- spdlog (used with fmt) (Cmake automatically downloads)

### Building nrsc5

This requires the nrsc5 fork I made with better buffering.
You can compile it just like the original nrsc5, but with the elastic-buffering branch.

```bash
... # Follow the original nrsc5 build instructions
git clone https://github.com/TheDaChicken/nrsc5/
cd nrsc5
git checkout elastic-buffering
... # Follow the original nrsc5 build instructions
```

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