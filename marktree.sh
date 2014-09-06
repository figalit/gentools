#!/bin/bash

if [ -z $1 ]; then
	if [ -z $2 ]; then
		for i in `cut -f 1,2 $2` 
		do
			a=`echo $i | awk '{print $1}'`
			b=`echo $i | awk '{print $2}'`
			cat $1 | sed s/"$a \[shape=circle, label=\"\""/"$a \[shape=circle, label=\"#s=$b\""/g 
		done
	fi
fi

