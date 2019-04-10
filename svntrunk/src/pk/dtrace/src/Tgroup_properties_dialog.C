// template <class Element>
// class IACollection;
#include <Tgroup_properties_dialog.hpp>

GroupPropDialog::GroupPropDialog(TraceGroup *pParentGroup)
{

   Arg args[20];
   int n =0;
   ParentItem = pParentGroup;
   Widget lb_Previous,lb_Post, Sep1, InvertLabel,wTraceFileLabel, ContourLabel;

   XmString xmString,str;
   
   XtSetArg( args[n],  XmNallowResize, False ); n++;
   XtSetArg( args[n],  XmNwidth,   600 ); n++;
   XtSetArg( args[n],  XmNheight,  300 ); n++;
   XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_MODELESS ); n++;
   XtSetArg( args[n],  XmNtitle, "Group Properties" ); n++;


   strcpy(ColourSelection,ParentItem->GroupColor);

   wGroupPropDialog = XmCreateFormDialog(ParentItem->ParentWindow->form0,
       (char *) "GroupPropDialog",args,n);


   /* Create the trace file title box */
   str = XmStringCreateSimple((char *) "Trace File:");
   wTraceFileLabel = XtVaCreateManagedWidget("wTraceFileLabel",
                                          xmLabelWidgetClass,
                                          wGroupPropDialog,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   XmStringFree(str);

   wTraceFileTextBox = XtVaCreateManagedWidget("wTraceFileTextBox",
                              xmTextFieldWidgetClass, wGroupPropDialog,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            wTraceFileLabel,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_FORM,
                              XmNleftOffset,       10,
                              XmNrightAttachment,   XmATTACH_FORM,
                              XmNrightOffset,       10,
                              NULL);


   /* Create the Previous filter text box */
   lb_Previous = XtVaCreateManagedWidget("Previous", xmLabelWidgetClass,
                                          wGroupPropDialog,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget,        wTraceFileTextBox,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   wPrevFileTextBox = XtVaCreateManagedWidget("wCreSetPrevTF",
                              xmTextFieldWidgetClass,  wGroupPropDialog,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_Previous,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_FORM,
                              XmNleftOffset,           10,
                              XmNrightAttachment,      XmATTACH_FORM,
                              XmNrightOffset,          10,
                              NULL);

   /* Create the Post filter text box */
   lb_Post = XtVaCreateManagedWidget("Post", xmLabelWidgetClass,
                                          wGroupPropDialog,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget,        wPrevFileTextBox,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       10,
                                          NULL);

   wPostFileTextBox = XtVaCreateManagedWidget("wCreSetPostTF",
                              xmTextFieldWidgetClass, wGroupPropDialog,
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

   /* Create the colour menu */

   mu_color = CreateGenericColorPullDownMenu( wGroupPropDialog,
                                   &GroupPropDialog::cbColourChoice,
                                   (XtPointer)this,
                                   GetPositionOfColor(ParentItem->GroupColor));

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
                                          wGroupPropDialog,
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
                            xmToggleButtonWidgetClass, wGroupPropDialog,
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


   // Create the contour toggle button
   str = XmStringCreateSimple((char *) "Contour Tracing:");
   ContourLabel = XtVaCreateManagedWidget("ContourLabel", xmLabelWidgetClass,
                                          wGroupPropDialog,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget,        wPostFileTextBox,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_WIDGET,
                                          XmNleftWidget,       wInvertedToggle,
                                          XmNleftOffset,       20,
                                          NULL);

   XmStringFree(str);

   str = XmStringCreateSimple((char *) "Contour Trace");

   wContourToggle = XtVaCreateManagedWidget("wContourToggle",
                            xmToggleButtonWidgetClass, wGroupPropDialog,
                            XmNtopAttachment,    XmATTACH_WIDGET,
                            XmNtopWidget,        ContourLabel,
                            XmNtopOffset,        5,
                            XmNleftAttachment,   XmATTACH_WIDGET,
                            XmNleftWidget,       wInvertedToggle,
                            XmNleftOffset,       20,
                            XmNlabelType,        XmSTRING,
                            XmNlabelString,      str,
                            XmNset,              ParentItem->ContourEnabled,
                            NULL);

   XmStringFree(str);

   // Create separator
   n = 0;
   XtSetArg( args[n],  XmNleftAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNleftOffset,   0 ); n++;
   XtSetArg( args[n],  XmNrightAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNrightOffset,   0 ); n++;
   XtSetArg( args[n],  XmNbottomAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNbottomOffset,   45 ); n++;

   Sep1 =  XmCreateSeparatorGadget(wGroupPropDialog,(char *) "Sep1",args,n);

   xmString = XmStringCreateSimple((char *) "Cancel");

   CancelButton = CreatePushButton(wGroupPropDialog,(char *) "PropsCancelButton",
                          &GroupPropDialog::cbCancelButton,
                          (XtPointer)this);

   XtVaSetValues(CancelButton,
                 XmNbottomAttachment,    XmATTACH_FORM,
                 XmNbottomOffset,        10,
                 XmNlabelString, xmString,
                 XmNrightAttachment,   XmATTACH_FORM,
                 XmNrightOffset,       10,
                 NULL);

   XmStringFree(xmString);

   OKButton = CreatePushButton(wGroupPropDialog,(char *) "PropsOKButton",
                          (XtCallbackProc)&GroupPropDialog::cbOKButton,
                          (XtPointer)this);

   xmString = XmStringCreateSimple((char *) "OK");

   XtVaSetValues(OKButton,
                 XmNbottomAttachment,    XmATTACH_FORM,
                 XmNbottomOffset,        10,
                 XmNlabelString, xmString,
                 XmNleftAttachment,   XmATTACH_FORM,
                 XmNleftOffset,       10,
                 NULL);
   XmStringFree(xmString);

   XtManageChild(Sep1);

   XtManageChild(mu_color);

   XtManageChild(wGroupPropDialog);

   UpdateDialog();
}

GroupPropDialog::~GroupPropDialog()
{

}

void GroupPropDialog::UpdateDialog()
{
    //   TraceListType::Cursor cursor(ParentItem->TraceList);
   Boolean ContourValid = TRUE;

   char  PrevRegExp[512];
   char  PostRegExp[512];
   sprintf(PrevRegExp,"%s",ParentItem->PrevRegExp);
   sprintf(PostRegExp,"%s",ParentItem->PostRegExp);

   XmTextSetString(wTraceFileTextBox,ParentItem->DirName);
   XmTextSetString(wPrevFileTextBox,PrevRegExp);
   XmTextSetString(wPostFileTextBox,PostRegExp);
   XmToggleButtonSetState(wInvertedToggle,ParentItem->InvertedTrace,FALSE);

//    for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
//    {
   for( int i = 0; i < ParentItem->TraceList.size(); i++ )
     {
     if( ParentItem->TraceList[ i ]->GraphStyle == VERT_LINE_STYLE)
       {
       ContourValid = FALSE;
       break;
       }
     }

   // Set sensitivity of the contour if valid
   XtSetSensitive(wContourToggle,ContourValid);
}

void GroupPropDialog::cbCancelButton(Widget w,
                                   XtPointer client_data, XtPointer call_data)
{
   GroupPropDialog *Dialog = (GroupPropDialog*) client_data;

   XtDestroyWidget(Dialog->wGroupPropDialog);
}

void GroupPropDialog::cbOKButton(Widget w,
                                   XtPointer client_data, XtPointer call_data)
{
   char *pColourSelection;

   GroupPropDialog *Dialog = (GroupPropDialog*) client_data;

   pColourSelection = (char*)(Dialog->ColourSelection);

//   Dialog->ParentItem->ChangeColor(pColourSelection);
   Dialog->ParentItem->ChangeProperties(pColourSelection,
                               XmToggleButtonGetState(Dialog->wInvertedToggle),
                               XmToggleButtonGetState(Dialog->wContourToggle));

   XtDestroyWidget(Dialog->wGroupPropDialog);
}

/*******************************************************************************
*
* Function: cbColourChoice
*
*
* Purpose:
*
*******************************************************************************/
void GroupPropDialog::cbColourChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data )
{
   GroupPropDialog *Dialog = (GroupPropDialog*) client_data;
   XmString xmstring;
   char *color_ptr;

   XtVaGetValues(w,XmNlabelString,&xmstring,NULL);
   XmStringGetLtoR(xmstring,XmSTRING_DEFAULT_CHARSET,&color_ptr);

   /* Set the global colour value to the selected colour */
   strcpy(Dialog->ColourSelection,color_ptr);

   XtFree(color_ptr);

}
