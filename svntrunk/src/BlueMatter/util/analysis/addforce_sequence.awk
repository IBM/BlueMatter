#!/bin/awk -f
BEGIN {
  slice=0
  }
{
  if ( $1 == "---" )
  {
     slice+= 1
     sequence=0
  } 
  if ( $5 == ">AddForce" && substr($6,1,8) == "aFragId=")
  {
     seq[slice,sequence]=$6 " " $7
     sequence += 1
     if ( seq[slice,sequence] != seq[0,sequence])
     {
       printf("%04d %08d %s\n",slice,sequence,$0) 
     }
  }
}