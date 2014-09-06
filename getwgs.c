/* given a list of wgs, separate abc* and g248, fetch fasta and qual accordingly */

#include <stdio.h>

main(int argc, char **argv){
  char name[100];
  FILE *in;
  FILE *abc7, *abc8, *abc9, *abc10, *abc11, *abc12, *abc13, *abc14, *g248;
  char command[1000];
  if (argc != 2){
    printf("%s [fetchfile]\n", argv[0]);
    exit(0);
  }
  in  = fopen(argv[1], "r");
  if (in==NULL)
    exit(0);
  
  abc7 = fopen("abc7.txt","w");
  abc8 = fopen("abc8.txt","w");
  abc9 = fopen("abc9.txt","w");
  abc10 = fopen("abc10.txt","w");
  abc11 = fopen("abc11.txt","w");
  abc12 = fopen("abc12.txt","w");
  abc13 = fopen("abc13.txt","w");
  abc14 = fopen("abc14.txt","w");
  g248 = fopen("g248.txt","w");

  while(fscanf(in, "%s", name) > 0){
    if (strstr(name, "ABC7"))
      fprintf(abc7, "%s\n", name);
    else if (strstr(name, "ABC8"))
      fprintf(abc8, "%s\n", name);
    else if (strstr(name, "ABC9"))
      fprintf(abc9, "%s\n", name);
    else if (strstr(name, "ABC10"))
      fprintf(abc10, "%s\n", name);
    else if (strstr(name, "ABC11"))
      fprintf(abc11, "%s\n", name);
    else if (strstr(name, "ABC12"))
      fprintf(abc12, "%s\n", name);
    else if (strstr(name, "ABC13"))
      fprintf(abc13, "%s\n", name);
    else if (strstr(name, "ABC14"))
      fprintf(abc14, "%s\n", name);
    else if (strstr(name, "G248"))
      fprintf(g248, "%s\n", name);
  }  
  fclose(abc7);fclose(abc8);fclose(abc9);fclose(abc10);fclose(abc11);fclose(abc12);fclose(g248); fclose(abc13); fclose(abc14);
  system("fastacmd -d /mnt/gpfs/blastdb/ABC7_WGS_0207 -i abc7.txt > abc7.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC7.qual -i abc7.txt | aa2qual > abc7.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC8_WGS_0506 -i abc8.txt > abc8.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC8.qual -i abc8.txt | aa2qual> abc8.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC9_WGS_0606 -i abc9.txt > abc9.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC9.qual -i abc9.txt | aa2qual> abc9.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC10_WGS_0506 -i abc10.txt > abc10.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC10.qual -i abc10.txt | aa2qual> abc10.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC11_WGS_1106  -i abc11.txt > abc11.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC11.qual -i abc11.txt | aa2qual> abc11.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC12_WGS_0906 -i abc12.txt > abc12.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC12.qual -i abc12.txt |aa2qual> abc12.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC13_WGS_0107 -i abc13.txt > abc13.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC13.qual -i abc13.txt |aa2qual> abc13.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/ABC14_WGS_0307 -i abc14.txt > abc14.fa");
  system("fastacmd -d /home/calkan/Singleton.Assembly/ABC.QUALS/ABC14.qual -i abc14.txt |aa2qual> abc14.qual");
  system("fastacmd -d /mnt/gpfs/blastdb/Hs_fosmid2 -i g248.txt > g248.fa");
  system("fastacmd -d /mnt/gpfs/blastdb/Hs_fosmid_qual -i g248.txt | aa2qual > g248.qual");


  /*
  system("fastacmd -d ABC7_WGS_0207.qual -i abc7.txt | base2qual_compact_acgt > abc7.qual");
  system("fastacmd -d ABC8_WGS_0506.qual -i abc8.txt | base2qual_compact_acgt> abc8.qual");
  system("fastacmd -d ABC9_WGS_0606.qual -i abc9.txt | base2qual_compact_acgt> abc9.qual");
  system("fastacmd -d ABC10_WGS_0506.qual -i abc10.txt | base2qual_compact_acgt> abc10.qual");
  system("fastacmd -d ABC11_WGS_1106.qual -i abc11.txt | base2qual_compact_acgt> abc11.qual");
  system("fastacmd -d ABC12_WGS_0906.qual -i abc12.txt |base2qual_compact_acgt> abc12.qual");
  system("fastacmd -d Hs_fosmid_qual -i g248.txt | aa2qual > g248.qual");
  */
  
  sprintf(command, "cat abc7.fa abc8.fa abc9.fa abc10.fa abc11.fa abc12.fa abc13.fa abc14.fa g248.fa > %s.fa", argv[1]);
  system(command);
  sprintf(command, "cat abc7.qual abc8.qual abc9.qual abc10.qual abc11.qual abc12.qual abc13.qual abc14.qual g248.qual > %s.qual", argv[1]);
  system(command);
  system("rm -f abc*.fa abc*.qual g248.* abc*.txt");

}
