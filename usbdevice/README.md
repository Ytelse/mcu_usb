# TOOLCHAIN

Get the arm-none-eabi toolchain here -- https://launchpad.net/gcc-arm-embedded/+download

Get SEGGER JLink here -- https://www.segger.com/downloads/jlink

# Makefile

A few variables have to be edited in order to make the project.

### SDK_PATH

Set this to point to the exx32 SDK released by Silicon Labs.

On MacOSX this is typically "/Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/exx32/vx.x.x".

Replace vx.x.x with current version (as of now v4.4.1).

### TOOLCHAIN\_PATH

Set this to point to the folder containing the toolchain binaries (see TOOLCHAIN section).

On my system this is "/usr/local/arm-none-eabi/gcc-arm-none-eabi-5_4-2016q3/bin".

### JLINK\_PATH

Set this to point to the folder containing JLinkExe. (See JLink section)

### CSRC

All files implementing your includes (including emlib and so on!) must be added to this list, as they need to be built and linked along with the source files.

### TARGET

Set this to whatever you want your binary to be called.

### JLink

The makefile is set up to automagically create a commanderscript for JLink in order to be able to flash the EFM32 with only "make upload".

In order for this to work, JLinkExe has to be installed (see TOOLCHAIN section).

The makefile assumes that JLinkExe can be found at the default install location. Change this if necessary.
