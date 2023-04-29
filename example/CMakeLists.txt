#[=========================================================================[
  Copyright (c) 2022-2023 Pedro López-Cabanillas

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
]=========================================================================]

cmake_minimum_required(VERSION 3.14)
project( sonivoxrender LANGUAGES C )

# Alternative 1: Using CMake only and sonivox installed:
find_package ( sonivox CONFIG REQUIRED )
add_executable ( sonivoxrender sonivoxrender.c )
target_compile_definitions( sonivoxrender PRIVATE _LARGEFILE64_SOURCE )
target_link_libraries ( sonivoxrender sonivox::sonivox-static )

# Alternative 2: Using pkg-config and sonivox installed:
#find_package ( PkgConfig REQUIRED )
#pkg_check_modules ( sonivox REQUIRED IMPORTED_TARGET sonivox-static )
#add_executable ( sonivoxrender sonivoxrender.c )
#target_link_libraries( sonivoxrender PkgConfig::sonivox )

# Alternative 3: sonivox in a subdirectory (for instance: a git submodule )
#add_subdirectory( sonivox )
#add_executable ( sonivoxrender sonivoxrender.c )
#target_link_libraries ( sonivoxrender sonivox::sonivox-static )
