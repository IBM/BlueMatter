#!/bin/sh

rm ala.amber.*

../top2xml_amber6.pl ala.top ala.amber.xml NULL

../ambercrd2dvs.pl ala.crd ala.amber.dvs

dvsconv ala.amber.dvs ala.amber.aix.dvs

xml2db2.pl ala.amber.xml

