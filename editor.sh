#!/bin/bash

if [ "$1" == "" ]; then
	printf "./editor.sh #file# :\n	ej: ./editor.sh map01.dat"
	exit
else
	cd ..;
	cd tile_editor;
	ls;
	./a.out res=640x480 cfg=../dfw-jumpstart/integration/editor_data.dat out=../dfw-jumpstart/data/app_data/maps/${1}
fi;
