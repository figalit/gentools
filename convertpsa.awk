{

if (NR==1)
     print "name1\tbegin1\tend1\tlen1\tname2\tbegin2\tend2\tlen2\tcolor\toffset\twidth\tdisplay\tsline\tscolor\thide\tclone\tspan\torienttype\tmin_ident\tbest\tplace\tmulti\tsize\tqualbp\trmunique\tfrmtype\trrmtype\thscore\thdrop"

if ($16 == "deletion") {
  color = "red";
  call="good_in";
}

else if ($16 == "delinv" || $16 == "insinv" || $16 == "inversion"){
  color = "orange"; 
  if ($7=="F")
    call="bad_same++";
  else
    call="bad_same--";
}
else if ($16 == "insertion") {
  color = "blue";
  call="good_in";
}
else break;

   print $3"\t"$4"\t"$5"\t300000000\t"$9"\t"$10"\t"$11"\t300000000\t"color"\t0\t\t\t\t\t\t"$1"\t"$19-$18"\t"call"\t1.000\tbest\tbestplace\tbestpair\t"$19-$18"\t100\t200\tNONE\tNONE\t4\t4"
#   print $17"\t"$6"\t"$6+36"\t300000000\tchr22\t"$8"\t"$8+36"\t300000000\t"$color"\t0\t\t\t\t\t\tname\t"$8-$6"\tgood_in\t1.000\tbest\tbestplace\tbestpair\t"$8-$6"\t100\t200\tNONE\tNONE\t4\t4"

}
