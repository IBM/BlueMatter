// template <class Element>
// class IACollection;
#include "Tgroup_save_dialog.hpp"

GroupSaveDialog::GroupSaveDialog()
{

}

GroupSaveDialog::GroupSaveDialog(TraceGroup *pParentGroup)
{

   Arg args[20];
   int n =0;
   ParentItem = pParentGroup;
   char *CurrentDirectory;

   XmString s, sDir, sFilter;
   char   *Initial_Filter = "*";

      CurrentDirectory = (char*)malloc(2048*(sizeof(char)));
      if (getcwd(CurrentDirectory,2048) == NULL)
      {
          printf("getcwd failed");
          sDir = XmStringCreateLocalized("~");
      }
      sDir = XmStringCreateLocalized(CurrentDirectory);

      free(CurrentDirectory);

   sFilter = XmStringCreateLocalized( Initial_Filter );

   s = XmStringCreateLocalized("Save Trace Set");

   XtSetArg(args[n],XmNallowResize, False ); n++;
   XtSetArg(args[n],XmNresizePolicy, XmRESIZE_NONE ); n++;
   XtSetArg(args[n],XmNwidth,   1000 ); n++;
   XtSetArg(args[n],XmNminWidth,   1000 ); n++;
   XtSetArg(args[n],XmNheight,  500 ); n++;
   XtSetArg(args[n],XmNminHeight, 500 ); n++;
   XtSetArg(args[n],XmNdialogTitle, s  ); n++;
   XtSetArg(args[n],XmNfileTypeMask,XmFILE_REGULAR); n++;
   XtSetArg(args[n],XmNdirectory,  sDir); n++;
   XtSetArg(args[n],XmNdirMask,  sFilter); n++;
   XtSetArg(args[n],XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;

   wGroupSaveDialog = XmCreateFileSelectionDialog(
                                                ParentItem->ParentWindow->form0,
                                                     "wGroupSaveDialog",args,n);


   /* Add callbacks to the main window */
   XtAddCallback(wGroupSaveDialog,XmNokCallback,
                 OKButtonCB,(XtPointer)this );
   XtAddCallback(wGroupSaveDialog,XmNcancelCallback,
                 CancelButtonCB,(XtPointer)this);


   XtManageChild(wGroupSaveDialog);
}

GroupSaveDialog::~GroupSaveDialog()
{
   XtDestroyWidget(wGroupSaveDialog);

}

void GroupSaveDialog::cbCancelButtonPressed(XtPointer call_data)
{
   delete this;
}

void GroupSaveDialog::CancelButtonCB(Widget w,
                                   XtPointer client_data, XtPointer call_data)
{
   GroupSaveDialog *Dialog = (GroupSaveDialog*) client_data;

   if (Dialog!= NULL)
   {
      Dialog->cbCancelButtonPressed(call_data);
   }
}

void GroupSaveDialog::cbOKButtonPressed(XtPointer call_data)
{
   XmFileSelectionBoxCallbackStruct *cbs =
               (XmFileSelectionBoxCallbackStruct*)call_data ;

   char FileOnly[1024];
   char *Filename;
 
   char DirName[2048];
   if (XmStringGetLtoR(cbs->value,XmFONTLIST_DEFAULT_TAG,&Filename))
   {
      // Check filename exists
      if (!*Filename)
      {
         printf("No Filename\n");
         XtFree(Filename);
      }
      else
      {

         GetPathFromFile(Filename,&(FileOnly[0]),&(DirName[0]));

         // Find where dir starts
         // there is a filename
         GroupFile NewFile(ParentItem->DirName,
                           ParentItem->GroupName,
                           ParentItem->PrevRegExp,
                           ParentItem->PostRegExp,
                           ParentItem->GroupColor,
                           ParentItem->GroupGraphStyle,
                           ParentItem->GroupYOffset,
                           ParentItem->TraceSeparation,
                           ParentItem->InvertedTrace,
                           ParentItem->ContourEnabled);


         if (!NewFile.Save(DirName,FileOnly))
         {
            printf("Failure saving file\n");
         }
      }
   }

   delete this;
}

void GroupSaveDialog::OKButtonCB(Widget w,
                                   XtPointer client_data, XtPointer call_data)
{
   GroupSaveDialog *Dialog = (GroupSaveDialog*) client_data;

   if (Dialog!= NULL)
   {
      Dialog->cbOKButtonPressed(call_data);
   }
}

WindowGroupSaveDialog::WindowGroupSaveDialog(TracerWindow *pParentWindow)
{

   Arg args[20];
   int n =0;
   ParentWindow = pParentWindow;
   char *CurrentDirectory;

   XmString s, sDir, sFilter;
   char   *Initial_Filter = "*";

      CurrentDirectory = (char*)malloc(2048*(sizeof(char)));
      if (getcwd(CurrentDirectory,2048) == NULL)
      {
          printf("getcwd failed");
          sDir = XmStringCreateLocalized("~");
      }
      sDir = XmStringCreateLocalized(CurrentDirectory);

      free(CurrentDirectory);

   sFilter = XmStringCreateLocalized( Initial_Filter );

   s = XmStringCreateLocalized("Save Trace Set");

   XtSetArg(args[n],XmNallowResize, False ); n++;
   XtSetArg(args[n],XmNresizePolicy, XmRESIZE_NONE ); n++;
   XtSetArg(args[n],XmNwidth,   1000 ); n++;
   XtSetArg(args[n],XmNminWidth,   1000 ); n++;
   XtSetArg(args[n],XmNheight,  500 ); n++;
   XtSetArg(args[n],XmNminHeight, 500 ); n++;
   XtSetArg(args[n],XmNdialogTitle, s  ); n++;
   XtSetArg(args[n],XmNfileTypeMask,XmFILE_REGULAR); n++;
   XtSetArg(args[n],XmNdirectory,  sDir); n++;
   XtSetArg(args[n],XmNdirMask,  sFilter); n++;
   XtSetArg(args[n],XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;

   wGroupSaveDialog = XmCreateFileSelectionDialog(ParentWindow->form0,
                                                     "wGroupSaveDialog",args,n);


   /* Add callbacks to the main window */
   XtAddCallback(wGroupSaveDialog,XmNokCallback,
                 OKButtonCB,(XtPointer)this );
   XtAddCallback(wGroupSaveDialog,XmNcancelCallback,
                 CancelButtonCB,(XtPointer)this);


   XtManageChild(wGroupSaveDialog);
}

WindowGroupSaveDialog::~WindowGroupSaveDialog()
{
   XtDestroyWidget(wGroupSaveDialog);
}


void WindowGroupSaveDialog::cbOKButtonPressed(XtPointer call_data)
{

   XmFileSelectionBoxCallbackStruct *cbs =
               (XmFileSelectionBoxCallbackStruct*)call_data ;

   char FileOnly[1024];
   char *Filename;

   char DirName[2048];

   if (XmStringGetLtoR(cbs->value,XmFONTLIST_DEFAULT_TAG,&Filename))
   {
      // Check filename exists
      if (!*Filename)
      {
         printf("No Filename\n");
         XtFree(Filename);
      }
      else
      {

         GetPathFromFile(Filename,&(FileOnly[0]),&(DirName[0]));

         // Find where dir starts
         // there is a filename

         // Iterate here
         // TraceGroupListType::Cursor cursor(ParentWindow->TraceListSeq);
         

         for ( int i=0; i < ParentWindow->TraceListSeq.size(); i++ )
         {

            GroupFile NewFile(
                 ParentWindow->TraceListSeq[ i ]->DirName,
                 ParentWindow->TraceListSeq[ i ]->GroupName,
                 ParentWindow->TraceListSeq[ i ]->PrevRegExp,
                 ParentWindow->TraceListSeq[ i ]->PostRegExp,
                 ParentWindow->TraceListSeq[ i ]->GroupColor,
                 ParentWindow->TraceListSeq[ i ]->GroupGraphStyle,
                 ParentWindow->TraceListSeq[ i ]->GroupYOffset,
                 ParentWindow->TraceListSeq[ i ]->TraceSeparation,
                 ParentWindow->TraceListSeq[ i ]->InvertedTrace,
                 ParentWindow->TraceListSeq[ i ]->ContourEnabled);

            if (!NewFile.Save(DirName,FileOnly))
            {
               printf("Failure saving file\n");
            }
         }
      }
   }

   delete this;
}


void WindowGroupSaveDialog::cbCancelButtonPressed(XtPointer call_data)
{
   delete this;
}

