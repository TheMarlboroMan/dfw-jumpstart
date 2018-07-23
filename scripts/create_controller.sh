if [ "$1" == "" ]; then
	echo "Use ./scripts/create_controller.sh controller_name";
else
	uppercasename=${1^^}
	ecl="#ENDCONTROLLERLIST";
	target="\$(DIR_O)$1_c.o"
	recipe_deps="$target:\$(DIR_CONTROLLERS)$1.h \$(DIR_CONTROLLERS)$1.cpp";
	recipe="	\$(CC) \$(CFLAGS) \$(INCLUDES) \$(DIR_CONTROLLERS)$1.cpp -o $target"

	cp class/controllers/_template.h class/controllers/$1.h
	cp class/controllers/_template.cpp class/controllers/$1.cpp
	sed -i -e "s/template/$1/g" class/controllers/$1.h
	sed -i -e "s/TEMPLATE/$uppercasename/g" class/controllers/$1.h
	sed -i -e "s/template/$1/g" class/controllers/$1.cpp
	sed -i -e "s/state_max/state_$1, state_max/g" class/controllers/states.h
	awk '{gsub("//register controllers here.", "reg(c_examples, t_states::state_examples, new controller_examples(log));");}' class/dfwimpl/state_driver.cpp


	echo "DEP_CONTROLLERS+= $target" >> make/controllers;
	echo "$recipe_deps" >> make/controllers;
	echo "$recipe" >> make/controllers;
	echo "Done";
fi;

