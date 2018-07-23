if [ "$1" == "" ]; then
	echo "Use ./scripts/create_controller.sh controller_name";
else
	uppercasename=${1^^}
	name=$1
	ecl="#ENDCONTROLLERLIST";
	target="\$(DIR_O)${name}_c.o"
	recipe_deps="$target:\$(DIR_CONTROLLERS)$name.h \$(DIR_CONTROLLERS)$name.cpp";
	recipe="	\$(CC) \$(CFLAGS) \$(INCLUDES) \$(DIR_CONTROLLERS)$name.cpp -o $target"

	cp class/controllers/_template.h class/controllers/$name.h
	cp class/controllers/_template.cpp class/controllers/$name.cpp
	sed -i -e "s/template/$name/g" class/controllers/$name.h
	sed -i -e "s/TEMPLATE/$uppercasename/g" class/controllers/$name.h
	sed -i -e "s/template/$name/g" class/controllers/$name.cpp
	sed -i -e "s/state_max/state_$name, state_max/g" class/controllers/states.h

	awk_include_pattern="//new controller includes here.";
	awk_include_replacement="#include \\\"../controllers/$name.h\\\"\n"
	awk -v ptr="$awk_include_pattern" "\$0 ~ ptr {gsub(ptr, \"$awk_include_replacement\"ptr)}1" class/dfwimpl/state_driver.h > class/dfwimpl/state_driver.tmp

	awk_declare_pattern="//controller instances here.";
	awk_declare_replacement="ptr_controller\t\t\t\t\tc_$name;\n\t"
	awk -v ptr="$awk_declare_pattern" "\$0 ~ ptr {gsub(ptr, \"$awk_declare_replacement\"ptr)}1" class/dfwimpl/state_driver.tmp > class/dfwimpl/state_driver.h
	rm class/dfwimpl/state_driver.tmp

	mv class/dfwimpl/state_driver.cpp class/dfwimpl/state_driver.tmp
	awk_register_pattern="//register controllers here.";
	awk_register_replacement="reg(c_$name, t_states::state_$name, new controller_$name(log));\\n\\t"
	awk -v ptr="$awk_register_pattern" "\$0 ~ ptr {gsub(ptr, \"$awk_register_replacement\"ptr)}1" class/dfwimpl/state_driver.tmp > class/dfwimpl/state_driver.cpp
	rm class/dfwimpl/state_driver.tmp

	echo "DEP_CONTROLLERS+= $target" >> make/controllers;
	echo "$recipe_deps" >> make/controllers;
	echo "$recipe" >> make/controllers;
	echo "Done";
fi;

