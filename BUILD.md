# Linux and BSD Unix

Ensure that the build required packages are installed. Full list of them you can find [here](pkgs).

Then type:

`cmake .`

or

`cmake . -DCMAKE_INSTALL_PREFIX="/usr"`

`make`

Finally as root type:

`make install -C build`

(The qpiano binary executable is now in the "build" subdirectory.)

To build a debug version create a dir called "debug" and change to that dir and then type:

`cmake -DCMAKE_BUILD_TYPE=Debug .`

(Alternatively you can use qmake followed by make.)

See [DEB](pkgs/deb) for more details.

See [RPM](pkgs/rpm/qpiano.spec) for more details.

See [ARCH](pkgs/arch/PKGBUILD) for more details.

See [TGZ](pkgs/slack) for more details.

# macOS

Install latest Xcode (from Apple Developer Connection, free registration required).

Install CMake and QT libraries via Homebrew:

`$ brew install cmake qt5 ftgl jack`

Generate XCode project file via CMake:

`$ cmake -G Xcode . -DCMAKE_PREFIX_PATH=$(brew --prefix qt)`

Open the project file in XCode, set whatever options you like (universal or single architecture,
debug or release etc.) and compile.

To make a self contained application bundle use QT's macdeployqt tool (included in QT 4.5.0).

# Windows

To compile in Windows install the Open Source version of Qt and CMake and optionally Geany.
When installing Qt select the option to download and install the MinGW compiler. Open the
Qt Command Prompt and then change to the "QPiano" directory and then type the
command below:

`"C:\Program Files\CMake 2.6\bin\cmake.exe" -G "MinGW Makefiles" .`

Once this is completed type:

`make`

Or alternatively you can install QtCreator and then open the qpiano.pro.

# Build options

**USE_FTGL**: link with ftgl; enabled by default; disabling notes localization.

**NO_DOCS**: do not install documents; disabled by default.

**NO_LICENSE**: do not install license; disabled by default.

**NO_CHANGELOG**: do not install changelog; disabled by default.

**WITH_MAN**: install man page; disabled by default.

**WITH_TIMIDITY**: install desktop file and wrapper script for timidity; disabled by default.

**WITH_FLUIDSYNTH**: install desktop file and wrapper script for fluidsynth; disabled by default.

**INSTALL_ALL_LANGS**: install all languages; disabled by default for cmake and always enabled for qmake.

**USE_SYSTEM_RTMIDI**: build with system (not bundled) rtmidi; disabled by default.

**USE_FLUIDSYNTH**: enable setting for fluidsynth in GUI; disabled by default; enable this option if your system has fluidsynth (Linux, BSD Unix, macOS) even if you do not have plans to install fluidsynth.

**USE_TIMIDITY**: enable setting for timidity in GUI; disabled by default; enable this option if your system has timidity (Linux, BSD Unix(?)) even if you do not have plans to install timidity.

**USE_SYSTEM_FONT**: do not use and do not install bundled font, use system font instead; enabled by default.

**USE_FONT**: build with specified font; null by default.

**DATA_DIR**: build with specified data directory; "share/games/qpiano" is default.

**NO_LANGS**: do not install languages; disabled by default.
