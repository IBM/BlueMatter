#!/bin/awk -f
function tagval(x)
{
  return substr(x,index(x,"=")+1) 
}
{
  if ( $5 == ">Benchmark" )
  {
    tag=$6 " " $7 " " $8 " " $9 " " $10 " " $11 " " $12
    iterations=substr($13,index($13,"=")+1)
    forward_ns=substr($14,index($14,"=")+1)*(1000.0/700.0)
    reverse_ns=substr($15,index($15,"=")+1)*(1000.0/700.0)
    q[tag]+=1
    i[tag]+=iterations
    f[tag]+=forward_ns
    if ( 1 == q[tag] || forward_ns < fm[tag] ) fm[tag] = forward_ns ;
    if ( 1 == q[tag] || forward_ns > fx[tag] ) fx[tag] = forward_ns ;
    r[tag]+=reverse_ns
    if ( 1 == q[tag] || reverse_ns < rm[tag] ) rm[tag] = reverse_ns ;
    if ( 1 == q[tag] || reverse_ns > rx[tag] ) rx[tag] = reverse_ns ;
  } else if ( $6 == ">Benchmark" )
  {
    tag=$7 " " $8 " " $9 " " $10 " " $11 " " $12 " " $13
    iterations=substr($14,index($14,"=")+1)
    forward_ns=substr($15,index($15,"=")+1)*(1000.0/700.0)
    reverse_ns=substr($16,index($16,"=")+1)*(1000.0/700.0)
    q[tag]+=1
    i[tag]+=iterations
    f[tag]+=forward_ns
    if ( 1 == q[tag] || forward_ns < fm[tag] ) fm[tag] = forward_ns ;
    if ( 1 == q[tag] || forward_ns > fx[tag] ) fx[tag] = forward_ns ;
    r[tag]+=reverse_ns
    if ( 1 == q[tag] || reverse_ns < rm[tag] ) rm[tag] = reverse_ns ;
    if ( 1 == q[tag] || reverse_ns > rx[tag] ) rx[tag] = reverse_ns ;
  
  }
}
END {
  for ( t in q )
  {
    split(t,a)
    
    printf "%d %03d %03d %03d %02d %02d %02d %04d %f %f %d %d\n", tagval(a[1]), tagval(a[2]), tagval(a[3]), tagval(a[4]), tagval(a[5]), tagval(a[6]), tagval(a[7]) , q[t], f[t]/i[t],r[t]/i[t],fx[t]-fm[t],rx[t]-rm[t]
  }
}
