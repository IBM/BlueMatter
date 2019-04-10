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
   File: MolSiteView.java
   Author: RSG
   Date: June 2, 2002
   Class: MolSiteView
   Description: Processes queries to return site information in molecular systems
                stored in a relational database.
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class MolSiteView
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
  String sysIDs[] = req.getParameterValues("sys_id");
  if (sysIDs == null)
      {
    out.println("ERROR: No sys_id provided in query");
    return;
      }
  String sysID = sysIDs[0];

  out.println("<html>\n<head>\n");
  out.println("<title>Molecular System " + sysID +
        ": Sites</title>\n</head>\n<body><br><br>\n");
  out.println("<h1> Molecular System <a href=molsys?mode=sys_view&sys_id=" + sysID + ">" + sysID + "</a>: Sites</h1><br><br>\n");

  StringBuffer cmd = new StringBuffer(512);
  cmd.append("select site_id, element_symbol, component_name, atom_tag, cc_instance_id, mol_instance_id, atomic_mass, charge, epsilon, rmin, epsilon14, rmin14 from mdsystem.site where sys_id = ");
  cmd.append(sysID);

  Enumeration e = req.getParameterNames();
  while (e.hasMoreElements())
      {
    String name = (String)e.nextElement();
    if (name.equals("site_id"))
        {
      cmd = 
          assemblePredicate("site_id",
                req.getParameterValues("site_id"),
                cmd);
        }
    else if (name.equals("component_name"))
        {
      cmd = 
          assembleStringPredicate("component_name",
                req.getParameterValues("component_name"),
                cmd);
        }
    else if (name.equals("cc_instance_id"))
        {
      cmd = 
          assemblePredicate("cc_instance_id",
                req.getParameterValues("cc_instance_id"),
                cmd);
        }
    else if (name.equals("mol_instance_id"))
        {
      cmd = 
          assemblePredicate("mol_instance_id",
                req.getParameterValues("mol_instance_id"),
                cmd);
        }
    else if (name.equals("element_symbol"))
        {
      cmd = 
          assembleStringPredicate("element_symbol",
                req.getParameterValues("element_symbol"),
                cmd);
        }
    else if (name.equals("atom_tag"))
        {
      cmd = 
          assembleStringPredicate("atom_tag",
                req.getParameterValues("atom_tag"),
                cmd);
        }
    else if (name.equals("site_low"))
        {
      String siteLow = req.getParameterValues("site_low")[0];
      if (siteLow.length() != 0)
          {
        cmd.append(" and site_id >= " + siteLow);
          }
        }
    else if (name.equals("site_high"))
        {
      String siteHigh = req.getParameterValues("site_high")[0];
      if (siteHigh.length() != 0)
          {
        cmd.append(" and site_id <= " + siteHigh);
          }
        }
      }

  out.println("<br><br>");
  try
      {
    out.println("<table border align=center cellpadding=8>");
    out.println("<caption>Site Data: ");
    out.println(cmd);
    out.println("</caption>");
    out.println("<tr align=center><th>SiteID</th><th>Element</th><th>Chem. Comp.</th><th>Atom Tag</th><th>Chem. Comp. Inst.</th><th>Mol. Inst.</th><th>Mass</th><th>Charge</th><th>epsilon</th><th>Rmin</th><th>epsilon14</th><th>Rmin14</th></tr>");
    Statement stmt = conn.createStatement();
    ResultSet rs = null;
    rs = stmt.executeQuery(cmd.toString());
    while (rs.next())
        {
      out.println("<tr align=left>");
      String site_ID = rs.getString(1);
      out.println("<td><a name=site_" + site_ID + ">" + site_ID + "</a></td>");
      String elt = rs.getString(2);
      out.println("<td><a href=molsys?mode=site_view&sys_id=" + sysID + "&element_symbol=" + elt + ">" + elt + "</a></td>");
      String chemComp = rs.getString(3);
      out.println("<td><a href=molsys?mode=site_view&sys_id=" + sysID + "&component_name=" + chemComp + ">" + chemComp + "</a></td>");
      String atomTag = rs.getString(4);
      out.println("<td><a href=molsys?mode=site_view&sys_id=" + sysID + "&atom_tag=" + atomTag + ">" + atomTag + "</a></td>");
      Integer ccInst = new Integer(rs.getInt(5));
      out.println("<td><a href=molsys?mode=site_view&sys_id=" + sysID + "&cc_instance_id=" + ccInst + ">" + ccInst + "</a></td>");
      Integer molInst = new Integer(rs.getInt(6));
      out.println("<td><a href=molsys?mode=site_view&sys_id=" + sysID + "&mol_instance_id=" + molInst + ">" + molInst + "</a></td>");
      String mass = rs.getString(7);
      out.println("<td>" + mass + "</td>");
      String charge = rs.getString(8);
      out.println("<td>" + charge + "</td>");
      String eps = rs.getString(9);
      out.println("<td>" + eps + "</td>");
      String rmin = rs.getString(10);
      out.println("<td>" + rmin + "</td>");
      String eps14 = rs.getString(11);
      out.println("<td>" + eps14 + "</td>");
      String rmin14 = rs.getString(12);
      out.println("<td>" + rmin14 + "</td>");
      out.println("</tr>");
        }
    out.println("</table><br><br>");
      }
  catch (SQLException ex)
      {
    out.println("SQL Error processing query:<br>");
    out.println(ex.toString());
    throw ex;
      }
  catch (Exception ex)
      {
    out.println("Error processing query:<br>");
    out.println(ex.toString());
    //		ex.printStackTrace();
    //		throw new ServletException();
    throw ex;
      }
  out.println("</body></html>");
  
    }

    public final static StringBuffer assembleStringPredicate(String column,
                   String[] value,
                   StringBuffer pred)
    {
  if ((value != null) && (value[0].length() != 0))
      {
    pred.append(" and " + column + " in (");
    for (int i = 0; i < value.length; ++i)
        {
      if (i > 0)
          {
        pred.append(", ");
          }
      pred.append("\'");
      pred.append(value[i]);
      pred.append("\'");
        }
    pred.append(")");
      }
  
  return(pred);
    }

    public final static StringBuffer assemblePredicate(String column,
                   String[] value,
                   StringBuffer pred)
    {
  if ((value != null) && (value[0].length() != 0))
      {
    pred.append(" and " + column + " in (");
    for (int i = 0; i < value.length; ++i)
        {
      if (i > 0)
          {
        pred.append(", ");
          }
      pred.append(value[i]);
        }
    pred.append(")");
      }
  
  return(pred);
    }
}
