/*
 * NAME:        tracer_lib.c
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

#include "Tlib.hpp"

char * scaleLabels[] = { "< 1000", " < 500 " ,"< 100", "< 10" , "< 1",
                         "< NEXT", " < MARK", " > MARK", "> NEXT",
                         "   > 1", "  > 10", " > 100" ,"> 500",  "> 1000"};

char* scaleValues[] = { "-1000", "-500", "-100 " ,"-10 " , "-1",
                        "-N", "-M", "+M",  "+N",
                         "1", "10", "100" ,"500", "1000"};

int ScaleArray[12] = { 0,1,2,3,4,5,6,7,8,9,10,USER_DEFINED};


char* ColourStrings[NUM_COLOUR_STRINGS] = {"white",
                                        "green",
                                        "red",
                                        "blue",
                                        "yellow",
                                        "cyan",
                                        "pink",
                                        "purple",
                                        "coral",
                                        "tomato",
                                        "maroon",
                                        "plum",
                                        "orchid",
                                        "magenta",
                                        "turquoise1",
                                        "grey",
                                        "DodgerBlue",
                                        "PowderBlue",
                                        "SpringGreen"};

char* GraphStyleStrings[NUM_GRAPH_STYLES] = {"Flat-Line",
                                        "Vertical",
                                        "Points"};

char* GCStrings[NUM_GC_FUNCTIONS] = {"GXand",
                                     "GXandInverted",
                                     "GXandReverse",
                                     "GXclear",
                                     "GXcopy",
                                     "GXcopyInverted",
                                     "GXequiv",
                                     "GXinvert",
                                     "GXnand",
                                     "GXnoop",
                                     "GXnor",
                                     "GXor",
                                     "GXorInverted",
                                     "GXorReverse",
                                     "GXset",
                                     "GXxor"};

char* ScaleStrings[12] = { "10-9s",
                           "10-8s",
                           "10-7s",
                           "10-6s",
                           "10-5s",
                           "10-4s",
                           "10-3s",
                           "10-2s",
                           "10-1s",
                           "s",
                           "10 s",
                           "User"};

double ZoomArray[] = {0.5,2};

char* ZoomStrings[] = { "Zoom In X 2",
                        "Zoom Out X 2" };

long zoomValues[] = { -10000000,-1000000, -500000 ,-250000,-100000,
                      -50000,-10000,-5000,-1000,-500,-100,-50,-10,-5,
                      -1,0, 1,5,10,50,100,500,1000,5000,10000, 50000,
                      100000, 250000, 500000, 1000000, 10000000 };

LTime TbToLTime( timebasestruct_t tb )
{
  LTime rc;
  rc = (((long long) tb.tb_high) * (long long)1000000000) + tb.tb_low;
  return( rc );
}

Boolean IsSelected(XmListCallbackStruct* ptr)
{
   int i;
   for(i= 0; i < ptr->selected_item_count; i++)
   {
      if (ptr->selected_item_positions[i] == ptr->item_position)
      {
         return True;
      }
   }
   return False;
}

int ConstructGC( GC * gc, Widget w, int width, char *fg,
                 char *bg, char *f, int Extraflags, int function )
{
    XGCValues      values;
    XColor         colour, exact;
    Font           font;

    values.line_width = width;
    if ( function )
       values.function = function;

    *gc = XCreateGC (XtDisplay(w),
                     XtWindow(w),
                     GCBackground | GCForeground |  GCLineWidth | Extraflags,
                     &values );
    if ( fg )
    {
      if (!XAllocNamedColor(XtDisplay(w),
                   DefaultColormapOfScreen(XtScreen(w)),
                   fg, &colour, &exact))
      fprintf( stderr, "Cannot allocate colour\n" );
      XSetForeground(XtDisplay(w), *gc, colour.pixel);
    }

    if ( bg )
    {
       if (!XAllocNamedColor(XtDisplay(w),
                   DefaultColormapOfScreen(XtScreen(w)),
                   bg, &colour, &exact))
       fprintf( stderr, "Cannot allocate colour\n" );
       XSetBackground(XtDisplay(w), *gc, colour.pixel);
    }
    if ( f )
    {
       font = XLoadFont( XtDisplay(w), f );
       XSetFont( XtDisplay(w), *gc, font );
    }
    return 0;
}

Widget CreateGenericColorPullDownMenu(Widget parent, XtCallbackProc callback,
                               XtPointer client_data, int SelectedValue)
{
  register int i;

  Arg args[20];
  Cardinal n = 0;
  Widget mu_colour, mu_ColourOption;
  XmString str ;
  Widget wButtons[NUM_COLOUR_STRINGS];

  if (SelectedValue != -1)
  {
     XtSetArg(args[n],XmNbuttonSet,SelectedValue); n++;
  }
  else
  {
     XtSetArg(args[n],XmNbuttonSet,0); n++;
  }

  mu_colour = XmCreatePulldownMenu( parent, "Colour_menu", args, 0 );
  str = XmStringCreateSimple("Colour:");

  n = 0;
  XtSetArg( args[n],XmNsubMenuId, mu_colour ); n++;
  XtSetArg( args[n],XmNlabelString, str ); n++;
  XtSetArg( args[n],XmNorientation, XmVERTICAL ); n++;
  XtSetArg( args[n],XmNentryAlignment, XmALIGNMENT_BEGINNING ); n++;
  XtSetArg( args[n],XmNbuttonSet,5); n++;

  mu_ColourOption = XmCreateOptionMenu( parent, "option", args, n );


  for (i = 0; i < NUM_COLOUR_STRINGS; i++)
  {
     wButtons[i] = CreatePushButton(mu_colour,ColourStrings[i],
                          (XtCallbackProc)callback,
                          (XtPointer)client_data);
  }

  if (SelectedValue != -1)
  {
     XtVaSetValues(mu_colour,XmNmenuHistory,wButtons[SelectedValue],NULL);
  }

  return (mu_ColourOption);
}

Widget CreateGenericGraphStylePullDownMenu(Widget parent,
                               XtCallbackProc callback,
                               XtPointer client_data,
                               int SelectedValue, 
                               Widget *VertButton)
{
  register int i;

  Arg args[20];
  Cardinal n = 0;
  Widget w_menu, mu_Option;
  XmString str ;
  Widget wButtons[NUM_GRAPH_STYLES];

  w_menu = XmCreatePulldownMenu( parent, "Colour_menu", args, 0 );
  str = XmStringCreateSimple("Graph Style:");

  n = 0;
  XtSetArg( args[n],XmNsubMenuId, w_menu ); n++;
  XtSetArg( args[n],XmNlabelString, str ); n++;
  XtSetArg( args[n],XmNorientation, XmVERTICAL ); n++;
  XtSetArg( args[n],XmNentryAlignment, XmALIGNMENT_BEGINNING ); n++;


  mu_Option = XmCreateOptionMenu( parent, "option", args, n );

  for (i = 0; i < NUM_GRAPH_STYLES; i++)
  {
     wButtons[i] = CreatePushButton(w_menu,GraphStyleStrings[i],
                          (XtCallbackProc)callback,
                          (XtPointer)client_data);
  }

  if (SelectedValue != -1)
  {
     XtVaSetValues(w_menu,XmNmenuHistory,wButtons[SelectedValue],NULL);
  }

  *VertButton = wButtons[(int)VERT_LINE_STYLE];

  return (mu_Option);
}

Widget CreatePushButton (Widget parent,
                         char * name,
                         XtCallbackProc  callback,
                         XtPointer       client_data)
{
   Widget Push;
   Arg args[20];
   Cardinal n;
   n = 0;
   Push = XmCreatePushButton(parent,name,args,n);

   XtAddCallback(Push, XmNactivateCallback,
                     callback,
                     client_data);

   XtManageChild(Push);

   return Push;
}

Widget CreateArrowButton (Widget parent,
                         char * name,
                         XtCallbackProc  callback,
                         XtPointer       client_data)
{
   Widget Arrow;
   Arg args[20];
   Cardinal n;
   n = 0;
   Arrow = XmCreateArrowButton(parent,name,args,n);

   XtAddCallback(Arrow, XmNactivateCallback,
                     callback,
                     client_data);

   XtManageChild(Arrow);

   return Arrow;
}

Widget CreateToggleButton (Widget parent,
                         char * name,
                         XtCallbackProc  callback,
                         XtPointer       client_data)
{
   Widget Toggle;
   Arg args[20];
   Cardinal n;
   n = 0;

   Toggle = XmCreateToggleButton(parent,name,args,n);

   XtAddCallback(Toggle, XmNarmCallback,
                       callback,
                       client_data);

   XtManageChild(Toggle);

   return Toggle;
}

int GetPositionOfColor(char* SelectedColor)
{
   register loop;

   for (loop = 0; loop < NUM_COLOUR_STRINGS; loop++)
   {
      if (strcmp(SelectedColor, ColourStrings[loop]) == 0)
      {
         return loop;
      }
   }
   printf("Error, colour not found\n");
   return (-1);
}

char* CreateSecondsFromLTime(LTime ltTime)
{
   char str[256];
   long SecondsValue, FractSecsValue;
   register int loop1;
   char NewAfterPointString[13];
   char AfterPointString[10];
   Boolean DigitFound;
   int CurrentPos;
   char *pstr;

   SecondsValue =   ltTime/(long)1000000000;
   FractSecsValue = ltTime%(long)1000000000;

   sprintf(AfterPointString,"%9ld",FractSecsValue);

   // Loop through inserting spaces
   for (loop1 = 0; loop1 < 9; loop1++)
   {
      if (AfterPointString[loop1] == ' ')
      {
         AfterPointString[loop1] = '0';
      }
   }

   // Move the blocks to new positons
   strncpy(&(NewAfterPointString[1]),AfterPointString,3);
   strncpy(&(NewAfterPointString[5]),&(AfterPointString[3]),3);
   strncpy(&(NewAfterPointString[9]),&(AfterPointString[6]),3);

   // Insert the spaces
   NewAfterPointString[4] = ' ';
   NewAfterPointString[8] = ' ';

   // Set the end of the line
   NewAfterPointString[0] = '.';

   NewAfterPointString[12] = '\0';

   CurrentPos = 11;
   DigitFound = FALSE;

   while (DigitFound == FALSE)
   {
      if ((NewAfterPointString[CurrentPos] == '0') ||
          (NewAfterPointString[CurrentPos] == ' ') ||
          (NewAfterPointString[CurrentPos] == '.'))
      {
         NewAfterPointString[CurrentPos] = '\0';

         if (NewAfterPointString[CurrentPos] == '.')
         {
            DigitFound = TRUE;
         }  
      }

      else
      {
         DigitFound = TRUE;
      }
      CurrentPos--;
   }
   sprintf(str,"%ld%s s",SecondsValue,NewAfterPointString);
   pstr = &(str[0]);

   return pstr ;

}

void GetPathFromFile(char* FileName,char* FileOnly,  char* DirName)
{
  register int nloop;
  Boolean Found;
  int FileNameStart;

  // loop through the file name until a / is found
  for (nloop = (strlen(FileName)-1), Found = False;
       nloop >= 0, Found == False;
       nloop--)
  {
    if (FileName[nloop] == '/')
    {
      FileNameStart = nloop + 1;
      Found = True;
    }
  }

  strcpy(FileOnly,&(FileName[FileNameStart]));
  strncpy(DirName,FileName,(FileNameStart - 1));
  DirName[FileNameStart - 1] = '\0';

}

Boolean CheckReg(char *RegExp, char* Tracename)
{

//   setlocale(LC_ALL, "");

   if ( fnmatch(RegExp, Tracename,FNM_PATHNAME | FNM_PERIOD) == 0)
   {
     return (TRUE);
   }
   else
   {
     return (FALSE);
   }
}

void no_storage_handler()
{
  printf("No space for new allocation\n");
  exit(1);
}

