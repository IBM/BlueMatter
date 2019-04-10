/*
 * NAME:        trace_file_class.C
 *
 * AUTHOR:      <<Author>>
 *
 * VERSION:     1.1
 *
 * DATE:        97/01/20
 *
 * COPYRIGHT:   %copyright%
 *
 * SYNOPSIS:
 *
 * <<Function Prototype 1>>
 *
 * <<Function Prototype 2>>
 *
 *
 * DESCRIPTION:
 *  words, eg ":hp2.", ":p.", "&colon." etc>>
 *
 * CAVEATS/WARNINGS:
 * <<None.>>
 *
 * RETURNED VALUES:
 * :sl.
 * :li.<<Constant - text>> e.g. BIM_SUCCESS - function executed ok
 * :esl.
 *
 * GENERATED MESSAGES:
 * :dl.
 * :dt.<<AISnnnns>>
 * :dd.<<"Message text">>
 * :edl.
 *
 * PRODUCT TRACE:
 * <<None.>>
 *
 * SIDE EFFECTS:
 * <<None.>>
 *
 * PERFORMANCE:
 * <<SCRIPT Text>>
 *
 * SPECIAL CONSIDERATIONS:
 * <<SCRIPT text>>.
 *
 * RELATED INFORMATION:
 * <<SCRIPT text>>.
 *
 * BUILD OPTIONS:
 * <<SCRIPT text>>.
 *
 * DESIGN:
 *
 * N/A
 */

#include "Tfile_class.hpp"
//
//
// Implementation of the TraceFile Class
//
//

TraceFile::TraceFile(__pktraceRead* MappedFile, int Position)
{

    // Initialize the data 
    pMappedFile = MappedFile;
    FilePosition = Position;

    // Load the trace data
    pMappedFile->LoadTraceData(FilePosition);
    NumTraceTimes = pMappedFile->LoadedTraces[FilePosition]->NumHits;
}

TraceFile::~TraceFile()
{
  pMappedFile   = NULL;
  FilePosition  = 0;
  NumTraceTimes = 0;
}

LTime TraceFile::GetLTimeAtPosition(int Position)
{
  return pMappedFile->LoadedTraces[FilePosition]->GetLTimeAtPosition(Position);
}


smalltime TraceFile::GetsmalltimeAtPosition(int Position)
{
  return pMappedFile->LoadedTraces[FilePosition]->GetsmalltimeAtPosition(Position);
}

LTime TraceFile::GetFirstHit()
{
  return 
        (pMappedFile->LoadedTraces[FilePosition]->GetStartTime());

}

LTime TraceFile::GetLastHit()
{
  return (pMappedFile->LoadedTraces[FilePosition]->GetEndTime()); 

}

int TraceFile::GetTimeBucketValue(int Position)
{
   return ( TimeBucket[Position].value);
}

void TraceFile::SetTimeBucketValue(int Position, int Value)
{
   TimeBucket[Position].value = Value;
}

LTime TraceFile::GetBucketThreshold(int Position)
{
   return ( TimeBucket[Position].Threshold);
}

int TraceFile::GetNumberOfHits()
{
   return NumTraceTimes;
}

LTime TraceFile::GetStartTime()
{
  return
        (pMappedFile->LoadedTraces[FilePosition]->Trace[0].Starttime);
}

char* TraceFile::GetName()
{
   return pMappedFile->GetTraceName(FilePosition);
}

void TraceFile::SetHitBeforeBucket(int Value)
{
   HitBeforeBucket = Value;
}

int TraceFile::GetHitBeforeBucket()
{
   return HitBeforeBucket;
}

int TraceFile::FindHitPos(LTime      SearchTime)
{
  return (pMappedFile->LoadedTraces[FilePosition]->FindHitPos(SearchTime));
}

int TraceFile::IsHitInRange(LTime      SearchTime)
{
  return (pMappedFile->LoadedTraces[FilePosition]->IsHitInRange(SearchTime));
}

////////////////////////////////////////////////////////////////////////////////
// FillBucket
// Probably the most performance critical function in the application.
// The latest approach is to calculate the number of hits in bucket
// by subtracting the hit number at the previous bucket threshold value from the
// hit number at the current bucket threshold. This means constant time 
// calculation regardless of the number of hits in the trace stream.
////////////////////////////////////////////////////////////////////////////////
void TraceFile::FillBucket(LTime      MidTime, int         Width,
                            long long Scale )
{
  int BucketNum;
  LTime WindowSpan;
  LTime LeftTime, RightTime;
  int FirstVisHit;
  register int nloop;
  TTimer LeftSearchTimer;
  int FirstHitNum, CurrentHitNum;
  int LeftTimeRangeResult;
  int RightTimeRangeResult;

  WindowSpan = Scale;
  WindowSpan *= Width;

  LeftTime  = (MidTime - 1) - (WindowSpan / 2 );
  RightTime = (MidTime - 1) + (WindowSpan / 2 );

  // Set the bounds of the search
  pMappedFile->LoadedTraces[FilePosition]->SetSearchBounds(LeftTime,RightTime);

  LeftTimeRangeResult = pMappedFile->LoadedTraces[FilePosition]->
                                                        IsHitInRange(LeftTime);
  RightTimeRangeResult = pMappedFile->LoadedTraces[FilePosition]->
                                                        IsHitInRange(RightTime);

  // Calculate where to run the bucket filling from
  if (LeftTimeRangeResult == TRUE && RightTimeRangeResult == TRUE )
  {
    CurrentHitNum = pMappedFile->LoadedTraces[FilePosition]->
                                                           FindHitPos(LeftTime);
  }
  else if (LeftTimeRangeResult == LOWER && RightTimeRangeResult == TRUE )
  {
    CurrentHitNum = -1;
  }
  else if (LeftTimeRangeResult == HIGHER && RightTimeRangeResult == HIGHER )
  {
    CurrentHitNum = pMappedFile->LoadedTraces[FilePosition]->NumHits - 1;
  }
  else if (LeftTimeRangeResult == LOWER && RightTimeRangeResult == HIGHER )
  {
    CurrentHitNum = -1;
  }
  else if (LeftTimeRangeResult == LOWER && RightTimeRangeResult == LOWER )
  {
    CurrentHitNum = - 1;
  }
  else if (LeftTimeRangeResult == TRUE && RightTimeRangeResult == HIGHER )
  {
    CurrentHitNum = pMappedFile->LoadedTraces[FilePosition]->
                                                           FindHitPos(LeftTime);
  }
  else
  {
    printf(
         "Unexpect values LeftTimeRangeResult:[%d] RightTimeRangeResult:[%d]\n",
                                      LeftTimeRangeResult,RightTimeRangeResult);
  }

  // Set the starting trace point reference
  FirstVisHit = CurrentHitNum;

  SetHitBeforeBucket(FirstVisHit);

  // First bucket is a special case
  SetTimeBucketValue(0,0);

  // Check for traces with single hits
  // These are special cases to prevent 
  if (pMappedFile->LoadedTraces[FilePosition]->NumHits == 1)
  {
    // Set up bucket threshold values
    for (nloop = 0; nloop < Width; nloop++)
    {
      TimeBucket[nloop].Threshold =(LTime)LeftTime +((LTime)(nloop)* Scale);
      SetTimeBucketValue(nloop,0);
    }

    // Check hit is in range of trace
    if ((pMappedFile->LoadedTraces[FilePosition]->GetLTimeAtPosition(0) >=
                                                              (LTime)LeftTime)
        &&
        (pMappedFile->LoadedTraces[FilePosition]->GetLTimeAtPosition(0) <=
                                                              (LTime)RightTime))
    {
      BucketNum = BinarySearch(pMappedFile->LoadedTraces[FilePosition]->
                                                         GetLTimeAtPosition(0));
      SetTimeBucketValue(BucketNum, 1);
    }
  } // End single hits check

  else
  {
    // Deal with traces with multiple hits
    // Loop through the bucket array
    for (nloop = 1; nloop < Width; nloop++)
    {
      // Set the current bucket threshold value
      TimeBucket[nloop].Threshold =(LTime)LeftTime + ((LTime)(nloop)* Scale);

      // Check hit is in range
      if (pMappedFile->LoadedTraces[FilePosition]->
                IsHitInRange(TimeBucket[nloop].Threshold ) == TRUE)
      {
        // Find the hit number at the current bucket threshold
        FirstHitNum = pMappedFile->LoadedTraces[FilePosition]->
                                FindHitPos(TimeBucket[nloop].Threshold);

        // Set the bucket value to the hit at the current threshold minus the
        // Current hit number
        SetTimeBucketValue(nloop,(FirstHitNum - CurrentHitNum));

        // Prepare for the next loop iteration of the loop
        CurrentHitNum = FirstHitNum;
      }

      // Search for exception to the rule
      // Prevents the trace runnig of the end and missing the last hit in the
      // stream. 
      else if ( pMappedFile->LoadedTraces[FilePosition]->
                IsHitInRange(TimeBucket[nloop].Threshold ) == HIGHER &&
           CurrentHitNum == pMappedFile->LoadedTraces[FilePosition]->NumHits- 2)
      {
        FirstHitNum =  pMappedFile->LoadedTraces[FilePosition]->NumHits - 1;

        SetTimeBucketValue(nloop,(FirstHitNum - CurrentHitNum));

        CurrentHitNum = FirstHitNum;
      }
      else
      {
        // No hits in the current bucket
        SetTimeBucketValue(nloop,0);
      }
    } // End loop
  } // End mulitple hits check
}

////////////////////////////////////////////////////////////////////////////////
// Finds the bucket number with the closet threshold to the TargetTime value
// The threshold values must be setup before attempting to run the BinarySearch
// on the TimeBucket array
////////////////////////////////////////////////////////////////////////////////
int TraceFile::BinarySearch(LTime TargetTime )
{

  int        Start, Middle, End;
  int        Found = 0;
  int        Position ;
  Start =    0;

  //
  //  Start Binary chop
  //
  End = daWidth;

  Middle = (int)((Start + End) / 2)  ;

  while ((Start < Middle) && (Found == 0) )
  {

    if (TargetTime == TimeBucket[Middle].Threshold)
    {
      Found = 1;
      Position = Middle;
    }
    else if (TargetTime > TimeBucket[Middle].Threshold)
    {
      Start = Middle ;
    }
    else
    {
      End = Middle ;
    }
    Middle = ((Start + End) / 2)  ;

  } /* End while loop */

  if (Found == 0)
  {
    Position = Middle;
  }

  return (Position);
}

LTime TraceFile::GetNextTime(int  Direction, LTime MidTime)
{
  int HitNumber1;
  int RangeResult;

  RangeResult =  pMappedFile->LoadedTraces[FilePosition]->IsHitInRange(MidTime);

  if (RangeResult == TRUE)
  {
    HitNumber1 = pMappedFile->LoadedTraces[FilePosition]->
                                                      FindHitPos(MidTime);
    
  }
  else if (RangeResult == LOWER)
  {
    return (GetFirstHit());
  }
  else
  {
    return (GetLastHit());
  }


  if (Direction == 1)
  {
    /* Upwards */
    if (GetLTimeAtPosition(HitNumber1) <= MidTime)
    {
      while (
             (GetLTimeAtPosition(HitNumber1) <= MidTime) &&
             (HitNumber1 < GetNumberOfHits())
            )
      {
        HitNumber1++;
      }
    }
 
  }
  else
  {
     /* Downwards */
     if (GetLTimeAtPosition(HitNumber1) >= MidTime)
     {
        while((GetLTimeAtPosition(HitNumber1) >= MidTime) && (HitNumber1 >= 0))
        {
           HitNumber1--;
        }
     }
  }

   return GetLTimeAtPosition(HitNumber1);
}

//
//
// Implementation of the Trace_Relative Class
//
//

Trace_Relative::Trace_Relative(__pktraceRead* MappedFile,
                               int pchPrevFileName, int pchPostFileName)
{
    PrevFile = new TraceFile(MappedFile,pchPrevFileName);
    PostFile = new TraceFile(MappedFile,pchPostFileName);
}

Trace_Relative::~Trace_Relative()
{
    delete(PrevFile);
    delete(PostFile);
}

//
//
// Implementation of the Trace_Plot Class
//
//

Trace_Plot::Trace_Plot(__pktraceRead* MappedFile,int pchFileName )
{
    PlotFile = new TraceFile(MappedFile,pchFileName);
}

Trace_Plot::~Trace_Plot()
{
    delete(PlotFile);
}
