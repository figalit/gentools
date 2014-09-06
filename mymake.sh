#!/bin/bash
for i in `ls *.c` 
do
  echo $i | awk '{
BINFILE = gensub(/\.c/,"","g",$1)
print "gcc " $1 " -o ~/bin/" BINFILE
}' | sh
done