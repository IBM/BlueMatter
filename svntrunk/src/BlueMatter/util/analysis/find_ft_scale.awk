#!/bin/awk -f
# Find an appropriate Fourier Transform scale from 'pkfxlog'
#
{
  if ( $5 == ">MaxModSQ" )
  {
    node=$1
    seq=seq[node]+1
    seq[node]=seq
    maxmodsq[node,seq]=$8
    summodsq[node,seq]=substr($11,1,length($11)-1)
    allnodes[node]=1
    allseqs[seq]=1
  }
}
END {
  for (seq in allseqs)
  {
    m=0
    s=0
    for (node in allnodes)
    {
      if ( m < maxmodsq[node,seq] ) m = maxmodsq[node,seq]
      s += summodsq[node,seq]
    }
    print seq " " summodsq " " maxmodsq
  }
}