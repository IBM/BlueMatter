
#include <pktracefile_api.hpp>

/*
 * TraceLineObject Class
 */
TraceLineObject::TraceLineObject(__pktraceRead* ServerRef,int Position)
{
  Server        = ServerRef;
  TracePosition = Position;
  Server->LoadTraceData(Position);
}

TraceLineObject::~TraceLineObject()
{
  Server = NULL;
  TracePosition = -1;
}

LTime TraceLineObject::GetLTimeAtPosition(int Position)
{
  return Server->LoadedTraces[TracePosition]->GetLTimeAtPosition(Position);
}

char* TraceLineObject::GetTraceName()
{
  return Server->GetTraceName(TracePosition);
}

int TraceLineObject::GetNumPoints()
{
  return Server->LoadedTraces[TracePosition]->NumHits;
}

/*
 * TraceFileAccess Class
 */

TraceFileAccess::TraceFileAccess()
{
  Server = NULL;
  CurrentTracePosition = -1;
}

TraceFileAccess::TraceFileAccess(char* Filename)
{
  Server = new __pktraceRead(Filename);
  CurrentTracePosition = 0;
}

TraceFileAccess::~TraceFileAccess()
{
//  if( Server != NULL )
//    {
//      delete Server;
//    }
  CurrentTracePosition = -1;
}

int TraceFileAccess::OpenFile(char* Filename)
{
  Server = new __pktraceRead(Filename);
  CurrentTracePosition = 0;

  if (Server != NULL)
    return (0);
  else
    return (-1);
}

int TraceFileAccess::CloseFile()
{
  return -1;
}

int TraceFileAccess::GetNumTraces()
{
  return Server->GetNumTraces();
}

TraceLineObject* TraceFileAccess::GetPrevTrace()
{

  TraceLineObject* ReturnObject;

  if (CurrentTracePosition <= 0)
    return NULL;

  CurrentTracePosition--;

  ReturnObject =  new TraceLineObject(Server,CurrentTracePosition);

  return ReturnObject;
}

TraceLineObject* TraceFileAccess::GetNextTrace()
{

  TraceLineObject* ReturnObject;

  if (CurrentTracePosition >= (Server->GetNumTraces() - 2))
    return NULL;

  CurrentTracePosition++;

  ReturnObject =  new TraceLineObject(Server,CurrentTracePosition);

  return ReturnObject;

}

TraceLineObject* TraceFileAccess::GetTraceNameAtPos(int   Position)
{
  TraceLineObject* ReturnObject;

  if (Position < 0 || Position > (Server->GetNumTraces() - 1))
    return NULL;

  ReturnObject =  new TraceLineObject(Server,Position);

  return ReturnObject;

}
