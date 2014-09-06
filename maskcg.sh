#!/bin/bash

if [ -f $1 ]; then
	cat $1 | singleline -name | sed s/CG/NG/g | cleanfa -name > $1.cgmask
fi 
