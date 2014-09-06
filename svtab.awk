{
	gsub(/:/, "\t", $0);
	if ($11=="1")
		call="insertion"
	else	if ($11=="2")
		call="deletion"
	else	if ($11=="3")
		call="inversion"
	
	if (call != "inversion")
		print $10"\t"$4"\t"$6"\t"call"\t"$16"\t"$14"\t"$18
	else
		print $10"\t"$2"\t"$8"\t"call"\t"$16"\t"$14"\t"$18

}
