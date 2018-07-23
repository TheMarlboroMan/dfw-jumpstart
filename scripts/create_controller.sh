if [ "" == $1 ]; then
	echo "Use ./scripts/create_controller.sh controller_name";
else
	cp class/controllers/_template.h class/controllers/$1.h
	cp class/controllers/_template.cpp class/controllers/$1.cpp
	sed -i -e 's/template/$1/g' class/controllers/$1.h
	sed -i -e 's/template/$1/g' class/controllers/$1.cpp
fi;
