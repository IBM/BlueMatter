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
 package com.ibm.bluematter.xml2db2;

import java.sql.*;
import java.text.*;
import java.io.*;
import java.util.*;


class DB2TableNames {
    private Vector tableNames = null;
    public DB2TableNames() {
        // Order is very important
        tableNames = new Vector();
        tableNames.add(new String ("CHAIN"));
        tableNames.add(new String ("CHEM_COMP_INSTANCE"));
        tableNames.add(new String ("MONOMER_INSTANCE"));
        tableNames.add(new String ("SPECIFIC_MOLECULE"));
        tableNames.add(new String ("MOLECULE_INSTANCE"));
        tableNames.add(new String ("SITE"));
        tableNames.add(new String ("PARAMTUPLELIST"));
        tableNames.add(new String ("PARAMTUPLE"));
        tableNames.add(new String ("PARAMTUPLESPEC"));
        tableNames.add(new String ("PARAMTUPLEDATA"));
        tableNames.add(new String ("SITETUPLELIST"));
        tableNames.add(new String ("SITETUPLE"));
        tableNames.add(new String ("SITETUPLEDATA"));
        tableNames.add(new String ("BOND"));
        tableNames.add(new String ("UDF_INVOCATION_TABLE"));
        tableNames.add(new String ("GLOBAL_PARAMETER_TYPE"));
        tableNames.add(new String ("GLOBAL_PARAMETER"));
    }

    public String getNthName(int i) {
        return (String) tableNames.elementAt(i);
    }

    public int size() {
        return tableNames.size();
    }
}
