# /*
# For more information, please see: http://software.sci.utah.edu
# 
# The MIT License
# 
# Copyright (c) 2018 Scientific Computing and Imaging Institute,
# University of Utah.
# 
# 
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
# */


SET_PROPERTY(DIRECTORY PROPERTY "EP_BASE" ${ep_base})

# The latest release is downloaded from FFmpeg's website directly. 
if( WIN32 )
  set( FFmpeg_url "http://ffmpeg.zeranoe.com/builds/win64/dev/ffmpeg-4.2-win64-dev.zip")
elseif( APPLE )
  set( FFmpeg_url "https://ffmpeg.zeranoe.com/builds/macos64/dev/ffmpeg-4.2-macos64-dev.zip")
endif()

# This is a little annoying, Ninja needs to know exactly where the libraries will be placed 
# or it will not build. Oddly, this is the only file that needs this. 
if(${GeneratorName} STREQUAL "Ninja")
  ExternalProject_Add(FFmpeg_external_download
    URL ${FFmpeg_url}
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    INSTALL_COMMAND ""
    INSTALL_DIR ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
	BUILD_BYPRODUCTS
	  <SOURCE_DIR>/lib/${prefix}avutil${suffix}
      <SOURCE_DIR>/lib/${prefix}avformat${suffix}
      <SOURCE_DIR>/lib/${prefix}avcodec${suffix}
      <SOURCE_DIR>/lib/${prefix}avdevice${suffix}
      <SOURCE_DIR>/lib/${prefix}avfilter${suffix}
      <SOURCE_DIR>/lib/${prefix}postproc${suffix}
      <SOURCE_DIR>/lib/${prefix}swresample${suffix}
      <SOURCE_DIR>/lib/${prefix}swscale${suffix}
	CMAKE_CACHE_ARGS
      -DCMAKE_C_COMPILER:PATH=${Compiler}
      -DCMAKE_CXX_COMPILER:PATH=${Compiler}
  )
else()
  ExternalProject_Add(FFmpeg_external_download
    URL ${FFmpeg_url}
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    INSTALL_COMMAND ""
    INSTALL_DIR ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
  )
endif()


ExternalProject_Get_Property(FFmpeg_external_download SOURCE_DIR)

set(FFmpeg_LIBRARY_DIR ${SOURCE_DIR}/lib CACHE INTERNAL "")
set(FFmpeg_INCLUDE_DIR ${SOURCE_DIR}/include CACHE INTERNAL "")

add_library(FFmpeg_external STATIC IMPORTED)

# The libraries are set manually and cached internally 
set(FFmpeg_LIBRARIES
  ${FFmpeg_LIBRARY_DIR}/${prefix}avutil${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}avformat${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}avcodec${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}avdevice${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}avfilter${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}postproc${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}swresample${suffix}
  ${FFmpeg_LIBRARY_DIR}/${prefix}swscale${suffix}
  CACHE INTERNAL "" 
)
message(STATUS "FFmpeg_DIR: ${FFmpeg_LIBRARY_DIR}")
