39DLL-4-Linux
=============

A port of the 39dll for Linux originally written for Win32 by Luke Graham (39ster in GMC)

==================
BUILD INSTRUCTIONS
==================

LINUX:

1. Make sure build-essential AND g++ is installed.

2. Cd to the source directory

3. execute the following FOR EACH cpp file: 
   g++ -fPIC -c -o obj/<INSERT CPP FILE NAME WITHOUT .CPP HERE>.o <INSERT CPP FILE NAME HERE>
     
     so an example would be:
       g++ -fPIC -c -o obj/main.o main.cpp

4. Then execute the following: g++ -shared obj/*.o -o 39dll-4-linux.so



ANDROID:

1. Make sure the NDK is installed and ndk-build is on your path. You will also require 
   the Android SDK with its tools and platform  -tools on your path

2. CD to the project root dir if you are not already there.

3. Simply execute the following command:
     ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application.mk

NOTE: If you get the following error: 'Android NDK: There is no Android.mk under ./jni' 
      then you are in the wrong directory (probably android-src). MAKE SURE you are in 
      the ROOT of the source tree
