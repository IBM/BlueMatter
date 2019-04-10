#!/usr/bin/perl
###############################################################################
# File: stringify_math_hpp.pl
# Author: cwo
# Date: June 18, 2006 
# Description: Small script used to convert bluematter's math header 
#              into a single string constant, so that that string can be 
#              used when writing .cpp files that must be compiled. 
###############################################################################

#$MATH_HPP_FILENAME = "../util/include/math.hpp";
#$PKMATH_H_FILENAME = "../../bgfe/pk/math/include/pkmath.h";
#$OUT_FILENAME= "./include/math_hpp.hpp";

#$MATH_HPP_FILENAME = "../math.hpp";
#$PKMATH_H_FILENAME = "../../pk/pkmath.h";
#$OUT_FILENAME= "./math_hpp.hpp";

$MATH_HPP_FILENAME = $ARGV[0];
$PKMATH_H_FILENAME = $ARGV[1];
$OUT_FILENAME= $ARGV[2];

open(OUTFILE, ">".$OUT_FILENAME);

print OUTFILE "\#ifndef MATH_HPP_HPP\n";
print OUTFILE "\#define MATH_HPP_HPP\n";
print OUTFILE "static const char * SRC_MATH_HPP_STRING = \" \"\n";



######### MATH.HPP Include ###############
open(INFILE, $MATH_HPP_FILENAME);
while (<INFILE>) {

  if (/^$/) {}
  elsif (/^\#include\s<pk\/pkmath.h/) { 

    ######### PKMATH Include ###############
    open(PKMATH_INFILE, $PKMATH_H_FILENAME);
    while (<PKMATH_INFILE>) {
      if (/^$/) {}
      else {
        chomp $_;
        $_ =~ s/"/\\"/g;
        print OUTFILE "\"$_\\n\"\n";
      }
    }
    close(PKMATH_INFILE);

  } else  {
    chomp $_;
    $_ =~ s/"/\\"/g;
    print OUTFILE "\"$_\\n\"\n";
  } 
}
close(INFILE);


print OUTFILE ";\n";
print OUTFILE "\#endif\n";

close(OUTFILE);
