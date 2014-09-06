#!/bin/bash

if [ -z "$1" ]; then
	echo Sorts a given FASTA File \(or FASTA-like Quality File\)
	echo $0 "<fasta file>"
	exit
elif [ -f $1 ]; then
	echo Sorting $1, hang on...
	fastatotab < $1 | sort > sorted
	tabtofasta sorted sorted.$1
	echo Sorted File dumped to: sorted.$1
	rm -f sorted
else
	echo File $1 Not Found.
	exit
fi
