 for i in `ls | sed s/"_1.recal.fastq.gz"// | sed s/"_2.recal.fastq.gz"//|sort -u`; do echo $i; mixcat -f $i\_1.recal.fastq.gz -r $i\_2.recal.fastq.gz -gz | gzip > $i.gz; done

