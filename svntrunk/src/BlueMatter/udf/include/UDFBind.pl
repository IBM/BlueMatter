#!/usr/bin/perl

# UDFBind
# Scans User Defined Function (UDF) registry and creates binding file of functions
# Requires existing header file for each udf in registry
# Output file has:
# enum { StdHarmonic_Code=1, foo=2, etc}
# struct StdHarmonic_Params { }
# case StdHarmonic_Code { code }
# inline void StdHarmonicBond_Exec()
# :
#
# The structures need to be public, for testing - everything else is accessed only through
# the UDF_ExecuteCode() public function


use strict;

#some globals

my $MaxColumnIndex = 10;
my @UDFTypes = ("NSQ0", "NSQ1", "NSQ2", "ListBased", "Function", "PairFunction", "Transform", "PairTransform");
my $UDFType;

my $UDFRegistryName = "UDF_Registry.txt";
my $UDFRegistryIFFile = "UDF_RegistryIF.hpp";
my $UDFBindingFileName = "UDF_Binding.hpp";
my $UDFCategoryString;
my $UDFName;
my $UDFNumber;
my $SourcePath = "./";

my %PrimitiveSizes = (double => 8, int => 4, float => 4, XYZ => 24);
my %PrimitiveTypeNums = (double => 0, int => 1, float => 2, XYZ => 3);

Main();
exit;



sub Main {

    if (@ARGV > 1) {
        if ($ARGV[0] eq "-buildall") {
            $SourcePath = $ARGV[1];
        }
    }

    open(BINDERFILE, ">$UDFBindingFileName") or die "Can't open $UDFBindingFileName: $!";
    select(BINDERFILE);

    PrintHead();

    DumpEnums();

    DumpStructs();

    DumpCases();

    DumpSizeofs();

    DumpNParams();

    DumpNSites();

    DumpInlines();

    PrintTail();

    select(STDOUT);

}


sub GetUDFType {
    my $cat;
    my $type;
    my @cats = split(':', $UDFCategoryString);
    foreach $cat (@cats) {
        foreach $type (@UDFTypes) {
            if ($cat eq $type) {
                $UDFType = $type;
            }
        }
    }
}


sub PrintHead() {
    print <<HEAD;
#ifndef UDF_BINDING_HPP
#define UDF_BINDING_HPP

//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//
//  This file was automatically created by the script UDFBind.pl, which constructs the UDF_Registry class
//  using the contents of the UDF registry and all its corresponding UDF .hpp files.
//
//  If changes are needed to this file, please communicate them to your local UDF control authority.
//

#include <cstring>
using namespace std ;

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/math.hpp>
#include <BlueMatter/UDF_State.hpp>
#include <BlueMatter/UDF_HelperWater.hpp>
#include <BlueMatter/UDF_HelperLekner.hpp>
#include <BlueMatter/UDF_HelperGroup.hpp>

class UDF_Binding
{
    struct RegistryRecord
    {
        int    mCode;
        char   *mCategories;
    };

    enum { TOKENBUFFERSIZE = 1024 };

    static char mTokenBuffer[TOKENBUFFERSIZE];

    // Private method for internal use...
    // This could be a bsearch or something.  Order should be maintained in registry.
    static inline int GetIndexByUDFCode(const int code)
    {
        int i;
        for (i=0; i<NUMENTRIES; i++) {
            if (code == UDF_RegistryList[i].mCode)
                return i;
        }
        return CODENOTFOUND;
    }


public:

    enum {CODENOTFOUND = -1};
    enum {ARGNOTFOUND = -1};

    template <class UDFParamType> class NSQ2_Param
    {
    public:
        UDFParamType *A;
        UDFParamType *B;
    };

    template <class InputParamType, class OutputParamType> class Function_Param
    {
    public:
        InputParamType  *A;
        OutputParamType *B;
    };

    template <class InputParamType, class OutputParamType> class PairFunction_Param
    {
    public:
        InputParamType *A;
        InputParamType *B;
        OutputParamType *C;
    };

    template <class UDFParamType> class Transform_Param
    {
    public:
        UDFParamType *A;
        UDFParamType *B;
    };

    template <class UDFParamType> class PairTransform_Param
    {
    public:
        UDFParamType *A;
        UDFParamType *B;
        UDFParamType *C;
    };


    // Does the UDF Code contain the requested category?
    static inline void IsUDFCodeCategory(const int code, const char *cat, int &result)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            result = 0;
	    // If we're building data only, we won't have functional logging
	    // so this would be a syntax error. However this should never
	    // be driven in the data only case ...
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "IsUDFCodeCategory failed - code " << code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
        }

        assert(strlen(UDF_RegistryList[i].mCategories) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mCategories);
        char *p = strtok(mTokenBuffer, ":");
        while (p && strcmp(p, cat))
            p = strtok(NULL, ":");

        result = (p != NULL);
    }


HEAD

    my $RecordCount = 0;
    my $MaxSiteCount = -1;
    my $first = 1;
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            $RecordCount++;
            if ($MaxSiteCount < $parms[6]) {
                $MaxSiteCount = $parms[6];
            }
        }
    }

    print "    enum { NUMENTRIES = $RecordCount };\n";
    print "\n";
    print "    enum { MAXSITECOUNT = $MaxSiteCount };\n";
    print "\n";
    print "    static RegistryRecord UDF_RegistryList[];\n";
    print "\n";
}

# go through registry and dump enum for each one
sub DumpEnums() {
    my $first = 1;
    my $maxcode = -1;
    my $codecount = 0;
    print "    enum\n";
    print "    {\n";

    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            if ($first) {
                $first = 0;
            } else {
                print ",\n";
            }
            printf "        $parms[1]\_Code = $parms[0]";
            if ($maxcode < $parms[0]) {
                $maxcode = $parms[0];
            }
            $codecount++;
        }
    }

    print "\n";
    print "    };\n";
    print "\n";
    print "    enum { UDF_CODE_COUNT = $codecount };\n";
    print "\n";
    print "    enum { UDF_MAX_CODE = $maxcode };\n";
    print "\n";
}

# go through registry and dump struct for each one
sub DumpStructs() {
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            if ($parms[9] ne "EMPTY:EMPTY") {
                my $UDFName = $parms[1];
                print "    struct $UDFName\_Params\n";
                print "    {\n";
                my $VariableString = $parms[9];
                my @Vars = split(/:/, $VariableString);
                if (@Vars[0] eq "None") {
                    print "        float mDummyPlaceHolder;\n";
                } else {
                    my $i = 0;
                    while ($i < $#Vars) {
                        print "        $Vars[$i] $Vars[$i+1];\n";
                        $i += 2;
                    }
                }
                print "    };\n";
                print "\n";
            }
            if ($parms[10] ne "EMPTY:EMPTY") {
                my $UDFName = $parms[1];
                print "    struct $UDFName\_OutputParams\n";
                print "    {\n";
                my $VariableString = $parms[10];
                my @Vars = split(/:/, $VariableString);
                if (@Vars[0] eq "None") {
                    print "        float mDummyPlaceHolder;\n";
                } else {
                    my $i = 0;
                    while ($i < $#Vars) {
                        print "        $Vars[$i] $Vars[$i+1];\n";
                        $i += 2;
                    }
                }
                print "    };\n";
                print "\n";
            }
        }
    }
}


# go through registry; for each code create a case statement calling the
# appropriate execute function (inlined) with the appropriate cast.
sub DumpCases() {
    print <<CASES;
// Shouldn't be calling anything via the 'switch' nowadays
#if 0

    template <class MDVM> static inline void UDF_Execute(const int Code, MDVM &aMDVM, void *Params)
    {
        switch (Code)
        {
CASES
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            my $UDFName = $parms[1];
            $UDFCategoryString = $parms[8];
            $UDFType = "ListBased";
            GetUDFType();
            print "        case $UDFName\_Code:\n";
            if ($UDFType eq "ListBased") {
                if ($parms[9] eq "EMPTY:EMPTY") {
                    print "            UDF\_$UDFName\_Execute(aMDVM);\n";
                } else {
                    print "            UDF\_$UDFName\_Execute(aMDVM, *($UDFName\_Params *)Params);\n";
                }
            } elsif ($UDFType eq "NSQ0") {
                    print "            UDF\_$UDFName\_Execute(aMDVM);\n";
            } elsif ($UDFType eq "NSQ1") {
                    print "            UDF\_$UDFName\_Execute(aMDVM, *($UDFName\_Params *)Params);\n";
            } elsif ($UDFType eq "NSQ2") {
                    print "            UDF\_$UDFName\_Execute(aMDVM, *(*(NSQ2_Param<$UDFName\_Params> *)Params).A, *(*(NSQ2_Param<$UDFName\_Params> *)Params).B);\n";
            } elsif ($UDFType eq "PairTransform") {
                    print "            UDF\_$UDFName\_Execute(aMDVM, *(*(PairTransform_Param<$UDFName\_Params> *)Params).A, *(*(PairTransform_Param<$UDFName\_Params> *)Params).B, *(*(PairTransform_Param<$UDFName\_Params> *)Params).C);\n";
            } elsif ($UDFType eq "Function") {
                    print "            UDF\_$UDFName\_Execute(aMDVM, *(*(Function_Param<$UDFName\_Params, $UDFName\_OutputParams> *)Params).A, *(*(Function_Param<$UDFName\_Params, $UDFName\_OutputParams> *)Params).B);\n";
            }
            print "            break;\n";
        }
    }
    print <<ENDCASES;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "UDF_Execute failed - Code " << Code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
            break;
        }
    }
#endif

ENDCASES
}

# go through registry; for each code create a case statement calling the
# appropriate sizeof
sub DumpSizeofs() {
    print <<SIZEOFS;

    static inline int GetParamSizeByUDFCode(const int Code)
    {
        switch (Code)
        {
SIZEOFS
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            my $UDFName = $parms[1];
            print "        case $UDFName\_Code:\n";
            if ($parms[9] eq "EMPTY:EMPTY") {
                print "            return 0;\n";
            } else {
                print "            return sizeof($UDFName\_Params);\n";
            }
            print "            break;\n";
        }
    }
    print <<ENDSIZEOFS;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "GetParamSizeByUDFCode failed - Code " << Code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
            break;
        }
        return -1;
    }

ENDSIZEOFS
}

    sub DumpNParams() {
        print <<NPARAMS;

    static inline int GetNumParamsByUDFCode(const int Code)
    {
        switch (Code)
        {
NPARAMS
        @ARGV = "$SourcePath/$UDFRegistryName";
        while (<>) {
            my @parms = split();
            if ($#parms == $MaxColumnIndex) {
                my $UDFName = $parms[1];
                print "        case $UDFName\_Code:\n";
                print "            return $parms[7];\n";
                print "            break;\n";
            }
        }
        print <<ENDNPARAMS;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
               << "GetNumParamsByUDFCode failed - Code " << Code << " not found"
               << EndLogLine ;
#endif
            assert(0);
            break;
        }
        return -1;
    }

ENDNPARAMS
    }

    sub DumpNSites() {
        print <<NSITES;

    static inline int GetNumSitesByUDFCode(const int Code)
    {
        switch (Code)
        {
NSITES
        @ARGV = "$SourcePath/$UDFRegistryName";
        while (<>) {
            my @parms = split();
            if ($#parms == $MaxColumnIndex) {
                my $UDFName = $parms[1];
                print "        case $UDFName\_Code:\n";
                print "            return $parms[6];\n";
                print "            break;\n";
            }
        }
                print <<ENDNSITES;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "GetNumSitesByUDFCode failed - Code " << Code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
            break;
        }
        return -1;
    }

ENDNSITES
    }

# For each code, dump the body of its function
sub DumpInlines() {
#   print "private:\n";
    print "\n";
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            $UDFCategoryString = $parms[8];
            $UDFType = "ListBased";
            GetUDFType();
            my $UDFName = $parms[1];
            my $PrintParam = 0;
            if ($UDFType eq "ListBased") {
                $PrintParam = ($parms[9] ne "EMPTY:EMPTY");
            }
            print "template\n";
            if ($PrintParam) {
                print "<class MDVM, class ParamsType>\n";
            } else {
                print "<class MDVM>\n";
            }
            print "static\n";
            print "inline\n";
            print "void\n";
            DumpBody($UDFName, $PrintParam);
            print "\n";
        }
    }
}

# For given code name, dump out function body of corresponding .hpp
# Later this will do a checkout of each .hpp
sub DumpBody {
    my $i;

    my $UDFHppName = "UDF\_@_[0]\.hpp";
    open(UDFHPP, $UDFHppName) or die "$UDFHppName not found: $!";
    # skip header line since we templatize the parameter, but only when ListBased
    # other udf types are just scooped in
    if ($UDFType eq "ListBased") {
        <UDFHPP>;
        if (@_[1]) {
            print "UDF\_@_[0]\_Execute(MDVM &aMDVM, ParamsType &Params)\n";
        } else {
            print "UDF\_@_[0]\_Execute(MDVM &aMDVM)\n";
        }
    } else {
        $_ = <UDFHPP>;
        s/void\s*/UDF_/;
        print;
    }
    while (<UDFHPP>) {
        print;
    }

    close(UDFHPP);
}


sub PrintTail() {
    print <<TAIL;
};

#ifndef MSD_COMPILE_CODE_ONLY

char UDF_Binding::mTokenBuffer[UDF_Binding::TOKENBUFFERSIZE];

UDF_Binding::RegistryRecord UDF_Binding::UDF_RegistryList[] =
{
TAIL

    my $first = 1;
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            if ($first) {
                $first = 0;
            } else {
                print ",\n";
            }
            printf '    {%d, "%s"}', $parms[0], $parms[8];
        }
    }
    print "\n";
    print "};\n";
    print "#endif\n";
    print "\n";
    print "#endif\n";
}
