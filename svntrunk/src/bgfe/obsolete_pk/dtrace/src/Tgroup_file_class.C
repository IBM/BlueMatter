// template <class Element>
// class IACollection;
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Tgroup_file_class.hpp"

GroupFile::GroupFile()
{
   Initialize();
}


GroupFile::GroupFile(char* FileName)
{
   Initialize();

   if (Load(FileName) != True)
   {
      printf("Error in GroupFile::GroupFile( %s)\n",
                                   FileName);
      Initialize();
   }
}

GroupFile::GroupFile(char* pchDirectory,
                char* pchGroupName,
                char* pchPrevRegExp,
                char* pchPostRegExp,
                char* pchColor,
                entGraphStyle enGraphStyle,
                int   Offset,
                int   Separation,
                Boolean Inverted,
                Boolean ContourEnabled)
{
   Initialize();
   (void)strcpy(GroupFileList[0].DirName,pchDirectory);
   (void)strcpy(GroupFileList[0].GroupName,pchGroupName);
   (void)strcpy(GroupFileList[0].PrevRegExp,pchPrevRegExp);
   (void)strcpy(GroupFileList[0].PostRegExp,pchPostRegExp);
   (void)strcpy(GroupFileList[0].GroupColor,pchColor);
   GroupFileList[0].GroupGraphStyle = enGraphStyle;
   GroupFileList[0].GroupYOffset = Offset;
   GroupFileList[0].TraceSeparation = Separation;
   GroupFileList[0].Inverted = Inverted;
   GroupFileList[0].ContourEnabled = ContourEnabled;

   NumTraceFiles = 1;
}

GroupFile::~GroupFile()
{

}


void GroupFile::Initialize()
{
   NumTraceFiles = 0;
}

Boolean GroupFile::Save(char* DirName, char* FileName)
{
   char FullName[2048];
   FILE *FileDes;
   int WriteResult;
   sprintf(FullName,"%s/%s",DirName,FileName);

   FileDes =fopen(FullName,"a");
   if (FileDes == NULL)
      return FALSE;

   WriteResult = fprintf(FileDes,"A::Directory  ::%s\n",
                                              GroupFileList[0].DirName);
   WriteResult = fprintf(FileDes,"B::GroupName  ::%s\n",
                                              GroupFileList[0].GroupName);
   WriteResult = fprintf(FileDes,"C::PrevRegExp ::%s\n",
                                              GroupFileList[0].PrevRegExp);
   WriteResult = fprintf(FileDes,"D::PostRegExp ::%s\n",
                                              GroupFileList[0].PostRegExp);
   WriteResult = fprintf(FileDes,"E::Colour     ::%s\n",
                                              GroupFileList[0].GroupColor);
   WriteResult = fprintf(FileDes,"F::Style      ::%d\n",
                                              GroupFileList[0].GroupGraphStyle);
   WriteResult = fprintf(FileDes,"G::Offset     ::%d\n",
                                              GroupFileList[0].GroupYOffset);
   WriteResult = fprintf(FileDes,"H::Separation ::%d\n",
                                              GroupFileList[0].TraceSeparation);
   WriteResult = fprintf(FileDes,"I::Inverted   ::%d\n",
                                              GroupFileList[0].Inverted);
   WriteResult = fprintf(FileDes,"J::Contour    ::%d\n",
                                              GroupFileList[0].ContourEnabled);

   WriteResult = fprintf(FileDes,"Z::End Of Data::\n");

   fclose(FileDes);

   return True;
}

void GroupFile::PrintGroupFile()
{
   printf("Printing Group Info...\n");
   printf("DirName = %s\n",GroupFileList[0].DirName);
   printf("GroupName = %s\n",GroupFileList[0].GroupName);
   printf("PrevRegExp = %s\n",GroupFileList[0].PrevRegExp);
   printf("PostRegExp = %s\n",GroupFileList[0].PostRegExp);
   printf("GroupColor = %s\n",GroupFileList[0].GroupColor);
   printf("GroupGraphStyle = %d\n",GroupFileList[0].GroupGraphStyle);

   printf("GroupYOffset = %d\n",GroupFileList[0].GroupYOffset);
   printf("TraceSeparation = %d\n",GroupFileList[0].TraceSeparation);
   printf("Inverted = %d\n",GroupFileList[0].Inverted);
   printf("Contour  = %d\n",GroupFileList[0].ContourEnabled);


}

Boolean GroupFile::Load(char* FileName)
{
   FILE *FileDes;
   char CurrentLine[512];
   char WorkingLine[512];

   FileDes = fopen(FileName,"r");

   NumTraceFiles = 0;

   while(fgets(&(CurrentLine[0]),512,FileDes) != NULL)
   {

      if ((CurrentLine[1]  != ':') ||
          (CurrentLine[2]  != ':') ||
          (CurrentLine[14] != ':') ||
          (CurrentLine[15] != ':'))
      {
         printf("Incorrect format\n");
         fclose(FileDes);

         return FALSE;
      }

      strcpy(WorkingLine,&(CurrentLine[16]));
      WorkingLine[strlen(WorkingLine)-1] = '\0';

      // find start of data
      switch (CurrentLine[0])
      {
         case 'A':
           strcpy(GroupFileList[NumTraceFiles].DirName,WorkingLine);
           break;

         case 'B':
           strcpy(GroupFileList[NumTraceFiles].GroupName,WorkingLine);
           break;

         case 'C':
           strcpy(GroupFileList[NumTraceFiles].PrevRegExp,WorkingLine);
           break;

         case 'D':
           strcpy(GroupFileList[NumTraceFiles].PostRegExp,WorkingLine);
           break;

         case 'E':
           strcpy(GroupFileList[NumTraceFiles].GroupColor,WorkingLine);
           break;

         case 'F':
           GroupFileList[NumTraceFiles].GroupGraphStyle = 
                                               (entGraphStyle)atoi(WorkingLine);
           break;

         case 'G':
           GroupFileList[NumTraceFiles].GroupYOffset = atoi(WorkingLine);
           break;

         case 'H':
           GroupFileList[NumTraceFiles].TraceSeparation = atoi(WorkingLine);
           break;

         case 'I':
           GroupFileList[NumTraceFiles].Inverted = atoi(WorkingLine);
           break;

         case 'J':
           GroupFileList[NumTraceFiles].ContourEnabled = atoi(WorkingLine);
           break;

         case 'Z':
           NumTraceFiles++;
           break;

         default:
            printf("Unknown designator\n");
            return FALSE;

      }
   }
   fclose(FileDes);

   return True;
}

char* GroupFile::GetDirectory(int Position)
{
   char* pGroupName = GroupFileList[Position].DirName;
   return pGroupName;
}

void  GroupFile::SetDirectory(int Position,char* Value)
{
   (void)strcpy(GroupFileList[Position].DirName,Value);
}

int   GroupFile::GetTraceSeparation(int Position)
{
   return GroupFileList[Position].TraceSeparation;
}

void GroupFile::SetTraceSeparation(int Position,int Value)
{
   GroupFileList[Position].TraceSeparation = Value;
}

int   GroupFile::GetGroupYOffset(int Position)
{
   return GroupFileList[Position].GroupYOffset;
}

void  GroupFile::SetGroupYOffset(int Position,int Value)
{
   GroupFileList[Position].GroupYOffset = Value;
}

Boolean   GroupFile::GetInverted(int Position)
{
   return GroupFileList[Position].Inverted;
}

void  GroupFile::SetInverted(int Position,Boolean Value)
{
   GroupFileList[Position].Inverted = Value;
}

Boolean   GroupFile::GetContourEnabled(int Position)
{
   return GroupFileList[Position].ContourEnabled;
}

void  GroupFile::SetContourEnabled(int Position,Boolean Value)
{
   GroupFileList[Position].ContourEnabled = Value;
}

char* GroupFile::GetGroupName(int Position)
{
   char* pGroupName = GroupFileList[Position].GroupName;
   return pGroupName;
}

void  GroupFile::SetGroupName(int Position,char* Value)
{
   (void)strcpy(GroupFileList[Position].GroupName,Value);
}
 
char* GroupFile::GetPrevRegExp(int Position)
{
   char* pString = GroupFileList[Position].PrevRegExp;
   return pString;

}

void  GroupFile::SetPrevRegExp(int Position, char* Value)
{
   (void)strcpy(GroupFileList[Position].PrevRegExp,Value);
}

char* GroupFile::GetPostRegExp(int Position)
{
   char* pString = GroupFileList[Position].PostRegExp;
   return pString;
}

void  GroupFile::SetPostRegExp(int Position,char* Value)
{
   (void)strcpy(GroupFileList[Position].PostRegExp,Value);
}

char* GroupFile::GetColor(int Position)
{
   char* pString = GroupFileList[Position].GroupColor;
   return pString;
}

void  GroupFile::SetColor(int Position,char* Value)
{
   (void)strcpy(GroupFileList[Position].GroupColor,Value);
}

entGraphStyle GroupFile::GetGraphStyle(int Position)
{
   return GroupFileList[Position].GroupGraphStyle;
}

void  GroupFile::SetGraphStyle(int Position,entGraphStyle Value)
{
   GroupFileList[Position].GroupGraphStyle = Value;
}

int  GroupFile::GetNumberOfLoadedGroups()
{
   return NumTraceFiles;
}

