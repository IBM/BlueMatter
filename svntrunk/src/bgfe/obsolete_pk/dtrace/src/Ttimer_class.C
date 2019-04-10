#include "Ttimer_class.hpp"

TTimer::TTimer()
{
   StartTime = (LTime)0;
   StopTime  = (LTime)0;
   Duration = (LTime)0;
}

TTimer::~TTimer()
{

}

void TTimer::Start()
{
  timebasestruct_t Now;
  read_real_time( &Now, TIMEBASE_SZ );
  time_base_to_time( &Now, TIMEBASE_SZ );

  StartTime = TbToLTime(Now);
}

void TTimer::Stop()
{

  timebasestruct_t Now;
  read_real_time( &Now, TIMEBASE_SZ );
  time_base_to_time( &Now, TIMEBASE_SZ );

  StopTime = TbToLTime(Now);

  Duration += (StopTime - StartTime);
}

void TTimer::DisplayDuration(char* Text)
{
   printf("%s took %s\n",Text,CreateSecondsFromLTime(Duration));
}
