#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct sv{
  char chr[30];
  int s, e;
  char call[50];
  char call2[50];
}_sv;

int main(int argc, char **argv){
  FILE *tab1, *tab2;
  int n1, n2;
  char chr1[30];
  int s1, e1;
  char c1[50];
  int i, j;
  struct sv **var1;
  struct sv **var2;
  float ratio;


  tab1 = fopen(argv[1], "r");
  tab2 = fopen(argv[2], "r");

  n1 = 0; n2 = 0;

  while(fscanf(tab1, "%s%d%d%s", chr1, &s1, &e1, c1) > 0)
    n1++;
  while(fscanf(tab2, "%s%d%d%s", chr1, &s1, &e1, c1) > 0)
    n2++;
 
  rewind(tab1);  rewind(tab2);

  fprintf(stderr, "%d from %s\t %d from %s\n", n1, argv[1], n2, argv[2]);

  var1 = (struct sv **)malloc(sizeof(struct sv *)*n1);
  for (i=0;i<n1;i++)
    var1[i] = (struct sv *)malloc(sizeof(struct sv));

  var2 = (struct sv **)malloc(sizeof(struct sv *)*n2);
  for (i=0;i<n2;i++)
    var2[i] = (struct sv *)malloc(sizeof(struct sv));

  i=0;
  while(fscanf(tab1, "%s%d%d%s", chr1, &s1, &e1, c1) > 0){
    strcpy(var1[i]->chr, chr1);
    var1[i]->s = s1;
    var1[i]->e = e1;
    
    if (strstr(c1, "B:in"))
      strcpy(c1, "deletion");
    else if (strstr(c1, "S:in"))
      strcpy(c1, "insertion");
    else if (strstr(c1, "++") || strstr(c1, "--"))
      strcpy(c1, "inversion");
    else if (strstr(c1, "Trans"))
      strcpy(c1, "transchromosomal");

    strcpy(var1[i]->call, c1);

    strcpy(var1[i]->call2, c1);

    if (strstr(var1[i]->call2, "Insertion"))
      strcpy(var1[i]->call2, "insertion");
    else if (strstr(var1[i]->call2, "Deletion"))
      strcpy(var1[i]->call2, "deletion");
    else if (strstr(var1[i]->call2, "Inversion"))
      strcpy(var1[i]->call2, "inversion");

    i++;
  }

  i=0;
  while(fscanf(tab2, "%s%d%d%s", chr1, &s1, &e1, c1) > 0){
    strcpy(var2[i]->chr, chr1);
    var2[i]->s = s1;
    var2[i]->e = e1;

    if (strstr(c1, "B:in") || !strcmp(c1, "B"))
      strcpy(c1, "deletion");
    else if (strstr(c1, "S:in") || !strcmp(c1, "S"))
      strcpy(c1, "insertion");
    else if (strstr(c1, "++") || strstr(c1, "--"))
      strcpy(c1, "inversion");
    else if (strstr(c1, "Trans"))
      strcpy(c1, "transchromosomal");

    strcpy(var2[i]->call, c1);


    strcpy(var2[i]->call2, c1);

    if (strstr(var2[i]->call2, "Insertion"))
      strcpy(var2[i]->call2, "insertion");
    else if (strstr(var2[i]->call2, "Deletion"))
      strcpy(var2[i]->call2, "deletion");
    else if (strstr(var2[i]->call2, "Inversion"))
      strcpy(var2[i]->call2, "inversion");

    i++;
  }


  //fprintf(stdout, "chr\tstart_kidd\tend_kidd\tspan_kidd\tcall_kidd\tstart_solid\tend_solid\tsize_solid\tcall_solid\tratio\n");

  for (i=0;i<n1;i++){
    for (j=0;j<n2;j++){
      //printf("%s\t%d\t%d\t\t\t%s\t%d\t%d\n", var1[i]->chr, var1[i]->s, var1[i]->e, var2[j]->chr, var2[j]->s, var2[j]->e);



      if (strcmp(var1[i]->chr, var2[j]->chr))
	continue;

      //ratio = ((float)var1[i]->e-(float)var1[i]->s) / ((float)var2[j]->e-(float)var2[j]->s);

      /*
      if (ratio< 0.5 || ratio > 2){
	continue;
      }
      */

      if (strcmp(var1[i]->call2, var2[j]->call2))
	continue;

      if (var1[i]->s <= var2[j]->s && var1[i]->e <= var2[j]->e && var1[i]->e > var2[j]->s){
	//if (!strcmp(var1[i]->call2, var2[j]->call2))

	ratio = (float)(var1[i]->e - var2[j]->s + 1) / (float)(var2[j]->e - var1[i]->s + 1);

	if (ratio>=0.5)
	  fprintf(stdout, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s\t%4.2f\n", var1[i]->chr, var1[i]->s, var1[i]->e, (var1[i]->e-var1[i]->s), var1[i]->call, var2[j]->s, var2[j]->e, (var2[j]->e-var2[j]->s), var2[j]->call, ratio);
	/*
	if (!strcmp(var1[i]->call2, var2[j]->call2))
	  fprintf(stdout, "concordant\n");
	else
	  fprintf(stdout, "discordant\n");
	*/
      }
      else if (var1[i]->s <= var2[j]->s && var1[i]->e >= var2[j]->e){
	//if (!strcmp(var1[i]->call2, var2[j]->call2))
	/* var2 inside var1 */
	
	ratio = (float)(var2[j]->e - var2[j]->s + 1) / (float)(var1[i]->e - var1[i]->s + 1);

	//if (ratio>0.5)
	  fprintf(stdout, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s\t%4.2f\n", var1[i]->chr, var1[i]->s, var1[i]->e, (var1[i]->e-var1[i]->s), var1[i]->call, var2[j]->s, var2[j]->e, (var2[j]->e-var2[j]->s), var2[j]->call, ratio);
	//fprintf(stdout, "%s\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%d\n", var1[i]->chr, var1[i]->s, var1[i]->e, var2[j]->s, var2[j]->e, var1[i]->call, var2[j]->call, (var1[i]->e-var1[i]->s), (var2[j]->e-var2[j]->s));
	/*
	if (!strcmp(var1[i]->call2, var2[j]->call2))
	  fprintf(stdout, "concordant\n");
	else
	  fprintf(stdout, "discordant\n");
	*/
      }
      else if (var1[i]->s >= var2[j]->s && var1[i]->s < var2[j]->e && var1[i]->e >= var2[j]->e){
	
	ratio = (float)(var2[j]->e - var1[i]->s + 1) / (float)(var1[i]->e - var2[j]->s + 1);

	//if (!strcmp(var1[i]->call2, var2[j]->call2))	  
	if (ratio>=0.5)
	  fprintf(stdout, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s\t%4.2f\n", var1[i]->chr, var1[i]->s, var1[i]->e, (var1[i]->e-var1[i]->s), var1[i]->call, var2[j]->s, var2[j]->e, (var2[j]->e-var2[j]->s), var2[j]->call, ratio);
	  //fprintf(stdout, "%s\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%d\n", var1[i]->chr, var1[i]->s, var1[i]->e, var2[j]->s, var2[j]->e, var1[i]->call, var2[j]->call, (var1[i]->e-var1[i]->s), (var2[j]->e-var2[j]->s));
	/*
	if (!strcmp(var1[i]->call2, var2[j]->call2))
	  fprintf(stdout, "concordant\n");
	else
	  fprintf(stdout, "discordant\n");
	*/
      }
      else if (var1[i]->s >= var2[j]->s && var1[i]->e <= var2[j]->e){
	//if (!strcmp(var1[i]->call2, var2[j]->call2))
	/*var1 inside var2; do nothing*/
	ratio = (float)(var1[i]->e - var1[i]->s + 1) / (float)(var2[j]->e - var2[j]->s + 1);
	//ratio = 0.0;
	if (ratio>=0.5)
	  fprintf(stdout, "%s\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s\t%4.2f\n", var1[i]->chr, var1[i]->s, var1[i]->e, (var1[i]->e-var1[i]->s), var1[i]->call, var2[j]->s, var2[j]->e, (var2[j]->e-var2[j]->s), var2[j]->call, ratio);
	//	  fprintf(stdout, "%s\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%d\n", var1[i]->chr, var1[i]->s, var1[i]->e, var2[j]->s, var2[j]->e, var1[i]->call, var2[j]->call, (var1[i]->e-var1[i]->s), (var2[j]->e-var2[j]->s));
	/*
	if (!strcmp(var1[i]->call2, var2[j]->call2))
	  fprintf(stdout, "concordant\n");
	else
	  fprintf(stdout, "discordant\n");
	*/
      }
    }
  }


}

