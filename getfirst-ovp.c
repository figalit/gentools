#include <stdio.h>

int ovp (int s, int e, int ps, int pe){
  if (s <= ps && pe<=e) return 1;
  else if (s<=ps && e<=pe && ps<=e) return 1;
  else if (ps<=s && e<=pe) return 1;
  else if (ps<=s && s<=pe && pe<=e) return 1;
  return 0;
}

main(){
  char name[100];
  char trans[100];

  char chrom[100];
  char strand[2];

  int s, e;
  
  char kname[100];

  char ktrans[100];

  char kchrom[100];
  char kstrand[2];
  char krest[200000];

  int ks, ke;
  

  char rest[200000];

  name[0] = 0;


  kname[0]=0; krest[0]=0; kchrom[0]=0; ktrans[0]=0; kstrand[0]=0; ks=0; ke=0;

  while(scanf("%s\t%s\t%s\t%s\t%d\t%d", name, trans, chrom, strand, &s, &e) > 0){
    fgets(rest, 200000, stdin);

    if (!strcmp(chrom, kchrom) && ovp(s, e, ks, ke)){
      if (ke-ks < e-s){
	strcpy(kname, name);
	strcpy(ktrans, trans);
	strcpy(kstrand, strand);
	strcpy(kchrom, chrom);
	strcpy(krest, rest);
	ks = s; ke = e;	
      }
    }
    else{
      if (kname[0]==0){
	strcpy(kname, name);
	strcpy(ktrans, trans);
	strcpy(kstrand, strand);
	strcpy(kchrom, chrom);
	strcpy(krest, rest);
	ks = s; ke = e;	
      }
      else{
	printf("%s\t%s\t%s\t%s\t%d\t%d%s", kname, ktrans, kchrom, kstrand, ks, ke, krest);
	strcpy(kname, name);
	strcpy(ktrans, trans);
	strcpy(kstrand, strand);
	strcpy(kchrom, chrom);
	strcpy(krest, rest);
	ks = s; ke = e;	
      }
    }

  }
}
