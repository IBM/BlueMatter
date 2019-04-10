#!/bin/awk -f
BEGIN {
# 'pi' from /usr/include/math.h
  pi = 3.14159265358979323846264338327950288
}
{
	if ( NF == 5 || NF == 6 ) 
	{
	  p=$4
	  if ( ( substr(p,1,1) == "(" ) && ( substr(p,length(p),1) == ")" )) 
	  {
	    q=substr(p,2,length(p)-2)
	    if ( substr(q,length(q)-13) == "*Math::Deg2Rad" )
	    {
	      degree_angle=substr(q,1,length(q)-14) 
	      radian_angle=degree_angle*pi/180.0 
	      printf "%s %s %s MSDAngleMacro(%s,%s,%.20g) %s %s\n ",$1,$2,$3,q,degree_angle,radian_angle,$5,$6
	    } else {
	      print $0 
	    } 
	  } else {
	    print $0 
	  } 
        } else if ( NF == 9 || NF == 10 )
	{
	  if ( ( substr($2,1,4) == "cos(" ) && ( substr($6,1,4) == "sin(" ) ) 
	  {
	    cosparm = substr($2,5,length($2)-5) 
	    sinparm = substr($6,5,length($6)-5)
	    if ( ( substr(cosparm,length(cosparm)-13) == "*Math::Deg2Rad" ) && ( substr(sinparm,length(sinparm)-13) == "*Math::Deg2Rad" ) )
	    {
	      cos_degree_angle=substr(cosparm,1,length(cosparm)-14) 
	      sin_degree_angle=substr(sinparm,1,length(sinparm)-14) 
	      cos_radian_angle=cos_degree_angle*pi/180.0 
	      sin_radian_angle=sin_degree_angle*pi/180.0 
	      cos_value=cos(cos_radian_angle)
	      sin_value=sin(sin_radian_angle)
	      printf "%s MSDCosMacro(%s,%s,%.20g,%.20g) %s %s %s MSDSinMacro(%s,%s,%.20g,%.20g) %s %s %s %s\n",$1,cosparm,cos_degree_angle,cos_radian_angle, cos_value,$3,$4,$5,sinparm,sin_degree_angle,sin_radian_angle, sin_value,$7,$8,$9,$10
	
	    } else {
	      print $0 
	    } 
	  } else {
	    print $0 
	  }
	} else {
	  print $0 
	} 
}
