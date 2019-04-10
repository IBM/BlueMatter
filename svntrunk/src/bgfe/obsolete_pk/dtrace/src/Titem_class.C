/*
 * NAME:        trace_item_class.C
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
// template <class Element>
// class IACollection;
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_VERTGRAPHICS
#define PKFXLOG_VERTGRAPHICS (0) 
#endif
#include <Titem_class.hpp>

//
//
// Implementation of the TraceItem Class
//
//

/*******************************************************************************
*
* Function: GetApplication
*
*
* Purpose:
*
*******************************************************************************/
TracerApplication* TraceItem::GetApplication()
{
   return Parent->GetApplication();
}

/*******************************************************************************
*
* Function: GetApplicationContext
*
*
* Purpose:
*
*******************************************************************************/
XtAppContext* TraceItem::GetApplicationContext()
{
   return Parent->GetApplicationContext();
}

/*******************************************************************************
*
* Function: GetApplicationShell
*
*
* Purpose:
*
*******************************************************************************/
Widget TraceItem::GetApplicationShell()
{
   return Parent->GetApplicationShell();
}

/*******************************************************************************
*
* Function: GetApplicationTopWindow
*
*
* Purpose:
*
*******************************************************************************/
Widget TraceItem::GetApplicationTopWindow()
{
   return Parent->GetApplicationTopWindow();
}

/*******************************************************************************
*
* Function: GetGroup
*
*
* Purpose:
*
*******************************************************************************/
TraceGroup* TraceItem::GetGroup()
{
   return Parent;
}

/*******************************************************************************
*
* Function: GetWindow
*
*
* Purpose:
*
*******************************************************************************/
TracerWindow* TraceItem::GetWindow()
{
   return Parent->GetWindow();
}

/*******************************************************************************
*
* Function: TraceItem
*
*
* Purpose:
*
*******************************************************************************/
TraceItem::TraceItem(TraceGroup*    ptgParent, 
                     int            YOffset,
                     int            Show,
                     char*          Colour,
                     TRACE_TYPE     Type,
                     __pktraceRead* MappedFile,
                     int            pchFile1,
                     int            pchFile2,
                     entGraphStyle  NewGraphStyle,
                     Boolean        Inverted )
{
   XGCValues      values;
   XColor         newcolour, exact;

   // Set name
   (void)sprintf(Label,"Test_%d",YOffset);

   Parent = ptgParent;

   DataUpToDate = FALSE;

   Y_AxisOffset = YOffset;

   wButton = CreateButton();

   show = Show;

   InvertedTrace = Inverted;

   (void)strcpy(color,Colour);

   type = Type;
   GraphStyle = NewGraphStyle;

   if (type == RELATIVE)
   {
      relative = new Trace_Relative(MappedFile,pchFile1,pchFile2);
      plot     = NULL;
      //Parent->ParentWindow->MidTime = relative->PrevFile->StartTime;
   }
   else
   {

      relative = NULL;
      plot = new Trace_Plot(MappedFile,pchFile1);
      //Parent->ParentWindow->MidTime = plot->PlotFile->StartTime;
   }

   values.line_width = 1;


   /* Set the foreground color */
   if (XAllocNamedColor(XtDisplay(wButton),
                         DefaultColormapOfScreen(XtScreen(wButton)),
                         color, &newcolour, &exact) == False)
   {
      fprintf( stderr, "Cannot allocate foreground colour\n" );
      values.foreground = WhitePixelOfScreen( XtScreen(wButton));
   }
   else
   {
      values.foreground = newcolour.pixel;
   }

   /* Set the background color */
   if (XAllocNamedColor(XtDisplay(wButton),
                         DefaultColormapOfScreen(XtScreen(wButton)),
                         "black", &newcolour, &exact) == False)
   {
      fprintf( stderr, "Cannot allocate background colour\n" );
      values.background = BlackPixelOfScreen(XtScreen(wButton));
   }
   else
   {
      values.background = newcolour.pixel;
   }

   values.function = GXcopy;

   traceGC  = XCreateGC(XtDisplay(wButton),XtWindow(wButton),
                   GCBackground | GCForeground |  GCLineWidth | GCFunction,
                        (XGCValues*)&values);
}

void TraceItem::ChangeColor(char *NewColor)
{
   ResetColourMemebers(NewColor);
}

void TraceItem::ChangeSelectedStyle(char *NewStyle)
{
   int loop;
   Boolean StyleFound = FALSE;

   for (loop = 0; loop < NUM_GRAPH_STYLES; loop++)
   {
     if(strcmp(NewStyle,GraphStyleStrings[loop]) == 0)
     {
       StyleFound = TRUE;
       GraphStyle = (entGraphStyle)loop;
       break;
     } 
   }
   assert (StyleFound == TRUE);

}

void TraceItem::ChangeGCFunction(char *NewGC)
{
   XGCValues      values;
   printf("NewGC = %s\n",NewGC);

   if (strcmp(NewGC,"GXand") == 0)
      values.function = GXand;

   else if (strcmp(NewGC,"GXandInverted") == 0)
      values.function = GXandInverted;

   else if (strcmp(NewGC,"GXandReverse") == 0)
      values.function = GXandReverse;

   else if (strcmp(NewGC,"GXclear") == 0)
      values.function = GXclear;

   else if (strcmp(NewGC,"GXcopy") == 0)
      values.function = GXcopy;

   else if (strcmp(NewGC,"GXcopyInverted") == 0)
      values.function = GXcopyInverted;

   else if (strcmp(NewGC,"GXequiv") == 0)
      values.function = GXequiv;

   else if (strcmp(NewGC,"GXinvert") == 0)
      values.function = GXinvert;

   else if (strcmp(NewGC,"GXnand") == 0)
      values.function = GXnand;

   else if (strcmp(NewGC,"GXnoop") == 0)
      values.function = GXnoop;

   else if (strcmp(NewGC,"GXnor") == 0)
      values.function = GXnor;

   else if (strcmp(NewGC,"GXor") == 0)
      values.function = GXor;

   else if (strcmp(NewGC,"GXorInverted") == 0)
      values.function = GXorInverted;

   else if (strcmp(NewGC,"GXorReverse") == 0)
      values.function = GXorReverse;

   else if (strcmp(NewGC,"GXset") == 0)
      values.function = GXset;

   else if (strcmp(NewGC,"GXxor") == 0)
      values.function = GXxor;

   else
   {
      printf("Error NO LEgal GC value found\n");
      values.function = GXcopy;
   }
   XChangeGC(XtDisplay(wButton),
             traceGC,
             GCFunction,
             &values);


}

void TraceItem::ResetColourMemebers(char *NewColor)
{
   XGCValues      values;
   XColor         newcolour, exact;


   strcpy(color,NewColor);

   /* Set the foreground color */
   if (XAllocNamedColor(XtDisplay(wButton),
                         DefaultColormapOfScreen(XtScreen(wButton)),
                         color, &newcolour, &exact) == False)
   {
      fprintf( stderr, "Cannot allocate foreground colour\n" );
      values.foreground = WhitePixelOfScreen( XtScreen(wButton));
   }

   else
   {
      values.foreground = newcolour.pixel;
   }

   XChangeGC(XtDisplay(wButton), traceGC,GCForeground,(XGCValues*)&values);


}

void TraceItem::ResetInvertedMemebers(Boolean Value)
{
   DataUpToDate = FALSE;

   InvertedTrace = Value;
}

TraceItem::~TraceItem()
{
   if (type == RELATIVE)
   {
      delete(relative);
   }
   else
   {
      delete(plot);
   }

   XtDestroyWidget(wButton);

   if (GetWindow()->CurrentFocusElement == this)
   {
      GetWindow()->CurrentFocusElement = NULL;
   }

   XFreeGC(XtDisplay(GetWindow()->da),traceGC);

}

void TraceItem::RedrawTrace(int flag)
{
   int CurrentStepHeight = GetGroup()->StepHeight;
   register int i;
   int xpos, ypos, plotValue = 0;
   int     slots;
   double d, start;
   long long halfScreenTime;
   TRACE_VIEWER   TraceView;
   LTime LeftTime;

   halfScreenTime =  (daWidth / 2) * GetWindow()->Scale;


   XtVaSetValues(wButton,
                 XmNtopOffset,(Parent->GroupYOffset + Y_AxisOffset) -
                                    Parent->ParentWindow->VirtualHeight ,
                 NULL);


   LeftTime = GetWindow()->MidTime - halfScreenTime ;

   if ( LeftTime < 0 )
      LeftTime = 0;

   /* Check if show is ON */
   if (show == 0)
      return;
 
   if ( type == PLOT )
   {
      TraceView.StartTime    = LeftTime;
      TraceView.TimeInterval = GetWindow()->Scale;
      TraceView.duration     = daWidth;

      /* Plot Graph */

      slots = daWidth ;
      d = GetWindow()->MidTime - GetWindow()->MinTime;
      d = d / GetWindow()->Scale;

      start = (( slots /2 ) - d );

      if ( start < 0.0 )
         start = 0.0;

      for( i= (int)start ;
           i < daWidth ;
           i ++)
      {
         xpos  =  i;

         ypos = daHeight - Y_AxisOffset -(plotValue * CurrentStepHeight);

         xpos = i + (1/2);
         slots = daWidth ;

         XFillArc(XtDisplay(GetWindow()->da),
                  XtWindow(GetWindow()->da),
                  traceGC, xpos, ypos,
                  (int)1/5,
                  (int)1/5,  0, 360 * 64);
      }
   }
   else
   {

      /* Relative Graph */

     if (GraphStyle == VERT_LINE_STYLE)
     {
         RelativePlot(flag); /* new one */
     }
     else if (GraphStyle == FLAT_LINE_STYLE)
     {
        RelativePlotNoVert(flag);
     }
     else if (GraphStyle == POINTS_ONLY_STYLE)
     {
        RelativePlotPointsOnly(flag);
     }

     else
     {
        printf("Serious error\n");
        exit(0);
     }
   }
}

int TraceItem::GetTraceStartingState()
{
   int NewCurrentState ;
   int State = (relative->PrevFile->GetHitBeforeBucket() - 
                 relative->PostFile->GetHitBeforeBucket());

   if (State == 0)
   {
      NewCurrentState = State;
   }
   else if (State > 0)
   {
      NewCurrentState = State;
   }
   else 
   {
      NewCurrentState = State;
   }
   return NewCurrentState;
}

void TraceItem::DrawVertGraphics(int StartCurrentState, int flag)
{
   int CurrentStepHeight = GetGroup()->StepHeight;
   int      CurrentY_AxisOffset;

   register int BaseY ;
   register int CurrentState = StartCurrentState;
   register int nloop;

  printf("got here\n") ;
  
  BegLogLine(PKFXLOG_VERTGRAPHICS)
    << "DrawVertGraphics StartCurrentState=" << StartCurrentState
    << " flag=" << flag 
    << EndLogLine ; 
  if (DataUpToDate == FALSE || flag == FULL_REDRAW)
  {
    CurrentY_AxisOffset = (daHeight - (Parent->GroupYOffset
                     - Parent->ParentWindow->VirtualHeight)) - Y_AxisOffset - 2
                      - (CurrentState * CurrentStepHeight);

    BaseY = daHeight - (CurrentY_AxisOffset );

    if (CurrentState == -1)
    {
      return;
    }

    saPos = 0;
    paPos = 0;
    assert (SegmentArray != NULL);
    assert (PointArray   != NULL);

    // Loop through the bucket array
    for (nloop= FIRST_BUCKET; nloop < daWidth; nloop++)
    {
    	BegLogLine(PKFXLOG_VERTGRAPHICS)
    	  << "nloop=" << nloop 
    	  << "PrevFile->GetTimeBucketValue=" << relative->PrevFile->GetTimeBucketValue(nloop)
    	  << "PostFile->GetTimeBucketValue(nloop)=" << relative->PostFile->GetTimeBucketValue(nloop)
    	  << EndLogLine ;
      if ((relative->PrevFile->GetTimeBucketValue(nloop) != 0) &&
                           (relative->PostFile->GetTimeBucketValue(nloop) == 0))
      {
        SegmentArray[saPos].x1 = (short)nloop;
        SegmentArray[saPos].y1 = (short)BaseY;

        SegmentArray[saPos].x2 = (short)nloop;
        SegmentArray[saPos].y2 = (short)(BaseY - (CurrentStepHeight)) ;
        saPos++;

        BaseY -= (CurrentStepHeight);

        CurrentState++;
      }

      else if ((relative->PostFile->GetTimeBucketValue(nloop) != 0) &&
                  (relative->PrevFile->GetTimeBucketValue(nloop) == 0))

      {
        SegmentArray[saPos].x1 = (short)nloop;
        SegmentArray[saPos].y1 = (short)BaseY;

        SegmentArray[saPos].x2 = (short)nloop;
        SegmentArray[saPos].y2 = (short)(BaseY + (CurrentStepHeight)) ;
        saPos++;

        BaseY += (CurrentStepHeight);

        CurrentState--;
      }

      // This is the bit that deals with hits int he smae same bucket
      else if ((relative->PostFile->GetTimeBucketValue(nloop) != 0) &&
                          (relative->PrevFile->GetTimeBucketValue(nloop) != 0) )

      { 
        // Check if the hit numbers are equal
        if (relative->PrevFile->GetTimeBucketValue(nloop) ==
                                  relative->PostFile->GetTimeBucketValue(nloop))
        {
          // Hits are equal, exit state will be same as entry state
          // show the hit
          if (InvertedTrace == FALSE)
          {
            PointArray[paPos].x = (short)nloop;
            PointArray[paPos].y = (short)BaseY;
            paPos++;
 
            // Draw a vertical line, choose the direction
            if (CurrentState == 0)
            {
              //Draw upwards
              SegmentArray[saPos].x1 = (short)nloop;
              SegmentArray[saPos].y1 = (short)BaseY;

              SegmentArray[saPos].x2 = (short)nloop;
              SegmentArray[saPos].y2 = (short)(BaseY - (CurrentStepHeight)) ;
              saPos++;
            }
            else
            {
              //Draw downwards
              SegmentArray[saPos].x1 = (short)nloop;
              SegmentArray[saPos].y1 = (short)BaseY;

              SegmentArray[saPos].x2 = (short)nloop;
              SegmentArray[saPos].y2 = (short)(BaseY + (CurrentStepHeight)) ;
              saPos++;
            }

          }
        }
        // PostFile->GetTimeBucketValue < PrevFile->GetTimeBucketValue
        // More hits in PrevFile bucket
        //
        else if (relative->PrevFile->GetTimeBucketValue(nloop) >
                              relative->PostFile->GetTimeBucketValue(nloop))
        {
          // More Prev than Post
          // Hit must raise the level
          SegmentArray[saPos].x1 = (short)nloop;
          SegmentArray[saPos].y1 = (short)BaseY;

          SegmentArray[saPos].x2 = (short)nloop;
          SegmentArray[saPos].y2 = (short)(BaseY - (CurrentStepHeight)) ;
          saPos++;

          BaseY -= (CurrentStepHeight);

          CurrentState++;
        }
        // PrevFile->GetTimeBucketValue < PostFile->GetTimeBucketValue
        // More hits in PostFile bucket
        else
        {
          // show the hit
          if (InvertedTrace == FALSE)
          {
            PointArray[paPos].x = (short)nloop;
            PointArray[paPos].y = (short)BaseY;
            paPos++;
          }

          SegmentArray[saPos].x1 = (short)nloop;
          SegmentArray[saPos].y1 = (short)BaseY;

          SegmentArray[saPos].x2 = (short)nloop;
          SegmentArray[saPos].y2 = (short)(BaseY + (CurrentStepHeight)) ;
          saPos++;

          BaseY += (CurrentStepHeight);

          CurrentState--;
        }
      }

      // Check for inverted trace
      if (InvertedTrace == FALSE)
      {
        if (IsPointValid(nloop))
        {
          PointArray[paPos].x = (short)nloop;
          PointArray[paPos].y = (short)BaseY ;
          paPos++;
        }
      }
      else
      {
        if (IsPointValid(nloop))
        {
          PointArray[paPos].x = (short)nloop;
          PointArray[paPos].y = (short)BaseY ;
          paPos++;
        }
      }

    }
    DataUpToDate = TRUE;
  }

 // printf("Number of points [%d] Number of segments [%d]\n",paPos,saPos);

  // Send draw commands to the XServer
  XDrawPoints(XtDisplay(GetWindow()->da),
               XtWindow(GetWindow()->da),
               traceGC,PointArray,paPos,CoordModeOrigin);

  XDrawSegments(XtDisplay(GetWindow()->da),
                 XtWindow(GetWindow()->da),traceGC,SegmentArray,saPos);

}

void TraceItem::RelativePlot(int flag)
{

   int CurrentState;

   // Check the type of draw
   if (flag == FULL_REDRAW)
   {
      // Fill the time buckets
      relative->PrevFile->FillBucket(GetWindow()->MidTime,
                                     daWidth,
                                     GetWindow()->Scale);

      relative->PostFile->FillBucket(GetWindow()->MidTime,
                                     daWidth,
                                     GetWindow()->Scale);
   }

   // Assign values
   CurrentState = GetTraceStartingState();

   // Do the draw
   DrawVertGraphics(CurrentState,flag);

}

void TraceItem::RelativePlotNoVert(int flag)
{
  int BaseY;
  int      CurrentY_AxisOffset;
  register int nloop;
  int CurrentState;

  assert (Parent != NULL);
  BegLogLine(PKFXLOG_VERTGRAPHICS)
    << "RelativePlotNoVert"
    << " flag=" << flag 
    << EndLogLine ; 

  // Check whether the trace to draw is in the vertical area
  if (((Parent->GroupYOffset + Y_AxisOffset )
                                       > Parent->ParentWindow->VirtualHeight) &&
     ((Parent->GroupYOffset + Y_AxisOffset )
                              < Parent->ParentWindow->VirtualHeight + daHeight))
  {

  }
  else
  {
    return;
  }

  // Check the type of draw
  if (flag == FULL_REDRAW)
  {
    // Fill the time buckets
    relative->PrevFile->FillBucket(GetWindow()->MidTime,
                                     daWidth,
                                     GetWindow()->Scale);

    relative->PostFile->FillBucket(GetWindow()->MidTime,
                                     daWidth,
                                     GetWindow()->Scale);
  }

  if (DataUpToDate == FALSE || flag == FULL_REDRAW)
  {
    CurrentY_AxisOffset = (daHeight - (Parent->GroupYOffset
                     - Parent->ParentWindow->VirtualHeight)) - Y_AxisOffset - 2;

    BaseY = daHeight - (CurrentY_AxisOffset );

    // Assign values
    CurrentState = GetTraceStartingState();

    if (CurrentState == -1)
    {
      // No hits to draw
      return ;
    }

    // Reset the array marker
    n = 0;
    assert(PointArray != NULL);

    // Loop through the bucket array
    for (nloop= FIRST_BUCKET; nloop < daWidth; nloop++)
    {
    	
      BegLogLine(PKFXLOG_VERTGRAPHICS &&
       (relative->PrevFile->GetTimeBucketValue(nloop) != 0 
        || relative->PostFile->GetTimeBucketValue(nloop) != 0 ) )
    	<< "nloop=" << nloop 
    	<< "PrevFile->GetTimeBucketValue=" << relative->PrevFile->GetTimeBucketValue(nloop)
    	<< "PostFile->GetTimeBucketValue(nloop)=" << relative->PostFile->GetTimeBucketValue(nloop)
    	<< EndLogLine ;
      if ((relative->PrevFile->GetTimeBucketValue(nloop) != 0) &&
                           (relative->PostFile->GetTimeBucketValue(nloop) == 0))
      {
        CurrentState++;
      }

      else if ((relative->PostFile->GetTimeBucketValue(nloop) != 0) &&
                  (relative->PrevFile->GetTimeBucketValue(nloop) == 0))

      {
        CurrentState--;
      }

      // This is the bit that deals with hits in the smae same bucket
      else if ((relative->PostFile->GetTimeBucketValue(nloop) != 0) &&
                          (relative->PrevFile->GetTimeBucketValue(nloop) != 0) )

      { 
        // Check if the hit numbers are equal
        if (relative->PrevFile->GetTimeBucketValue(nloop) ==
                                  relative->PostFile->GetTimeBucketValue(nloop))
        {
          // Hits are equal, exit state will be same as entry state
          // show the hit
          if (InvertedTrace == FALSE)
          {
            PointArray[n].x = (short)nloop;
            PointArray[n].y = (short)BaseY;
            n++;
          }
        }
        // PostFile->GetTimeBucketValue < PrevFile->GetTimeBucketValue
        // More hits in PrevFile bucket
        //
        else if (relative->PrevFile->GetTimeBucketValue(nloop) >
                              relative->PostFile->GetTimeBucketValue(nloop))
        {
          // More Prev than Post
          // Hit must raise the level
          CurrentState++;
        }
        // PrevFile->GetTimeBucketValue < PostFile->GetTimeBucketValue
        // More hits in PostFile bucket
        else
        {
          // show the hit
          if (InvertedTrace == FALSE)
          {
            PointArray[n].x = (short)nloop;
            PointArray[n].y = (short)BaseY;
            n++;
          }

          CurrentState--;
        }
      }

      // Check for inverted trace
      if (InvertedTrace == FALSE)
      {
        if ( (IsPointValid(nloop)) && (CurrentState > 0 ))
        {
          PointArray[n].x = (short)nloop;
          PointArray[n].y = (short)BaseY ;
          n++;
        }
      }
      else
      {
        if ( (IsPointValid(nloop)) && (CurrentState <= 0 ))
        {
          PointArray[n].x = (short)nloop;
          PointArray[n].y = (short)BaseY ;
          n++;
        }
      }

    }
    DataUpToDate = TRUE;
  }
 
 // printf("Number of points [%d]\n",n);
  // Send the structure to the X server
  XDrawPoints(XtDisplay(GetWindow()->da),
               XtWindow(GetWindow()->da),
               traceGC,PointArray,n,CoordModeOrigin);

}

void TraceItem::RelativePlotPointsOnly(int flag)
{

  int BaseY;
  int      CurrentY_AxisOffset;
  register int nloop;

  // Check the type of draw
  if (flag == FULL_REDRAW)
  {
    // Fill the time buckets
    relative->PrevFile->FillBucket(GetWindow()->MidTime,
                                     daWidth,
                                     GetWindow()->Scale);
  }

  assert (Parent != NULL);
  if (DataUpToDate == FALSE || flag == FULL_REDRAW)
  {
    CurrentY_AxisOffset = (daHeight - (Parent->GroupYOffset
                     - Parent->ParentWindow->VirtualHeight)) - Y_AxisOffset - 2;

    BaseY = daHeight - (CurrentY_AxisOffset );

    // Reset the array marker
    saPos = 0;
    assert(SegmentArray != NULL);
    // Loop through the bucket array
    for (nloop= FIRST_BUCKET; nloop < daWidth; nloop++)
    {
      if (relative->PrevFile->GetTimeBucketValue(nloop) != 0)
      {
        if (IsPointValid(nloop))
        {
          // Draw Vertical cross
          SegmentArray[saPos].x1 = (short)nloop;
          SegmentArray[saPos].y1 = (short)BaseY - 2 ;
          SegmentArray[saPos].x2 = (short)nloop;
          SegmentArray[saPos].y2 = (short)BaseY + 2 ;
          saPos++;

          // Draw Horizontal cross
          SegmentArray[saPos].x1 = (short)nloop - 2;
          SegmentArray[saPos].y1 = (short)BaseY ;
          SegmentArray[saPos].x2 = (short)nloop + 2;
          SegmentArray[saPos].y2 = (short)BaseY ;
          saPos++;

        }
      }
    }
    DataUpToDate = TRUE;
  }

 // printf("Number of points [%d]\n",saPos);

  XDrawSegments(XtDisplay(GetWindow()->da),
                 XtWindow(GetWindow()->da),
                 traceGC,SegmentArray,saPos);
}

int TraceItem::IsPointValid(int Position)
{

   if ((relative->PrevFile->GetBucketThreshold(Position) > GetStartTime())
       &&
       ( relative->PrevFile->GetBucketThreshold(Position) < GetEndTime()))
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

int TraceItem::operator == (TraceItem const& t) const
{
   if (Y_AxisOffset != t.Y_AxisOffset)
      return 0;
   else
      return 1;
}

int TraceItem::operator  < (TraceItem const& t) const
{
   if (strcmp(Label,t.Label) < 0)
   {
      return 1;
   }
   return 0;
}

TraceItem& TraceItem::operator = (TraceItem const& from)
{
   Trace_Relative *tmpTrace_Relative = from.relative;
   Trace_Plot     *tmpTrace_Plot     = from.plot;
   if (relative != NULL)
   {
       delete relative;
       relative = NULL;
   }

   if (plot != NULL)
   {
       delete plot;
       plot = NULL;
   }

   wButton = from.wButton;
   strcpy(Label,from.Label);
   Parent = from.Parent;

   Y_AxisOffset = from.Y_AxisOffset;
   show = from.show;
   strcpy(color,from.color);

   type = from.type;

   if (type == RELATIVE)
   {
      relative = new Trace_Relative(*tmpTrace_Relative);   
   }
   else
   {
      plot = new Trace_Plot(*tmpTrace_Plot);
   }

   return *this;
}

Widget TraceItem::CreateButton( )
{
   Widget w;
   Arg  args[16];

   int n =0;

   XmFontList       fontlist;
   XmFontListEntry  entry;
   XFontStruct      *font;

   font = XLoadQueryFont(
                     XtDisplay(GetApplicationShell()),
                     "6x10" );
   entry = XmFontListEntryCreate( "tag1", XmFONT_IS_FONT , font);
   fontlist = XmFontListAppendEntry( NULL, entry );

   assert (Parent != NULL);

   // Label for trace
   XtSetArg( args[n],XmNtopAttachment,      XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNtopOffset,((Parent->GroupYOffset + Y_AxisOffset) - 
                                    Parent->ParentWindow->VirtualHeight) ); n++;

   XtSetArg( args[n],XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNleftOffset,       13 ); n++;
   XtSetArg( args[n],XmNfontList,         fontlist ); n++;
   XtSetArg( args[n],XmNheight,         6 ); n++;
   XtSetArg( args[n],XmNwidth,         15 ); n++;
   XtSetArg( args[n],XmNhighlightOnEnter, TRUE  ); n++;

   // Create the Widget
   w = XmCreatePushButton( GetWindow()->formLabel, ".", args, n );


   // Label for trace
   XtAddEventHandler(w, ButtonPressMask, False,
                     (XtEventHandler)&(TraceItem::label_press_handler),
                     (XtPointer)this );


   XtAddEventHandler(w, ButtonReleaseMask, False,
                     (XtEventHandler)&(TraceItem::label_release_handler),
                     (XtPointer)this );

   XtAddEventHandler(w, Button1MotionMask, False,
                     (XtEventHandler)&(TraceItem::label_motion_handler),
                     (XtPointer)this );

   XtAddEventHandler(w, EnterWindowMask, False,
                     (XtEventHandler)&(TraceItem::label_entered_handler),
                     (XtPointer)this );

   XtVaSetValues( w,
                  XtVaTypedArg, XmNbackground,  XmRString, "DimGrey", 8,
                  NULL);

   XtManageChild( w );

   XmFontListFree( fontlist );


   return w;
}

void TraceItem::label_press_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event)
{
   Position rootx, rooty;

  TraceItem*   Item ;
  Arg args[20];
  Cardinal n;
  XmString xmString;
  char FileName[1024];
  Widget wButton;

  Item = ((TraceItem*)p);
  XtRemoveEventHandler(w, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)Item );

  XtRemoveEventHandler(w,LeaveWindowMask,False,
                       (XtEventHandler)&(TraceItem::label_exited_bd_handler),
                       (XtPointer)Item );

  XtTranslateCoords( w,event->x,event->y, &rootx, &rooty );

  switch (event->button)
  {

    case Button1:
          Item->Movey = rooty ;
         break;

     case Button2:
     case Button3:
          n = 0;
          sprintf(FileName,"Prev File: %s",
                             Item->relative->PrevFile->GetName());
          xmString = XmStringCreateSimple(FileName);
          Item->wPopUp = XmCreatePopupMenu(w,"wPopUp",args,n);
          (void) XtVaCreateManagedWidget("ItemPrevTitle",xmLabelWidgetClass,
                                         Item->wPopUp,
                                         XmNlabelString, xmString,
                                         XmNalignment, XmALIGNMENT_BEGINNING,
                                         NULL);

          XmStringFree(xmString);

          sprintf(FileName,"Post File: %s",
                             Item->relative->PostFile->GetName());
          xmString = XmStringCreateSimple(FileName);

          (void) XtVaCreateManagedWidget("ItemPostTitle",xmLabelWidgetClass,
                                         Item->wPopUp,
                                         XmNlabelString, xmString,
                                         XmNalignment, XmALIGNMENT_BEGINNING,
                                         NULL);

          XmStringFree(xmString);

          (void) XtVaCreateManagedWidget("Sep1",xmSeparatorWidgetClass,
                                         Item->wPopUp,NULL);

          (void) CreatePushButton(Item->wPopUp,"Delete",
                 (XtCallbackProc) Item->cbDelete,
                 (XtPointer) Item);

          (void) XtVaCreateManagedWidget("Sep2",xmSeparatorWidgetClass,
                                         Item->wPopUp,NULL);

          wButton= CreatePushButton(Item->wPopUp,"MakeFocus",
                 (XtCallbackProc) Item->cbMakeFocus,
                 (XtPointer) Item);

          xmString = XmStringCreateSimple("Make Focus");

          XtVaSetValues(wButton,
                      XmNlabelString,xmString,
                      NULL);

          XmStringFree(xmString);

          (void) XtVaCreateManagedWidget("Sep4",xmSeparatorWidgetClass,
                                         Item->wPopUp,NULL);

          wButton= CreatePushButton(Item->wPopUp,"GotoStart",
                 (XtCallbackProc) Item->cbGotoStart,
                 (XtPointer) Item);

          xmString = XmStringCreateSimple("Goto Start");

          XtVaSetValues(wButton,
                      XmNlabelString,xmString,
                      NULL);

          XmStringFree(xmString);

          wButton= CreatePushButton(Item->wPopUp,"GotoEnd",
                 (XtCallbackProc) Item->cbGotoEnd,
                 (XtPointer) Item);

          xmString = XmStringCreateSimple("Goto End");

          XtVaSetValues(wButton,
                      XmNlabelString,xmString,
                      NULL);

          XmStringFree(xmString);


          (void) XtVaCreateManagedWidget("Sep3",xmSeparatorWidgetClass,
                                         Item->wPopUp,NULL);

          (void) CreatePushButton(Item->wPopUp,"Properties...",
                 (XtCallbackProc) Item->cbProperties,
                 (XtPointer) Item);


          XmMenuPosition(Item->wPopUp,event);
          Item->Movey = event->y ;
          XtManageChild(Item->wPopUp);
          break;

   }

}

void TraceItem::label_release_handler(Widget w, XtPointer p, XButtonPressedEvent
                                             *event)
{
  TraceItem*   Item = (TraceItem*)p;

  switch (event->button)
  {
      case Button1:

         if (Item->Y_AxisOffset < Item->Parent->CurrentHighest->Y_AxisOffset)
         {
            Item->Parent->CurrentHighest = Item;
         }

         if (Item->Y_AxisOffset > Item->Parent->CurrentLowest->Y_AxisOffset)
         {
            Item->Parent->CurrentLowest = Item;
         }

         Item->Parent->ResizeGroupBar();
         Item->DataUpToDate = FALSE;

         Item->GetWindow()->SetMidTime(REFRESH);
         break;

      default:
         break;
   }

  XtAddEventHandler(w, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)Item );


}

void TraceItem::label_motion_handler(Widget w,  XtPointer p,
                                              XPointerMovedEvent *event)
{
   Position rootx, rooty;
   int x,y, diff;
   TraceItem*   Item = (TraceItem*)p;

   x = event->x;
   y = event->y;

   XtTranslateCoords( w,x,y, &rootx, &rooty );

   diff =  rooty - Item->Movey;

   Item->Movey = rooty ;

   Item->Y_AxisOffset += diff;

   XtVaSetValues( Item->wButton,
                  XmNtopOffset, Item->Y_AxisOffset + Item->Parent->GroupYOffset,
                  NULL);

}

/*******************************************************************************
*
* Function: GetNextTime
*
*
* Purpose:
*
*******************************************************************************/
LTime TraceItem::GetNextTime( int direction )
{
   LTime    NextTime = -1;
   LTime    NewPrevNextTime, NewPostNextTime;

   /* Check direction of next */
   if ( direction == 1 )
   {
      // Search upwards
      NewPrevNextTime= relative->PrevFile->GetNextTime(direction,
                                                   GetWindow()->MidTime);
      if (NewPrevNextTime > GetWindow()->MidTime)
      {
         NextTime = NewPrevNextTime;
      }

      NewPostNextTime = relative->PostFile->GetNextTime(direction,
                                                   GetWindow()->MidTime);
      if (NewPostNextTime > GetWindow()->MidTime)
      {
         if (NewPostNextTime <= NextTime || NextTime == -1)
         {
            NextTime = NewPostNextTime;
         }
      }
   }

   else
   {
      // Search downwards
      NewPrevNextTime= relative->PrevFile->GetNextTime(direction,
                                                   GetWindow()->MidTime);
      if (NewPrevNextTime < GetWindow()->MidTime)
      {
         NextTime = NewPrevNextTime;
      }

      NewPostNextTime = relative->PostFile->GetNextTime(direction,
                                                   GetWindow()->MidTime);
      if (NewPostNextTime < GetWindow()->MidTime)
      {
         if (NewPostNextTime >= NextTime || NextTime == -1)
         {
            NextTime = NewPostNextTime;
         }
      }
   }
   return NextTime;
}

void TraceItem::cbProperties(Widget w, XtPointer client_data,
                               XtPointer call_data )
{

     TraceItem *TItem = ( TraceItem *)client_data;

     PropDialog  *PropsDialog = PropDialog::GetPropDialog(TItem);

     XtAddEventHandler(TItem->wButton, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TItem );
}

void TraceItem::cbDelete(Widget w, XtPointer client_data,
                               XtPointer call_data )
{

   TraceItem *TItem = ( TraceItem *)client_data;
   //   TraceListType::Cursor cursor(TItem->Parent->TraceList);
   Boolean Found = False;

     XtDestroyWidget (TItem->wPopUp);
     TItem->wPopUp = NULL;

     XtAddEventHandler(TItem->wButton, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TItem );

//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {

     // printf("%s on line %d\n", __FILE__, __LINE__);
     TraceListType::iterator iter = TItem->Parent->TraceList.begin();
     for (  ; iter != TItem->Parent->TraceList.end();
           ++iter)
         {                         
             if ( *((TraceItem *) *iter) == *TItem )
                 {
                     Found = True;
                     break;
                 }
         }
     // printf("%s on line %d\n", __FILE__, __LINE__);

     if (Found == False)
         {
             printf("Error didn't find element\n");
         }
     
     TItem->Parent->TraceList.erase( iter );
     
     delete TItem;
}

void TraceItem::cbMakeFocus(Widget w, XtPointer client_data,
                               XtPointer call_data )
{

   TraceItem *TItem = ( TraceItem *)client_data;


   if (TItem->GetWindow()->CurrentFocusElement != NULL)
   {
      XtVaSetValues(TItem->GetWindow()->CurrentFocusElement->wButton,
                                            XtVaTypedArg, XmNbackground,
                                            XmRString, "DimGrey", 9 , NULL);
   }

   TItem->GetWindow()->SetCurrentFocusElement(TItem);

   XtDestroyWidget (TItem->wPopUp);

   XtAddEventHandler(TItem->wButton, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TItem );

    XtVaSetValues(TItem->wButton,
                   XtVaTypedArg, XmNbackground,
                   XmRString, "yellow", 7 ,
                   NULL);

}

void TraceItem::cbGotoStart(Widget w, XtPointer client_data,
                               XtPointer call_data )
{

     TraceItem *TItem = ( TraceItem *)client_data;

     TItem->GetWindow()->MidTime = TItem->GetStartTime();
     TItem->GetWindow()->SetMidTime(FULL_REDRAW);

     XtDestroyWidget (TItem->wPopUp);

     XtAddEventHandler(TItem->wButton, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TItem );

}

void TraceItem::cbGotoEnd(Widget w, XtPointer client_data,
                               XtPointer call_data )
{

     TraceItem *TItem = ( TraceItem *)client_data;

     TItem->GetWindow()->MidTime = TItem->GetEndTime();
     TItem->GetWindow()->SetMidTime(FULL_REDRAW);

     XtDestroyWidget (TItem->wPopUp);

     XtAddEventHandler(TItem->wButton, EnterWindowMask, False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TItem );

}


XtEventHandler TraceItem::label_entered_handler(Widget w, XtPointer ptr,
                                      XEnterWindowEvent *event)
{
   TransMechItem *TransMech;


   TransMech = ( TransMechItem *)malloc(sizeof(TransMechItem));

   TransMech->pthis = (TraceItem*)ptr;
   TransMech->x = event->x;
   TransMech->y = event->y;

   XtAddEventHandler(w,LeaveWindowMask,False,
                       (XtEventHandler)&(TraceItem::label_exited_bd_handler),
                       (XtPointer)TransMech->pthis );
   XtRemoveEventHandler(w,EnterWindowMask,False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TransMech->pthis );


   return NULL;
}

XtEventHandler TraceItem::label_exited_bd_handler(Widget w, XtPointer ptr,
                                      XLeaveWindowEvent *event)
{

   TraceItem *TItem = (TraceItem*)ptr;

   XtRemoveEventHandler(w,LeaveWindowMask,False,
                       (XtEventHandler)&(TraceItem::label_exited_bd_handler),
                       (XtPointer)TItem );

   XtAddEventHandler(w,EnterWindowMask,False,
                       (XtEventHandler)&(TraceItem::label_entered_handler),
                       (XtPointer)TItem );

   return NULL;
}

XtEventHandler TraceItem::label_exited_handler(Widget w, XtPointer ptr,
                                      XLeaveWindowEvent *event)
{
   TraceItem *TItem = (TraceItem*)ptr;
   
   XtRemoveEventHandler(w,LeaveWindowMask,False,
                       (XtEventHandler)&(TraceItem::label_exited_handler),
                       (XtPointer)TItem );

   XtAddEventHandler(w,EnterWindowMask,False,
                     (XtEventHandler)&(TraceItem::label_entered_handler),
                     (XtPointer)TItem );

   return NULL;
}

LTime TraceItem::GetStartTime()
{
   LTime NewTime;

   if (type == RELATIVE)
   {
      if ( relative->PrevFile->GetFirstHit() <
           relative->PostFile->GetFirstHit() )
      {
         NewTime =  relative->PrevFile->GetFirstHit();
      }
      else
      {
         NewTime = relative->PostFile->GetFirstHit();
      }
   }
   else
   {
      NewTime = plot->PlotFile->GetFirstHit();
   }

   return NewTime;
}

LTime TraceItem::GetEndTime()
{
   LTime NewTime;

   if (type == RELATIVE)
   {
      if ( relative->PrevFile->GetLastHit() >
           relative->PostFile->GetLastHit() )
      {
         NewTime = relative->PrevFile->GetLastHit();
      }
      else
      {
         NewTime = relative->PostFile->GetLastHit();
      }
   }
   else
   {
      NewTime = plot->PlotFile->GetLastHit();
   }

   return NewTime;
}

void TraceItem::Detach()
{
}

void TraceItem::Attach()
{
  XtVaSetValues(wButton,
                XmNtopOffset,     (Parent->GroupYOffset + Y_AxisOffset),
                NULL);

}
