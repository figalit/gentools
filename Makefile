CODEDIR=/net/eichler/vol3/home/calkan/code/gentools
BINDIR=/net/eichler/vol3/home/calkan/bin
CFLAGS=-g

all: clustermonomers hormatcher horpattern fastafix fastadel fastatotab fastatabclean qualfetch plotcorrelatedarray revcomp qual-tandem-subseq tabtofasta qualitymask clip complement seqlen cleanfa fastasort qualcount fastatools.o mstalign allpairs centerstar trie trie32 makecontig pfast pfaster affine2 align2sets repeatnet hormatcher-454 sff2fasta
pimp.o:	$(CODEDIR)/pimp.c $(CODEDIR)/pimp.h
	gcc -c $(CODEDIR)/pimp.c -o $(CODEDIR)/pimp.o -O3 -msse2 -ftree-vectorizer-verbose=6 -ftree-vectorize
clustermonomers:	$(CODEDIR)/clustermonomers.c $(CODEDIR)/pimp.c $(CODEDIR)/pimp.h $(CODEDIR)/pimp.o
	gcc $(CODEDIR)/clustermonomers.c $(CODEDIR)/pimp.o -o $(BINDIR)/clustermonomers -g -O3 -msse2 -ftree-vectorizer-verbose=6 -ftree-vectorize
hormatcher-454:	$(CODEDIR)/hormatcher.c 
	gcc $(CODEDIR)/hormatcher-454.c -o $(BINDIR)/hormatcher-454 -lm -O3
hormatcher:	$(CODEDIR)/hormatcher.c $(CODEDIR)/pimp.c $(CODEDIR)/pimp.h
	gcc $(CODEDIR)/hormatcher.c $(CODEDIR)/pimp.o -o $(BINDIR)/hormatcher -g -lm
horpattern:	$(CODEDIR)/horpattern.c $(CODEDIR)/pimp.c $(CODEDIR)/pimp.h $(CODEDIR)/pimp.o
	gcc $(CODEDIR)/horpattern.c $(CODEDIR)/pimp.o -o $(BINDIR)/horpattern -g
centerstar:	$(CODEDIR)/centerstar.c $(CODEDIR)/pimp.c $(CODEDIR)/pimp.h $(CODEDIR)/pimp.o
	gcc $(CODEDIR)/centerstar.c $(CODEDIR)/pimp.o -o $(BINDIR)/centerstar -g
fastafix:	$(CODEDIR)/fastafix.c
	gcc $(CODEDIR)/fastafix.c -o $(BINDIR)/fastafix -g
fastadel:	$(CODEDIR)/fastadel.c
	gcc $(CODEDIR)/fastadel.c -o $(BINDIR)/fastadel -g -O3
fastatotab:	$(CODEDIR)/fastatotab.c
	gcc $(CODEDIR)/fastatotab.c -o $(BINDIR)/fastatotab -g
fastatabclean:	$(CODEDIR)/fastatabclean.c
	gcc $(CODEDIR)/fastatabclean.c -o $(BINDIR)/fastatabclean -g
qualfetch:	$(CODEDIR)/qualfetch.c
	gcc $(CODEDIR)/qualfetch.c -o $(BINDIR)/qualfetch -g
plotcorrelatedarray:	$(CODEDIR)/plotcorrelatedarray.c
	gcc $(CODEDIR)/plotcorrelatedarray.c -o $(BINDIR)/plotcorrelatedarray -g
revcomp:	$(CODEDIR)/revcomp.c
	gcc $(CODEDIR)/revcomp.c -o $(BINDIR)/revcomp -g
clip:	$(CODEDIR)/clip.c
	gcc $(CODEDIR)/clip.c -o $(BINDIR)/clip -g
seqlen:	$(CODEDIR)/seqlen.c
	gcc $(CODEDIR)/seqlen.c -o $(BINDIR)/seqlen -g
complement:	$(CODEDIR)/complement.c
	gcc $(CODEDIR)/complement.c -o $(BINDIR)/complement -g
qual-tandem-subseq:	$(CODEDIR)/qual-tandem-subseq.c
	gcc $(CODEDIR)/qual-tandem-subseq.c -o $(BINDIR)/qual-tandem-subseq -g
tabtofasta:	$(CODEDIR)/tabtofasta.c
	gcc $(CODEDIR)/tabtofasta.c -o $(BINDIR)/tabtofasta -g
qualitymask: $(CODEDIR)/qualitymask.c
	gcc $(CODEDIR)/qualitymask.c -o $(BINDIR)/qualitymask -g
cleanfa:	$(CODEDIR)/cleanfa.c
	gcc $(CODEDIR)/cleanfa.c -o $(BINDIR)/cleanfa -g	
qualcount:	$(CODEDIR)/qualcount.c
	gcc $(CODEDIR)/qualcount.c -o $(BINDIR)/qualcount -g	
fastasort:	$(CODEDIR)/fastasort.sh
	cp -f $(CODEDIR)/fastasort.sh $(BINDIR)/
fastatools.o:	$(CODEDIR)/fastatools.c $(CODEDIR)/fastatools.h
	gcc -c $(CODEDIR)/fastatools.c -o $(CODEDIR)/fastatools.o
mstalign:	$(CODEDIR)/mstalign.c $(CODEDIR)/fastatools.h $(CODEDIR)/fastatools.o $(CODEDIR)/fastatools.c
	gcc $(CODEDIR)/mstalign.c $(CODEDIR)/fastatools.o -o $(BINDIR)/mstalign -g
allpairs: $(CODEDIR)/allpairs.c $(CODEDIR)/fastatools.c $(CODEDIR)/fastatools.o $(CODEDIR)/fastatools.h $(CODEDIR)/pimp.c $(CODEDIR)/pimp.h $(CODEDIR)/pimp.o
	gcc $(CODEDIR)/allpairs.c $(CODEDIR)/pimp.o $(CODEDIR)/fastatools.o -o $(BINDIR)/allpairs -g
trie:	$(CODEDIR)/trie.c
	gcc $(CODEDIR)/trie.c -o $(BINDIR)/trie -g -lm -Wall
pfast:	$(CODEDIR)/pfast.c
	gcc $(CODEDIR)/pfast.c -o $(BINDIR)/pfast -O3 -lm -Wall -lpthread
pfaster:	$(CODEDIR)/pfaster.c
	gcc $(CODEDIR)/pfaster.c -o $(BINDIR)/pfaster -O3 -lm -Wall -lpthread
pfast-debug:	$(CODEDIR)/pfast-debug.c
	gcc $(CODEDIR)/pfast-debug.c -o $(BINDIR)/pfast-debug -g -lm -Wall -lpthread
trie32:	$(CODEDIR)/trie.c
	gcc $(CODEDIR)/trie.c -o $(BINDIR)/trie32 -g -lm -Wall
makecontig:	$(CODEDIR)/makecontig.c $(CODEDIR)/fastatools.h $(CODEDIR)/fastatools.o $(CODEDIR)/fastatools.c
	gcc $(CODEDIR)/makecontig.c $(CODEDIR)/fastatools.o -o $(BINDIR)/makecontig -g
affine2: $(CODEDIR)/affine2.c $(CODEDIR)/fastatools.h $(CODEDIR)/fastatools.o $(CODEDIR)/fastatools.c
	gcc $(CODEDIR)/affine2.c $(CODEDIR)/fastatools.o -o $(BINDIR)/affine2
align2sets: $(CODEDIR)/align2sets.c
	gcc $(CODEDIR)/align2sets.c  -o $(BINDIR)/align2sets -lm -g
repeatnet:  $(CODEDIR)/repeatnet2.c
	gcc $(CODEDIR)/repeatnet2.c -o $(BINDIR)/repeatnet -lm -O3 -g
sff2fasta:  $(CODEDIR)/sff2fasta.c
	gcc $(CODEDIR)/sff2fasta.c -o $(BINDIR)/sff2fasta -O3 
genslide:	$(CODEDIR)/genslide.c  $(CODEDIR)/fastatools.h $(CODEDIR)/fastatools.o $(CODEDIR)/fastatools.c
	gcc $(CODEDIR)/genslide.c $(CODEDIR)/fastatools.o -o  $(BINDIR)/genslide -O3
clean:
	rm -f *o *~ \#*
