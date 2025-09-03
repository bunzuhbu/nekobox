#!/bin/bash -x
rm -rfv build
mkdir -pv build vcpkg_installed

cmd <<< "mklink /D %cd%\\build\\vcpkg_installed %cd%\\vcpkg_installed"

source "prep.sh"
cmake --build build --config Release
#mvn clean install -f pom.xml
