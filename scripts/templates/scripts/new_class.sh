#!/bin/bash

current_dir=`pwd | awk -F '/' '{print $NF}'`
if [ "scripts" != "$current_dir" ]
then
	echo "this script must be called from within the scripts directory"
	exit 1
fi

if [ 0 -eq $# ]
then

	echo "use ./new_class.sh path to class"
	exit 1
fi

ready_header() {

	path=$1
	filename=$2
	namespace=$3
	classname=$4

	namespace_str_begin="";
	namespace_str_end="";

	for piece in ${namespace[@]};
	do

		namespace_str_begin="${namespace_str_begin}namespace ${piece} { "
		namespace_str_end="${namespace_str_end}}"
	done

	mkdir -p $path
	cp templates/_class_template.h $filename
	sed -i "s/_class/$classname/g" $filename
	sed -i "s/_namespace/$namespace_str_begin/g" $filename
	sed -i "s/_endnamespace/$namespace_str_end/g" $filename
}

ready_implementation() {

	path=$1
	filename=$2
	namespace=$3
	classname=$4

	namespace_str="";
	h_file_location=""

	for piece in ${namespace[@]};
	do

		namespace_str="${namespace_str}${piece}::"
		h_file_location="${h_file_location}${piece}/";
	done

	h_file_location="${h_file_location}${classname}.h"

	mkdir -p $path
	cp templates/_class_template.cpp $filename

	sed -i "s|_h_file_location|${h_file_location}|g" $filename
	sed -i "s/_namespace/$namespace_str/g" $filename
	sed -i "s/::;/;/g" $filename
}

argpos=1
header_path="../include"
src_path="../src"
header_file=""
src_file=""
namespace=()
classname=""

for arg in "$@"
do

	if [ "$argpos" -eq "$#" ]
	then

		#is a file
		header_file="${header_path}/${arg}.h"
		src_file="${src_path}/${arg}.cpp"
		classname=$arg

		if [ -f "$header_file" ]
		then

			echo "$header_file already exists, will abort!"
			exit 2
		fi

		if [ -f "$src_file" ]
		then

			echo "$src_file already exists, will abort!"
			exit 2
		fi

		ready_header $header_path $header_file $namespace $classname
		ready_implementation $src_path $src_file $namespace $classname

	else

		namespace+=("$arg")
		header_path="${header_path}/${arg}"
		src_path="${src_path}/${arg}"
	fi

	argpos=$(( $argpos + 1))
done
