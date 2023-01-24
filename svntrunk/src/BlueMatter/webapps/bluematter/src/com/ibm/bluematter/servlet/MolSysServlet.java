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
   File: MolSysServlet.java
   Author: RSG
   Date: May 23, 2002
   Class: MolSysServlet
   Extends: HttpServlet
   Description: Dynamically serves up web pages representing
   data associated with molecular systems stored in db2
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;


public class MolSysServlet extends javax.servlet.http.HttpServlet
{
    // private data

    Connection d_conn;

    static
    {
	try
	    {
		//  register the driver with DriverManager 
		//  The newInstance() call is needed for the sample to work with 
		//  JDK 1.1.1 on OS/2, where the Class.forName() method does not 
		//  run the static initializer. For other JDKs, the newInstance 
		//  call can be omitted. 
		Class.forName("com.ibm.db2.jcc.DB2Driver");
	    }
	catch (Exception e)
	    {
		e.printStackTrace();
	    }
    }


    public void log(String msg)
    {
	System.err.println(msg);
    }

    public synchronized boolean getConnection()
    {
	try
	    {
		//		String dburl = getServletConfig().getInitParameter("dburl");
		if (d_conn == null)
		    {
			// Type 2 connection (requires shared library)
			d_conn = DriverManager.getConnection("jdbc:db2:mdsetup");
			// Type 4 connection ("pure java")
			//			d_conn = DriverManager.getConnection("jdbc:db2://pepsi.watson.ibm.com:60017/mdsetup2:user=mdlook;");
		    }
	    }
	catch (Exception e)
	    {
		log(e.toString());
		e.printStackTrace();
		d_conn = null;
		return(false);
	    }
	return(true);
    }

    public void init(ServletConfig config) throws ServletException
    {
	super.init(config);

	// get connection to database
	if (!getConnection())
	    {
		log("Unable to get connection to database during initialization");
		throw(new ServletException());
	    }
    }

    public void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException
    {
	PrintWriter	out;

	res.setContentType("text/html");
	out = res.getWriter();
	
	if (d_conn == null)
	    {
		if (!getConnection())
		    {
			out.println("Unable to get connection to database at this time<br>");
			out.println("This may be due to the database being offline for scheduled maintenance<br>");
			out.println("Please try to connect to the database from the command line to determine whether the problem is due to an offline backup or a real problem<br>");
			out.println("If it is due to an offline backup, please wait half an hour or so and try again<br>");
			return;
		    }
	    }
	try
	    {
		String mode = req.getParameter("mode");
		if (mode.equals("select_system"))
		    {
			MolSysQuery.execute(req, out, d_conn);
		    }
		else if (mode.equals("sys_view"))
		    {
			MolSysView.execute(req, out, d_conn);
		    }
		else if (mode.equals("udf_view"))
		    {
			MolUdfView.execute(req, out, d_conn);
		    }
		else if (mode.equals("site_view"))
		    {
			MolSiteView.execute(req, out, d_conn);
		    }
		else if (mode.equals("param_vocab_view"))
		    {
			ParamVocabView.execute(req, out, d_conn);
		    }
		else if (mode.equals("select_ctp"))
		    {
			CompileTimeQuery.execute(req, out, d_conn);
		    }
		else if (mode.equals("ctp_view"))
		    {
			CompileTimeTable.execute(req, out, d_conn);
		    }
	    }
	catch(SQLException ex)
	    {
		log(ex.toString());
		d_conn = null;
	    }
	catch(Exception ex)
	    {
		log(ex.toString());
		throw new ServletException();
	    }
    }

    public void doGet(HttpServletRequest req, HttpServletResponse res)
	throws ServletException, IOException
    {
	doPost(req, res);
    }

}

