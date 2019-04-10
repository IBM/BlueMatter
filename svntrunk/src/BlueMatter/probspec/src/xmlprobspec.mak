# Makefile for
# xmlprobspec 

# Makefile generated with AutoMake (Ver. 4.15a)
# Generation time: Thu May 17 17:25:10 2001

# User profile: xmlprobspec.pro

# Command line argument to produce this makefile
# am4new -Pxmlprobspec.pro xmlprobspec.cpp 

# Pragma: Global compiler -> CC.  Alias -> xlc
# Pragma: Global C++ compiler -> CC.  Alias -> xlC
# Pragma: Global linker -> CC.  Alias -> xlC
# Pragma: CPP  .C
# Pragma: CPP  .cpp
# Pragma: CPP  .sqc
# Pragma: C  .cfile
# Pragma: HPP  .H
# Pragma: HPP  .hpp
# Pragma: HPP  .c
# Pragma: H  .h

ERASE=rm
GCC=xlc
GCPPC=xlC
GPL8C=plix
GLINK=xlC
GSLINK=$AUTOLINKER
GLIBMAN=ar
GASM=as

GCPPFLAGS=\
	-+ -qnotempinc $(DEBUGFLAGS) -g  -I$(XERCESCROOT) -D PK_SMP -D PK_AIX -D PK_XLC -D bool=int \
	-I$(INC40) -I$(INC41) -I$(INC42) 

GCFLAGS=-I$(INC40) -I$(INC41) -I$(INC42) 

GLFLAGS=-L. 

GPL8FLAGS=-I$(INC40) -I$(INC41) -I$(INC42) 

GPL8LFLAGS=-L. 

GCPPLFLAGS=$(GCPPFLAGS) -g   -L$(XERCESCROOT)/lib -L. 

GLIBFLAGS=osr 

# Predefined symbols



# Include directories
INC40=/u/germain/projects/cbc/alphaworks/xml4c_debug/include
INC41=/ssa09/work/GFEBUILDROOT/include
INC42=/ssa09/work/GFEBUILDROOT/include/BlueMatter


# Source directories
SRC43=.
SRC44=/ssa09/work../src


all:  xmlprobspec 


xmlprobspec:  xmlprobspec.o parsexmlspec.o headerhandler.o molsys.o \
	SAXFFParamHandlers.o syscell.o fragspec.o ffhandler.o terms.o \
	list.o termspec.o ffheaderhandler.o ffsitehandler.o \
	bondlisthandler.o anglelisthandler.o torsionlisthandler.o \
	improperlisthandler.o atom.o bond.o expair.o pair14.o angle.o \
	torsion.o improper.o sitespeclisthandler.o 
	$(GLINK) $(GCPPLFLAGS) -oxmlprobspec xmlprobspec.o \
	  parsexmlspec.o headerhandler.o molsys.o SAXFFParamHandlers.o \
	  syscell.o fragspec.o ffhandler.o terms.o list.o termspec.o \
	  ffheaderhandler.o ffsitehandler.o bondlisthandler.o \
	  anglelisthandler.o torsionlisthandler.o improperlisthandler.o \
	  atom.o bond.o expair.o pair14.o angle.o torsion.o improper.o \
	  sitespeclisthandler.o -lxerces-c1_3 

xmlprobspec.o:  xmlprobspec.cpp $(INC42)/xmlprobspec.hpp $(INC42)/molsys.hpp \
	  $(INC42)/syscell.hpp $(INC42)/terms.hpp $(INC42)/atom.hpp \
	  $(INC42)/list.hpp $(INC42)/bond.hpp $(INC42)/angle.hpp \
	  $(INC42)/torsion.hpp $(INC42)/improper.hpp $(INC42)/expair.hpp \
	  $(INC42)/pair14.hpp $(INC42)/fragscheme.hpp $(INC42)/fragspec.hpp \
	  $(INC42)/termspec.hpp $(INC42)/probspecmacs.hpp \
	  $(INC42)/headerhandler.hpp $(INC42)/parsexmlspec.hpp \
	  $(INC42)/pushparams.hpp 
	$(GCPPC) -c $(GCPPFLAGS) xmlprobspec.cpp 

parsexmlspec.o:  $(SRC43)/parsexmlspec.cpp $(INC41)/BlueMatter/parsexmlspec.hpp \
	  $(INC41)/BlueMatter/pushparams.hpp \
	  $(INC41)/BlueMatter/SAXFFParamHandlers.hpp \
	  $(INC40)/sax/HandlerBase.hpp $(INC40)/sax/DocumentHandler.hpp \
	  $(INC40)/util/XercesDefs.hpp $(INC40)/util/AutoSense.hpp \
	  $(INC40)/util/Platforms/AIX/AIXDefs.hpp \
	  $(INC40)/util/Compilers/VCPPDefs.hpp \
	  $(INC40)/util/Compilers/CSetDefs.hpp \
	  $(INC40)/util/Compilers/BorlandCDefs.hpp \
	  $(INC40)/util/Compilers/SunCCDefs.hpp \
	  $(INC40)/util/Compilers/SCOCCDefs.hpp \
	  $(INC40)/util/Compilers/SunKaiDefs.hpp \
	  $(INC40)/util/Compilers/HPCCDefs.hpp \
	  $(INC40)/util/Compilers/MIPSproDefs.hpp \
	  $(INC40)/util/Compilers/TandemCCDefs.hpp \
	  $(INC40)/util/Compilers/GCCDefs.hpp \
	  $(INC40)/util/Compilers/MVSCPPDefs.hpp \
	  $(INC40)/util/Compilers/IBMVAW32Defs.hpp \
	  $(INC40)/util/Compilers/IBMVAOS2Defs.hpp \
	  $(INC40)/util/Compilers/CodeWarriorDefs.hpp \
	  $(INC40)/util/Compilers/PTXCCDefs.hpp \
	  $(INC40)/util/Compilers/OS400SetDefs.hpp \
	  $(INC40)/util/Compilers/DECCXXDefs.hpp \
	  $(INC40)/sax/DTDHandler.hpp $(INC40)/sax/EntityResolver.hpp \
	  $(INC40)/sax/InputSource.hpp $(INC40)/sax/ErrorHandler.hpp \
	  $(INC40)/sax/SAXParseException.hpp $(INC40)/sax/SAXException.hpp \
	  $(INC40)/util/XMLString.hpp $(INC40)/util/XMLUni.hpp \
	  $(INC41)/BlueMatter/ffhandler.hpp $(INC40)/util/PlatformUtils.hpp \
	  $(INC40)/util/XMLException.hpp $(INC40)/util/XMLExceptMsgs.hpp \
	  $(INC40)/framework/StdInInputSource.hpp \
	  $(INC40)/parsers/SAXParser.hpp $(INC40)/sax/Parser.hpp \
	  $(INC40)/internal/VecAttrListImpl.hpp \
	  $(INC40)/sax/AttributeList.hpp $(INC40)/framework/XMLAttr.hpp \
	  $(INC40)/framework/XMLAttDef.hpp $(INC40)/util/RefVectorOf.hpp \
	  $(INC40)/util/ArrayIndexOutOfBoundsException.hpp \
	  $(INC40)/util/XMLEnumerator.hpp $(INC40)/util/RefVectorOf.c \
	  $(INC40)/framework/XMLDocumentHandler.hpp \
	  $(INC40)/framework/XMLElementDecl.hpp \
	  $(INC40)/framework/XMLAttDefList.hpp \
	  $(INC40)/framework/XMLContentModel.hpp \
	  $(INC40)/framework/XMLEntityHandler.hpp \
	  $(INC40)/framework/XMLErrorReporter.hpp \
	  $(INC40)/util/XMLMsgLoader.hpp \
	  $(INC40)/validators/DTD/DocTypeHandler.hpp \
	  $(INC40)/framework/XMLNotationDecl.hpp \
	  $(INC40)/validators/DTD/DTDAttDef.hpp \
	  $(INC40)/validators/DTD/DTDElementDecl.hpp \
	  $(INC40)/util/RefHashTableOf.hpp $(INC40)/util/KeyValuePair.hpp \
	  $(INC40)/util/KeyValuePair.c $(INC40)/util/HashBase.hpp \
	  $(INC40)/util/IllegalArgumentException.hpp \
	  $(INC40)/util/NoSuchElementException.hpp \
	  $(INC40)/util/RuntimeException.hpp $(INC40)/util/HashXMLCh.hpp \
	  $(INC40)/util/RefHashTableOf.c \
	  $(INC40)/validators/DTD/DTDEntityDecl.hpp \
	  $(INC40)/framework/XMLEntityDecl.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/parsexmlspec.cpp 

headerhandler.o:  $(SRC43)/headerhandler.cpp $(INC41)/BlueMatter/headerhandler.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/headerhandler.cpp 

molsys.o:  molsys.cpp $(INC42)/molsys.hpp $(INC42)/syscell.hpp \
	  $(INC42)/terms.hpp $(INC42)/atom.hpp $(INC42)/list.hpp \
	  $(INC42)/bond.hpp $(INC42)/angle.hpp $(INC42)/torsion.hpp \
	  $(INC42)/improper.hpp $(INC42)/expair.hpp $(INC42)/pair14.hpp \
	  $(INC42)/fragscheme.hpp $(INC42)/fragspec.hpp \
	  $(INC42)/termspec.hpp $(INC42)/probspecmacs.hpp \
	  $(INC42)/headerhandler.hpp $(INC42)/parsexmlspec.hpp 
	$(GCPPC) -c $(GCPPFLAGS) molsys.cpp 

SAXFFParamHandlers.o:  $(SRC43)/SAXFFParamHandlers.cpp $(INC40)/util/XMLUni.hpp \
	  $(INC40)/util/XercesDefs.hpp $(INC40)/util/AutoSense.hpp \
	  $(INC40)/util/Platforms/AIX/AIXDefs.hpp \
	  $(INC40)/util/Compilers/VCPPDefs.hpp \
	  $(INC40)/util/Compilers/CSetDefs.hpp \
	  $(INC40)/util/Compilers/BorlandCDefs.hpp \
	  $(INC40)/util/Compilers/SunCCDefs.hpp \
	  $(INC40)/util/Compilers/SCOCCDefs.hpp \
	  $(INC40)/util/Compilers/SunKaiDefs.hpp \
	  $(INC40)/util/Compilers/HPCCDefs.hpp \
	  $(INC40)/util/Compilers/MIPSproDefs.hpp \
	  $(INC40)/util/Compilers/TandemCCDefs.hpp \
	  $(INC40)/util/Compilers/GCCDefs.hpp \
	  $(INC40)/util/Compilers/MVSCPPDefs.hpp \
	  $(INC40)/util/Compilers/IBMVAW32Defs.hpp \
	  $(INC40)/util/Compilers/IBMVAOS2Defs.hpp \
	  $(INC40)/util/Compilers/CodeWarriorDefs.hpp \
	  $(INC40)/util/Compilers/PTXCCDefs.hpp \
	  $(INC40)/util/Compilers/OS400SetDefs.hpp \
	  $(INC40)/util/Compilers/DECCXXDefs.hpp \
	  $(INC40)/sax/AttributeList.hpp \
	  $(INC41)/BlueMatter/SAXFFParamHandlers.hpp \
	  $(INC40)/sax/HandlerBase.hpp $(INC40)/sax/DocumentHandler.hpp \
	  $(INC40)/sax/DTDHandler.hpp $(INC40)/sax/EntityResolver.hpp \
	  $(INC40)/sax/InputSource.hpp $(INC40)/sax/ErrorHandler.hpp \
	  $(INC40)/sax/SAXParseException.hpp $(INC40)/sax/SAXException.hpp \
	  $(INC40)/util/XMLString.hpp $(INC41)/BlueMatter/ffhandler.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/SAXFFParamHandlers.cpp 

syscell.o:  syscell.cpp $(INC42)/syscell.hpp $(INC42)/terms.hpp \
	  $(INC42)/atom.hpp $(INC42)/list.hpp $(INC42)/bond.hpp \
	  $(INC42)/angle.hpp $(INC42)/torsion.hpp $(INC42)/improper.hpp \
	  $(INC42)/expair.hpp $(INC42)/pair14.hpp 
	$(GCPPC) -c $(GCPPFLAGS) syscell.cpp 

fragspec.o:  fragspec.cpp $(INC42)/fragspec.hpp $(INC42)/syscell.hpp \
	  $(INC42)/terms.hpp $(INC42)/atom.hpp $(INC42)/list.hpp \
	  $(INC42)/bond.hpp $(INC42)/angle.hpp $(INC42)/torsion.hpp \
	  $(INC42)/improper.hpp $(INC42)/expair.hpp $(INC42)/pair14.hpp \
	  $(INC42)/termspec.hpp $(INC42)/probspecmacs.hpp \
	  $(INC42)/headerhandler.hpp 
	$(GCPPC) -c $(GCPPFLAGS) fragspec.cpp 

ffhandler.o:  $(SRC43)/ffhandler.cpp $(INC41)/BlueMatter/ffhandler.hpp \
	  $(INC41)/BlueMatter/ffheaderhandler.hpp \
	  $(INC41)/BlueMatter/ffsitehandler.hpp \
	  $(INC41)/BlueMatter/bondlisthandler.hpp \
	  $(INC41)/BlueMatter/anglelisthandler.hpp \
	  $(INC41)/BlueMatter/torsionlisthandler.hpp \
	  $(INC41)/BlueMatter/improperlisthandler.hpp \
	  $(INC40)/util/XMLString.hpp $(INC40)/util/XercesDefs.hpp \
	  $(INC40)/util/AutoSense.hpp \
	  $(INC40)/util/Platforms/AIX/AIXDefs.hpp \
	  $(INC40)/util/Compilers/VCPPDefs.hpp \
	  $(INC40)/util/Compilers/CSetDefs.hpp \
	  $(INC40)/util/Compilers/BorlandCDefs.hpp \
	  $(INC40)/util/Compilers/SunCCDefs.hpp \
	  $(INC40)/util/Compilers/SCOCCDefs.hpp \
	  $(INC40)/util/Compilers/SunKaiDefs.hpp \
	  $(INC40)/util/Compilers/HPCCDefs.hpp \
	  $(INC40)/util/Compilers/MIPSproDefs.hpp \
	  $(INC40)/util/Compilers/TandemCCDefs.hpp \
	  $(INC40)/util/Compilers/GCCDefs.hpp \
	  $(INC40)/util/Compilers/MVSCPPDefs.hpp \
	  $(INC40)/util/Compilers/IBMVAW32Defs.hpp \
	  $(INC40)/util/Compilers/IBMVAOS2Defs.hpp \
	  $(INC40)/util/Compilers/CodeWarriorDefs.hpp \
	  $(INC40)/util/Compilers/PTXCCDefs.hpp \
	  $(INC40)/util/Compilers/OS400SetDefs.hpp \
	  $(INC40)/util/Compilers/DECCXXDefs.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/ffhandler.cpp 

terms.o:  terms.cpp $(INC42)/terms.hpp $(INC42)/atom.hpp $(INC42)/list.hpp \
	  $(INC42)/bond.hpp $(INC42)/angle.hpp $(INC42)/torsion.hpp \
	  $(INC42)/improper.hpp $(INC42)/expair.hpp $(INC42)/pair14.hpp 
	$(GCPPC) -c $(GCPPFLAGS) terms.cpp 

list.o:  list.cpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) list.cpp 

termspec.o:  termspec.cpp $(INC42)/termspec.hpp $(INC42)/terms.hpp \
	  $(INC42)/atom.hpp $(INC42)/list.hpp $(INC42)/bond.hpp \
	  $(INC42)/angle.hpp $(INC42)/torsion.hpp $(INC42)/improper.hpp \
	  $(INC42)/expair.hpp $(INC42)/pair14.hpp 
	$(GCPPC) -c $(GCPPFLAGS) termspec.cpp 

ffheaderhandler.o:  $(SRC43)/ffheaderhandler.cpp \
	  $(INC41)/BlueMatter/ffheaderhandler.hpp \
	  $(INC41)/BlueMatter/ffhandler.hpp \
	  $(INC41)/BlueMatter/headerhandler.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/ffheaderhandler.cpp 

ffsitehandler.o:  $(SRC43)/ffsitehandler.cpp $(INC41)/BlueMatter/ffsitehandler.hpp \
	  $(INC41)/BlueMatter/ffhandler.hpp \
	  $(INC41)/BlueMatter/sitespeclisthandler.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/ffsitehandler.cpp 

bondlisthandler.o:  $(SRC43)/bondlisthandler.cpp \
	  $(INC41)/BlueMatter/bondlisthandler.hpp \
	  $(INC41)/BlueMatter/pushparams.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/bondlisthandler.cpp 

anglelisthandler.o:  $(SRC43)/anglelisthandler.cpp \
	  $(INC41)/BlueMatter/anglelisthandler.hpp \
	  $(INC41)/BlueMatter/pushparams.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/anglelisthandler.cpp 

torsionlisthandler.o:  $(SRC43)/torsionlisthandler.cpp \
	  $(INC41)/BlueMatter/torsionlisthandler.hpp \
	  $(INC41)/BlueMatter/pushparams.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/torsionlisthandler.cpp 

improperlisthandler.o:  $(SRC43)/improperlisthandler.cpp \
	  $(INC41)/BlueMatter/improperlisthandler.hpp \
	  $(INC41)/BlueMatter/pushparams.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/improperlisthandler.cpp 

atom.o:  atom.cpp $(INC42)/atom.hpp $(INC42)/list.hpp $(INC42)/bond.hpp \
	  $(INC42)/angle.hpp $(INC42)/torsion.hpp $(INC42)/improper.hpp 
	$(GCPPC) -c $(GCPPFLAGS) atom.cpp 

bond.o:  bond.cpp $(INC42)/bond.hpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) bond.cpp 

expair.o:  expair.cpp $(INC42)/expair.hpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) expair.cpp 

pair14.o:  pair14.cpp $(INC42)/pair14.hpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) pair14.cpp 

angle.o:  angle.cpp $(INC42)/angle.hpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) angle.cpp 

torsion.o:  torsion.cpp $(INC42)/torsion.hpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) torsion.cpp 

improper.o:  improper.cpp $(INC42)/improper.hpp $(INC42)/list.hpp 
	$(GCPPC) -c $(GCPPFLAGS) improper.cpp 

sitespeclisthandler.o:  $(SRC43)/sitespeclisthandler.cpp \
	  $(INC41)/BlueMatter/sitespeclisthandler.hpp \
	  $(INC41)/BlueMatter/pushparams.hpp 
	$(GCPPC) -c $(GCPPFLAGS) $(SRC43)/sitespeclisthandler.cpp 

gfebuild:
	mkdir -p $(GFEBUILDROOT)/src
	mkdir -p $(GFEBUILDROOT)/include/BlueMatter
	/usr/bin/cp $(GFESOURCEROOT)/BlueMatter/*/include/*.h* $(GFEBUILDROOT)/include/BlueMatter

clean:
	$(ERASE) xmlprobspec
	$(ERASE) xmlprobspec.o
	$(ERASE) parsexmlspec.o
	$(ERASE) headerhandler.o
	$(ERASE) molsys.o
	$(ERASE) SAXFFParamHandlers.o
	$(ERASE) syscell.o
	$(ERASE) fragspec.o
	$(ERASE) ffhandler.o
	$(ERASE) terms.o
	$(ERASE) list.o
	$(ERASE) termspec.o
	$(ERASE) ffheaderhandler.o
	$(ERASE) ffsitehandler.o
	$(ERASE) bondlisthandler.o
	$(ERASE) anglelisthandler.o
	$(ERASE) torsionlisthandler.o
	$(ERASE) improperlisthandler.o
	$(ERASE) atom.o
	$(ERASE) bond.o
	$(ERASE) expair.o
	$(ERASE) pair14.o
	$(ERASE) angle.o
	$(ERASE) torsion.o
	$(ERASE) improper.o
	$(ERASE) sitespeclisthandler.o
