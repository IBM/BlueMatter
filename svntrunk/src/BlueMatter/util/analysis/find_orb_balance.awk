#!/bin/awk -f
#
# Go through the 'pkfxlog' to pick out information for ORB weighing optimisation
#
# 2005-08-16 tjcw written
#

# We get told a 'bucket number' for a frag-frag pair.
# This maps to the number of atoms in the source frag, the number in the target frag, and whether
# there are 'exclusions' (forced handled by bonded force evaluation)
# This information is what we have to make predictions at ORB time, and needs to be
# recovered at analysis time so that we can parametrise the ORB for another run

function map_tag(tag)
{
   
   may_have_exclusions = (tag>=16) ? "X" : "N"
   source_frag_size = ((int(tag / 4)) % 4) + 1
   target_frag_size = ( tag % 4 ) + 1
#   if ( source_frag_size < target_frag_size )
#   {
     return may_have_exclusions "_" target_frag_size "_" source_frag_size
#   } else {
#     return may_have_exclusions "_" source_frag_size "_" target_frag_size
#   }
}

# We're working with an IFP which handles 'water-water' (actually any 3:3 non-exclustion case)
# and 'site-site'; anything else is reduced to combinations of these. 

# For any given 'tag', these functions tell you how many water-water and site-site
# interactions they represent.

function count_33(tag)
{
   source_frag_size = ((int(tag / 4)) % 4) + 1
   target_frag_size = ( tag % 4 ) + 1
   if ( 0 && ( 3 == source_frag_size && 3 == target_frag_size ) ) 
   {
     return 1 ;
   } else {
     return 0 ;
   }
}

function count_11(tag)
{
   source_frag_size = ((int(tag / 4)) % 4) + 1
   target_frag_size = ( tag % 4 ) + 1
   if ( 0 && ( 3 == source_frag_size && 3 == target_frag_size ) ) 
   {
     return 0 ;
   } else {
     return source_frag_size * target_frag_size ;
   }

}

# We get log lines like
#  stdout[0]:  Node 1       nsites 1340    verletsum   2632.125652
#  stdout[0]: Node 0 Volume = 546.112566
#  stdout[172]:    282.059645998 AC 0 >Verlet list q[0] = 60398<    /include/BlueMatter/NSQ_watersite_verlet_phase4.hpp 68
#  stdout[0]:    282.106967998 0 0 >Statistic Count=1024 First=15069417 Min=251 Max=15069417 Mean= 973350.8847656250000 Variance= 6188478206492.0136718750000<    /include/pk/pkstatistic.hpp 110
# indicating what the ORB thought would happen, what was in the active Verlet list,
# and how long it took to run the Verlet list respectively.
# The idea is to pick these out with a view to answering the questions
# 1) What is the processing cost of a water-water ? an atom-atom ?
# 2) What are the best parameters to put into the ORB
# 3) Does the ORB have the right information to model what actually happens
# 
{
   if ( $2 == "Node" )
   {
#      print "node " $3
      node="stdout["$3"]:"
      if ( $4 == "nsites" )
      {
#        print "nsites " $5
        nsites[node] = $5
        verletsum[node] = $7
      } else if ( $4 == "Volume" )
      {
#        print "volume " $6
        volume[node] = $6
      }
   } 
   else if ( $5 == ">Verlet" )
   {
      node=$1
      tag=substr($7,3,length($7)-3)
      qx=map_tag(tag)
      qv=substr($9,1,length($9)-1) 
      q[node,qx]=qv
      q33[node] += qv*count_33(tag)
      q11[node] += qv*count_11(tag)
      if ( qv != 0 ) 
      {
        exists[qx] = 1
      }
   } else if ( $5 == ">Statistic") {
      node=$1
      statcount[node] += 1
      if ( 2 == statcount[node] )
      {
        first[node] = substr($7,7)
        time[node] = $11
      }  
   } else if ( $5 == ">Observation" ) {
      node=$1
      if (2 == statcount[node])
      {
        timeseq=$6
#        print node " " timeseq " " $8
        obs[node,timeseq]=substr($8,1,length($8)-1)
      }  
   }
}

# OK, we've read all the data. Now write it out in a form that a spreadsheet can read
# and then we can let the spreadsheet do the correlations, regressions, etc.

END {
  printf "Node,Sequence,Cycles\n"
  for ( v=0;v<=1024;v+=1)
  {
    for ( w = 0 ; w < 1024 ; w += 1)
    {
      printf "%d,%d,%d\n",v,w,obs["stdout["v"]:",w]
    }
  }
}
