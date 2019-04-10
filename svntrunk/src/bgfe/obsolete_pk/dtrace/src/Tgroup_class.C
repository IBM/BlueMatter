/*
 * NAME:        trace_group_class.C
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
#include "Tgroup_class.hpp"
//
//
// Implementation of the TraceGroup Class
//
//

TracerApplication* TraceGroup::GetApplication()
{
   return ParentWindow->GetApplication();
}

XtAppContext* TraceGroup::GetApplicationContext()
{
   return ParentWindow->GetApplicationContext();
}

Widget TraceGroup::GetApplicationShell()
{
   return ParentWindow->GetApplicationShell();
}

Widget TraceGroup::GetApplicationTopWindow()
{
   return ParentWindow->GetApplicationTopWindow();
}

TracerWindow* TraceGroup::GetWindow()
{
   return ParentWindow;
}

TraceGroup::TraceGroup(TracerWindow *Parent,
                       char*         Name,
                       char*         pchDirName,
                       char*         pchPrevRegExp,
                       char*         pchPostRegExp,
                       char*         pchColor,
                       entGraphStyle enGraphStyle,
                       int           GroupOffset,
                       int           NewTraceSeparation,
                       Boolean       Inverted,
                       Boolean       Contoured)
{
  Initialize(Parent,Name,pchDirName,pchPrevRegExp,pchPostRegExp,pchColor,
             enGraphStyle,GroupOffset,NewTraceSeparation,Inverted,Contoured);
}

TraceGroup::TraceGroup()
{
}

void TraceGroup::Initialize(TracerWindow *Parent,
                       char*         Name,
                       char*         pchDirName,
                       char*         pchPrevRegExp,
                       char*         pchPostRegExp,
                       char*         pchColor,
                       entGraphStyle enGraphStyle,
                       int           GroupOffset,
                       int           NewTraceSeparation,
                       Boolean       Inverted,
                       Boolean       Contoured)
{
   ParentWindow = Parent;
   (void)strcpy(GroupName,Name);
   (void)strcpy(DirName,pchDirName);
   (void)strcpy(PrevRegExp,pchPrevRegExp);
   (void)strcpy(PostRegExp,pchPostRegExp);
   (void)strcpy(GroupColor,pchColor);
   GroupGraphStyle = enGraphStyle;

   InvertedTrace = Inverted;
   ContourEnabled = Contoured;

   GroupYOffset = GroupOffset;
   TraceSeparation = NewTraceSeparation;

   wGroupButton = CreateButton();

   StepHeight = 4;
 
   ContourSpacing = 5;

   if ( ! TraceList.empty() )
   {
     //       printf("%s on line %d TraceList.clear()\n", __FILE__, __LINE__);   
      TraceList.clear();
   }

   CurrentHighest = NULL;
   CurrentLowest  = NULL;

   GroupStartTime = -1;
   GroupEndTime   = -1;

}

TraceGroup::~TraceGroup()
{
   TraceItem* TempPtr;

   //printf("%s on line %d\n", __FILE__, __LINE__);   

   while ( ! TraceList.empty() )
   {
      TempPtr = TraceList.back();

      TraceList.pop_back();

      delete TempPtr;
   }

   XtDestroyWidget(wGroupButton);
   XtDestroyWidget(wVirtualGroupButton);

}

int TraceGroup::operator == (TraceGroup const& t) const
{

   return 1;
}

int TraceGroup::operator  < (TraceGroup const& t) const
{
   return 1;
}

TraceGroup& TraceGroup::operator = (TraceGroup const& from) 
{
   return *this;
}

void   TraceGroup::DrawGroup(int flag)
{
    // TraceListType::Cursor cursor(TraceList);

   XtVaSetValues(wGroupButton,
                 XmNtopOffset, (GroupYOffset - ParentWindow->VirtualHeight),
                 NULL);

//    XtVaSetValues(wVirtualGroupButton,
//                  XmNtopOffset, (int)(GroupYOffset/VIRTUAL_FACTOR) ,
//                  NULL);

//    for ( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
//    {
//      TraceList.elementAt( cursor )->RedrawTrace(flag);
//    }

   // printf("%s on line %d\n", __FILE__, __LINE__);   

   // printf( "TraceList.size()=%d\n", TraceList.size() );

   for( int i = 0; i < TraceList.size(); i++ )
       {
           TraceList[ i ]->RedrawTrace( flag );
       }

  if (ContourEnabled == TRUE)
  {
    DoContourMapping();
  }
}

void TraceGroup::DoContourMapping()
{
   GC contourGC;
   XGCValues      values;
   XColor         newcolour, exact;

   int n = 0; //Number of lines to draw
//    TraceListType::Cursor cursor(TraceList);
//    TraceListType::Cursor cursornext(TraceList);

   TraceListType::iterator cursor = TraceList.begin();
   TraceListType::iterator cursornext = TraceList.begin();
   
   int CurrentPosition;
   int TopLimitVar ;
   int BottomLimitVar ;
   TraceItem* TraceTop;
   TraceItem* TraceBottom;
   int SizeOfArray = 2 * TraceList.size() 
       * (int)( daWidth / ContourSpacing );
   
   XSegment *XDrawingBlock;

   XDrawingBlock = new XSegment[SizeOfArray];

   // Set up first cursor
//    cursor.setToFirst();

//    cursornext.setToFirst();
//    cursornext.setToNext();
   ++cursornext;
   
   // while( cursornext.isValid() )
   while( cursornext != TraceList.end() )
   {
     // Do contour processing
     CurrentPosition = 0;

     TraceTop    = ( TraceItem * ) *cursor;
     TraceBottom = ( TraceItem * ) *cursornext;

     if (TraceTop->GraphStyle == FLAT_LINE_STYLE)
     {
       TopLimitVar = TraceTop->n;
     }
     else
     {
       TopLimitVar = TraceTop->saPos;
     }

     if (TraceBottom->GraphStyle == FLAT_LINE_STYLE)
     {
       BottomLimitVar = TraceBottom->n;
     }
     else
     {
       BottomLimitVar = TraceBottom->saPos;
     }

     // loop while there are still points in the traces
     while (CurrentPosition < TopLimitVar && CurrentPosition < BottomLimitVar )
     {
       // Check graph style of the top line
       if (TraceTop->GraphStyle == FLAT_LINE_STYLE)
       {
         // Flat line get PointArrayValues
         XDrawingBlock[n].x1 = TraceTop->PointArray[CurrentPosition].x;
         XDrawingBlock[n].y1 = TraceTop->PointArray[CurrentPosition].y;
       }
       else
       {
         XDrawingBlock[n].x1 = TraceTop->SegmentArray[CurrentPosition].x1;
         XDrawingBlock[n].y1 = TraceTop->SegmentArray[CurrentPosition].y2;
       }

       // Check graph style of the bottom line
       if (TraceBottom->GraphStyle == FLAT_LINE_STYLE )
       {
         // Flat line get PointArrayValues
         XDrawingBlock[n].x2 = TraceBottom->PointArray[CurrentPosition].x;
         XDrawingBlock[n].y2 = TraceBottom->PointArray[CurrentPosition].y;
       }
       else
       {
         XDrawingBlock[ n ].x2 = TraceBottom->SegmentArray[CurrentPosition].x1;
         XDrawingBlock[ n ].y2 = TraceBottom->SegmentArray[CurrentPosition].y2;
       }

       // up the counter for lines to draw
       n++;
      
       if ( n >= SizeOfArray )
       {
         printf("Exceeded array bounds\n");
       }

       // Shift up 
       CurrentPosition += ContourSpacing;
     }

     // Move on to the next item
     ++cursor;
     ++cursornext;
   }

   // Set up the CG values
   values.line_width = 1;

   /* Set the foreground color */
   if (XAllocNamedColor(XtDisplay(wGroupButton),
                         DefaultColormapOfScreen(XtScreen(wGroupButton)),
                         "white", &newcolour, &exact) == False)
   {
      fprintf( stderr, "Cannot allocate foreground colour\n" );
      values.foreground = WhitePixelOfScreen( XtScreen(wGroupButton));
   }
   else
   {
      values.foreground = newcolour.pixel;
   }

   /* Set the background color */
   if (XAllocNamedColor(XtDisplay(wGroupButton),
                         DefaultColormapOfScreen(XtScreen(wGroupButton)),
                         "black", &newcolour, &exact) == False)
   {
      fprintf( stderr, "Cannot allocate background colour\n" );
      values.background = BlackPixelOfScreen(XtScreen(wGroupButton));
   }
   else
   {
      values.background = newcolour.pixel;
   }

   values.function = GXcopy;

   contourGC = XCreateGC(XtDisplay(wGroupButton),XtWindow(wGroupButton),
                   GCBackground | GCForeground |  GCLineWidth | GCFunction,
                        (XGCValues*)&values);

   XDrawSegments(XtDisplay(ParentWindow->da),
                XtWindow(ParentWindow->da),contourGC,XDrawingBlock,n);

   delete XDrawingBlock;
   XFreeGC(XtDisplay(ParentWindow->da),contourGC);

}

void   TraceGroup::ChangeColor(char *NewColour)
{

    //   TraceListType::Cursor cursor(TraceList);

//    strcpy(GroupColor,NewColour);

//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
//       TraceList.elementAt(cursor)->ResetColourMemebers(NewColour);
//    }

   for( int i = 0; i < TraceList.size(); i++ )
       {
           TraceList[ i ]->ResetColourMemebers( NewColour );
       }

   DrawGroup(REFRESH);
}

void   TraceGroup::ChangeProperties(char *NewColour, Boolean Value,
                                    Boolean ContourValue)
{

    // TraceListType::Cursor cursor(TraceList);

   strcpy(GroupColor,NewColour);
   InvertedTrace = Value;
   ContourEnabled = ContourValue;

//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
//       TraceList.elementAt(cursor)->ResetColourMemebers(NewColour);
//       TraceList.elementAt(cursor)->ResetInvertedMemebers(Value);
//    }

   for( int i = 0; i < TraceList.size(); i++ )
     {
     TraceList[ i ]->ResetColourMemebers( NewColour );
     TraceList[ i ]->ResetInvertedMemebers( Value );           
     }
   
   DrawGroup(REFRESH);
}

void   TraceGroup::ChangeInverted(Boolean Value)
{

    // TraceListType::Cursor cursor(TraceList);

   InvertedTrace = Value;
//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
//       TraceList.elementAt(cursor)->ResetInvertedMemebers(Value);
//    }

   for( int i = 0; i < TraceList.size(); i++ )
       {
           TraceList[ i ]->ResetInvertedMemebers( Value );
       }

   DrawGroup(REFRESH);
}

void   TraceGroup::ChangeContour(Boolean Value)
{

   ContourEnabled = Value;

   DrawGroup(REFRESH);
}

void TraceGroup::group_press_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event)
{
  TraceGroup*   Group ;
  Position rootx, rooty;
  XmString xmString;
  char FileName[MAX_FILENAME];

  Arg args[20];
  Cardinal n;

  Group = ((TraceGroup*)p);
  Group->Lab_button = event->button;

 
  XtTranslateCoords( w,event->x,event->y, &rootx, &rooty );

  switch (event->button)
  {

      case Button1:
         Group->Movey = rooty ;
         Group->MoveActive = True ;
         Group->GetApplication()->
                           SendMessageText("\nPreparing for group movement...");
         Group->GetApplication()->
                           SendMessageText("\nMoving group...");

         
         break;

      case Button2:
      case Button3:
          /* Create the PopUp menu */
          n = 0;
          Group->wPopUp = XmCreatePopupMenu(w,"wPopUp",args,n);

          // Create Prev file regular expression label
          sprintf(FileName,"File: %s",Group->DirName);
          xmString = XmStringCreateSimple(FileName);

          (void) XtVaCreateManagedWidget("GroupPrevTitle",xmLabelWidgetClass,
                                         Group->wPopUp,
                                         XmNlabelString, xmString,
                                         XmNalignment, XmALIGNMENT_BEGINNING,
                                         NULL);

          XmStringFree(xmString);

          // Create Prev file regular expression label
          sprintf(FileName,"Prev Reg Exp: %s",Group->PrevRegExp);
          xmString = XmStringCreateSimple(FileName);

          (void) XtVaCreateManagedWidget("GroupPrevTitle",xmLabelWidgetClass,
                                         Group->wPopUp,
                                         XmNlabelString, xmString,
                                         XmNalignment, XmALIGNMENT_BEGINNING,
                                         NULL);

          XmStringFree(xmString);

          // Create Post file regular expression label
          sprintf(FileName,"Post Reg Exp: %s", Group->PostRegExp);
          xmString = XmStringCreateSimple(FileName);

          (void) XtVaCreateManagedWidget("GroupPostTitle",xmLabelWidgetClass,
                                         Group->wPopUp,
                                         XmNlabelString, xmString,
                                         XmNalignment, XmALIGNMENT_BEGINNING,
                                         NULL);

          XmStringFree(xmString);

          (void) XtVaCreateManagedWidget(Group->GroupName,xmLabelWidgetClass,
                                         Group->wPopUp,NULL);

          (void) XtVaCreateManagedWidget("Sep1",xmSeparatorWidgetClass,
                                         Group->wPopUp,NULL);

          (void) CreatePushButton(Group->wPopUp,"Delete",
                 (XtCallbackProc) Group->cbDelete,
                 (XtPointer) Group);

          (void) XtVaCreateManagedWidget("Sep2",xmSeparatorWidgetClass,
                                         Group->wPopUp,NULL);


          (void) CreatePushButton(Group->wPopUp,"Save...",
                 (XtCallbackProc) Group->cbSave,
                 (XtPointer) Group);

          (void) XtVaCreateManagedWidget("Sep3",xmSeparatorWidgetClass,
                                         Group->wPopUp,NULL);

          (void) CreatePushButton(Group->wPopUp,"Properties...",
                 (XtCallbackProc) Group->cbProperties,
                 (XtPointer) Group);

          XmMenuPosition(Group->wPopUp,event);
          XtManageChild(Group->wPopUp);
          break;

   }

}

void TraceGroup::group_release_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event)
{
   TraceGroup*   Group = (TraceGroup*)p;

   switch (event->button)
   {

      case Button3:
         break;

      case Button1:
         Group->MoveActive = False;
         Group->GetApplication()->
                           SendMessageText("\nEnding group movement...");
         Group->ReattachItems();
         Group->SetItemsOutOfDate();
         Group->GetWindow()->SetMidTime(REFRESH);
         Group->GetApplication()->
                           SendMessageText("\nReady...");


         break;

      default:
      //   SetMidTime(1);
         Group->MoveActive = False;
         break;
   }
}

void TraceGroup::group_motion_handler(Widget w,  XtPointer p,
                                              XPointerMovedEvent *event)
{
   register int diff;
   Position rootx, rooty;
   TraceGroup*   Group = (TraceGroup*)p;

   XtTranslateCoords( w,event->x,event->y, &rootx, &rooty );

   diff =  rooty - (Group->Movey);
   Group->Movey = rooty ;

   Group->GroupYOffset += diff;

   if (( diff < 5) && (diff > -5))
   { 
      XtVaSetValues( w,
                  XmNtopOffset, Group->GroupYOffset - 
                                             Group->ParentWindow->VirtualHeight,
                  NULL);

      XtVaSetValues( Group->wVirtualGroupButton,
                  XmNtopOffset, (int)(Group->GroupYOffset/VIRTUAL_FACTOR) ,
                  NULL);

   }
}

Widget  TraceGroup::CreateButton()
{
   Widget w;
   Arg  args[16];
   int n =0;
   XmFontList       fontlist;
   XmFontListEntry  entry;
   XFontStruct      *font;
   int RealOffset;
   
   RealOffset = GroupYOffset - ParentWindow->VirtualHeight;

   font = XLoadQueryFont( XtDisplay(GetApplicationShell()),
                            "6x10" );
   entry = XmFontListEntryCreate( "tag1", XmFONT_IS_FONT , font);
   fontlist = XmFontListAppendEntry( NULL, entry );

   // Label for trace
   XtSetArg( args[n],XmNtopAttachment,    XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNtopOffset,        RealOffset ); n++;
   XtSetArg( args[n],XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNleftOffset,       1 ); n++;
   XtSetArg( args[n],XmNfontList,         fontlist ); n++;
   XtSetArg( args[n],XmNheight,           5 ); n++;
   XtSetArg( args[n],XmNwidth,            10 ); n++;
   XtSetArg( args[n],XmNhighlightOnEnter, TRUE  ); n++;


   // Create the Widget
   w = XmCreatePushButton( GetWindow()->formLabel, ".", args, n );


   //Create the virtual button
   n = 0;
   XtSetArg( args[n],XmNtopAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNtopOffset,       (int)(RealOffset/VIRTUAL_FACTOR)); n++;
   XtSetArg( args[n],XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNleftOffset,       0 ); n++;
   XtSetArg( args[n],XmNrightAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n],XmNrightOffset,      0 ); n++;
   XtSetArg( args[n],XmNheight,           5); n++;
   XtSetArg( args[n],XmNhighlightOnEnter, TRUE  ); n++;

   wVirtualGroupButton = 
           XmCreatePushButton( GetWindow()->formVirtualMap, ".", args, n );

   XtVaSetValues( wVirtualGroupButton,
                      XtVaTypedArg, XmNbackground,  XmRString, "DimGrey", 8,
                      NULL);
   // Label for trace
   XtAddEventHandler(w, ButtonPressMask, False,
                       (XtEventHandler)&(TraceGroup::group_press_handler),
                       (XtPointer)this );

   XtAddEventHandler(w, ButtonReleaseMask, False,
                       (XtEventHandler)&(TraceGroup::group_release_handler),
                       (XtPointer)this );

   XtAddEventHandler(w, Button1MotionMask, False,
                       (XtEventHandler)&(TraceGroup::group_motion_handler),
                       (XtPointer)this );

   XtVaSetValues( w,
                      XtVaTypedArg, XmNbackground,  XmRString, "DimGrey", 8,
                      NULL);

   XtManageChild( w );
   XtManageChild( wVirtualGroupButton );

   XmFontListFree( fontlist );

   return w;
}

void TraceGroup::AddTrace(int YOffset,
                          int Show, 
                          char *Colour,
                          TRACE_TYPE Type,
                          __pktraceRead* MappedFile,
                          int pchFile1,
                          int pchFile2,
                          entGraphStyle NewGraphStyle,
                          Boolean       Inverted)
{

   int TraceOffset ;
   int ypos;

   // Check if no elements in group
   if (TraceList.size() == 0)
   {
      GroupYOffset = YOffset;
      ypos = GroupYOffset;

      XtVaSetValues (wGroupButton,
                     XmNtopOffset,ypos - ParentWindow->VirtualHeight,
                     NULL);

      XtVaSetValues (wVirtualGroupButton,
                     XmNtopOffset,(int)(ypos/VIRTUAL_FACTOR) ,
                     NULL);

   }

   TraceOffset = (int)(TraceList.size() * TraceSeparation);

   TraceItem* Item = new TraceItem(this, TraceOffset,
                                   Show,
                                   Colour,
                                   Type,
                                   MappedFile,
                                   pchFile1, pchFile2,
                                   NewGraphStyle, Inverted);


   // printf("%s on line %d\n", __FILE__, __LINE__);   
   TraceList.push_back( Item );
   //printf( "TraceList.size()=%d\n", TraceList.size() );

   if ((CurrentHighest == NULL) || (TraceOffset < CurrentHighest->Y_AxisOffset))
   {
      CurrentHighest = Item;
   }

   if ((CurrentLowest == NULL) || (TraceOffset > CurrentLowest->Y_AxisOffset))
   {
      CurrentLowest = Item;
   }

   XtVaSetValues(wGroupButton,
                 XmNheight,((CurrentLowest->Y_AxisOffset - 
                                     CurrentHighest->Y_AxisOffset) + 5),
                 NULL);

   XtVaSetValues(wVirtualGroupButton,
                 XmNheight,(int)(((CurrentLowest->Y_AxisOffset -
                             CurrentHighest->Y_AxisOffset) + 5)/VIRTUAL_FACTOR),
                 NULL);

   // Calculate the group start and end times
   // Check if value is already set
   if (GetGroupStartTime() != -1)
   {
      if (Item->GetStartTime() < GetGroupStartTime())
      {
         SetGroupStartTime(Item->GetStartTime());
      }
   }
   else
   {
      SetGroupStartTime(Item->GetStartTime());
   }

   // Check if value is already set
   if (GetGroupEndTime() != -1)
   {
      if (Item->GetEndTime() > GetGroupEndTime())
      {
         SetGroupEndTime(Item->GetEndTime());
      }
   }
   else
   {
      SetGroupEndTime(Item->GetEndTime());
   }


   // Calculate the window start and end times
   if (ParentWindow->GetWindowStartTime() != -1)
   {
      if (GetGroupStartTime() < ParentWindow->GetWindowStartTime())
      {
         ParentWindow->SetWindowStartTime(GetGroupStartTime());
      }
   }
   else
   {
      ParentWindow->SetWindowStartTime(GetGroupStartTime());
   }

   // Calculate the window start and end times
   if (ParentWindow->GetWindowEndTime() != -1)
   {
      if (GetGroupEndTime() > ParentWindow->GetWindowEndTime())
      {
         ParentWindow->SetWindowEndTime(GetGroupEndTime());
      }
   }
   else
   {
      ParentWindow->SetWindowEndTime(GetGroupEndTime());
   }

}

Widget TraceGroup::GetHighestTraceYCoord()
{
   Widget   TempWidget;
   int CurrentHighest = 0;
   // TraceListType::Cursor cursor(TraceList);
   int Coord;

//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
   for( int i = 0; i < TraceList.size(); i++ )
     {
         
         XtVaGetValues(TraceList[i]->wButton,
                       XmNtopOffset, &Coord,
                       NULL); 
         
         /* Remember its offset, higher it is, further from the top of the form*/
         if ((Coord - ParentWindow->VirtualHeight) <= CurrentHighest)
             {
                 CurrentHighest = TraceList[i]->Y_AxisOffset;
                 TempWidget = TraceList[i]->wButton;
             }
     }
   
   return TempWidget;
}

void TraceGroup::SetItemsOutOfDate()
{
//    TraceListType::Cursor cursor(TraceList);
//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
  for( int i = 0; i < TraceList.size(); i++ )
    {            
    TraceList[ i ]->DataUpToDate = FALSE;
    }
}

void TraceGroup::ReattachItems()
{
//    TraceListType::Cursor cursor(TraceList);
//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
   for( int i = 0; i < TraceList.size(); i++ )
     {    
     TraceList[ i ]->Attach();
     }
}

void TraceGroup::DetachItems()
{
//    TraceListType::Cursor cursor(TraceList);
//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
    for( int i = 0; i < TraceList.size(); i++ )
      {              
      TraceList[ i ]->Detach();
      }
}

Widget TraceGroup::GetLowestTraceYCoord()
{
   Widget   TempWidget;
   int CurrentLowest = 0;

//    TraceListType::Cursor cursor(TraceList);
//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
   for( int i = 0; i < TraceList.size(); i++ )
       {              
           
           /* Remember: it's offset, lower it is, closer to the top of the form*/
           if ( TraceList[ i ]->Y_AxisOffset >= CurrentLowest )
               {
                   CurrentLowest = TraceList[ i ]->Y_AxisOffset;
                   TempWidget = TraceList[i]->wButton;
               }
       }

   return TempWidget;

}

void TraceGroup::ResizeGroupBar()
{
   GroupYOffset += CurrentHighest->Y_AxisOffset;

   XtVaSetValues( wGroupButton,
                  XmNtopOffset, GroupYOffset - ParentWindow->VirtualHeight ,
                  XmNheight,    ((CurrentLowest->Y_AxisOffset -
                                  CurrentHighest->Y_AxisOffset) + 5),
                  NULL);
   
/*   XtVaSetValues( wGroupButton,
                  XmNtopOffset, (int)(GroupYOffset/VIRTUAL_FACTOR) ,
                  XmNheight, (int)( ((CurrentLowest->Y_AxisOffset -
                             CurrentHighest->Y_AxisOffset) + 5)/VIRTUAL_FACTOR),
                  NULL);*/

   CurrentHighest->Y_AxisOffset = 0;

   XtVaSetValues(CurrentHighest->wButton,
                 XmNtopOffset, 0,
                 NULL);
}

void TraceGroup::cbDelete(Widget w, XtPointer client_data,
                          XtPointer call_data )
{
    
    TraceGroup *TGroup = ( TraceGroup *)client_data;
    Boolean Found = False;
    XtDestroyWidget (TGroup->wPopUp);
    TGroup->wPopUp = NULL;
   
    // TraceGroupListType::Cursor cursor(TGroup->ParentWindow->TraceListSeq);
    
    // for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
   TraceGroupListType::iterator iter =  TGroup->ParentWindow->TraceListSeq.begin();
   
   for( ; iter != TGroup->ParentWindow->TraceListSeq.end();
        ++iter  ) 
   {
      if( *(( TraceGroup *) *iter ) == *TGroup )
      {
         Found = True;
         printf("Element found\n");
         break;
      }
   }
   if (Found == False)
   {
      printf("Error didn't find element\n");
   }

   TGroup->ParentWindow->TraceListSeq.erase( iter );
   delete TGroup;



}

void TraceGroup::cbProperties(Widget w, XtPointer client_data,
                              XtPointer call_data )
{
     TraceGroup *TGroup = ( TraceGroup *)client_data;
     XtDestroyWidget (TGroup->wPopUp);
     TGroup->wPopUp = NULL;

     GroupPropDialog  *PropsDialog = new GroupPropDialog(TGroup);
}

void TraceGroup::cbSave(Widget w, XtPointer client_data, XtPointer call_data )
{
     TraceGroup *TGroup = ( TraceGroup *)client_data;
     XtDestroyWidget(TGroup->wPopUp);

     TGroup->wPopUp = NULL;

     GroupSaveDialog  *SaveDialog = new GroupSaveDialog(TGroup);
}

LTime TraceGroup::GetGroupStartTime()
{
   return GroupStartTime;
}

LTime TraceGroup::GetGroupEndTime()
{
   return GroupEndTime;
}

void TraceGroup::SetGroupStartTime(LTime NewValue)
{
   GroupStartTime = NewValue;
}

void TraceGroup::SetGroupEndTime(LTime NewValue)
{
   GroupEndTime = NewValue;
}
