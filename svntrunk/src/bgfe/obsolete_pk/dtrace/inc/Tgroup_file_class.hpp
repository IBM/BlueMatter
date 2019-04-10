/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 #ifndef TGROUP_FILE_CLASS_HPP
#define TGROUP_FILE_CLASS_HPP

#include "Tinclude.hpp"

class GroupFile{
   struct GroupFileItem{
      char          DirName[1024];
      char          GroupName[33];
      char          GroupColor[33];
      entGraphStyle          GroupGraphStyle;
      char          PrevRegExp[1024];
      char          PostRegExp[1024];
      int           GroupYOffset;
      int           TraceSeparation;
      Boolean       Inverted;
      Boolean       ContourEnabled;
       };

   int NumTraceFiles ;
   GroupFileItem GroupFileList[1024];
   public:

      // Constructors
      GroupFile();
      // Create an instance loading from the specified file
      GroupFile(char* FileName);

      // Create and instantiate an instance
      GroupFile(char* pchDirectory,
                char* pchGroupName,
                char* pchPrevRegExp,
                char* pchPostRegExp,
                char* pchColor,
                entGraphStyle pchGraphStyle,
                int   Offset,
                int   Separation,
                Boolean Inverted,
                Boolean ContourEnabled);

      // Destructors
      ~GroupFile();

      int  GetNumberOfLoadedGroups();

      void Initialize();

      void PrintGroupFile();

      Boolean Save(char* DirName, char* FileName);
      Boolean Load(char* FileName);

      char* GetDirectory(int Position);
      void  SetDirectory(int Position, char* Value);

      int   GetTraceSeparation(int Position);
      void  SetTraceSeparation(int Position, int Value);

      int   GetGroupYOffset(int Position);
      void  SetGroupYOffset(int Position, int Value);

      Boolean   GetInverted(int Position);
      void  SetInverted(int Position, Boolean Value);

      Boolean   GetContourEnabled(int Position);
      void  SetContourEnabled(int Position, Boolean Value);

      char* GetGroupName(int Position);
      void  SetGroupName(int Position, char* Value);
 
      char* GetPrevRegExp(int Position);
      void  SetPrevRegExp(int Position, char* Value);

      char* GetPostRegExp(int Position);
      void  SetPostRegExp(int Position, char* Value);

      char* GetColor(int Position);
      void  SetColor(int Position, char* Value);

      entGraphStyle GetGraphStyle(int Position);
      void  SetGraphStyle(int Position, entGraphStyle Value);

};

#endif
