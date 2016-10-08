# orchestrack

orchestrack will be a digital audio workstation inspired by music trackers.

# Dependencies

- SDL2

- SDL2\_ttf

- libsndfile

- JACK Audio Connection Kit (on Linux, OS X/macOS and Windows via MinGW) (optional)

# Compiling

## Linux and OS X/macOS

```
mkdir build
cd build
cmake ..
make
```

Under OS X/macOS you may use Xcode as well:

```
mkdir build
cd build
cmake -G "Xcode" ..
xcodebuild
```

## Windows with MinGW

Assuming you have set your Path to MinGW directories:

```
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=mingw32-g++.exe -DCMAKE_MAKE_PROGRAM=mingw32-make.exe ..
mingw32-make.exe
```

## Windows with Visual Studio

1. Retrieve/build all dependencies.

2. Create a new directory named "build" under project directory.

3. Open a command prompt, change dir to "build" and do

```
cmake -DSDL_PATH={pathToSDLLibrary} -DSDL_TTF_PATH={pathToSDLTTFLibrary} -DSNDFILE_PATH={pathToSNDFileLibrary} -DSDL_IPATH={pathToSDLIncludeFiles} -DSDL_TTF_IPATH={pathToSDLTTFIncludeFiles} -DSNDFILE_IPATH={pathToSNDFileIncludeFile} ..
```

4. Load the generated solution under Visual Studio, and build.
