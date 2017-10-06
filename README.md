# pari-android
Port of Pari to android, PARI/GP is a widely used computer algebra system designed for fast computations in number theory (factorizations, algebraic number theory, elliptic curves...), but also contains a large number of other useful functions to compute with mathematical entities such as matrices, polynomials, power series, algebraic numbers etc., and a lot of transcendental functions. PARI is also available as a C library to allow for faster computations. [see more](https://pari.math.u-bordeaux.fr)

## Build Instructions
to build - <br/>
`./gradlew assembleDebug`

for installation - <br/>
`./gradlew installDebug`

CMake pulls pari sources automatically from [git](http://pari.math.u-bordeaux.fr/git/pari.git).

## Application
The demo app is very simple, performs very basic operations and displays their results.<br/>

<img src="https://github.com/sakchhams/pari-android/raw/master/meta/screenshot.png" width="45%" height="45%"/>

## Requirements 
* android-ndk-r14 and up (tested for 15c)
* andorid-sdk (compiled against 26)
* flex and bison
* git
