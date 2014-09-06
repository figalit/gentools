#!/bin/sh

autobac=$1
sexbac=$2
autofull=$3
sexfull=$4
auto1k=$5
sex1k=$6
average=$7
xaverage=$8
autocut=$9
auto1cut=$10
xcut=$11
x1cut=$12


BINDIR=/net/eichler/vol3/home/calkan/bin

#join GC content info on BACs

awk '{print $1"-"$2"-"$3"\t"$4}' $autobac | sort -k 1,1 > $autobac.tab
awk '{print $1"-"$2"-"$3"\t"$4}' $sexbac | sort -k 1,1 > $sexbac.tab

join /net/eichler/vol3/home/calkan/FosmidSearch/BACS/bacs_aggressive_5k-gc.tab $autobac.tab | sed s/-/"\t"/g | sed s/" "/"\t"/g | sort -k 1,1 -k 2,2n > $autobac.gc.vs.depth
join /net/eichler/vol3/home/calkan/FosmidSearch/BACS/bacs_aggressive_5k-gc.tab $sexbac.tab | sed s/-/"\t"/g | sed s/" "/"\t"/g | sort -k 1,1 -k 2,2n > $sexbac.gc.vs.depth

rm $autobac.tab $sexbac.tab

cat  $autobac.gc.vs.depth |  $BINDIR/partgcdepth > $autobac.gc.vs.depth-avg
cat  $sexbac.gc.vs.depth |   $BINDIR/partgcdepth > $sexbac.gc.vs.depth-avg

# BAC normalization not crucial but sometimes it's good to see how scatterplot changed

$BINDIR/depthloess-avg -i $autobac.gc.depth -a $autobac.gc.vs.depth-avg -e $average > $autobac.gc.depth.normalized
$BINDIR/depthloess-avg -i $sexbac.gc.depth -a $sexbac.gc.vs.depth-avg -e $xaverage > $sexbac.gc.depth.normalized

#  move to hg17 depth

#5K
awk '{print $1"-"$2"-"$3"\t"$4}' $autofull | sort -k 1,1 > $autofull.tab
awk '{print $1"-"$2"-"$3"\t"$4}' $sexfull | sort -k 1,1 > $sexfull.tab

#1K
awk '{print $1"-"$2"-"$3"\t"$4}' $auto1k | sort -k 1,1 > $auto1k.tab
awk '{print $1"-"$2"-"$3"\t"$4}' $sex1k | sort -k 1,1 > $sex1k.tab

# merge GC info

join /net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/wssdborders/hg17_aggmask_5k.gccount.tab $autofull.tab | sed s/-/"\t"/g | sed s/" "/"\t"/g | sort -k 1,1 -k 2,2n > $autofull.gc.depth
join /net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/wssdborders/hg17_aggmask_5k.gccount.tab $sexfull.tab | sed s/-/"\t"/g | sed s/" "/"\t"/g | sort -k 1,1 -k 2,2n > $sexfull.gc.depth

join /net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/wssdborders/hg17_aggmask_1k.gccount.tab $auto1k.tab | sed s/-/"\t"/g | sed s/" "/"\t"/g | sort -k 1,1 -k 2,2n > $auto1k.gc.depth
join /net/eichler/vol3/home/calkan/FosmidSearch/BUILD35/wssdborders/hg17_aggmask_1k.gccount.tab $sex1k.tab | sed s/-/"\t"/g | sed s/" "/"\t"/g | sort -k 1,1 -k 2,2n > $sex1k.gc.depth

rm $autofull.tab $sexfull.tab $auto1k.tab $auto1k.tab $sex1k.tab

# normalize 5K; -e value is the average depth from unique BACs; autosomal & sex respectively

$BINDIR/depthloess-avg -i $autofull.gc.depth -a $autobac.gc.vs.depth-avg -e $average  > $autofull.gc.depth.normalized
$BINDIR/depthloess-avg -i $sexfull.gc.depth -a $autobac.gc.vs.depth-avg -e $xaverage > $sexfull.gc.depth.normalized

# normalize 1K; -e value is the average depth from unique BACs; autosomal & sex respectively; -d 5.0 divides by 5 since we have 5K BAC averages above

$BINDIR/depthloess-avg -i $auto1k.gc.depth -a $autobac.gc.vs.depth-avg -e $average -d 5.0 > $auto1k.gc.depth.normalized
$BINDIR/depthloess-avg -i $sex1k.gc.depth -a $autobac.gc.vs.depth-avg -e $xaverage -d 5.0  > $sex1k.gc.depth.normalized

# remove gc column

cut -f 1,2,3,5 $autofull.gc.depth.normalized > $autofull.gc.depth.normalized.tab
cut -f 1,2,3,5 $sexfull.gc.depth.normalized > $sexfull.gc.depth.normalized.tab
cut -f 1,2,3,5 $auto1k.gc.depth.normalized > $auto1k.gc.depth.normalized.tab
cut -f 1,2,3,5 $sex1k.gc.depth.normalized > $sex1k.gc.depth.normalized.tab


# wssd_picker, remove gap, etc. comes after this

wssd_picker.pl -f $autofull.gc.depth.normalized.tab  -w 7 -s 6 -c $autocut -b 3 -k $auto1k.gc.depth.normalized.tab  -n 5 -i 1 -t 804.386 -o wssd-auto.tab
wssd_picker.pl -f $sexfull.gc.depth.normalized.tab  -w 7 -s 6 -c $xcut -b 3 -k $sex1k.gc.depth.normalized.tab  -n 5 -i 1 -t 655.006 -o wssd-sex.tab

cat wssd-auto.tab wssd-sex.tab > wssd.tab
coordsMerger_sort.pl -i wssd.tab -h -u -n 0 -b 1 -e 2 -o wssd.merged
cat wssd.merged | gawk '{ if($3-$2>=10000) print $0; }' > wssdGE10K.tab
twoPartOvp_mgsrt.pl  -i wssdGE10K.tab -f -j gaps.tab -t  -L -o abc8.gcnorm.wssd.tab
rm wssd-auto.tab wssd-sex.tab wssd.tab wssd.merged wssdGE10K.tab


# add deletion detection & CN estimation as necessary


#  LocalWords:  sexfull
