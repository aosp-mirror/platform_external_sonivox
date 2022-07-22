# Fork of the AOSP 'platform_external_sonivox' project to use it outside of Android 

This project is a fork of the AOSP project 'platform_external_sonivox', including a CMake based build system to be used not on Android, but on any other computer Operating System. You may find and use the libraries with pkg-config or the find_package() cmake command.

There is neither MIDI input nor Audio output facilities included in the library. You need to provide your own input/output.

You may find several projects already using this library as a git submodule:

* [Linux-SonivoxEas](https://github.com/pedrolcl/Linux-SonivoxEas) with ALSA Sequencer MIDI input and Pulseaudio output.
* [multiplatform-sonivoxeas](https://github.com/pedrolcl/multiplatform-sonivoxeas) with Drumstick::RT MIDI input and Qt Multimedia audio output.

The build system has two options: BUILD_SONIVOX_STATIC and BUILD_SONIVOX_SHARED to control the generation and install of both the static and shared libraries from the sources.

This fork currently reverts these commits:
* [af41595](https://github.com/pedrolcl/platform_external_sonivox/commit/af41595537b044618234fe7dd9ebfcc652de1576) (Remove unused code from midi engine)
* [34ba480](https://github.com/pedrolcl/platform_external_sonivox/commit/34ba4804f643549b8ac74e5f56bfe64db3234447) (Remove unused code)
* [2fa59c8](https://github.com/pedrolcl/platform_external_sonivox/commit/2fa59c8c6851b453271f33f254c7549fa79d07fb) (Partial Revert of "Build separate sonivox libs with and without jet")

All the sources from the Android repository are kept in place, but some are not built and included in the compiled products. A few headers, mostly empty, are included in the 'fakes' subdirectory, to allow compilation outside Android.

## License

Copyright (c) 2004-2006 Sonic Network Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
