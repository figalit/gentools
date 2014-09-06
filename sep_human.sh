#!/bin/bash

if [ -z "$1" ]; then
	exit
fi
if [ -f $1 ]; then
    echo Extracting G248
    cat $1 | grep G248 | sort -u > g248.$1    
    echo Extracting S213
    cat $1 | grep S213 | awk -F "|" '{print $2}' | sort -u >  s213.$1
    echo Extracting HWWall
    cat $1 | grep "lcl|4\|lcl|5" | awk -F "|" '{print $2}' | sort -u > hwwall.$1
    echo Extracting JM
    cat $1 | grep "lcl|H" | awk -F "|" '{print $2}' | sort -u > jm.$1
    
    len=`wc g248.$1 | awk '{print $1}'`
    if [ "$len" = 0 ]; then
	rm -f g248.$1
    else
    	echo Fetching G248
    	fetchg248 < g248.$1
    	mv temp g248.$1.fa
	echo Qualfetch G248
	qualfetch ~/Alpha.Satellite/wgs_human/QUALS/G248.hits.m2.fetchlist.qual g248.$1
    fi

    len=`wc s213.$1 | awk '{print $1}'`
    if [ "$len" = 0 ]; then
	rm -f s213.$1
    else
	echo Fetching S213
	fetchs213 < s213.$1
	mv temp s213.$1.fa
	echo Qualfetch S213
	qualfetch ~/Alpha.Satellite/wgs_human/QUALS/S213.hits.m2.fetchlist.qual S213.$1
    fi

    len=`wc hwwall.$1 | awk '{print $1}'`
    if [ "$len" = 0 ]; then
	rm -f hwwall.$1
    else
	echo Fetching HWWall
	fetchhwwall < hwwall.$1
	mv temp hwwall.$1.fa
	echo Qualfetch HWWall
	qualfetch ~/Alpha.Satellite/wgs_human/QUALS/HWWall.hits.m2.fetchlist.qual hwwall.$1
    fi
    
    len=`wc jm.$1 | awk '{print $1}'`
    if [ "$len" = 0 ]; then
	rm -f jm.$1
    else
	echo Fetching JM
	fetchjm < jm.$1
	mv temp jm.$1.fa
	echo Qualfetch JM
	qualfetch ~/Alpha.Satellite/wgs_human/QUALS/JM.hits.m2.fetchlist.qual jm.$1
    fi

    cat *$1.fa > all.fasta
    cat *$1.qual > all.qual
    
    mkdir fasta
    mkdir qual
    echo Breaking FASTA
    fasta_batch_breaker.pl -i all.qual -o qual
    echo Breaking QUAL
    fasta_batch_breaker.pl -i all.fasta -o fasta
    
    echo Renaming Quals
    for i in `ls qual/`
    do
      mv qual/$i qual/$i.qual
    done

    echo Creating MKTRACE input
    for i in `ls fasta/`
    do
      echo fasta\/$i > fasta/$i.in
      echo fasta\/$i.trace >> fasta/$i.in
    done

    mv qual/* fasta/
    rm -fr qual
    
    echo MKTRACE
    for i in `ls fasta/*in`
    do
      mktrace < $i > /dev/null
    done
    
    mkdir fasta/traces
    mv fasta/*trace fasta/traces

    echo CONSEDRUN coming up!!

    consedrun fasta/traces -forcelevel 10

fi
    
