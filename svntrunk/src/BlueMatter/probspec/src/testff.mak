# Makefile for
# testff 

# Makefile generated with AutoMake (Ver. 4.15a)
# Generation time: Thu Apr 26 15:58:23 2001

# User profile: testff.pro

# Command line argument to produce this makefile
# am4new -Ptestff.pro testff.cpp 

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
GCC=xlc_r
GCPPC=xlC_r
GPL8C=plix
GLINK=xlC_r
GSLINK=$AUTOLINKER
GLIBMAN=ar
GASM=as

DEBUGFLAGS=-g 

GCPPFLAGS=\
	-+ -qnotempinc $(DEBUGFLAGS) -I$(XERCESCROOT)/include -I$(XERCESCROOT)/include/dom  \
	-I$(INC40) -I$(INC41) 

GCFLAGS=-I$(INC40) -I$(INC41) 

GLFLAGS=-L. 

GPL8FLAGS=-I$(INC40) -I$(INC41) 

GPL8LFLAGS=-L. 

GCPPLFLAGS=$(GCPPFLAGS)   -L$(XERCESCROOT)/lib -L. 

GLIBFLAGS=osr 

# Predefined symbols



# Include directories
INC40=$(XERCESCROOT)/include
INC41=$(XERCESCROOT)/include/dom


all:  testff 


testff:  testff.o ffparameters.o ffparametersinterface.o 
	$(GLINK) $(GCPPLFLAGS) -otestff testff.o ffparameters.o \
	  ffparametersinterface.o -lxerces-c1_3 

testff.o:  testff.cpp ffparameters.hpp ffparametersinterface.hpp 
	$(GCPPC) -c $(GCPPFLAGS) testff.cpp 

ffparameters.o:  ffparameters.cpp ffparameters.hpp ffparametersinterface.hpp \
	  $(INC40)/util/PlatformUtils.hpp $(INC40)/util/XercesDefs.hpp \
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
	  $(INC40)/util/Compilers/DECCXXDefs.hpp \
	  $(INC40)/util/XMLException.hpp $(INC40)/util/XMLExceptMsgs.hpp \
	  $(INC40)/util/XMLUni.hpp $(INC40)/util/XMLString.hpp \
	  $(INC40)/framework/XMLFormatter.hpp \
	  $(INC40)/util/TranscodingException.hpp \
	  $(INC40)/dom/DOM_DOMException.hpp $(INC40)/dom/DOMString.hpp \
	  $(INC40)/parsers/DOMParser.hpp $(INC40)/dom/DOM_Document.hpp \
	  $(INC40)/dom/DOM_DocumentType.hpp $(INC40)/dom/DOM_Node.hpp \
	  $(INC40)/dom/DOM_DOMImplementation.hpp \
	  $(INC40)/dom/DOM_Element.hpp \
	  $(INC40)/dom/DOM_DocumentFragment.hpp \
	  $(INC40)/dom/DOM_Comment.hpp $(INC40)/dom/DOM_CharacterData.hpp \
	  $(INC40)/dom/DOM_CDATASection.hpp $(INC40)/dom/DOM_Text.hpp \
	  $(INC40)/dom/DOM_ProcessingInstruction.hpp \
	  $(INC40)/dom/DOM_Attr.hpp $(INC40)/dom/DOM_Entity.hpp \
	  $(INC40)/dom/DOM_EntityReference.hpp \
	  $(INC40)/dom/DOM_NodeList.hpp $(INC40)/dom/DOM_Notation.hpp \
	  $(INC40)/dom/DOM_NodeIterator.hpp $(INC41)/DOM_NodeFilter.hpp \
	  $(INC41)/DOM_Node.hpp $(INC40)/dom/DOM_TreeWalker.hpp \
	  $(INC40)/dom/DOM_XMLDecl.hpp $(INC40)/dom/DOM_Range.hpp \
	  $(INC40)/framework/XMLDocumentHandler.hpp \
	  $(INC40)/util/RefVectorOf.hpp \
	  $(INC40)/util/ArrayIndexOutOfBoundsException.hpp \
	  $(INC40)/util/XMLEnumerator.hpp $(INC40)/util/RefVectorOf.c \
	  $(INC40)/framework/XMLAttr.hpp $(INC40)/framework/XMLAttDef.hpp \
	  $(INC40)/framework/XMLErrorReporter.hpp \
	  $(INC40)/util/XMLMsgLoader.hpp \
	  $(INC40)/framework/XMLEntityHandler.hpp \
	  $(INC40)/util/ValueStackOf.hpp \
	  $(INC40)/util/EmptyStackException.hpp \
	  $(INC40)/util/ValueVectorOf.hpp $(INC40)/util/ValueVectorOf.c \
	  $(INC40)/util/ValueStackOf.c \
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
	  $(INC40)/framework/XMLElementDecl.hpp \
	  $(INC40)/framework/XMLAttDefList.hpp \
	  $(INC40)/framework/XMLContentModel.hpp \
	  $(INC40)/validators/DTD/DTDEntityDecl.hpp \
	  $(INC40)/framework/XMLEntityDecl.hpp \
	  $(INC40)/validators/DTD/DTDValidator.hpp \
	  $(INC40)/util/NameIdPool.hpp $(INC40)/util/NameIdPool.c \
	  $(INC40)/util/StringPool.hpp $(INC40)/framework/XMLValidator.hpp \
	  $(INC40)/framework/XMLValidityCodes.hpp \
	  $(INC40)/framework/XMLRefInfo.hpp $(INC40)/dom/DOM.hpp \
	  $(INC40)/dom/DOM_NamedNodeMap.hpp 
	$(GCPPC) -c $(GCPPFLAGS) ffparameters.cpp 

ffparametersinterface.o:  ffparametersinterface.cpp ffparametersinterface.hpp 
	$(GCPPC) -c $(GCPPFLAGS) ffparametersinterface.cpp 

gfebuild:
	mkdir -p $(GFEBUILDROOT)/src
	mkdir -p $(GFEBUILDROOT)/include/BlueMatter
	/usr/bin/cp $(GFESOURCEROOT)/BlueMatter/*/include/*.h* $(GFEBUILDROOT)/include/BlueMatter

clean:
	$(ERASE) testff
	$(ERASE) testff.o
	$(ERASE) ffparameters.o
	$(ERASE) ffparametersinterface.o
