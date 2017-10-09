# Copyright 2017 Sakchham Sharma
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.4.1)

#Extract the ANDROID_ARCH from ARCH_ABI, work around as CMAKE_ANDROID_ARCH is only available 3.7 onwards
IF(CMAKE_ANDROID_ARCH_ABI MATCHES "armeabi")
    SET(ANDROID_ARCH arm)
ELSEIF(CMAKE_ANDROID_ARCH_ABI MATCHES "arm64")
    SET(ANDROID_ARCH arm64)
ELSE()
    SET(ANDROID_ARCH ${CMAKE_ANDROID_ARCH_ABI})
ENDIF()

MESSAGE(STATUS "building for architecture: " ${ANDROID_ARCH})

IF(ANDROID_ARCH MATCHES "64")
    set(ANDROID_MIN_API 21) #x86-64, arm64, mips64 only supported on 21 and up
ELSE()
    set(ANDROID_MIN_API 19)
ENDIF()

set(PARI_TOOLCHAIN ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/pari-toolchain-${ANDROID_ARCH})
#create a custom toolchain
IF( IS_DIRECTORY ${PARI_TOOLCHAIN}/bin)
    MESSAGE(STATUS "previously generated toolchain detected.")
ELSE()
    MESSAGE(STATUS ${ANDROID_ARCH} " toolchain not found, creating.")
    EXECUTE_PROCESS (COMMAND python ${ANDROID_NDK}/build/tools/make_standalone_toolchain.py --arch ${ANDROID_ARCH} --api ${ANDROID_MIN_API} --stl gnustl --install-dir=${PARI_TOOLCHAIN})
ENDIF()

#download the sources for pari
set(CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp)
set(PARI_SRC ${CPP_SRC}/pari)
IF( IS_DIRECTORY ${PARI_SRC})
    MESSAGE(STATUS "pari sources already exist")
ELSE()
    MESSAGE(STATUS "fetching pari sources from Git.")
    EXECUTE_PROCESS (COMMAND git clone http://pari.math.u-bordeaux.fr/git/pari.git ${PARI_SRC})
ENDIF()


configure_file(${CPP_SRC}/pari.cfg.in ${PARI_SRC}/pari.cfg)
MESSAGE(STATUS "Building pari for android")
EXECUTE_PROCESS(COMMAND bash Configure -l pari.cfg WORKING_DIRECTORY ${PARI_SRC})
EXECUTE_PROCESS(COMMAND make -j4 WORKING_DIRECTORY ${PARI_SRC}/Oandroid-${ANDROID_ARCH})

set(PARI_INCLUDES ${CPP_SRC}/includes)
# Gather generated headers
file(GLOB HeaderFiles ${PARI_SRC}/src/headers/*.h)
foreach(HeaderFile ${HeaderFiles})
  file(COPY ${HeaderFile} DESTINATION ${PARI_INCLUDES})
endforeach()

add_library( pari-lib SHARED IMPORTED )
set_target_properties( pari-lib
                       PROPERTIES IMPORTED_LOCATION
                       ${PARI_SRC}/Oandroid-${ANDROID_ARCH}/libpari.so )

find_library( log-lib log )

add_library( native-lib SHARED src/main/cpp/native-lib.cpp )
target_include_directories(native-lib PRIVATE
                           ${PARI_INCLUDES}
                           ${PARI_SRC}/Oandroid-${ANDROID_ARCH})

#link the built library to built version of Pari
target_link_libraries( native-lib android ${log-lib} pari-lib )

#Copy the built Pari shared library to libs folder to bundle it with the apk
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
file(COPY ${PARI_SRC}/Oandroid-${ANDROID_ARCH}/libpari.so DESTINATION ${LIBS_DIR}/${CMAKE_ANDROID_ARCH_ABI})
