#!/bin/bash
set -e

if [[ $1 == 'arm64' ]]; then
  ARCH="arm64"
else
  ARCH="amd64"
fi

source script/env_deploy.sh
DEST=$DEPLOYMENT/macos-$ARCH
rm -rf $DEST
mkdir -p $DEST

#### copy golang => .app ####
cd download-artifact
cd *darwin-$ARCH
tar xvzf artifacts.tgz -C ../../
cd ../..

mv deployment/macos-$ARCH/* $BUILD/nekobox.app/Contents/MacOS

#### copy srslist ####
cp download-artifact/srslist $BUILD/nekobox.app/Contents/MacOS/srslist
#### deploy qt & DLL runtime => .app ####
pushd $BUILD
macdeployqt nekobox.app -verbose=3
popd

codesign --force --deep --sign - $BUILD/nekobox.app

mv $BUILD/nekobox.app $DEST
