#!/usr/bin/perl -w


use strict 'vars';
use Getopt::Long;
use vars qw(%opt);

#pre-declaring variables


if (! defined $ARGV[0]) {
print "USAGE
MAIN DATA INPUT (at least one of the following)
-in [input file's first column should include the Readnames]
-out [Output file that has the other end's of the readname, 
=>xxxFy.TO's reverse is xxxRy.TO, I don't look at T1,T2, etc.
";
exit;
}





if ( &GetOptions( \%opt,"in:s","out:s") ) {
	print "\nCommand line arguments parsed successfully\n";
} else {
	die "\nINPUT ERROR: Command line arguments were not successfully parsed\n";
}




my $sequencename; 

my $line; #every line of the fasta file
open(OUT,">$opt{'out'}");

	open (IN1, "$opt{'in'}");
	while ($line=<IN1>) {
		chomp($line);

###READFORMAT####
#G248P801147FG9 # 
#G248P800336RE10#
#################
		my @xarray=split(/\t/,$line);
		my @array=split(/\./,$xarray[0]);


            		if ( $array[0]=~ /(G\d+P\d+)([FR])(\w+)/   ) {
                 
				if ($2 eq "F")
				{
		               		$sequencename="$1R$3.$array[1]";  #clone

				}
				elsif ($2 eq "R")
				{
					$sequencename="$1F$3.$array[1]";  #clone
				
				}
				else
				{
					print "warning, read format is wrong\n";
					my $pause=<STDIN>;
				}	
                        }

			print OUT "$sequencename\n";

	}
	close IN1;





close OUT;


