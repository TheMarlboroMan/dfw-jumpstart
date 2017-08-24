#!/bin/bash

if [ "$1" == "" ]; then
	printf "./tiles.sh #tiles|logic|shadow|decoration#\n"
	exit
else

	cd ..;
	cd tabla_sprites;

	case "$1" in
		"tiles")
			./a.out img=../dfw-jumpstart/data/textures/scifitiles-sheet-alt.png out=../dfw-jumpstart/data/app_data/scifitiles-sheet-alt.dat 
		;;
		"logic")
			./a.out img=../dfw-jumpstart/integration/basic-logic-sheet.png out=../dfw-jumpstart/integration/basic-logic-sheet.dat 
		;;
		"shadow")
			./a.out img=../dfw-jumpstart/data/textures/shadows-sheet.png out=../dfw-jumpstart/data/app_data/shadows-sheet.dat 
		;;
		"decoration")
			./a.out img=../dfw-jumpstart/data/textures/decoration-sheet.png out=../dfw-jumpstart/data/app_data/decoration-sheet.dat 
		;;
		?) 
			echo "Error, use tiles|logic|shadow\n";
		;;
	esac
fi;
