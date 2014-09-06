#!/usr/bin/perl -w

sub overlap
{
  my($c1,$s1,$e1,$c2,$s2,$e2)=(@_);
  if($c1 ne $c2){ return 0; } #differnt chrms
  my($s_l,$e_l,$s_r,$e_r, $over);
  if($s1<=$s2)
      {
        $s_l=$s1;
        $e_l=$e1;
        $s_r=$s2;
        $e_r=$e2;
      }
      else
      {
        $s_l=$s2;
        $e_l=$e2;
        $s_r=$s1;
        $e_r=$e1;
      }
   if($s_r > $e_l) {return 0; } #no no overlap
   return 1; #overlaps
}






if(@ARGV != 4) {print "Three args: region target file, outfile, cn file and chrm name\n"; 
print "output is chr begin end median CN, 5% lower bound, 5% upper bound, number of windows\n";
print "removes first and last window (if >= 3 windows present) to avoid edge effects\n";

exit; }

($targetfile,$oname,$inf,$chrm)=@ARGV;
print "($targetfile,$oname,$inf,$chrm)\n";

open IN, "< $inf " or die "NO IN $inf";
while($l=<IN>)
{
  chomp($l);
  ($c,$b,$e,$v)=(split /\s+/,$l)[0,1,2,3];
  $b=$b++; 
  $nl=join ":", $c,$b,$e,$v;
  if($c ne $chrm) {next; }
  push(@depths,$nl);
}



#open IN, "< /net/eichler/vol4/home/kiddj/projects/people_projects/personal_dupmaps/renorm_design_17nov2008/7Dec2008/yh_pairs/breakdowns/combined.all.uniq.refs " or die "NO IN";
open IN, "< $targetfile" or die "NO IN";
while($l=<IN>)
{
  chomp($l);
  ($c,$b,$e)=(split /\t/,$l)[0,1,2];
  $nl=join ":", $c,$b,$e;
  if($c ne $chrm){next; }
  push(@ints,$nl);
}
close IN;

open OUT, "> $oname " or die "NO OUT $oname\n";

foreach $inl (@ints)
{
  ($tc,$tb,$te)=(split /:/,$inl);
 #if($te != 62420166) {next; }
 @vals=();
  foreach $nl (@depths)
  {
   ($c,$b,$e,$v)=(split /:/,$nl);
   $over=overlap($tc,$tb,$te,$c,$b,$e);
   if($over ==1 ) {push(@vals,$v); } 
  } #end foreach depths


#get ride of first and last windows...
if(@vals > 2)
{
  pop(@vals);
  shift(@vals);
}
@vals=sort {$a <=> $b} @vals;
$n=$#vals+1;
if($n % 2 == 0) #even
{
 $i=int($n/2);
 $med=($vals[$i]+$vals[$i-1])/2;
}
else #odd
{
 $i=int($n/2);
 $med=$vals[$i];
}

if($n==-1) {$med=0; }
#print "num vals=$n\n";
#foreach $v (@vals) {print "$v\n"; }
$ncut=0.05*$n;
$nlb=int(0+$ncut);
$nup=$n-int($ncut)-1;
$nup=int($nup);
$nl=join "\t", $tc,$tb,$te,$med,$vals[$nlb],$vals[$nup],$n;
#print "$nl\n";

#print OUT "$tc\t$tb\t$te\t$med\n";
print OUT "$nl\n";
#last;
} #end foreach inl


close OUT;