#!/bin/awk -f
#
# Go through the 'pkfxlog' to pick out information for ORB weighing optimisation
#
# 2005-08-16 tjcw written
#
function map_tag(tag)
{
   
   may_have_exclusions = (tag>=16) ? "X" : "N"
   source_frag_size = ((int(tag / 4)) % 4) + 1
   target_frag_size = ( tag % 4 ) + 1
   if ( source_frag_size < target_frag_size )
   {
     return may_have_exclusions "_" target_frag_size "_" source_frag_size
   } else {
     return may_have_exclusions "_" source_frag_size "_" target_frag_size
   }
}

{
   node = $1 
   if ( $5 == ">Verlet" )
   {
      qx=map_tag(substr($7,3,length($7)-3))
      qv=substr($9,1,length($9)-1) 
      q[node,qx]=qv
      if ( qv != 0 ) 
      {
        exists[qx] = 1
      }
   } else if ( $5 == ">Statistic") {
      statcount[node] += 1
      if ( 2 == statcount[node] )
      {
        time[node] = $11
      }  
   }
}

END {
  count=asorti(exists, x)
  printf "Cycles"
  for ( v=1;v<=count;v+=1)
  {
    printf ",Q_%s",x[v]
  }
  for ( v=1;v<=count;v+=1)
  {
    printf ",E_%s",x[v]
  }
  printf "\n"
  for ( node in statcount )
  {
    printf "%f", time[node] 
    for ( v=1;v<=count;v+=1 )
    {
      printf ",%d", q[node,x[v]]
    } 
    for ( v=1;v<=count;v+=1 )
    {
      printf ",%d",(q[node,x[v]] > 0 ? 1 : 0 )
    } 
    printf "\n"
  }
}