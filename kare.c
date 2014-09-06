#include <stdio.h>
#include <math.h>
#include <ctype.h>
main(int argc, char **argv){
int sayi;
double kok;
int intkok;
double testkok;
double kare;
sayi=atoi(argv[1]);
kok=sqrt(sayi);
intkok=(int)kok;
kare=intkok*intkok;
printf("initkok: %d kok: %f double : %f\n",intkok,kok,kare);
}
