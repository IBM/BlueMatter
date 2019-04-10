//#include <ilanglvl.hpp>

// template <class Element>
// class IACollection;
// #include <iseq.h>
// #include <ieqseq.h>
// #include <iptr.h>

#include <new>
using namespace std;
#include <Tapplication_class.hpp>
#include <sched.h>

#include <pk/fxlogger.hpp>

const char * pkTraceFileName ;

#ifndef PKFXLOG_TRACER_MAIN
#define PKFXLOG_TRACER_MAIN (0)
#endif
int main(int argc, char *argv[])
{

  BegLogLine(PKFXLOG_TRACER_MAIN)
    << "main argc=" << argc
    << EndLogLine ;
   TracerApplication *Application = TracerApplication::GetApp(argc,argv);

//   TracerApplication *Application = new TracerApplication(argc,argv);

   set_new_handler(&no_storage_handler);
   Application->run();
   BegLogLine(PKFXLOG_TRACER_MAIN)
     << "leaving main" << argc
     << EndLogLine ;
   return 0 ;
}
