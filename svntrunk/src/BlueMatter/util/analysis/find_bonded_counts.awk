#!/bin/awk -f
#
# Find the number of 'bonded' ops on each node
#
#
{
  if ( $5 == ">LocalTupleListDriver::Execute" )
  {
      node=$1
      udf_name=$6
      list_size=substr($7,16,length($7)-16)+0
#      print node " " udf_name " " list_size
      sizes[node,udf_name]=list_size
      nodes[node]=1
      udf_names[udf_name]=1
  }
}
END {
  for ( udf_name in udf_names )
  {
    min_length=999999999
    max_length=0
    sv=0
    s1=0
    for (node in nodes)
    {
#      print node " " udf_name " " sizes[node,udf_name]
      if (sizes[node,udf_name] < min_length) min_length=sizes[node,udf_name]
      if (sizes[node,udf_name] > max_length) max_length=sizes[node,udf_name] 
      sv += sizes[node,udf_name]
      s1 += 1
    }
    printf "%30s %8d %8d %8d\n",udf_name,min_length,max_length,sv/s1
  }
}
