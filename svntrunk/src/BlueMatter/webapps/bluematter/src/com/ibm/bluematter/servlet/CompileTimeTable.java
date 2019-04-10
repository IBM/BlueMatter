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
 // ****************************************************************
/**
   File: CompileTimeTable.java
   Author: RSG
   Date: June 23, 2002
   Class: CompileTimeTable
   Description: Displays the list of compile-time parameter sets
                recorded in the system for a specified ctp_id
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class CompileTimeTable
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
	String ctpID = req.getParameterValues("ctp_id")[0];

	out.println("<html>\n<head>");
	out.println("<title>Query Results</title>\n</head>\n<body><br><br>");
	out.println("<h1>Compile Time Parameters for ctp_id = " + ctpID + "</h1>");

	String cmd = new String("select name, type, value from experiment.ct_params where ctp_id=" + ctpID);

	out.println("<br><br>");
	out.println("<table align=center border cellpadding=8>");
	out.println("<caption>Results from query: ");
	out.println(cmd);
	out.println("</caption>");
	out.println("<tr align=center><th>Name</th><th>Type</th><th>Value</th></tr>");
	try
	    {
		Statement stmt = conn.createStatement();
		ResultSet rs = null;
		rs = stmt.executeQuery(cmd);
		while (rs.next())
		    {
			out.println("<tr align=left>");
			String ctpName = rs.getString(1);
			String ctpType = rs.getString(2);
			String ctpValue = rs.getString(3);

			out.println("<td>" + ctpName + "</td>");
			out.println("<td>" + ctpType + "</td>");
			out.println("<td>" + ctpValue + "</td>");
			out.println("</tr>");
		    }
	    }
	catch (Exception ex)
	    {
		out.println("Error processing query:<br>");
		out.println(ex.toString());
		throw ex;
	    }
	out.println("</table></body></html>");
	
    }
}
