// template <class Element>
// class IACollection;
#include "Tset_scale_dialog.hpp"

/*******************************************************************************
*
* Function: ScaleDialog
*
*
* Purpose:
*
*******************************************************************************/
ScaleDialog::ScaleDialog(TracerWindow *ParentWindow)
{

   Arg args[20];
   int n =0;
   double index;
   Parent = ParentWindow;

   XtSetArg( args[n],  XmNallowResize, True ); n++;
   XtSetArg( args[n],  XmNwidth,   350 ); n++;
   XtSetArg( args[n],  XmNheight,  150 ); n++;
   XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_MODELESS ); n++;
   XtSetArg( args[n],  XmNtitle, "Scale Dialog" ); n++;


   wScaleDialog = XmCreateFormDialog( Parent->form0,  "Scale", args, n );

   guiDrawTimeScale();

   index = log10( (double) Parent->Scale);
   
   XtVaSetValues( mu_TimeScale, XmNmenuHistory, bnScale[(int)index], NULL );

   XtManageChild( wScaleDialog );

}

ScaleDialog::~ScaleDialog()
{

}

/*******************************************************************************
*
* Function: guiDrawTimeScale
*
*
* Purpose: 
*
*******************************************************************************/
void ScaleDialog::guiDrawTimeScale( )
{
   TransportMech *TransMech;
   XmString str;
   Arg args[16];
   int n, i;
   char color[16] = BG_COLOR;
   Widget rc_RowCol0, rc_RowCol1;

 
   if (wScaleDialog == NULL)
      printf("wScaleDialog == NULL\n");

   if (fr_ScaleFrame == NULL)
      printf("fr_ScaleFrame == NULL\n");
   fr_ScaleFrame = XtVaCreateWidget("fr_ScaleFrame",
                             xmFrameWidgetClass, wScaleDialog ,
                             NULL);

   (void)XtVaCreateManagedWidget( "Scale", xmLabelWidgetClass,
                         fr_ScaleFrame,
                         XmNchildType, XmFRAME_TITLE_CHILD,
                         XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                         XmNchildVerticalAlignment, XmALIGNMENT_BASELINE_BOTTOM,
                         NULL );


   rc_RowCol0  = XtVaCreateWidget( "rc_RowCol0", xmRowColumnWidgetClass,
                             fr_ScaleFrame,
                             XmNnumColumns,          1,
                             XmNorientation,         XmVERTICAL,
                             NULL );


   rc_Menu  = XtVaCreateWidget( "rc_Menu", xmRowColumnWidgetClass,
                             rc_RowCol0,
                             NULL );

   mu_TimeScale = XmCreatePulldownMenu( rc_Menu, "menu", args, 0 );


   str = XmStringCreateSimple("Time Scale");

   n = 0;
   XtSetArg( args[n],XmNsubMenuId, mu_TimeScale ); n++;
   XtSetArg( args[n],XmNlabelString, str ); n++;

   mu_Option = XmCreateOptionMenu( rc_Menu, "option", args, n );


   for( i=0; i < 12; i++ )
   {
      TransMech = (TransportMech*)malloc(sizeof(struct TransportMech));

      TransMech->pthis = (ScaleDialog*)this;
      TransMech->iValue = ScaleArray[i];

      bnScale[i]   = XtVaCreateManagedWidget( ScaleStrings[i],
                                              xmPushButtonWidgetClass,
                                              mu_TimeScale,
                                              NULL);

      XtAddCallback(bnScale[i], XmNactivateCallback,
                   (XtCallbackProc)&ScaleDialog::TimeScaleProc,
                   (XtPointer)TransMech);

      XtVaSetValues(bnScale[i],   XtVaTypedArg, XmNbackground,
                                     XmRString, color, strlen(color) + 1, NULL);
   }

   rc_RowCol1  = XtVaCreateWidget( "", xmRowColumnWidgetClass,
                                   rc_RowCol0,
                                   XmNnumColumns,          1,
                                   XmNorientation,         XmHORIZONTAL,
                                   NULL );

   XtManageChild(rc_RowCol1);
   XtManageChild(rc_RowCol0 );
   XtManageChild(fr_ScaleFrame );
//   XtManageChild(rc_Scale );
   XtManageChild(mu_Option);
   XtManageChild( rc_Menu);
   XmStringFree(str);

}

/*******************************************************************************
*
* Function: TimeScaleProc
*
*
* Purpose:
*
*******************************************************************************/
void ScaleDialog::TimeScaleProc( Widget w, XtPointer client_data,
                                       XtPointer call_data )
{
  XmPushButtonCallbackStruct* cbs = (XmPushButtonCallbackStruct*)(call_data);

  TransportMech *TransMechanism = (TransportMech*)client_data;

  ScaleDialog *Dialog = (ScaleDialog*)(TransMechanism->pthis);

  Dialog->SetTimeScale(  TransMechanism->iValue);
}

void ScaleDialog::SetTimeScale(int NewScale)
{

   int i;
   double index;
   long long factor = 1;
   XmString txt_str;
   char TempString[512];

   if ( NewScale != USER_DEFINED )
   {
      Parent->UserDefFlag = False;
      for ( i =0; i < NewScale; i++ )
      {
         factor  = factor * 10;
      }
   }
   else
   {
     
      Parent->UserDefFlag = True;
      factor = NewScale;
   }

   Parent->Scale = factor;
 

   Parent->SetMidTime(FULL_REDRAW);

   index  = log10((double) Parent->Scale);

   if (Parent->UserDefFlag == False )
   {
      // Not User defined scale so print the menu value
      txt_str = XmStringCreateSimple( ScaleStrings[(int)index] );
   }
   else
   {
      // User defined scale
      sprintf(TempString,"%s : %d ns",ScaleStrings[Parent->Scale],Parent->Scale);
      txt_str = XmStringCreateSimple( TempString );
   }


   XmStringFree(txt_str);
}

