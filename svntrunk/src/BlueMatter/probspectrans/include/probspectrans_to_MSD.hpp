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
 // ***********************************************************************
// File: probspectrans_to_msd.hpp
// Author: cwo 
// Date: July 13, 2006 
// Namespace: probspectrans
// Class: ProbSpecTransToMSD 
// Description:  Reads DB entry from a run of probspectrans and outputs
//               strings with array in MSD file format
// ***********************************************************************
#ifndef INCLUDE_PROBSPECTRANS_TO_MSD_HPP
#define INCLUDE_PROBSPECTRANS_TO_MSD_HPP

#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>
#include <db2/handleexception.hpp>

#define SQLCMD_BUFSZ 512
#define SQLVARCHAR_BUFSZ 255

#define STREAM_PRECISION 16

#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

//#define PROBSPECTRANSTOMSD_SINGLETON_CLASS

//#define TEST_PROBSPECDB2

namespace probspectrans 
{

  class ProbSpecTransToMSD 
  {
    private:

#ifdef PROBSPECTRANSTOMSD_SINGLETON_CLASS
    static ProbSpecTransToMSD *obj_instance; 
#endif
    db2::ConnHandle& db2conn;
    int pst_id;
    int sys_id;

    ProbSpecTransToMSD(std::string, int, int );
   
    public:
    static ProbSpecTransToMSD *instance(std::string, int, int );
    ~ProbSpecTransToMSD();

    void output_rigidsites_tables(std::ostream&);
    void output_siteid_mapping_tables(std::ostream&);
    void output_fragmentation_tables(std::ostream&);
    void output_siteinfo_tables(std::ostream&);
    void output_excludedsites_tables(std::ostream&);
    void output_pair14sites_tables(std::ostream&);
    void output_harmbondedsites_tables(std::ostream&);
    void output_harmanglesites_tables(std::ostream&);
    void output_ureybradleysites_tables(std::ostream&);
    void output_swopetorsionsites_tables(std::ostream&);
    void output_improperdihedralsites_tables(std::ostream&);
    void output_oplstorsionsites_tables(std::ostream&);
    void output_oplsimpropersites_tables(std::ostream&);
    void output_nsqsites_tables(std::ostream&);
    void output_watersites_tables(std::ostream&);
    
    void output_entire_MSD(std::ostream& file_ostream);

  };
}

#endif
