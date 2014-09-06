{

#if (NR==1)
#     print "seq\tbegin\tend\tcolor\toffset\twidth\torient\tname"

if ($4=="deletion")
     print $1"\t"$2"\t"$3"\tgreen\t-10\t4\t\tTHE SPOTS"
else if ($4=="insertion")
     print $1"\t"$2"\t"$3"\tblue\t-16\t4\t\tTHE SPOTS"
else if ($4=="inversion")
     print $1"\t"$2"\t"$3"\torange\t-4\t4\t\tTHE SPOTS"

}
