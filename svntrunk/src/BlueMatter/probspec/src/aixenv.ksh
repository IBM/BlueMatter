#!/usr/bin/ksh

export PATH=/u/germain/projects/cbc/alphaworks/xml4c/bin:$PATH

# export XERCESCROOT=/u/germain/projects/cbc/alphaworks/xml4c
export XERCESCROOT=/u/germain/projects/cbc/alphaworks/xml4c_debug
export ICUROOT=/u/germain/projects/cbc/alphaworks/xml4c/include/unicode
export LIBPATH=$XERCESCROOT/lib:$LIBPATH
# export STLROOT=/u/germain/projects/cbc/STLport-4.0/stlport

#########################################################################
# Environment set-up for XALAN (xsl processor)
#
# PATH must include /usr/jdk_base/bin
#
export CLASSPATH=$CLASSPATH:/usr/jdk_base/lib/classes.zip:/u/germain/projects/cbc/xalan/xalan.jar:/u/germain/projects/cbc/xalan/xerces.jar:/u/germain/projects/cbc/xalan/bsf.jar:/u/germain/projects/cbc/xalan/samples/xalansamples.jar

