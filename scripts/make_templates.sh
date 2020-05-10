#!/bin/bash

if [ -f templates.tar.gz ]
then
	rm templates.tar.gz
fi

#tar -zvcf templates.tar.gz --directory=templates ./*
cd templates
tar -zcf ../templates.tar.gz ./*
