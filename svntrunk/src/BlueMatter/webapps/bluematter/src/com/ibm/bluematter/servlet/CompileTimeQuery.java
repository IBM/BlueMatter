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
   File: CompileTimeQuery.java
   Author: RSG
   Date: June 23, 2002
   Class: CompileTimeTable
   Description: Displays the list of compile-time parameter sets
                recorded in the system according to query parameters
		provided by a POST operation.
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class CompileTimeQuery
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
	StringBuffer pred = new StringBuffer(128);
	int predCount = 0;
	Enumeration e = req.getParameterNames();
	while (e.hasMoreElements())
	    {
		String name = (String)e.nextElement();
		String[] value = req.getParameterValues(name);
		if (value[0].length() != 0)
		    {

			if (name.equals("ctp_id"))
			    {
				++predCount;
				pred.append(" and experiment.ctp_table.ctp_id = ");
				pred.append(value[0]);
			    }
			else if (name.equals("desc"))
			    {
				++predCount;
				pred.append(" and experiment.ctp_table.desc = \'");
				pred.append(value[0]);
				pred.append("\'");
			    }
			else if (name.equals("creator"))
			    {
				++predCount;
				pred.append(" and experiment.ctp_table.creator = \'");
				pred.append(value[0]);
				pred.append("\'");
			    }
			else if (name.equals("created_after"))
			    {
				++predCount;
				pred.append(" and experiment.ctp_table.created > timestamp_iso(\'");
				pred.append(value[0]);
				pred.append("\')");
			    }
			else if (name.equals("created_before"))
			    {
				++predCount;
				pred.append(" and experiment.ctp_table.created < timestamp_iso(\'");
				pred.append(value[0]);
				pred.append("\')");
			    }
			else if (name.equals("file_name"))
			    {
				++predCount;
				pred.append(" and filename = \'");
				pred.append(value[0].toUpperCase());
				pred.append("\'");
			    }
			else if (name.equals("desc"))
			    {
				++predCount;
				pred.append(" and desc = \'");
				pred.append(value[0].toUpperCase());
				pred.append("\'");
			    }

		    }
	    }

	out.println("<html>\n<head>");
	out.println("<title>Query Results</title>\n</head>\n<body><br><br>");
	out.println("<h1>Query Results</h1>");

	String cmd = new String("select ctp_id, filename, desc, creator, created from experiment.ctp_table where ctp_id > -1 " + pred.toString());

	out.println("<br><br>");
	out.println("<table align=center border cellpadding=8>");
	out.println("<caption>Results from query: ");
	out.println(cmd);
	out.println("</caption>");
	out.println("<tr align=center><th>CTP ID</th><th>File Name</th><th>Descrition</th><th>Creator</th><th>Created</th></tr>");
	try
	    {
		Statement stmt = conn.createStatement();
		ResultSet rs = null;
		rs = stmt.executeQuery(cmd);
		while (rs.next())
		    {
			out.println("<tr align=left>");
			Integer ctpID = new Integer(rs.getInt(1));
			String fileName = rs.getString(2);
			String ctpDesc = rs.getString(3);
			String creator = rs.getString(4);
			Timestamp created = rs.getTimestamp(5);
			out.println("<td><a href=molsys?mode=ctp_view&ctp_id=" + ctpID.toString() + ">" + ctpID.toString() + "</a></td>");
			out.println("<td>" + fileName + "</td>");
			out.println("<td>" + ctpDesc + "</td>");
			out.println("<td>" + creator + "</td>");
			out.println("<td>" + created + "</td>");
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
