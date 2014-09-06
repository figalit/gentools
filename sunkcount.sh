sed s/":"/"\t"/ abc8-uniqgene.30bp.sunkhits | sed s/"-"/"\t"/ | sed s/"\."/"\t"/ | awk '{print $2"\t"$3+$5"\t"$3+$5+29"\t"$1}'
