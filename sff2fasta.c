#include <stdio.h>
#include <linux/types.h>

void endian_swap16(unsigned short *a)
{
    unsigned short x = *a;
    x = (x>>8) | 
        (x<<8);
    *a = x;
}

void endian_swap32(unsigned int *a)
{
    unsigned int x = *a;
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
    *a = x;
}



// __int64 for MSVC, "long long" for gcc
void endian_swap64(unsigned long long *a)
{
    unsigned long long x = *a;
    x = (x>>56) | 
        ((x<<40) & 0x00FF000000000000) |
        ((x<<24) & 0x0000FF0000000000) |
        ((x<<8)  & 0x000000FF00000000) |
        ((x>>8)  & 0x00000000FF000000) |
        ((x>>24) & 0x0000000000FF0000) |
        ((x>>40) & 0x000000000000FF00) |
        (x<<56);
    *a = x;
}

void round8(unsigned int *r){
  int this = *r;
  while (this % 8 != 0)
    this++;
  *r = this;
}

void printQual(FILE *q, unsigned char *quality_scores, int nbases){
  int i;
  for (i=0;i<nbases;i++){
    if (i%17 == 0 && i!=nbases-1 && i!=0)
      fprintf(q, "\n");
    fprintf(q, "%d ", quality_scores[i]);
  }
  fprintf(q, "\n");
}

void printFasta(FILE *f, char *bases, int nbases){
  int i;
  for (i=0;i<nbases;i++){
    if (i%60 == 0 && i!=nbases-1 && i!=0)
      fprintf(f, "\n");
    fprintf(f, "%c", bases[i]);
  }
  fprintf(f, "\n");
}

main(int argc, char **argv){
  char fastaname[300];
  char qualname[300];
  unsigned int magic_number; //4
  char version[4]; //8
  unsigned long long index_offset; //16
  unsigned int index_length; //20
  unsigned int number_of_reads; //24
  unsigned short header_length; //26
  unsigned short key_length; //28
  unsigned short number_of_flows_per_read; //30
  unsigned char flowgram_format_code; // 31
  char *flow_chars;// char[number_of_flows_per_read]
  char  *key_sequence;// char[key_length]
  unsigned char eight_byte_padding;// uint8_t[*]
  unsigned short flowgram_bytes_per_flow;   

  
  FILE *in;
  FILE *fasta;
  FILE *qual;

  unsigned short read_header_length;
  unsigned short name_length;
  unsigned short read_name_length;
  unsigned int  number_of_bases;
  unsigned short clip_qual_left;
  unsigned short clip_qual_right;
  unsigned short clip_adapter_left;
  unsigned short clip_adapter_right;
  char *name; // char[name_length]

  long position;


  unsigned int read_data_length;
  unsigned int number_of_flows;
  unsigned int real_rdl;

  unsigned short *flowgram_values;// uint*_t[number_of_flows]
  unsigned char *flow_index_per_base;// uint8_t[number_of_bases]
  char *bases;// char[number_of_bases]
  unsigned char *quality_scores;// uint8_t[number_of_bases]
  
  unsigned short temp;

  int i;
  
  in = fopen(argv[1], "r");

  sprintf(fastaname, "%s.fa", argv[1]);
  sprintf(qualname, "%s.qual", argv[1]);

  fread(&magic_number, sizeof(magic_number), 1, in);
  endian_swap32(&magic_number);
  fread(&version, 4, 1, in);
  fread(&index_offset, sizeof(index_offset), 1, in);
  endian_swap64(&index_offset);
  fread(&index_length, sizeof(index_length), 1, in);
  endian_swap32(&index_length);
  fread(&number_of_reads, sizeof(number_of_reads), 1, in);
  endian_swap32(&number_of_reads);

  fread(&header_length, sizeof(header_length), 1, in);
  endian_swap16(&header_length);

  fread(&key_length, sizeof(key_length), 1, in);
  endian_swap16(&key_length);

  fread(&number_of_flows_per_read, sizeof(number_of_flows_per_read), 1, in);
  endian_swap16(&number_of_flows_per_read);

  fread(&flowgram_format_code, sizeof(flowgram_format_code), 1, in);


  number_of_flows = number_of_flows_per_read * number_of_reads;

  flowgram_bytes_per_flow = 2; // looks like unnecessary; or not

  flow_chars = (char *) (sizeof(char)*(number_of_flows_per_read+1));
  key_sequence = (char *) (sizeof(char)*(key_length+1));
  
  printf("magicnum: %x\nversion: %d %d %d %d\noffset: %lld\nilength: %d\nnreads: %d\nheaderlen: %d\nkeylen: %d\nnofflowsperread: %d\nfformatcode: %d\n",
	 magic_number, version[0], version[1], version[2], version[3], index_offset, index_length, number_of_reads, header_length, key_length, number_of_flows_per_read, flowgram_format_code);
  
  /*
  fread(&flow_chars, number_of_flows_per_read, 1, in);
  flow_chars[number_of_flows_per_read] = 0;
  
  fread(&key_sequence, key_length, 1, in);
  key_sequence[key_length] = 0;
  */

  fseek(in, header_length, SEEK_SET);
  fasta = fopen(fastaname, "w");
  qual = fopen(qualname, "w");

  while (!feof(in)){
    position = ftell(in);
    if (position == index_offset)
      return; // for now
    fread(&read_header_length, sizeof(read_header_length), 1, in);
    
    endian_swap16(&read_header_length);
    
    if (feof(in))
      break;
    fread(&name_length, sizeof(name_length), 1, in);
    endian_swap16(&name_length);

    fread(&number_of_bases, sizeof(number_of_bases), 1, in);
    endian_swap32(&number_of_bases);

    //printf("\nrhl:%d\nnl: %d\nnob: %d\n\n", read_header_length, name_length, number_of_bases);

    
    fread(&clip_qual_left, sizeof(clip_qual_left), 1, in);
    endian_swap16(&clip_qual_left);
    
    //printf("%d\n", clip_qual_left);

    fread(&clip_qual_right, sizeof(clip_qual_right), 1, in);
    endian_swap16(&clip_qual_right);
    //printf("%d\n", clip_qual_right);

    fread(&clip_adapter_left, sizeof(clip_adapter_left), 1, in);
    endian_swap16(&clip_adapter_left);
    //printf("%d\n", clip_adapter_left);

    fread(&clip_adapter_right, sizeof(clip_adapter_right), 1, in);
    endian_swap16(&clip_adapter_right);
    //printf("%d\n", clip_adapter_right);
    
    read_name_length = read_header_length - 16;

    name = (char *) malloc(sizeof(char *) * (read_name_length+1));

    fread(name, read_name_length, 1, in);
    
    name[name_length] = 0;
    
    //printf("name : %s\n", name);

    fprintf(fasta, ">%s\n", name);
    fprintf(qual, ">%s\n", name);


    read_data_length = number_of_flows_per_read * 2 + 3*number_of_bases; // CALKAN: round this to the nearest 8-divisible
    real_rdl = read_data_length;

    round8(&read_data_length);
    
    //printf("rdl: %d\n", read_data_length);

    flowgram_values = (unsigned short *) malloc(sizeof(unsigned short) * number_of_flows_per_read); ;// uint*_t[number_of_flows]
    flow_index_per_base= (unsigned char *) malloc(sizeof(unsigned char) * number_of_bases); ;// uint*_t[number_of_flows]
    bases= (char *) malloc(sizeof(char) * (number_of_bases+1)); ;// uint*_t[number_of_flows]
    quality_scores= (unsigned char *) malloc(sizeof(unsigned char) * number_of_bases); ;// uint*_t[number_of_flows]

    for (i=0;i<number_of_flows_per_read;i++){
      fread(&temp, sizeof(temp), 1, in);
      endian_swap16(&temp);
      flowgram_values[i] = temp;
    }

    fread(flow_index_per_base, number_of_bases, 1, in);

    fread(bases, number_of_bases, 1, in);
    bases[number_of_bases] = 0;
    fread(quality_scores, number_of_bases, 1, in);
    
    printQual(qual, quality_scores, number_of_bases);
    printFasta(fasta, bases, number_of_bases);

    //printf("bases: %s\n", bases);
    //fprintf(fasta, "%s\n", bases);
    free(flowgram_values);
    free(flow_index_per_base);
    free(bases);
    free(quality_scores);
    free(name);

    // CALKAN: read the padding depending on the read_data_length

    for (i=0;i<read_data_length-real_rdl;i++)
      fread(&eight_byte_padding, sizeof(eight_byte_padding), 1, in);


  }

}

