rm tiles.sh;
rm editor.sh;
rm README.md;
rm -rf integration;
rm -rf logs/*;
rm -rf class;
rm -rf data;
rm -rf class/dfwimpl;
rm make/app;
rm make/app_dependencies;
rm make/controllers;
mkdir logs;
mkdir class;
mv templates/input.h class;
mkdir class/dfwimpl;
mv templates/app_config.* class/dfwimpl;
mv templates/state_driver.* class/dfwimpl;
mkdir class/controllers;
mv templates/states.h class/controllers;
mv templates/_template.* class/controllers;
mkdir data;
mkdir data/config;
mv templates/config.dnot data/config;
mv templates/app make;
mv templates/app_dependencies make;
mv templates/controllers make;
rm main.cpp;
mv templates/main.cpp ./
mv templates/GETTING_STARTED.txt .
rm -rf templates;
if [ -d objects ]; then rm -rf objects; fi;
rm -rf .git;
rm scripts/setup_fresh.sh;
cat GETTING_STARTED.txt;
