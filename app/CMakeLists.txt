# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(PARI_TOOLCHAIN ${CMAKE_CURRENT_SOURCE_DIR}/pari-toolchain)

#create a custom toolchain
IF( IS_DIRECTORY ${PARI_TOOLCHAIN}/bin)
    MESSAGE(STATUS "previously generated toolchain detected.")
ELSE()
    EXECUTE_PROCESS (COMMAND python ${ANDROID_NDK}/build/tools/make_standalone_toolchain.py --arch arm --api 19 --stl gnustl --install-dir=${PARI_TOOLCHAIN})
ENDIF()


#copy the required headers to the new toolchain
file(COPY /usr/include/termios.h DESTINATION ${PARI_TOOLCHAIN}/sysroot/usr/include)

#download the sources for pari
set(CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp)
set(PARI_SRC ${CPP_SRC}/pari)
IF( IS_DIRECTORY ${PARI_SRC})
    MESSAGE(STATUS "pari sources already exist")
ELSE()
    EXECUTE_PROCESS (COMMAND git clone http://pari.math.u-bordeaux.fr/git/pari.git ${PARI_SRC})
ENDIF()


configure_file(${CPP_SRC}/pari.cfg.in ${PARI_SRC}/pari.cfg)
MESSAGE(STATUS "Building pari for android")
EXECUTE_PROCESS(COMMAND bash Configure -l pari.cfg WORKING_DIRECTORY ${PARI_SRC})
EXECUTE_PROCESS(COMMAND make -j4 WORKING_DIRECTORY ${PARI_SRC}/Oandroid-arm)

set(PARI_INCLUDES ${CPP_SRC}/includes)

#Gather generated headers
file(GLOB HeaderFiles ${PARI_SRC}/Oandroid-arm/*.h
                      ${PARI_SRC}/src/headers/*.h)
foreach(HeaderFile ${HeaderFiles})
  file(COPY ${HeaderFile} DESTINATION ${PARI_INCLUDES})
endforeach()

#add built pari library
add_library( pari-lib STATIC IMPORTED )
set_target_properties( # Specifies the target library.
                       pari-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${PARI_SRC}/Oandroid-arm/libpari.so )

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(${PARI_INCLUDES})

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

target_link_libraries( native-lib pari-lib )
