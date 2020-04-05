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

#remove shell scripts, main and readme file
rm tiles.sh editor.sh README.md main.cpp

#remove classes and dirs
rm -rf integration logs class data main.cpp

#refresh the directories
mkdir -p logs class/dfwimpl class/controllers data/config

#move the blank templates
tar -xzf scripts/templates.tar.gz

#Remove git and scripts
rm -rf .git;
rm scripts/setup_fresh.sh;
rm scripts/make_templates.sh
rm scripts/templates.tar.gz

#Rename project, create the build directory
sed -i s/dfw-jumpstart/$PROJECT/g CMakeLists.txt
mkdir build
cd build
$CMAKE .. > /dev/null
if [ 0 != $? ]
then
	echo "cmake failed"
	exit 1
fi

cd ..
mkdir logs

#give a little help, which was on the tar file.
cat GETTING_STARTED.txt;

exit 0
