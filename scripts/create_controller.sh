if [ "$1" == "" ]; then
	echo "Use ./scripts/create_controller.sh controller_name";
else
	uppercasename=${1^^}
	ecl="#ENDCONTROLLERLIST";
	target="\$(DIR_O)$1_c.o"
	recipe_deps="$target:\$(DIR_CONTROLLERS)$1.h \$(DIR_CONTROLLERS)$1.cpp";
	recipe="	\$(CC) \$(CFLAGS) \$(INCLUDES) \$(DIR_CONTROLLERS)$1.cpp -o $target"
	awk_register_pattern="//register controllers here";
	awk_register_replacement="reg(c_$1, t_states::state_$1, new controller_$1(log));\n\t$awk_register_pattern"

	cp class/controllers/_template.h class/controllers/$1.h
	cp class/controllers/_template.cpp class/controllers/$1.cpp
	sed -i -e "s/template/$1/g" class/controllers/$1.h
	sed -i -e "s/TEMPLATE/$uppercasename/g" class/controllers/$1.h
	sed -i -e "s/template/$1/g" class/controllers/$1.cpp
	sed -i -e "s/state_max/state_$1, state_max/g" class/controllers/states.h
	awk -v p="$awk_register_pattern" "\$0 ~ p {gsub(p, "$awk_register_replacement"p)}1" class/dfwimpl/state_driver.cpp


	echo "DEP_CONTROLLERS+= $target" >> make/controllers;
	echo "$recipe_deps" >> make/controllers;
	echo "$recipe" >> make/controllers;
	echo "Done";
fi;

