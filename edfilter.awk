{


if ($1 != clone){
  clone = $1;
  dist = $15;
  print $0;
}

else{

  if ($15 == dist) 
    print $0;

}



}
