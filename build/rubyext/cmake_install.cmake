# Install script for directory: /Users/itoyamato/Downloads/software/ComptonSoft/rubyext

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/itoyamato")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/itoyamato/lib/ruby/comptonsoft/comptonSoft.bundle")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/itoyamato/lib/ruby/comptonsoft" TYPE MODULE FILES "/Users/itoyamato/Downloads/software/ComptonSoft/build/rubyext/comptonSoft.bundle")
  if(EXISTS "$ENV{DESTDIR}/Users/itoyamato/lib/ruby/comptonsoft/comptonSoft.bundle" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/itoyamato/lib/ruby/comptonsoft/comptonSoft.bundle")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/itoyamato/Downloads/software/ComptonSoft/build/source/astroh"
      -delete_rpath "/Users/itoyamato/Downloads/software/ComptonSoft/build/source/simulation"
      -delete_rpath "/Users/itoyamato/Downloads/software/ComptonSoft/build/source/modules"
      -delete_rpath "/Users/itoyamato/Downloads/software/ComptonSoft/build/source/core"
      -delete_rpath "/Users/itoyamato/Downloads/software/ComptonSoft/build/anlgeant4"
      "$ENV{DESTDIR}/Users/itoyamato/lib/ruby/comptonsoft/comptonSoft.bundle")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/itoyamato/lib/ruby/comptonsoft/comptonSoft.bundle")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/itoyamato/include/comptonsoft/ruby/comptonSoft.i;/Users/itoyamato/include/comptonsoft/ruby/decayChains.i;/Users/itoyamato/include/comptonsoft/ruby/class_list_comptonSoft.hh")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/itoyamato/include/comptonsoft/ruby" TYPE FILE FILES
    "/Users/itoyamato/Downloads/software/ComptonSoft/rubyext/interface/comptonSoft.i"
    "/Users/itoyamato/Downloads/software/ComptonSoft/rubyext/interface/decayChains.i"
    "/Users/itoyamato/Downloads/software/ComptonSoft/rubyext/interface/class_list_comptonSoft.hh"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/itoyamato/lib/ruby/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/itoyamato/lib/ruby" TYPE DIRECTORY FILES "/Users/itoyamato/Downloads/software/ComptonSoft/rubyext/library/" FILES_MATCHING REGEX "/[^/]*\\.rb$")
endif()

