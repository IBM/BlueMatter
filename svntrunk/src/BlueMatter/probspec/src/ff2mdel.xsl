<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:lxslt="http://xml.apache.org/xslt"
                xmlns:redirect="org.apache.xalan.xslt.extensions.Redirect"
                extension-element-prefixes="redirect"
                version="1.0">
<xsl:output method="text"/>

<!-- command line invocation of xalan to process input file input.xml:
     java org.apache.xalan.xslt.Process -in input.xml -xsl ff2mdel.xsl -out foo.out
     Description:  takes an xml document in the format described by
                   ffparams.dtd and outputs a set of DEL formatted files
                   suitable for LOADing into db2 -->

  <xsl:template match="/ffparams">
<redirect:write file="siteinstance.del">
<!--***BEGIN SITEINSTANCE.DEL: site_id, site_type, mass, charge, epsilon, sigma***-->
<xsl:apply-templates select="sitelist"/>
<!--***END SITEINSTANCE.DEL***-->
</redirect:write>

<redirect:write file="bondinstance.del">
<!--***BEGIN BONDINSTANCE.DEL: bond_id, k, r0***-->
<xsl:apply-templates select="bondlist" mode="instance"/>
<!--***END BONDINSTANCE.DEL***-->
</redirect:write>

<redirect:write file="bondlist.del">
<!--***BEGIN BONDLIST.DEL: bond_id, site_id***-->
<xsl:apply-templates select="bondlist" mode="list"/>
<!--***END BONDLIST.DEL***-->
</redirect:write>

<redirect:write file="angleinstance.del">
<!--***BEGIN ANGLEINSTANCE.DEL: angle_id, k0, th0***-->
<xsl:apply-templates select="anglelist" mode="instance"/>
<!--***END ANGLEINSTANCE.DEL***-->
  </redirect:write>

<redirect:write file="anglelist.del">
<!--***BEGIN ANGLELIST.DEL: angle_id, site_id, site_ordinal***-->
<xsl:apply-templates select="anglelist" mode="list"/>
<!--***END ANGLELIST.DEL***-->
</redirect:write>

<redirect:write file="torsioninstance.del">
<!--***BEGIN TORSIONINSTANCE.DEL: torsion_id, mult, k, theta_0***-->
<xsl:apply-templates select="torsionlist" mode="instance"/>
<!--***END TORSIONINSTANCE.DEL***-->
</redirect:write>

<redirect:write file="torsionlist.del">
<!--***BEGIN TORSIONLIST.DEL: torsion_id, site_id, site_ordinal***-->
<xsl:apply-templates select="torsionlist" mode="list"/>
<!--***END TORSIONLIST.DEL***-->
</redirect:write>


<redirect:write file="improperinstance.del">
<!--***BEGIN IMPROPERINSTANCE.DEL: irregular_id, k, theta_0***-->
<xsl:apply-templates select="improperlist" mode="instance"/>
<!--***END IMPROPERINSTANCE.DEL-->
</redirect:write>

<redirect:write file="improperlist.del">
<!--***BEGIN IMPROPERLIST.DEL: irregular_id, site_id, site_ordinal ***-->
<xsl:apply-templates select="improperlist" mode="list"/>
<!--***END IMPROPERLIST.DEL***-->
</redirect:write>
</xsl:template>


  <xsl:template match="sitelist"><xsl:apply-templates/></xsl:template>
  <xsl:template match="bondlist" mode="instance"><xsl:apply-templates mode="instance"/></xsl:template>
  <xsl:template match="bondlist" mode="list"><xsl:apply-templates mode="list"/></xsl:template>
  <xsl:template match="anglelist" mode="instance"><xsl:apply-templates mode="instance"/></xsl:template>
  <xsl:template match="anglelist" mode="list"><xsl:apply-templates mode="list"/></xsl:template>

<xsl:template match="siterec">"<xsl:value-of select="siteid"/>", "<xsl:value-of select="sitetype"/>", <xsl:value-of select="mass"/>, <xsl:value-of select="charge"/>, <xsl:value-of select="epsilon"/>, <xsl:value-of select="sigma"/></xsl:template>

<xsl:template match="bondrec" mode="instance"><xsl:number/>, <xsl:value-of select="k"/>, <xsl:value-of select="r0"/></xsl:template>

<xsl:template match="bondrec" mode="list">
<xsl:number/>, "<xsl:value-of select="siteid[position()=1]"/>"
<xsl:number/>, "<xsl:value-of select="siteid[position()=2]"/>"</xsl:template>

<xsl:template match="anglerec" mode="instance">
<xsl:number/>, <xsl:value-of select="k"/>, <xsl:value-of select="th0"/></xsl:template>

<xsl:template match="anglerec" mode="list">
<xsl:number/>, "<xsl:value-of select="siteid[position()=1]"/>", 1
<xsl:number/>, "<xsl:value-of select="siteid[position()=2]"/>", 2
<xsl:number/>, "<xsl:value-of select="siteid[position()=3]"/>", 3</xsl:template>

<xsl:template match="charmmtorsionrec" mode="instance">
<xsl:number/>, <xsl:value-of select="k"/>, <xsl:value-of select="mult"/>, <xsl:value-of select="th0"/></xsl:template>

<xsl:template match="charmmtorsionrec" mode="list">
<xsl:number/>, "<xsl:value-of select="siteid[position()=1]"/>", 1
<xsl:number/>, "<xsl:value-of select="siteid[position()=2]"/>", 2
<xsl:number/>, "<xsl:value-of select="siteid[position()=3]"/>", 3
<xsl:number/>, "<xsl:value-of select="siteid[position()=4]"/>", 4</xsl:template>

<xsl:template match="charmmimproperrec" mode="instance">
<xsl:number/>, <xsl:value-of select="k"/>, <xsl:value-of select="th0"/></xsl:template>

<xsl:template match="charmmimproperrec" mode="list">
<xsl:number/>, "<xsl:value-of select="siteid[position()=1]"/>", 1
<xsl:number/>, "<xsl:value-of select="siteid[position()=2]"/>", 2
<xsl:number/>, "<xsl:value-of select="siteid[position()=3]"/>", 3
<xsl:number/>, "<xsl:value-of select="siteid[position()=4]"/>", 4</xsl:template>

</xsl:stylesheet>
