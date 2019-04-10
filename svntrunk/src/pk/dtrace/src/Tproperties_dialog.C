// template <class Element>
// class IACollection;
#include <Tproperties_dialog.hpp>

PropDialog* PropDialog::GetPropDialog(TraceItem *pParentItem)
{
  static PropDialog *SingletonPtr;

  if ( SingletonPtr == NULL )
  {
    SingletonPtr = new PropDialog(pParentItem);
  }
  assert( SingletonPtr != NULL );

  return (SingletonPtr);
}

PropDialog::PropDialog(TraceItem *pParentItem)
{

   Arg args[20];
   int n =0;
   ParentItem = pParentItem;
   Widget lb_Previous,lb_Post, lb_PrevHits, lb_PostHits, Sep1;
   XGCValues      values;
   XmString xmString, str;
   Widget InvertLabel;

   XtSetArg( args[n],  XmNallowResize, False ); n++;
   XtSetArg( args[n],  XmNwidth,   750 ); n++;
   XtSetArg( args[n],  XmNheight,  450 ); n++;
   XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_MODELESS ); n++;
   XtSetArg( args[n],  XmNtitle, "Trace Item Properties" ); n++;

   PropDialog::wPropDialog = XmCreateFormDialog( TracerApplication::GetApp()->MainWinForm,
       (char *) "PropDialog",args,n);

   strcpy(ColourSelection,"");
   strcpy(GC_Function,"");
   strcpy(SelectedStyle,"");

   /* Create the Previous filter text box */
   lb_Previous = XtVaCreateManagedWidget("Previous", xmLabelWidgetClass,
                                          wPropDialog,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   wPrevFileTextBox = XtVaCreateManagedWidget("wCreSetPrevTF",
                              xmTextFieldWidgetClass, wPropDialog,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_Previous,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_FORM,
                              XmNleftOffset,       10,
                              XmNrightAttachment,   XmATTACH_FORM,
                              XmNrightOffset,       10,
                              NULL);

   /* Create the Post filter text box */
   lb_Post = XtVaCreateManagedWidget("Post", xmLabelWidgetClass,
                                          wPropDialog,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget,        wPrevFileTextBox,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   wPostFileTextBox = XtVaCreateManagedWidget("wCreSetPostTF",
                              xmTextFieldWidgetClass, wPropDialog,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_Post,
                              XmNtopOffset,            5,
                              XmNleftAttachment,   XmATTACH_FORM,
                              XmNleftOffset,       10,
                              XmNrightAttachment,   XmATTACH_FORM,
                              XmNrightOffset,       10,
                              NULL);

   mu_color = CreateGenericColorPullDownMenu( wPropDialog,
                                              &PropDialog::cbColourChoice,
                                              this, 
                                        GetPositionOfColor(ParentItem->color) );


//   XtVaSetValues(mu_color,XmNbuttonSet,6,NULL);

   XtVaSetValues(mu_color,
                 XmNtopAttachment,    XmATTACH_WIDGET,
                 XmNtopWidget,        wPostFileTextBox,
                 XmNtopOffset,        5,
                 XmNleftAttachment,   XmATTACH_FORM,
                 XmNleftOffset,       5,
                 NULL );


   // Create the inverted toggle button
   str = XmStringCreateSimple((char *) "Trace Inversion:");
   InvertLabel = XtVaCreateManagedWidget("InvertLabel", xmLabelWidgetClass,
                                          wPropDialog,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget,        wPostFileTextBox,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_WIDGET,
                                          XmNleftWidget,       mu_color,
                                          XmNleftOffset,       20,
                                          NULL);

   XmStringFree(str);

   str = XmStringCreateSimple((char *) "Inverted Trace");

   wInvertedToggle = XtVaCreateManagedWidget("wInvertedToggle",
                            xmToggleButtonWidgetClass, wPropDialog,
                            XmNtopAttachment,    XmATTACH_WIDGET,
                            XmNtopWidget,        InvertLabel,
                            XmNtopOffset,        5,
                            XmNleftAttachment,   XmATTACH_WIDGET,
                            XmNleftWidget,       mu_color,
                            XmNleftOffset,       20,
                            XmNlabelType,        XmSTRING,
                            XmNlabelString,      str,
                            XmNset,              ParentItem->InvertedTrace,
                            NULL);

   XmStringFree(str);

   mu_style = CreateGenericGraphStylePullDownMenu( wPropDialog,
                                              &PropDialog::cbStyleChoice,
                                              this,
                                        int(ParentItem->GraphStyle) ,
                                        &wVertLineButton);

   XtVaSetValues(mu_style,
                 XmNtopAttachment,    XmATTACH_WIDGET,
                 XmNtopWidget,        mu_color,
                 XmNtopOffset,        5,
                 XmNleftAttachment,   XmATTACH_FORM,
                 XmNleftOffset,       5,
                 NULL );


   XGetGCValues(XtDisplay(ParentItem->wButton),
                ParentItem->traceGC,
                GCFunction,
                &values);

   mu_GC = CreateGCPullDownMenu( wPropDialog,
                                 (XtCallbackProc)&PropDialog::cbGCChoice,
                                 (XtPointer)this,
                                 values.function);

   XtVaSetValues(mu_GC,
                 XmNtopAttachment,    XmATTACH_WIDGET,
                 XmNtopWidget,        mu_style,
                 XmNtopOffset,        5,
                 XmNleftAttachment,   XmATTACH_FORM,
                 XmNleftOffset,       5,
                 NULL );


   // Create the Num hits data
   xmString = XmStringCreateSimple((char *) "Number Of Hits (Prev):");

   lb_PrevHits = XtVaCreateManagedWidget("lb_PrevHits", xmLabelWidgetClass,
                                          wPropDialog,
                                          XmNlabelString, xmString,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget ,       mu_GC,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   XmStringFree(xmString);

   tbPrevNumHits = XtVaCreateManagedWidget("tbPrevNumHits",
                              xmTextFieldWidgetClass, wPropDialog,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            5,
                              XmNcolumns,              5,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            mu_GC,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           lb_PrevHits,
                              XmNleftOffset,           10,
                              NULL);

   xmString = XmStringCreateSimple((char *) "Number Of Hits (Post):");

   lb_PostHits = XtVaCreateManagedWidget("lb_PostHits", xmLabelWidgetClass,
                                          wPropDialog,
                                          XmNlabelString, xmString,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget ,       lb_PrevHits,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   XmStringFree(xmString);

   tbPostNumHits = XtVaCreateManagedWidget("tbPostNumHits",
                              xmTextFieldWidgetClass, wPropDialog,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            5,
                              XmNcolumns,              5,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_PrevHits,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           lb_PostHits,
                              XmNleftOffset,           10,
                              NULL);

   n = 0;
   XtSetArg( args[n],  XmNleftAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNleftOffset,   0 ); n++;
   XtSetArg( args[n],  XmNrightAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNrightOffset,   0 ); n++;
   XtSetArg( args[n],  XmNbottomAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNbottomOffset,   45 ); n++;


   Sep1 =  XmCreateSeparatorGadget(wPropDialog,(char *) "Sep1",args,n);
                                  

   CancelButton = CreatePushButton(wPropDialog,(char *) "PropsCancelButton",
                          (XtCallbackProc)&PropDialog::cbCancelButton,
                          (XtPointer)this);

   xmString = XmStringCreateSimple((char *) "Cancel");

   XtVaSetValues(CancelButton,
                 XmNbottomAttachment,    XmATTACH_FORM,
                 XmNbottomOffset,        10,
                 XmNlabelString, xmString,
                 XmNrightAttachment,   XmATTACH_FORM,
                 XmNrightOffset,       10,
                 NULL);
   XmStringFree(xmString);

   // OK Button
   xmString = XmStringCreateSimple((char *) "OK");
   OKButton = CreatePushButton(wPropDialog,(char *) "PropsOKButton",
                      (XtCallbackProc)&PropDialog::cbPropDialogOKButtonPressed,
                      (XtPointer)this);

   XtVaSetValues(OKButton,
                 XmNbottomAttachment,    XmATTACH_FORM,
                 XmNlabelString, xmString,
                 XmNbottomOffset,        10,
                 XmNleftAttachment,   XmATTACH_FORM,
                 XmNleftOffset,       10,
                 NULL);
   XmStringFree(xmString);

   XtManageChild(mu_color);
   XtManageChild(mu_style);

   XtManageChild(Sep1);

   XtManageChild(mu_GC);

   XtManageChild(wPropDialog);
   UpdateDialog();

}

Widget PropDialog::CreateGCPullDownMenu(Widget parent,
                                        XtCallbackProc callback,
                                        XtPointer client_data,
                                        int SelectedValue)
{
  register int i;

  Arg args[20];
  Cardinal n = 0;
  Widget mu_GC, mu_GCOption;
  XmString str ;
  Widget wGCButtons[NUM_GC_FUNCTIONS];

  if (SelectedValue != -1)
  {
     XtSetArg(args[n],XmNbuttonSet,SelectedValue); n++;
  }
  else
  {
     XtSetArg(args[n],XmNbuttonSet,0); n++;
  }

  mu_GC = XmCreatePulldownMenu( parent, (char *) "mu_GC", args, 0 );
  str = XmStringCreateSimple((char *) "Graphic Context:");

  n = 0;
  XtSetArg( args[n],XmNsubMenuId, mu_GC ); n++;
  XtSetArg( args[n],XmNlabelString, str ); n++;
  XtSetArg( args[n],XmNorientation, XmVERTICAL ); n++;
  XtSetArg( args[n],XmNentryAlignment, XmALIGNMENT_BEGINNING ); n++;
  XtSetArg( args[n],XmNbuttonSet,5); n++;

  mu_GCOption = XmCreateOptionMenu( parent,(char *)  "GCoption", args, n );


  for (i = 0; i < NUM_GC_FUNCTIONS; i++)
  {
     wGCButtons[i] = CreatePushButton(mu_GC,
                                      GCStrings[i],
                                      (XtCallbackProc)callback,
                                      (XtPointer)client_data);
  }

  if (SelectedValue != -1)
  {
     XtVaSetValues(mu_GC,XmNmenuHistory,wGCButtons[SelectedValue],NULL);
  }

  return (mu_GCOption);
}

PropDialog::~PropDialog()
{


}

void PropDialog::UpdateDialog()
{
   char str[12];

   XmTextSetString(wPrevFileTextBox,ParentItem->relative->PrevFile->GetName());
   XmTextSetString(wPostFileTextBox,ParentItem->relative->PostFile->GetName());

   sprintf(str,"%d",ParentItem->relative->PrevFile->GetNumberOfHits());
   XmTextSetString(tbPrevNumHits,str);

   sprintf(str,"%d",ParentItem->relative->PostFile->GetNumberOfHits());

   XmTextSetString(tbPostNumHits,str);

   if (ParentItem->Parent->ContourEnabled == TRUE)
     XtSetSensitive(wVertLineButton,FALSE);
   else
     XtSetSensitive(wVertLineButton,TRUE);

}

void PropDialog::cbCancelButton(Widget w,
                                   XtPointer client_data, XtPointer call_data)
{
   PropDialog *Dialog = (PropDialog*) client_data;

//   XtDestroyWidget(Dialog->wPropDialog);
}

void PropDialog::cbPropDialogOKButtonPressed(Widget w,XtPointer client_data,
                                              XtPointer call_data)
{
   PropDialog *Dialog = (PropDialog*)client_data;

   if (Dialog != NULL)
   {
      Dialog->PropDialogOKButtonPressedCB(w, call_data);
   }
}

void PropDialog::PropDialogOKButtonPressedCB(Widget w,
                            XtPointer call_data)
{
   char *pColourSelection;
   char *pGCSelection;
   char *pStyleSelection;

   pColourSelection = (char*)(ColourSelection);
   pGCSelection = (char*)(GC_Function);
   pStyleSelection = (char*)(SelectedStyle);


   if (strcmp(pColourSelection,"") != 0)
     ParentItem->ResetColourMemebers(pColourSelection);

   if (strcmp(pGCSelection,"") != 0)
     ParentItem->ChangeGCFunction(pGCSelection);

   ParentItem->ResetInvertedMemebers(
                               XmToggleButtonGetState(wInvertedToggle));

   printf("About to do ChangeSelectedStyle\n");

   if (strcmp(pStyleSelection,"") != 0)
     ParentItem->ChangeSelectedStyle(pStyleSelection);

   printf("Done ChangeSelectedStyle\n");
   ParentItem->GetWindow()->SetMidTime(REFRESH);

//   XtDestroyWidget(Dialog->wPropDialog);
}

/*******************************************************************************
*
* Function: cbColourChoice
*
*
* Purpose:
*
*******************************************************************************/
void PropDialog::cbColourChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data )
{
   PropDialog *Dialog = (PropDialog*) client_data;
   XmString xmstring;
   char *color_ptr;

   XtVaGetValues(w,XmNlabelString,&xmstring,NULL);
   XmStringGetLtoR(xmstring,XmSTRING_DEFAULT_CHARSET,&color_ptr);


   /* Set the global colour value to the selected colour */
   strcpy(Dialog->ColourSelection,color_ptr);

   XtFree(color_ptr);

}

void PropDialog::cbGCChoice( Widget w,
                                 XtPointer client_data,
                                 XtPointer call_data)
{

   PropDialog *Dialog = (PropDialog*) client_data;
   XmString xmstring;
   char *GC_Function_ptr;

   XtVaGetValues(w,XmNlabelString,&xmstring,NULL);
   XmStringGetLtoR(xmstring,XmSTRING_DEFAULT_CHARSET,&GC_Function_ptr);


   /* Set the global colour value to the selected colour */
   strcpy(Dialog->GC_Function, GC_Function_ptr);

   XtFree(GC_Function_ptr);

}

void PropDialog::cbStyleChoice( Widget w,
                                 XtPointer client_data,
                                 XtPointer call_data)
{

   PropDialog *Dialog = (PropDialog*) client_data;
   XmString xmstring;
   char *SelectedStyle_ptr;

   XtVaGetValues(w,XmNlabelString,&xmstring,NULL);
   XmStringGetLtoR(xmstring,XmSTRING_DEFAULT_CHARSET,&SelectedStyle_ptr);

   printf("Selected style [%s]\n", SelectedStyle_ptr);

   /* Set the global colour value to the selected colour */
   strcpy(Dialog->SelectedStyle, SelectedStyle_ptr);

   XtFree(SelectedStyle_ptr);
}

