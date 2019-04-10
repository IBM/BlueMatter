#!/bin/awk -f
# Attempt to check the behaviour of the 3D FFT
#
{
  if ( $5 == ">ReorderDoFFTs" )
  {
    node=$1
    seqn=seq[node]+1
    seq[node]=seqn
    sumSqIn[node,seqn]=$8
    sumSqOut[node,seqn]=substr($10,1,length($10)-1)
    allnodes[node]=1
    allseqs[seqn]=1
  }
  if ( $5 == ">ReorderDoFFTs_R" )
  {
    node=$1
    seqn=seqr[node]+1
    seqr[node]=seqn
    sumSqInr[node,seqn]=$8
    sumSqOutr[node,seqn]=substr($10,1,length($10)-1)
    allnodes[node]=1
    allseqs[seqn]=1
  }
  
}
END {
  for (seqn in allseqs)
  {
    m=0
    s=0
    ssi=0
    sso=0
    ssir=0
    ssor=0
    for (node in allnodes)
    {
      ssi += sumSqIn[node,seqn]
      sso += sumSqOut[node,seqn]
      ssir += sumSqInr[node,seqn]
      ssor += sumSqOutr[node,seqn]
    }
    printf "%06d %18.2f %18.2f %18.2f %18.2f\n", seqn, ssi, sso, ssir, ssor
  }
}
