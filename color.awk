BEGIN{
	print "track name="sample"-CN description=\"Copy numbers for sample "sample"\" useScore=0 itemRgb=on"

}

{
  OFS="\t"; 
  if ($4>=9.5) 
    color="255,0,0"; 
  else if ($4>=8.5) color="139,26,26"; 
  else if ($4>=7.5) color="255,165,0"; 
  else if ($4>=6.5) color="255,255,0"; 
  else if ($4>=5.5) color="180,238,180"; 
  else if ($4>=4.5) color="100,149,237"; 
  else if ($4>=3.5) color="65,105,225"; 
  else if ($4>=2.5) color="0,0,205"; 
  else if ($4>=1.5) color="0,0,0"; 
  else if ($4>=0.5) color="196,196,196"; 
  else color="229,229,229";

  print $1,$2,$3,$4, "0", ".", "0", "0", color

}
