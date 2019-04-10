#!/bin/awk -f

function show_octiles()
{
  for ( seq=0;seq<=maxseq;seq+=1)
  {
    delete seqcyc ;
    for ( nod=0;nod<=maxnode;nod+=1 )
    {
      seqcyc[nod]=cyc[seq,nod] 
    }
    asort(seqcyc) 
    for (a=0;a<8;a+=1)
    {
       x=a*(maxnode+1)/8
       print seq "," x "," seqcyc[x+1]
    }
    print seq "," maxnode "," seqcyc[maxnode+1] 
  }
}

function trace_quartiles()
{
  for ( nod=0;nod<=maxnode;nod+=1 )
  {
    ntim[cyc[0,nod]]=nod
  }
  q=asorti(ntim,ntims) 
  n[1]=ntim[ntims[0+1]]
  n[2]=ntim[ntims[128+1]]  
  n[3]=ntim[ntims[256+1]]  
  n[4]=ntim[ntims[384+1]]  
  n[5]=ntim[ntims[q]]
  for (seq=0;seq<=maxseq;seq+=1)
  {
    printf "%d",seq
    for (x=1;x<=5;x+=1)
    {
      printf  ",%d",cyc[seq,n[x]] 
    }
    printf "\n"
  }
}

function geom_x(node)
{
  return node % 8 ;
}
function geom_y(node)
{
  return (int(node / 8) ) % 8 ; 
}
function geom_z(node) 
{
  return (int(node / 64)) % 8 ;
}

function geom_node(x,y,z)
{
  return ((x+8)%8)+8*((y+8)%8)+64*((z+8)%8)
}

function showload(node)
{
  printf "%d,%d,%d\n",node,seq,cyc[seq,node]
}

function trace_adj_to_max()
{
  for ( seq=0;seq<=maxseq;seq+=1)
  {
    maxload=-1
    maxloadnode=-1
    for(nod=0;nod<=maxnode;nod+=1 )
    {
      if (cyc[seq,nod]>maxload)
      {
         maxloadnode=nod
         maxload=cyc[seq,nod]
      }
    }
    gx=geom_x(maxloadnode)
    gy=geom_y(maxloadnode)
    gz=geom_z(maxloadnode)
    
    for (dx=-1;dx<=1;dx+=1)
    {
       for (dy=-1;dy<=1;dy+=1)
       {
         for (dz=-1;dz<=1;dz+=1)
         {
           showload(geom_node(gx+dx,gy+dy,gz+dz)) 
         }
       }
    }
    
#   showload(maxloadnode) 
#   showload(geom_node(gx, gy, gz))
#   showload(geom_node(gx-1,gy,gz)) 
#   showload(geom_node(gx+1,gy,gz)) 
#   showload(geom_node(gx,gy-1,gz)) 
#   showload(geom_node(gx,gy+1,gz)) 
#   showload(geom_node(gx,gy,gz-1)) 
#   showload(geom_node(gx,gy,gz+1))
    
  }
}


BEGIN {
  FS=","
  maxnode=0
  maxseq=0
}
{
  node=$1
  sequence=$2
  cycles=$3
  if ( node != "Node" )
  {
    cyc[sequence,node]=cycles
    if ( node > maxnode ) maxnode = node 
    if ( sequence > maxseq ) maxseq = sequence
  }
}

END {
  trace_adj_to_max() 
}