/* 
   given an mfold energy density file, creates an image
   last modified: Sep 1, 2005
   Can Alkan
*/

#include <stdio.h>
#include <tiffio.h>
#include <math.h>

main(int argc, char **argv){
  int width;
  int height;
  //float w[width+1][width+1];
  float **w;
  int i,j;
  float min;
  float value;
  TIFF *tif;
  FILE *enfile;
  char tifname[100];
  unsigned int color;
  unsigned int r,g,b;
  int sampleperpixel = 3;    // or 3 if there is no alpha channel, you should get a understanding of alpha in class soon.

  //char image[(width+1)*(height+1)*sampleperpixel];
  char *image;
  uint32 row;

  if (argc != 3){
    printf("given an mfold energy density file, creates an image\nlast modified: Sep 1, 2005\n");
    printf("%s [energyfile] [seqlength]\n", argv[0]);
    exit(0);
  }

  width = atoi(argv[2]);
  if (width==0){
    printf("enter the seqlength correctly.\n");
    exit(0);
  }
  height = width;

  enfile = fopen(argv[1], "r");
  if (enfile == NULL){
    printf("File %s not opened!\n", argv[1]);
    exit(0);
  }


  w = (float **)malloc((width+1)*sizeof(float*));
  for (i=0;i<=width;i++)
    w[i] = (float *)malloc((width+1)*sizeof(float));    

  image = (char *)malloc(((width+1)*(height+1)*sampleperpixel)*sizeof(char));

  for (i=0;i<=width;i++)
    for (j=0;j<=width;j++)
      w[i][j] = 0;
  
  min = 0;
  while (fscanf(enfile,"%d %d %f", &i,&j,&value) > 0){
    w[i][j] = value;
    w[j][i] = value;   
    if (value < min)
      min = value;
  }
  
  fclose(enfile);


  sprintf(tifname, "%s.tif", argv[1]);

  // add a black frame to match i-j numbers

  i=0;
  for (j=0;j<=width;j++){
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel] = 0;
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+1] = 0;
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+2] = 0;
  }
  j=0;
  for (i=0;i<=width;i++){
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel] = 0;
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+1] = 0;
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+2] = 0;
  }
  

  for (i=1;i<=width;i++){
    for (j=1;j<=width;j++){
      if (w[i][j]>=0)
	color = 0; // black
      else
	color = (255*(w[i][j]/min)*-1);
	//color = 256*256*256*w[i][j]*-1;

      /*
      r = color >> 16;
      g = (color << 16) >> 24;
      b = (color << 24) >> 24;
      */

      /*
      r = (int)(sin(i)+cos(j)) % 256;
      g = (int)(log(i)+exp(j)) % 256;
      b = (i^2+i*j) % 256;

      r = (int)(i*j+i^j) % 256;
      g = 0;//(int)(log(i)*exp(j)) % 256;
      b = 0;//(i^2+i*j) % 256;
      r = (int)(i^3+3*i^2*j+3*i*j^2+j^3) % 256;
      g = 0;//(int)((i+j)^4) % 256;
      b = 0;//(int)((i+j)^5) % 256;


      */

      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel] = r;
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+1] = g;
      image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+2] = b;
      
      /* red scale */
      
      if (w[i][j]<-0.5){
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel] = color;
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+1] = 0;
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+2] = 0;
      } 
      /* green scale */
      else if (w[i][j]<-0.40){
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel] = 0;
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+1] = color;
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+2] = 0;
      }
      /* blue scale */
	   
      else{
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel] = 0;
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+1] = 0;
	image[(i)*sampleperpixel*(width+1)+(j)*sampleperpixel+2] = color;
	}
      //      image[(i-1)+(j-1)*sampleperpixel+3] = 0;
      
    }
  }
  
  
  tif = TIFFOpen(tifname, "w");

  TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width+1);  // set the width of the image
  TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height+1);    // set the height of the image
  TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // set number of channels per pixel
  TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
  TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
  //   Some other essential fields to set that you do not have to understand for now.
  TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  

  TIFFWriteRawStrip(tif, 0, image, (width+1)*(height+1)*sampleperpixel);


  /*
  size_t linebytes = sampleperpixel * width;     // length in memory of one row of pixel in the image.
  
  unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
  //    Allocating memory to store the pixels of current row
  if (TIFFScanlineSize(tif)==linebytes)
    buf =(unsigned char *)_TIFFmalloc(linebytes);
  else
    buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
  
  // We set the strip size of the file to be size of one row of pixels
  TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, width*sampleperpixel));
  
  //Now writing image to the file one strip at a time
  for (row = 0; row < height; row++)
    {
      memcpy(buf, &image[(height-row-1)*linebytes], linebytes);  // check the index here, and figure out why not using h*linebytes
      if (TIFFWriteScanline(tif, buf, row, 0) < 0)
	break;
    }

  */

  TIFFClose(tif);
  
  /*  if (image)
      _TIFFfree(image);*/

  /*
  html = fopen("table.html", "w");
  fprintf(html,"<html><head><title>mfold normalized energy table</title></head>\n");
  fprintf(html,"<body bgcolor=white>\n");
  fprintf(html,"<table border=0>\n");
  for (i=1;i<=377;i++){
    fprintf(html,"<tr>\n");
    for (j=1;j<=377;j++){
      if (w[i][j]>=0)
	color = 0; // black
      else
	color = (255*w[i][j]*-1)*(256*256);
      //fprintf(html,"<td bgcolor=rgb(%d,0,0)>&nbsp;</td>\n",color);
      fprintf(html,"<td bgcolor=\"#%x\">&nbsp;</td>\n",color);
      
    }
    fprintf(html,"</tr>\n");
  }  
  fprintf(html, "</table></body></html>");
  */
}
