// c compiler is xlc
gcc CC, "xlc";

// c++ compiler is xlC
//gcppc CC, "/usr/vacpp/bin/xlC";
gcppc CC, "xlC";

// linker is xlC
//glink "CC", "/usr/vacpp/bin/xlC";
glink "CC", "xlC";

// directories to search for dependencies
inclook env(XERCESCROOT)+"/include", env(GFEBUILDROOT)+"/include", env(GFEBUILDROOT)+"/include/BlueMatter";
srclook ".", env(GFESOURCEROOT)+"../src";

// library flags
glibflags "osr";

// extensions belonging to c++ files
cpp .C, .cpp, .sqc;

// extensions belonging to c files
c .cfile;

// extensions belonging to c++ header files
// treat .c extensions as include files.  Actually, they are implementation
// files for template classes and functions.
hpp .H, .hpp, .c;

// c++ compiler flags
//gcppflags "-+ -qlanglvl=ansi -qtempinc=xlc_r_tempinc $(DEBUGFLAGS) -bmaxdata:0x20000000  -D_HEAVY_FBMDATA";
//gcppflags "-+ -qnotempinc -DPKFXLOG_ALL $(DEBUGFLAGS)";
//gcppflags "-+ -qnotempinc $(DEBUGFLAGS) -D__STL_NO_IOSTREAMS ";
gcppflags "-+ -qnotempinc $(DEBUGFLAGS) -g  -I$(XERCESCROOT) -D PK_SMP -D PK_AIX -D PK_XLC -D bool=int";

// c++ linker flags (need to include compiler flags for templates)
gcpplflags "$(GCPPFLAGS) -g ";

tryinclude header.pro;

glibrary linkonly "xerces-c1_3";
gcpplflags " -L$(XERCESCROOT)/lib";

// force dependency
//file "pk/fxlogger.hpp"
//{
//source "fxlogger.cpp";
//}

target "gfebuild" command "mkdir -p $(GFEBUILDROOT)/src", "mkdir -p $(GFEBUILDROOT)/include/BlueMatter", "/usr/bin/cp $(GFESOURCEROOT)/BlueMatter/*/include/*.h* $(GFEBUILDROOT)/include/BlueMatter";

file "xmlprobspec.cpp"
{
// executable name is xmlprobspec
exename "xmlprobspec";
// error output from automake goes to xmlprobspec.err
error "xmlprobspec.err";
// makefile name is xmlprobspec.mak
output "xmlprobspec.mak";
}


