#!/bin/bash -x

VCPKG_TOOLCHAIN_FILE="C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"

vcpkg(){
 (
  exec "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/vcpkg/vcpkg.exe" "${@}"
 )
}

cmake(){
 (
  exec "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "${@}"
 )
}

vcpkg new --application
vcpkg add port qt
vcpkg add port dukglue

cmake -DCMAKE_BUILD_TYPE=Release -S . -B build "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_TOOLCHAIN_FILE" -DVCPKG_BUILD_TYPE=release

#-DVCPKG_HOST_TRIPLET=x86-windows-static-md -DVCPKG_BUILD_TYPE=release 
#-DVCPKG_TARGET_TRIPLET=x86-windows-static-md 
#-Dprotobuf_MSVC_STATIC_RUNTIME=OFF -DBUILD_SHARED_LIBS=OFF -DVCPKG_LIBRARY_LINKAGE=static 
#-DVCPKG_TARGET_ARCHITECTURE=x86
