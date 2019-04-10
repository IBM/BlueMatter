//#include <ilanglvl.hpp>

// template <class Element>
// class IACollection;
// #include <iseq.h>
// #include <ieqseq.h>
// #include <iptr.h>

#include <new.h>
#include "Tapplication_class.hpp"
#include <sched.h>

int main(int argc, char *argv[])
{

   TracerApplication *Application = TracerApplication::GetApp(argc,argv);

//   TracerApplication *Application = new TracerApplication(argc,argv);

   set_new_handler(&no_storage_handler);
   Application->run();
}
