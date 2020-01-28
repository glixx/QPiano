![logo](logo/Logotype_horizontal.png)

INTRODUCTION
============

QPiano is a MIDI file player that displays the musical notes AND teaches you how to
play the piano. You can play along to any track in the midi file and QPiano will
follow YOUR playing. QPiano makes sight reading fun!

QPiano is a free (Open Source) program available from: <https://github.com/glixx/QPiano/>

Original work by Louis J. Barman can be found here: <http://pianobooster.sourceforge.net/>

A huge thanks to him and to all the contributers.

WINDOWS
=======

To run QPiano you need a MIDI Piano Keyboard and a MIDI interface for the PC. (If you
don't have a MIDI keyboard you can still try out QPiano using the PC keyboard, 'x' is
middle C - but a MIDI piano is recommend).

To hear the music you will need a General Midi compatible sound synthesizer. Unfortunately
the "Microsoft GS Wavetable software synthesizer" that comes with Windows XP introduces an
unacceptable delay (latency) between pressing a note and hearing the sound. For this reason
the use of "Microsoft GS Wavetable software synthesizer" is not recommend. Please see the
forum for possible solutions.

Double Click on the EXE file of Windows installer for installation.

macOS
=====

To run QPiano you need a MIDI Piano Keyboard and a MIDI-USB interface if the keyboard
does not have a USB interface. (If you don't have a MIDI keyboard you can still try out 
QPiano using the computer's keyboard, 'x' is middle C).

To hear music from the computer you will need a General Midi compatible sound synthesizer.
While there are many possible choices for the Mac (Timidity, FluidSynth or an external MIDI
Synth), the easiest way is to use SimpleSynth, which can be obtained for free (MIT license)
from: <http://notahat.com/simplesynth>

This must be started before QPiano, and also allows to load custom Soundfonts.

Double Click on the DMG file containing QPiano, then just drag the qpiano.app
file wherever you wish (/Application folder is suggested).

LINUX and BSD UNIX
==================

To run QPiano you need a MIDI Piano Keyboard and a MIDI interface for the PC. (If you
don't have a MIDI keyboard you can still try out QPiano using the PC keyboard, 'x' is
middle C - but a MIDI piano is recommend).

To hear music from the computer you will need a General Midi compatible sound synthesizer.
While there are many possible choices for the Linux (Timidity, FluidSynth), the easiest way is
to use Timidity. Timidity is BSD Unix is not workable, please use FluidSynth.

This must be started before QPiano, and also allows to load custom Soundfonts.

Install RPM, DEB, ARCH, TGZ etc. package.

Packages for Linux and BSD Unix are available in repositories of distributions [here](https://pkgs.org/download/qpiano).

Packages for Linux (not for BSD Unix) are available in OBS repository:

[qpiano](https://software.opensuse.org/download.html?project=home%3Aloginov_alex_valer&package=qpiano)

[qpiano-timidity](https://software.opensuse.org/download.html?project=home%3Aloginov_alex_valer&package=qpiano-timidity)

[qpiano-fluidsynth](https://software.opensuse.org/download.html?project=home%3Aloginov_alex_valer&package=qpiano-fluidsynth)

*Note:* The packages provided with distributions may not exist or are out of date,
in which case you will have to compile from the source code.
However compiling from source is quite easy on Linux and BSD Unix.

MIDI FILES
==========

To run QPiano you will need some Midi files preferably with right and left piano
parts on channels 4 and 3. Some high quality demo MIDI files that are compatible with
QPiano are available in doc/courses/*/ directories.

INSTRUCTIONS
============

The MIDI input device is used to connect your piano keyboard to the PC. First ensure that your
piano keyboard is connected up to the PC using a MIDI USB adaptor (or through a PC sound card).
Once the QPiano application starts up you must first setup the midi input and midi
output interface from the Setup menu. Next open a midi file ".mid" or a karaoke ".kar" file
using File/Open from the QPiano menu. Now choose the skill level, if you want to
just listen to the midi music  - select 'listen', to play along with a midi keyboard with
the music following your playing - select 'follow you'. Finally click on the Play icon to
start.

It is recommended that you shut down all other programs while running QPiano so that
the scrolling notes move smoothly across the screen.

TRANSLATIONS
============

QPiano isn't translated in your language? Help us, and we will gladly ship
new translations with it. Use our
[Collaborative Translation Platform](https://www.transifex.com/Magic/qpiano)
(Transifex) to add your favorite languages.

Using Transifex is not mandatory.
You can translate by [Pull Request](https://github.com/glixx/QPiano/pulls).

You need to translate:

[Main program](translations/qpiano_blank.ts)

[Courses](translations/music_blank.ts)

[Desktop file #1](qpiano.desktop)

[Desktop file #2](tools/timidity/qpiano-timidity.desktop)

[Desktop file #3](tools/fluidsynth/qpiano-fluidsynth.desktop)

[Script](tools/fluidsynth/qpiano-fluidsynth)

[Script](tools/timidity/qpiano-timidity)

LICENSE
=======

QPiano is fully copyrighted by the author and all rights are reserved.

QPiano is free software (Open Source software): you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

QPiano is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License in the file "gplv3.txt" or from the web site
<http://www.gnu.org/licenses/> or [gplv3.txt](gplv3.txt).

The QPiano Documentation and Music are released under the Creative Commons License (CC-BY).

[QPiano License](license.txt)

SOURCE CODE
===========

Download and uncompress the archive file below and then follow the instructions in
the *BUILD.md* file.

[QPiano releases](https://github.com/glixx/QPiano/releases)

As part of GNU General Public License the source code for QPiano is available from
the GitHub site  <https://github.com/glixx/QPiano>. See the [BUILD.md](BUILD.md) for
instructions on how to build the source code.

DEBUGGING
=========

To get a backtrace, build QPiano with DEBUG option 

type:

`cmake -DCMAKE_BUILD_TYPE=Debug .`

type:

`gdb ./qpiano`

then:

`run`

then:

`bt`

DOCUMENTATION
=============

[Documentation](doc/README.md)

[Changelog](Changelog.txt)

BUGTRACKER
==========

[Issues](https://github.com/glixx/QPiano/issues)

CONTRIBUTING
============

QPiano is community developing project since 2018. You're welcome with patches:

[Pull Requests](https://github.com/glixx/QPiano/pulls)

We have many [issues](https://github.com/glixx/QPiano/issues) always.

We are looking for musicians to make music for qpiano. It can be educational courses or other melodies: [music sources](music/src). For more information look at [README.md](doc/courses/README.md) and [faq.md](doc/faq.md).

PLATFORMS
=========

We support Linux, BSD Unix and Windows platforms. We do not have macOS developers at the moment.
