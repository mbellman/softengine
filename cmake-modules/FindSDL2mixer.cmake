# - Locate SDL2_mixer library
# This module defines:
#  SDL2_MIXER_FOUND - System has SDL2_mixer
#  SDL2_MIXER_INCLUDE_DIRS - SDL2_mixer include directories
#  SDL2_MIXER_LIBRARIES - Libraries needed to link against SDL2_mixer

SET(SDL2_MIXER_SEARCH_PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  C:/mingw-dev/SDL2_mixer
)

find_path(SDL2_MIXER_INCLUDE_DIR SDL_mixer.h
  PATH_SUFFIXES include/SDL2 include
  PATHS ${SDL2_MIXER_SEARCH_PATHS}
)

find_library(SDL2_MIXER_LIBRARY
  NAMES SDL2_mixer
  PATH_SUFFIXES lib
  PATHS ${SDL2_MIXER_SEARCH_PATHS}
)

set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})
set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  SDL2_mixer
  DEFAULT_MSG SDL2_MIXER_LIBRARIES SDL2_MIXER_INCLUDE_DIRS)

mark_as_advanced(SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR)
