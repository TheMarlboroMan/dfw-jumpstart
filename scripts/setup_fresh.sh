#!/bin/bash
#set -x

current_dir=`pwd | awk -F '/' '{print $NF}'`

if [ "scripts" != "$current_dir" ]
then
	echo "this script must be called from within the scripts directory"
	exit 1
fi

if [ -z $1 ] || [ -z $2 ]
then
	echo "use: setup_fresh.sh #project_name# #cmake_binary#"
	exit 1
fi


PROJECT=$1
CMAKE=$2
which $CMAKE > /dev/null
if [ 0 != $? ]
then
	echo "invalid cmake-binary"
	exit 1
fi

cd ..

#remove build, if need be...
if [ -d build ]
then 
	rm -rf build
fi

#remove the project specifics...
rm -rf integration logs class data main.cpp tiles.sh editor.sh README.md CMakeLists.txt .git

#refresh the directories...
mkdir -p class/dfwimpl class/controllers

#decompress the blank templates...
tar -xzf scripts/templates.tar.gz

#rename the project
sed -i s/project-placeholder/$PROJECT/g CMakeLists.txt

#create the build directory
mkdir build
cd build
$CMAKE .. > /dev/null
if [ 0 != $? ]
then
	echo "cmake failed"
	exit 1
fi

#cleanup
rm scripts/setup_fresh.sh;
rm scripts/templates.tar.gz

#give a little help, which was on the tar file.
cd ..
cat GETTING_STARTED.txt;
exit 0
