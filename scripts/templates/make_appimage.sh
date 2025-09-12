#!/bin/bash

if [[ $# -ne 3 ]]
then
	echo "usage: make_appimage.sh path_to_executable path_to_resources path_to_linuxdeploy"
	echo 'eg: make_appimage.sh build/myexec build/resources $(which linuxdeploy)'
	exit 1
fi;

EXECUTABLE_PATH=$1
RESOURCES_PATH=$2
LINUXDEPLOY_PATH=$3

#export SIGN=1
#export SIGN_KEY=0xLAST8CHARSOFGPGKEYID
#Well, not really, but hey...
MAJOR_VERSION=$(grep "set(MAJOR_VERSION" CMakeLists.txt | awk '{print $2}' | tr -d ")")
MINOR_VERSION=$(grep "set(MINOR_VERSION" CMakeLists.txt | awk '{print $2}' | tr -d ")")
PATCH_VERSION=$(grep "set(PATCH_VERSION" CMakeLists.txt | awk '{print $2}' | tr -d ")")

export ARCH=x86_64
export LINUXDEPLOY_OUTPUT_VERSION="$MAJOR_VERSION.$MINOR_VERSION.$PATCH_VERSION"
echo "will build $LINUXDEPLOY_OUTPUT_VERSION"
$LINUXDEPLOY_PATH --appdir AppDir
cp -r $RESOURCES_PATH AppDir/usr/share
mkdir -p AppDir/usr/share/metainfo
cp linuxdeploy/*.xml AppDir/usr/share/metainfo/
$LINUXDEPLOY_PATH --executable $EXECUTABLE_PATH --appdir AppDir -i ./linuxdeploy/icon.png -d ./linuxdeploy/app.desktop --output appimage
rm -rf AppDir
exit 0
