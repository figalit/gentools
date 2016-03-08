import sys

def classify(sample):
    c = sample.split(':')[0]

    if c == '0/0':
        return 'ref/ref'
    if c == '0/1':
        return 'ref/alt'
    if c == '1/1':
        return 'alt/alt'
    if c == '0/2':
        return 'ref/alt2'
    if c == '1/2':
        return 'alt/alt2'
    if c == '2/2':
        return 'alt2/alt2'
    if c == './.':
        return 'non'
    else:
        return 'other'

geneCheck = None

if len(sys.argv) > 3 and sys.argv[2] == '-g':
    filename = sys.argv[1]
    geneCheck = sys.argv[3]
elif len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    print 'Usage: python filter.py <input.vcf> [-g <gene>]'

samples = []
header = ['CHROM', 'POS', 'DBSNP', 'REF', 'ALT', 'TYPE', 'Consequence', 'Putative_Impact', 'Gene', 'GeneID', 'Feature_type', 'FeatureID', 'Transcript_biotype', 'Rank/total', 'HGVS.c', 'HGVS.p', 'cDNA_position/cDNA_length', 'CDS_position/CDS_length Protein_position/Protein_length', 'Distance_to_feature', 'Errors']

with open(filename, 'r') as infile, open(filename[:-4] + '_output.tsv', 'w') as outfile:
    for line in infile:
        if line.startswith('##'):
            continue

        row = line.strip().split('\t')

        if line.startswith('#'):
            samples = row[9:]
            header = header[:6] + samples + header[6:]
            if geneCheck is not None:
                header.append(geneCheck)
            outfile.write('\t'.join(header))
            outfile.write('\n')
            continue

        # first5 : CHROM POS ID REF ALT
        first5 = row[:5]
        if first5[2] == '.':
            first5[2] == str(0)

        typ = 'SNV' if len(first5[3]) == len(first5[4]) else 'INDEL'

        if typ == 'SNV' and len(first5[3]) > 1 and len(first5[3]) == len(first5[4]):
            typ = 'MULT'

        classification = map(classify, row[-4:])

        ann = None
        info = row[7].split(';')
        for inf in info:
            if inf.startswith('ANN='):
                ann = inf[4:].split(',')[0]
                break

        ann = ann.split('|')
        gene = ann[3]
        features = ann[1:]

        outrow = first5 + [typ] + classification + features

        if geneCheck is not None:
                outrow.append('YES' if geneCheck == gene else 'NO')

        outfile.write('\t'.join(outrow))
        outfile.write('\n')
