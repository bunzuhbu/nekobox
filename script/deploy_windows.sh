#!/bin/bash
set -e

source script/env_deploy.sh
if [[ $1 == "new-x86_64" || -z $1 ]]; then
  ARCH="windows-amd64"
  DEST=$DEPLOYMENT/windows64
else if [[ $1 == 'i686' ]]; then
  ARCH="windowslegacy-386"
  DEST=$DEPLOYMENT/windows32
else if [[ $1 == 'x86_64' ]]; then
  ARCH="windowslegacy-amd64"
  DEST=$DEPLOYMENT/windowslegacy64
else if [[ $1 == "arm64" ]]; then
  ARCH="windowslegacy-arm64"
  DEST=$DEPLOYMENT/windowslegacy-arm64
fi; fi; fi; fi;

rm -rf $DEST
mkdir -p $DEST

#### get the pdb ####
curl -fLJO https://github.com/rainers/cv2pdb/releases/download/v0.53/cv2pdb-0.53.zip
7z x cv2pdb-0.53.zip -ocv2pdb
./cv2pdb/cv2pdb64.exe ./build/nekobox.exe ./tmp.exe ./nekobox.pdb
rm -rf cv2pdb-0.53.zip cv2pdb
cd build
strip -s nekobox.exe
cd ..
rm tmp.exe
mv nekobox.pdb $DEST

#### download srslist ####
curl -fLso $DEST/srslist "https://raw.githubusercontent.com/throneproj/routeprofiles/rule-set/list"

#### copy exe ####
cp $BUILD/nekobox.exe $DEST

cd download-artifact
ls
cd *$ARCH
tar xvzf artifacts.tgz -C ../../
cd ../..
