#BEGIN {
#	low = ARGV[1]
#	upper = ARGV[2]
#	print $low "\t" $upper
#}

{

	if ($16=="concordant" || $16=="deletion" || $16=="insertion"){
		if ($14 >= low && $14 <= upper){
			call = "concordant"
		}
	
		else if ($14 > upper){
			call = "deletion"
		}

		else if ($14 < low){
			call = "insertion"
		}

		$16 = call
		
	}

	print $0

}
