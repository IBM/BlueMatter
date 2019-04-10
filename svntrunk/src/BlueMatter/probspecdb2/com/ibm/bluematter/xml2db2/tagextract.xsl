<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:lxslt="http://xml.apache.org/xslt"
                xmlns:redirect="org.apache.xalan.xslt.extensions.Redirect"
                extension-element-prefixes="redirect"
                version="1.0">
<xsl:output method="text"/>

<!-- command line invocation of xalan to process input file input.xml:
     java org.apache.xalan.xslt.Process -in input.xml -xsl xml2del.xsl -out foo.out
     Description:  takes an xml document in the format described by
                   ffparams.dtd and outputs a set of DEL formatted files
                   suitable for LOADing into db2 -->

<xsl:variable name="site_pp" select="'site_'"/>

<xsl:template match="/ffparams">
<!-- ***BEGIN GLOBAL_PARAMETER_TYPE.DEL -->

<!--***BEGIN SITE.DEL: int site_id, int cc_inst_id, int mol_inst_id, int lj_type, int lj14_type, int atomic_number, char atomic_mass, char charge***-->
<xsl:apply-templates select="data_list[@data_class_prepend='site_']" mode="atom_tag"/>
<!--***END SITE.DEL***-->

</xsl:template>

<xsl:template match="data_list[@data_class_prepend='site_']" mode="atom_tag">
<!--***BEGIN SITE.DEL: int site_id, int cc_inst_id, int mol_inst_id, int lj_type, int lj14_type, int atomic_number, double atomic_mass, double charge***-->
<xsl:variable name="param_prepend" select="data_parameter_list/@param_prepend"/>
<xsl:variable name="atom_tag_pid" select="data_parameter_list/data_parameter_spec[.='atom_tag']/@param_id"/>
<xsl:variable name="cc_inst_pid" select="data_parameter_list/data_parameter_spec[.='cc_instance_id']/@param_id"/>
<xsl:variable name="cci_pid" select="/ffparams/data_list[@data_class_prepend='cci_']/data_parameter_list/data_parameter_spec[.='cc_id']/@param_id"/>

<xsl:for-each select="./data_record">
<xsl:variable name="cc_pid" select="data_parameter[@param_ref=$cc_inst_pid]"/>

<xsl:text>"</xsl:text>

<xsl:variable name="temp" select="concat('cci_', $cc_pid)"/>
<xsl:value-of select="id($temp)/data_parameter[@param_ref=$cci_pid]"/>

<xsl:text>", "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$atom_tag_pid]"/>
<!-- <xsl:text>C</xsl:text> -->
<xsl:text>"
</xsl:text>
</xsl:for-each>
<!-- ***END SITE.DEL*** -->
</xsl:template>
</xsl:stylesheet>
