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
 package com.ibm.bluematter.db2probspec;

public class UDFInvocation {
    
    private int udfCode;
    private String siteTupleList;
    private String paramTupleList;
    private String udfName;
    private int siteTupleCount;

    public UDFInvocation() {
	udfCode = -1;
	siteTupleList = "";
	paramTupleList = "";
	udfName = "";
    }

    public UDFInvocation(int code, String siteList, String paramList, UDF_RegistryIF udfRegistry, int siteTupleCount) {
	udfCode = code;
        this.siteTupleCount = siteTupleCount;
	siteTupleList = siteList;
	paramTupleList = paramList;
	
	try {
	    udfName = udfRegistry.GetUDFNameByCode(code);
	} catch (Exception e) {
	    System.out.println(e.getMessage());
	    e.printStackTrace();
	    System.exit(1);
	}
    }


    public void setUdfCode(int code) {
	udfCode = code;
    }

    public void setUdfName(String name) {
	udfName = name;
    }

    public void setParamTupleListName(String list) {
	paramTupleList = list;
    }

    public void setSiteTupleListName(String list) {
	siteTupleList = list;
    }

    public String getSiteTupleListName() {
	return siteTupleList;
    }

    public String getParamTupleListName() {
	return paramTupleList;
    }

    public int geUdfCode() {
	return udfCode;
    }

    public int getSiteTupleCount() {
	return siteTupleCount;
    }

    public String getUdfName() {
	return udfName;
    }
}
