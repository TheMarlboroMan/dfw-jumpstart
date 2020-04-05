#!/bin/bash

current_dir=`pwd | awk -F '/' '{print $NF}'`
if [ "scripts" != "$current_dir" ]
then
	echo "this script must be called from within the scripts directory"
	exit 1
fi

if [ "$1" == "" ]; 
then
	echo "Use ./create_controller.sh controller_name";
	exit 1
fi

cd ..

name=$1

#Check if the controller already exists.
if [ -f class/controllers/$name.h ] || [ -f class/controllers/$name.cpp ]
then
	echo "Controller files with that name already exist. Aborting";
	exit 1
fi 

#Copy files and replace their contents
cp class/controllers/_template.h class/controllers/$name.h
cp class/controllers/_template.cpp class/controllers/$name.cpp
sed -i "s/template/$name/g" class/controllers/$name.h
sed -i "s/template/$name/g" class/controllers/$name.cpp

#Add to controller to the state list
sed -i "/new states go above here/i state_$name," class/controllers/states.h

#Add the include and instance to the header file
sed -i "/new controller includes here/i #include \"../controllers/$name.h\"" class/dfwimpl/state_driver.h
sed -i "/controller instances here/i \\\tptr_controller\t\t\t\t\tc_$name;" class/dfwimpl/state_driver.h

#Instance the controller in the implementation file
sed -i "/register controllers here/i \\\treg(c_$name, t_states::state_$name, new controller_$name(log));" class/dfwimpl/state_driver.cpp

#Add the file to the cmake recipes...
sed -i "/_CONTROLLER_.cpp/i \\\t\$\{CMAKE_CURRENT_SOURCE_DIR\}/$name.cpp" class/controllers/CMakeLists.txt

#Finally, if that was the first controller, just set it up...
grep -q 'state_driver_interface(app::t_states::state_min)' class/dfwimpl/state_driver.cpp
if [ 0 -eq $? ]
then
	sed -i "s/state_driver_interface(app::t_states::state_min)/state_driver_interface(app::t_states::state_$name)/g" class/dfwimpl/state_driver.cpp
	echo "$name was setup as the starting controller"
fi

echo "Done"
exit 0
