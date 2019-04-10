/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /*
 * This file is used to generate the Common Metadata File or CMF
 * It may be depricated in future versions of ODEi
*/

// #define _aix_
/* utility macros */
#define QUOTE(s2) #s2
#define STRING(s) QUOTE(s)
#define APPEND(value1, value2)  value1 ## value2
#define MKPATH(path, filename)  APPEND(path, filename)
#define MKSEP(pathsep) pathsep

#ifdef _x86_nt_
    #define PATHSEP MKSEP(\) 
    #define SHAREDLIB1 libexa.dll
    #define GROUPID "sys"
#else
    #define PATHSEP MKSEP(/)
#endif

#define PARENT_DIR     bluegene
#ifdef _x86_sco_
    #define INSTALL_DIR    /opt/
    #define BLUEGENE_DIR   /opt/bluegene/
    #define BLUEGENEDIR   "/opt/bluegene/"
    #define BINPATH        /opt/bluegene/bin
    #define HTMLPATH       /opt/bluegene/html
    #define LINKPATH       /opt/bluegene/links
#else
//    #define INSTALL_DIR    MKPATH(BASE_NAME,PATHSEP)
    #define INSTALL_DIR    /usr/opt/bluegene
    #define BLUEGENE_DIR   MKPATH(INSTALL_DIR,MKPATH(PARENT_DIR,PATHSEP))
//    #define BLUEGENEDIR    STRING(BLUEGENE_DIR)
    #define BLUEGENEDIR    BLUEGENE_DIR
    #define BINPATH   MKPATH(BLUEGENE_DIR,MKPATH(bin,PATHSEP))
    #define HTMLPATH  MKPATH(BLUEGENE_DIR,MKPATH(html,PATHSEP))
    #define LINKPATH  MKPATH(BLUEGENE_DIR,MKPATH(links,PATHSEP))
#endif
//#define INSTALLDIR   STRING(INSTALL_DIR)
#define INSTALLDIR    INSTALL_DIR

#ifdef _linux_
    #define ENTITY1 "bin"
    #define ENTITY2 ""
    #define ENTITY3 ""
    #define OS_NAME        [ "Linux" ]
    #define OS_RELEASE     [ "6" ]
    #define OS_VERSION     [ "1" ]
    #define CATEGORY       'Application/Networking'
    #define CATEGORY2      'Applicatoin/Networking'
    #define REQUISITES
    #define PREFIX STRING(BASE_NAME)
#else
    #define OS_NAME        [ "mips_irix" "sparc_solaris" "x86_solaris" \
                             "rios_aix" "ia64_aix" "hp9000_ux" "x86_nt" ]
    #define OS_RELEASE     [ "6" "2" "2" "4" "5" "10" "4" ]
    #define OS_VERSION     [ "5" "3" "6" "1.5.0" "0" "0.0" "0" ]
    #define CATEGORY       'application'
    #define CATEGORY2     
    #define PREFIX STRING(MKPATH(INSTALL_DIR,PARENT_DIR))
#endif
#define USERID    "root"
// #define BINSRC    STRING(BINPATH)
#define HTMLSRC   STRING(HTMLPATH)
#define LINKSRC   STRING(LINKPATH)
#ifdef _mvs390_oe_
    #define BINTGT   "abpsbin"
    #define HTMLTGT  "abpshtml"
    #define PARENT   NULL
    #define SHAREDLIB1 libexa.a
    #define ENTITY1 "hbps112"
    #define ENTITY2 "jbps112"
    #define ENTITY3 ""
    #define GROUPID "sys"
    #define COPYRIGHT < ./copyright.txt >
    #define REQUISITES
    #define REQUISITES1 [ ("S" "hbps111") ]
    #define REQUISITES2
#else
    #define BINTGT   BINSRC
    #define HTMLTGT  HTMLSRC
    #define LINKTGT  LINKSRC
    #define PARENT   STRING(PARENT_DIR)
    #define COPYRIGHT "Copyright IBM, 2006"
#endif

#ifdef _hp9000_ux_
    #define BOOTRQMT 'n'
    #define MODE "-m 755"
    #define ENTITY1 "bluegenebin"
    #define ENTITY2 "bluegenedoc"
    #define ENTITY3 ""
    #define SHAREDLIB1 libexa.sl
    #define PKGCONSTANT1 ""
    #define PKGCONSTANT2 ""
    #define GROUPID "sys"
    #define FTYPE 'F'
    #define DTYPE 'D'
    #define LTYPE 'S'
    #define REQUISITES
    #define REQUISITES1
    #define REQUISITES2
    #define CONFIG_FILES [ ("postinstall" "post") \
                         ("preremove" "preremove") ]
    #define MACHINE_TYPE
#else
    #define MODE    "755"
    #define BOOTRQMT 'No' 
    #define CONFIG_FILES
    #define MACHINE_TYPE "R" 
    #ifdef _aix_
        #define PARENT STRING(PARENT_DIR)
        #define ENTITY1 "bluegene.bin"
        #define ENTITY2 "bluegene.doc"
        #define ENTITY3 "bluegene.links"
        #define PKGCONSTANT1 "" 
        #define PKGCONSTANT2 "" 
        #define GROUPID "sys"
        #define SHAREDLIB1 libexa.a
        #define FTYPE 'F'
        #define DTYPE 'D'
        #define LTYPE 'S'
        #define REQUISITES  [ ("P" "pkgtools.core 1.0.1.0") ("S" "1" "1") ]
        #define REQUISITES1 [ ("P" "pkgtools.core 1.0.1.0") ]
        #define REQUISITES2 [ ("P" "pkgtools.core 1.0.1.0") ]
        #define REQUISITES3 [ ("P" "pkgtools.core 1.0.1.0") ] 
    #elif !defined(_x86_nt_) && !defined(_mvs390_oe_)
        #define PARENT STRING(PARENT_DIR)
        #define PKGCONSTANT1 "!BIN=/usr/bin"
        #define PKGCONSTANT2 "!PROJDIR=/usr/projects"
        #define GROUPID "other"
        #define SHAREDLIB1 libexa.so
        #define FTYPE 'f'
        #define DTYPE 'd'
        #define LTYPE 's'
        #define REQUISITES1
        #define REQUISITES2
        #define REQUISITES3
    #endif
#endif
#define SHAREDLIB  STRING(SHAREDLIB1)

#if defined(_solaris_) || defined(_mips_irix_) || defined(_x86_sco_)
    #define REQUISITES [ ("P" "nsu" "Network Support Utilities") ("S" "1" "1") ]
    #define ENTITY1 "bluegene.bin"
    #define ENTITY2 "bluegene.doc"
    #define ENTITY3 "bluegene.links"
    #define SHAREDLIB1 libexa.so
#endif


//#define QUOTE(s2) #s2
//#define STRING(s) QUOTE(s)
//#define APPEND(value1,value2) value1 ## value2
//#define MKPATH(path,filename) APPEND(path,filename)
#define ADDEXT(filename,extension) APPEND(filename, extension)

#define MODE "755"
#define EXEC_MODE "555"
#define READ_MODE "444"
#define RW_MODE "666"
#define SU_EXEC_MODE "4555"


  #define VERSION    '0'
  #define RELEASE    '0'
  #define MAINTLEVEL '0606'
  #define FIXLEVEL   '28'

//  #define COPYRIGHT "Copyright IBM 2001,2006 All Rights Reserved"

#if 1 || defined(rios_aix_4) || defined(rios_aix_5)

  /* AIX specific settings */

  #define BOOTRQMT 'No'
  #define FILE 'F'
  #define DIR 'D'
  #define LINK 'S'

//  #define INSTALLDIR  /usr/opt/bluegene

  #define LIBEXT .a
  #define DYNLOADOBJEXT .so

  #define root     STRING(0)
  #define system   STRING(0)
  /* Can't have just 'bin' - would screw up path names */
  /* #define bin_user STRING(2) */

#endif

#define NR_ENTITY_NAME "bluegene.pk.adt.nr"
#define NR_FULL_ENTITY_NAME NR_ENTITY_NAME

#define bin_user STRING(2)

   #define INSTALLSRC INSTALLDIR/
  #define BINSRC      INSTALLDIR/bin/
  #define INCSRC      INSTALLDIR/include/
  #define SRCSRC      INSTALLDIR/src/
  #define LIBSRC      INSTALLDIR/lib/
  #define SAMPSRC     INSTALLDIR/sample/
  #define DOCSRC      INSTALLDIR/html/

#define BINTGT   BINSRC
#define INCTGT   INCSRC
#define SRCTGT   SRCSRC
#define LIBTGT   LIBSRC
#define SAMPTGT  SAMPSRC
#define DOCTGT   DOCSRC

#include <cmf/pk.entity>
#include <cmf/pk_adt.entity>
#include <cmf/pk_nr.entity>
#define PACKAGE_FILE(directory, filename)        \
file                                             \
{                                                 \
                                                   \
  partNum = '1';                                 \
  fileType = FILE;                               \
  sourceFile = STRING(filename);                 \
  targetFile = STRING(filename);                 \
  sourceDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  targetDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  permissions = READ_MODE;                       \
  userId = bin_user;                             \
  groupId = bin_user;                            \
}

#define PACKAGE_FILE_EXEC(directory, filename)        \
file                                             \
{                                                 \
                                                   \
  partNum = '1';                                 \
  fileType = FILE;                               \
  sourceFile = STRING(filename);                 \
  targetFile = STRING(filename);                 \
  sourceDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  targetDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  permissions = EXEC_MODE;                       \
  userId = bin_user;                             \
  groupId = bin_user;                            \
}

#define PACKAGE_FILE_SETUID(directory, filename)        \
file                                             \
{                                                 \
                                                   \
  partNum = '1';                                 \
  fileType = FILE;                               \
  sourceFile = STRING(filename);                 \
  targetFile = STRING(filename);                 \
  sourceDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  targetDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  permissions = SU_EXEC_MODE;                       \
  userId = root;                             \
  groupId = bin_user;                            \
}

#define PACKAGE_DIRECTORY(directory)             \
file                                             \
{                                                \
                                                   \
  partNum = '1';                                 \
  fileType = DIR;                                \
  sourceDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  targetDir  = STRING(MKPATH(INSTALLSRC,directory)); \
  permissions = EXEC_MODE;                       \
  userId = bin_user;                             \
  groupId = bin_user;                            \
}
#include <cmf/pk_adt.files>
#include <cmf/pk_nr.files>
#undef PACKAGE_FILE
#undef PACKAGE_FILE_EXEC
#undef PACKAGE_FILE_SETUID
#undef PACKAGE_DIRECTORY

