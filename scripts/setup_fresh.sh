current_dir=`pwd | awk -F '/' '{print $NF}'`

if [ "scripts" != "$current_dir" ]
then
	echo "this script must be called from within the scripts directory"
	exit 1
fi

cd ..

#remove shell scripts and readme file
rm tiles.sh;
rm editor.sh;
rm README.md;

#remove integration, logs, classes, entrypoint...
rm -rf integration;
rm -rf logs/*;
rm -rf class;
rm -rf data;
rm -rf class/dfwimpl;
rm main.cpp;

#refresh the directories
mkdir logs;
mkdir -p class/dfwimpl class/controllers;
mkdir -p data/config;

#move the blank templates
mv templates/input.h class;
mv templates/app_config.* class/dfwimpl;
mv templates/state_driver.* class/dfwimpl;
mv templates/states.h class/controllers;
mv templates/_template.* class/controllers;
mv templates/config.dnot data/config;
mv templates/main.cpp ./
mv templates/GETTING_STARTED.txt .

#remove templates, git and so on...
rm -rf templates;
rm -rf .git;
rm scripts/setup_fresh.sh;

#give a little help...
cat GETTING_STARTED.txt;
