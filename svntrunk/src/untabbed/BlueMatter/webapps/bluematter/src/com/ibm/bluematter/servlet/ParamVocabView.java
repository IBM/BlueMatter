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
   File: ParamVocabView.java
   Author: RSG
   Date: June 21, 2002
   Class: ParamVocabView
   Description: Displays the current vocabulary for parameters
   (compile-time and run-time) that can be specified by a user.
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class ParamVocabView
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
  out.println("<html>\n<head>\n");
  out.println("<title>Parameter Vocabulary</title>\n</head>\n<body><br><br>\n");

  String cmd = new String("select name, attribute, desc from experiment.vocab");
  out.println("<br><br>");
  try
      {
    out.println("<table align=center cellpadding=8>");
    out.println("<caption>Run-time and Compile-time Parameters: ");
    out.println(cmd);
    out.println("</caption>");
    out.println("<tr align=center><th>Name</th><th>Attribute</th><th>Description</th></tr>");
    Statement stmt = conn.createStatement();
    ResultSet rs = null;
    rs = stmt.executeQuery(cmd);
    while (rs.next())
        {
      out.println("<tr align=left>");
      String paramName = rs.getString(1);
      String paramAttribute = rs.getString(2);
      String paramDescription = rs.getString(3);
      out.println("<td>" + paramName + "</td>");
      out.println("<td>" + paramAttribute + "</td>");
      out.println("<td>" + paramDescription + "</td>");
      out.println("</tr>");
        }
    out.println("</table><br><br>");
    out.println("</body></html>");
  
      }
  catch (SQLException ex)
      {
    out.println("SQL Error processing query:<br>");
    out.println(ex.toString());
    throw(ex);
      }
  catch (Exception ex)
      {
    out.println("Other Error processing query:<br>");
    out.println(ex.toString());
    throw(ex);
      }
    }
}

