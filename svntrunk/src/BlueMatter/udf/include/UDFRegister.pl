#!/usr/bin/perl

# UDFRegister.pl
# Enter new UDF (User Defined Function) into UDF registry
# Create new code id if needed, or reuse previous one
# Store code, name, function arguments in registry
# Store function code in separate .hpp file
# Reorders parameters - largest to smallest
#
# Also creates UDF_RegistryIF.hpp, which allows query of the registry:
# GetCodeByName, GetArgByName, GetArgByNum, IsCodeCategory

use strict;

#some globals

my $UDFRegistryName = "UDF_Registry.txt";
my $UDFRegistryIFFile = "UDF_RegistryIF.hpp";
my $UDFFileName = "";
my @Variables = ();
my @OutputVariables = ();
my $InputStructName = "";
my $OutputStructName = "";
my $UDFName;
my $UDFNumber;
my $UDFVariableString;
my $UDFOutputVariableString;
my $UDFCategoryString = "Default";
my $UDFArgCheckSum = 1111;
my $UDFBodyCheckSum = 2222;
my $UDFSiteCount = 2;
my $UDFParamCount = 0;
my $NewRegistryEntry = 0;
my $ExistingRegistryEntry = "";
my @SortedIndices;
my @SortedOutputIndices;
my $MaxColumnIndex = 10;
my $SourcePath = "./";

my %PrimitiveSizes = (double => 8,
                      int => 4,
                      float => 4,
                      XYZ => 24 );

my %PrimitiveTypeNums = (double => 0, int => 1, float => 2, XYZ => 3);
# NSQ and ListBased use the MDVM
# A function takes an input param and returns a different param type as a result
# A transform returns the same type as input
# A Pair operation takes two things of the same type
# NSQ0 is a parameterless operation that returns a parameter
# NSQ1 is a single parameter operation returning nothing
# NSQ2 is a paired parameter operation returning nothing
my @UDFTypes = ("NSQ0", "NSQ1", "NSQ2", "ListBased", "Function", "PairFunction", "Transform", "PairTransform");
my $UDFType = "ListBased";

Main();
exit;


sub Main {

    if (@ARGV < 1) {
        die "usage: UDFRegister [-build] [-reregister UDFName] [-reregisterall] UDFName NSites NParams cat1:cat2:cat3\n";
    }

    if ($ARGV[0] eq "-build") {
        BuildRegistryIFFile();
        return;
    }

    if ($ARGV[0] eq "-reregisterall") {
        ReRegisterAll();
        return;
    }

    if ($ARGV[0] eq "-buildall") {
        if (@ARGV > 1) {
            $SourcePath = $ARGV[1];
            print "building from $SourcePath\n";
        }
        ReRegisterAll();
        return;
    }

    if ($ARGV[0] eq "-reregister") {
        if (@ARGV < 2) {
            print "-reregister requires UDFName\n";
            exit;
        }
        $UDFName = $ARGV[1];
        ReRegister();
        return;
    }

    if (@ARGV > 1) {
        $UDFSiteCount = $ARGV[1];
    }
    if (@ARGV > 2) {
        $UDFParamCount = $ARGV[2];
    }
    if (@ARGV > 3) {
        $UDFCategoryString = $ARGV[3];
    }

    $UDFFileName = "$SourcePath/$ARGV[0]\.udf";

    GetUDFType();

    GetStructs();

    GetUDFNumber();

    ParseArguments();

    DumpBody();

    AddToRegistry();

    BuildRegistryIFFile();

}


sub ReRegisterAll {
    open(REGISTRYFILE, "$SourcePath/$UDFRegistryName") || die "cannot open $SourcePath/$UDFRegistryName: $!";
    while (<REGISTRYFILE>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            $UDFSiteCount = $parms[6];
            $UDFParamCount = $parms[7];
            $UDFCategoryString = $parms[8];

            $UDFFileName = "$SourcePath/$parms[1]\.udf";

            GetUDFType();

            GetStructs();

            GetUDFNumber();

            ParseArguments();

            DumpBody();

            AddToRegistry();
        }
    }
    close(REGISTRYFILE);
    BuildRegistryIFFile();
}

sub ReRegister {
    my $DidIt = 0;
    open(REGISTRYFILE, "$SourcePath/$UDFRegistryName") || die "cannot open $SourcePath/$UDFRegistryName: $!";
    while (<REGISTRYFILE>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex && @parms[1] eq $UDFName) {
            $UDFSiteCount = $parms[6];
            $UDFParamCount = $parms[7];
            $UDFCategoryString = $parms[8];

            $UDFFileName = "$SourcePath/$parms[1]\.udf";

            GetUDFType();

            GetStructs();

            GetUDFNumber();

            ParseArguments();

            DumpBody();

            AddToRegistry();

            $DidIt = 1;

            last;
        }
    }
    close(REGISTRYFILE);
    if (!$DidIt) {
        die "Couldn't find registry entry $UDFName";
    }
    BuildRegistryIFFile();
}

sub GetUDFType {
    my $cat;
    my $type;
    my @cats = split(':', $UDFCategoryString);
    $UDFType = "ListBased";
    foreach $cat (@cats) {
        foreach $type (@UDFTypes) {
            if ($cat eq $type) {
                $UDFType = $type;
            }
        }
    }
}

sub GetStructs {
    @Variables = ();
    @OutputVariables = ();
    open(UDFFILE, $UDFFileName) || die "cannot open $UDFFileName: $!";
    if ($UDFType eq "ListBased" || $UDFType eq "NSQ0") {
        return;
    }
    $InputStructName = <UDFFILE>;
    $InputStructName =~ s/\bstruct\b//;
    $InputStructName =~ s/\s*//g;
    # skip { line
    <UDFFILE>;
    LINE: while (<UDFFILE>) {
        last LINE if /\};/;
        s/,//;
        s/^\s*//;
        s/\s*$//;
        s/\s+/ /g;
        s/\;$//;
        my $name;
        my $type;
        ($type, $name) = split(/\s/);
        # push(@Variables, {Name => $name, Type => $type, Size => $PrimitiveSizes{$type}});
        push(@Variables, {Name => $name, Type => $type, Size => 1 });
    }

    my @indices = (0 .. $#Variables);
    @SortedIndices = sort { $Variables[$b]{Size} == $Variables[$a]{Size} ?
            $Variables[$a]{Name} cmp $Variables[$b]{Name} : $Variables[$b]{Size} <=> $Variables[$a]{Size} } @indices;
    if ($UDFType ne "Function" && $UDFType ne "PairFunction") {
        return;
    }
    $OutputStructName = <UDFFILE>;
    $OutputStructName =~ s/\bstruct\b//;
    $OutputStructName =~ s/\s*//g;
    # skip { line
    <UDFFILE>;
    LINE: while (<UDFFILE>) {
        last LINE if /\};/;
        s/,//;
        s/^\s*//;
        s/\s*$//;
        s/\s+/ /g;
        s/\;$//;
        my $name;
        my $type;
        ($type, $name) = split(/\s/);
        #push(@OutputVariables, {Name => $name, Type => $type, Size => $PrimitiveSizes{$type}});
        push(@OutputVariables, {Name => $name, Type => $type, Size => 1 });
    }
    my @indices = (0 .. $#OutputVariables);
    @SortedOutputIndices = sort { $OutputVariables[$b]{Size} == $OutputVariables[$a]{Size} ?
            $OutputVariables[$a]{Name} cmp $OutputVariables[$b]{Name} : $OutputVariables[$b]{Size} <=> $OutputVariables[$a]{Size} } @indices;
}

# This relies on grep, sort, and tail
sub GetUDFNumber {
#    open(UDFFILE, $UDFFileName) || die "cannot open $UDFFileName: $!";
    $UDFName = <UDFFILE>;
    $UDFName =~ s/\bvoid\b//;
    $UDFName =~ s/\binline\b//;
    $UDFName =~ s/\(//;
    $UDFName =~ s/\s*//g;
    if (!(-e "$SourcePath/$UDFRegistryName")) {
        die "Registry file $SourcePath/$UDFRegistryName doesn't exist";
    }
    my $s = `grep \" $UDFName \" $SourcePath/$UDFRegistryName`;
        chomp $s;
    $ExistingRegistryEntry = $s;
    $s =~ s/^\s*//;
        my @vals = split(/\s+/, $s);
    if (@vals > 0) {
        $UDFNumber = $vals[0];
        $NewRegistryEntry = 0;
    } else {
        $s = `sort -k 1 $SourcePath/$UDFRegistryName | tail -1`;
        chomp $s;
        $s =~ s/^\s*//;
            @vals = split(/\s+/, $s);
        if (@vals > 0) {
            $UDFNumber = $vals[0] + 1;
            $NewRegistryEntry = 1;
        } else {
            die "Registry looks empty: $!";
        }
    }
}


sub ParseArguments {
    if ($UDFType ne "ListBased") {
        LINE: while (<UDFFILE>) {
            last LINE if /\)/;
        }
        return;
    }
    my $i;
    @Variables = ();
    # Assume name starts immediately at top
    # skip MDVM_IF line (must be MDVM_IF &aMDVM_IF,)
    <UDFFILE>;

    # find all variables and build hash
    LINE: while (<UDFFILE>) {
        last LINE if /\)/;
        s/,//;
        s/^\s*//;
        s/\s*$//;
        s/\s+/ /g;
        my $name;
        my $type;
        ($type, $name) = split(/\s/);
        push(@Variables, {Name => $name, Type => $type, Size => $PrimitiveSizes{$type}});
    }

    # created index list sorted by variable size, descending order
    # variables of same size are then sorted in ascending alphabetical order
    my @indices = (0 .. $#Variables);
    @SortedIndices = sort { $Variables[$b]{Size} == $Variables[$a]{Size} ?
            $Variables[$a]{Name} cmp $Variables[$b]{Name} : $Variables[$b]{Size} <=> $Variables[$a]{Size} } @indices;
}


sub DumpBody {
    my $i;

    open(UDFHPPFILE, ">UDF\_$UDFName\.hpp") or die "UDF\_$UDFName\.hpp: $!";
    select(UDFHPPFILE);

    if ($UDFType eq "ListBased") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &Params)\n";
    } elsif ($UDFType eq "NSQ0") {
        print "void $UDFName\_Execute( MDVM &aMDVM )\n";
    } elsif ($UDFType eq "NSQ1") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &A)\n";
    } elsif ($UDFType eq "NSQ2") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &A, const $UDFName\_Params &B)\n";
    } elsif ($UDFType eq "PairFunction") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &A, const $UDFName\_Params &B, $UDFName\_OutputParams &C)\n";
    } elsif ($UDFType eq "Function") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &A, $UDFName\_OutputParams &B)\n";
    } elsif ($UDFType eq "PairTransform") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &A, const $UDFName\_Params &B, $UDFName\_Params &C)\n";
    } elsif ($UDFType eq "Transform") {
        print "void $UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Params &A, $UDFName\_Params &B)\n";
    } else {
        die "$UDFType not recognized in DumpBody\n";
    }

    # print "{\n";

    # only change the variable text if it's a list based call
    if ($UDFType eq "ListBased") {
        # grab and dump, swapping in UDF__p
        while (<UDFFILE>) {
            for $i (0 .. $#Variables) {
                my $var = $Variables[$i]{Name};
                s/([^.a-zA-Z_])$var\b/\1Params\.$var/g;
            }
            print;
        }
    } else {
        while (<UDFFILE>) {
            print;
        }
    }

    select(STDOUT);
    close(UDFHPPFILE);
    close(UDFFILE);
}

# This relies on Unix date functionality
sub AddToRegistry {
    my $dummy;

    ($UDFBodyCheckSum, $dummy) = split(/\s/,`sum UDF_$UDFName\.hpp`);
    # $UDFBodyCheckSum = 0;

    my $EntryDate = `date -u +"%Y/%m/%d"`;
    # my $EntryDate = "12/11/01";
    chomp $EntryDate;
    my $EntryTime = `date -u +"%T"`;
    # my $EntryTime = "10:00";
    chomp $EntryTime;
    my $i;
    $UDFVariableString = "";
    for $i (0 .. $#Variables) {
        if ($i > 0) {
            $UDFVariableString = $UDFVariableString.":";
        }
        my $i_sorted = $SortedIndices[$i];
        $UDFVariableString = $UDFVariableString.$Variables[$i_sorted]{Type}.":".$Variables[$i_sorted]{Name};
    }
    if ($#Variables < 0) {
        $UDFVariableString = "EMPTY:EMPTY";
    }

    $UDFOutputVariableString = "";
    for $i (0 .. $#OutputVariables) {
        if ($i > 0) {
            $UDFOutputVariableString = $UDFOutputVariableString.":";
        }
        my $i_outputsorted = $SortedOutputIndices[$i];
        $UDFOutputVariableString = $UDFOutputVariableString.$OutputVariables[$i_outputsorted]{Type}.":".$OutputVariables[$i_outputsorted]{Name};
    }
    if ($#OutputVariables < 0) {
        $UDFOutputVariableString = "EMPTY:EMPTY";
    }


    ($UDFArgCheckSum, $dummy) = split(/\s/,`echo \"$UDFCategoryString $UDFVariableString $UDFOutputVariableString\" | sum`);

    my $RegistryString = sprintf( "%6d %-30s %10s %8s %7d %7d %3d %3d %-20s %s %s",
    $UDFNumber, $UDFName, $EntryDate, $EntryTime, $UDFArgCheckSum, $UDFBodyCheckSum,
    $UDFSiteCount, $UDFParamCount, $UDFCategoryString, $UDFVariableString, $UDFOutputVariableString);
    if ($NewRegistryEntry) {
        print "Creating new entry in registry for code $UDFNumber : $UDFName\n";
        print $RegistryString, "\n";
        system("echo \"$RegistryString\" >> $UDFRegistryName");
        print "Done\n";
    } else {
        print "Replacing registry entry for code $UDFNumber : $UDFName\n";
        print "\n";
        print "Previous entry:\n";
        print $ExistingRegistryEntry, "\n";
        print "New entry:\n";
        print $RegistryString, "\n";

        if ($SourcePath ne "./") {
            system("cp $SourcePath/$UDFRegistryName .");
        }
        # use perl-created .bak file to change the line
        @ARGV = "$UDFRegistryName";
        $^I = ".bak";
        while (<>) {
            s/$ExistingRegistryEntry/$RegistryString/;
            print;
        }
        $^I = undef;
        print "\n";
        print "Done\n";
    }
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


# go through registry; for each code create a case statement calling the
# appropriate sizeof
sub DumpSizeofs() {
    print <<SIZEOFS;
public:

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
                print "            {\n";
                print "                struct $UDFName\_LocalStruct\n";
                print "                {\n";
                my $VariableString = $parms[9];
                my @Vars = split(/:/, $VariableString);
                my $i = 0;
                while ($i < $#Vars) {
                    print "                    $Vars[$i] $Vars[$i+1];\n";
                    $i += 2;
                }
                print "                };\n";
                print "                return sizeof($UDFName\_LocalStruct);\n";
                print "            }\n";
            }
            print "            break;\n";
        }
    }
    print <<ENDSIZEOFS;
        default:
            BegLogLine(1)
                 << "GetParamSizeByUDFCode failed - Code " << Code << " not found"
                 << EndLogLine ;
            assert(0);
            break;
        }
        return -1;
    }

    enum {CODENOTFOUND = -1};
    enum {ARGNOTFOUND = -1};

private:
ENDSIZEOFS
}


sub BuildRegistryIFFile {

    open(UDFREGIFFILE, ">$UDFRegistryIFFile") or die "$UDFRegistryIFFile: $!";
    select(UDFREGIFFILE);

    print <<HEADER;
#ifndef UDF_REGISTRYIF_HPP
#define UDF_REGISTRYIF_HPP

//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//
//  This file was automatically created by the script UDFRegister.pl, which constructs the UDF_RegistryIF class
//  using the contents of the UDF registry.
//
//  If changes are needed to this file, please communicate them to your local UDF control authority.
//

// #include <BMStuff.hpp>
#include <assert.h>
#include <string.h>
#include <stdio.h>

class UDF_RegistryIF
{
private:

    struct RegistryRecord
    {
        int    mCode;
        char   *mName;
        char   *mDate;
        char   *mTime;
        int    mArgCheckSum;
        int    mBodyCheckSum;
        int    mSiteCount;
        int    mParamCount;
        char   *mCategories;
        char   *mVariables;
        char   *mOutputVariables;
    };

    enum { TOKENBUFFERSIZE = 1024 };
    enum { STRUCTUREBUFFERSIZE = TOKENBUFFERSIZE + 256 };

    static char mTokenBuffer[TOKENBUFFERSIZE];
    static char mStructureBuffer[STRUCTUREBUFFERSIZE];

HEADER

    my $RecordCount = 0;
    my $first = 1;
    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            $RecordCount++;
        }
    }

    print "    static RegistryRecord UDF_RegistryList[];\n";
    print "\n";
    print "    enum { NUMENTRIES = $RecordCount };\n";
    print "\n";

    DumpEnums();

    DumpSizeofs();

    print <<TAIL;

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

    // Go through all codes rather than assume they are sorted...
    static inline void GetMaxUDFCode(int &code)
    {
        assert(NUMENTRIES > 0);
        code = -1;
        for (int i=0; i<NUMENTRIES; i++)
            if (code < UDF_RegistryList[i].mCode)
                code = UDF_RegistryList[i].mCode;
    }

    static inline void GetUDFCodeByName(const char *name, int &code)
    {
        for (int i=0; i<NUMENTRIES; i++) {
            if (!strcmp(name, UDF_RegistryList[i].mName)) {
                code = UDF_RegistryList[i].mCode;
                return;
            }
        }
        code = CODENOTFOUND;
        printf("GetUDFCodeByName failed - CodeName %s not found\\n", name);
        assert(0);
    }

    static inline void GetNSitesByUDFCode(const int code, int &num)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            num = CODENOTFOUND;
            printf("GetNSitesByUDFCode failed - code %d not found\\n", code);
            assert(0);
        }

        num = UDF_RegistryList[code].mSiteCount;
    }

    static inline void GetParamCountByUDFCode(const int code, int &count)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            count = CODENOTFOUND;
            printf("GetParamCountByUDFCode failed - code %d not found\\n", code);
            assert(0);
        }

        count = UDF_RegistryList[code].mParamCount;
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetArgByName(const int code, const char *name, int &ord, char* &type)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            ord = ARGNOTFOUND;
            type = NULL;
            printf("GetArgByName failed - code %d not found\\n", code);
            assert(0);
        }

        ord = 0;
        assert(strlen(UDF_RegistryList[i].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mVariables);
        char *p = strtok(mTokenBuffer, ":");
        while (p) {
            type = p;
            p = strtok(NULL, ":");
            if (p && !strcmp(p, name))
                break;
            ord++;
            p = strtok(NULL, ":");
        }
        if (!p) {
            ord = ARGNOTFOUND;
            type = NULL;
            printf("GetArgByName failed - ArgName %s not found\\n", name);
            assert(0);
        }
    }

    static inline void GetArgCountByUDFCode(const int code, int &count)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            count = CODENOTFOUND;
            printf("GetArgCountByCode failed - code %d not found\\n", code);
            assert(0);
        }

        count = 0;
        assert(strlen(UDF_RegistryList[i].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mVariables);
        char *p = strtok(mTokenBuffer, ":");
        while (p) {
            if (!strcmp(p, "EMPTY"))
                break;
            p = strtok(NULL, ":");
            count++;
            if (p)
                p = strtok(NULL, ":");
        }
    }

    // The returned strings are volatile and should be used immediatly or copied locally
    static inline void GetArgByOrd(const int code, const int ord, char* &name, char* &type)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            name = NULL;
            type = NULL;
            printf("GetArgByOrd failed - Arg %d not found\\n", ord);
            assert(0);
        }

        assert(strlen(UDF_RegistryList[i].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mVariables);
        char *p = strtok(mTokenBuffer, ":");

        // Skip variables to the requested one
        for (i=0; p && (i<=ord); i++) {
            type = p;
            name = strtok(NULL, ":");
            if (name)
                p = strtok(NULL, ":");
        }
        // If either one is null, something's messed up
        // assert on failure
        if (!type || !name) {
            name = NULL;
            type = NULL;
            printf("Bad name or type in GetArgByOrd - Ord %d caused a problem\\n", ord);
            assert(0);
        }
    }

    // Does the UDF Code contain the requested category?
    static inline void IsUDFCodeCategory(const int code, const char *cat, int &result)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            result = 0;
            printf("IsUDFCodeCategory failed - code %d not found\\n", code);
            assert(0);
        }

        assert(strlen(UDF_RegistryList[i].mCategories) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mCategories);
        char *p = strtok(mTokenBuffer, ":");
        while (p && strcmp(p, cat))
            p = strtok(NULL, ":");

        result = (p != NULL);
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetStructureNameByUDFCode(const int code, char* &name)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            name = NULL;
            printf("GetStructureNameByUDFCode failed - Code %d not found\\n", code);
            assert(0);
        }
        sprintf(mTokenBuffer, "%s_Params", UDF_RegistryList[i].mName);
        name = mTokenBuffer;
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetStructureStringByUDFCode(const int code, char* &struc)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            struc = NULL;
            printf("GetStructureStringByUDFCode failed - Code %d not found\\n", code);
            assert(0);
        }

        sprintf(mStructureBuffer, "struct %s_Params\\n{\\n", UDF_RegistryList[i].mName);
        assert(strlen(UDF_RegistryList[code].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[code].mVariables);
        char *p = strtok(mTokenBuffer, ":");
        int first = 1;
        while (p) {
            if (!strcmp(p, "EMPTY"))
                break;
            char *type = p;
            p = strtok(NULL, ":");
            char *name = p;
            if (first)
                first = 0;
            else
                strcat(mStructureBuffer, ";\\n");
            strcat(mStructureBuffer, "\t");
            strcat(mStructureBuffer, type);
            strcat(mStructureBuffer, " ");
            strcat(mStructureBuffer, name);
            p = strtok(NULL, ":");
        }
        strcat(mStructureBuffer, ";\\n};\\n");
        struc = mStructureBuffer;
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetStructureContentsStringByUDFCode(const int code, char* &struc)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            struc = NULL;
            printf("GetStructureContentsStringByUDFCode failed - Code %d not found\\n", code);
            assert(0);
        }

        struc = UDF_RegistryList[code].mVariables;
    }
};

char UDF_RegistryIF::mTokenBuffer[UDF_RegistryIF::TOKENBUFFERSIZE];

char UDF_RegistryIF::mStructureBuffer[UDF_RegistryIF::STRUCTUREBUFFERSIZE];

UDF_RegistryIF::RegistryRecord UDF_RegistryIF::UDF_RegistryList[] =
{
TAIL

    @ARGV = "$SourcePath/$UDFRegistryName";
    while (<>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
            $RecordCount++;
            if ($first) {
                $first = 0;
            } else {
                print ",\n";
            }
            printf '{%d, "%s", "%s", "%s", %d, %d, %d, %d, "%s", "%s", "%s"}', $parms[0], $parms[1], $parms[2], $parms[3], $parms[4], $parms[5], $parms[6], $parms[7], $parms[8], $parms[9], $parms[10];
        }
    }
    print "\n";
    print "};\n";
    print "\n";
    print "#endif\n";
    select(STDOUT);
    close(UDFREGIFFILE);
}
