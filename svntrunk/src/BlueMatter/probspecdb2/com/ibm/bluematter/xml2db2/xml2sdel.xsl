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
***BEGIN GLOBAL_PARAMETER_TYPE.DEL
0, char
1, int
2, double
***END GLOBAL_PARAMETER_TYPE.DEL
<!-- ***END GLOBAL_PARAMETER_TYPE.DEL -->

<!--***BEGIN GLOBAL_PARAMETER.DEL: char param_id, int param_type, int symbolic_constant, char param_value***-->
***BEGIN GLOBAL_PARAMETER.DEL: char param_id, int param_type, int symbolic_constant, char param_value***
<xsl:apply-templates select="header_parameter_list"/>
***END GLOBAL_PARAMETER.DEL***
<!--***END GLOBALPARAMETER.DEL***-->

<!--***BEGIN CHAIN.DEL: sys_id, chain_id, chaind_desc ***-->
***BEGIN CHAIN.DEL***
<xsl:apply-templates select="data_list[@data_class_prepend='ch_']" mode="chain"/>
***END CHAIN.DEL***
<!--***END CHAIN.DEL***-->

<!--***BEGIN CHEM_COMP_INSTANCE.DEL: sys_id, cc_instance_id, component_name ***-->
***BEGIN CHEM_COMP_INSTANCE.DEL***
<xsl:apply-templates select="data_list[@data_class_prepend='cci_']" mode="cc_comp_instance"/>
***END CHEM_COMP_INSTANCE.DEL***
<!--***END CHEM_COMP_INSTANCE.DEL***-->


<!--***BEGIN MONOMER_INSTANCE.DEL: sys_id, chain_id, monomer_ordinal, cc_instance_id ***-->
***BEGIN MONOMER_INSTANCE.DEL***
<xsl:apply-templates select="data_list[@data_class_prepend='moni_']" mode="monomer_instance"/>
***END MONOMER_INSTANCE.DEL***
<!--***END MONOMER_INSTANCE.DEL***-->

<!--***BEGIN SPECIFIC_MOLECULE.DEL: sys_id, chain_id, monomer_ordinal, cc_instance_id ***-->
***BEGIN SPECIFIC_MOLECULE.DEL***
<xsl:apply-templates select="data_list[@data_class_prepend='mn_']" mode="specific_molecule"/>
***END SPECIFIC_MOLECULE.DEL***
<!--***END SPECIFIC_MOLECULE.DEL***-->

<!--***BEGIN MOLECULE_INSTANCE.DEL: sys_id, chain_id, monomer_ordinal, cc_instance_id ***-->
***BEGIN MOLECULE_INSTANCE.DEL***
<xsl:apply-templates select="data_list[@data_class_prepend='mi_']" mode="molecule_instance"/>
***END MOLECULE_INSTANCE.DEL***
<!--***END MOLECULE_INSTANCE.DEL***-->

<!--***BEGIN SITE.DEL: int site_id, int cc_inst_id, int mol_inst_id, int lj_type, int lj14_type, int atomic_number, char atomic_mass, char charge***-->
***BEGIN SITE.DEL: int site_id, int cc_inst_id, int mol_inst_id, int lj_type, int lj14_type, int atomic_number, char atomic_mass, char charge***
<xsl:apply-templates select="data_list[@data_class_prepend='site_']" mode="site"/>
***END SITE.DEL***
<!--***END SITE.DEL***-->


<!--***BEGIN SITE_PARAM_SPEC.DEL: int param_id, int type_id, char name***-->
***BEGIN SITE_PARAM_SPEC.DEL: int param_id, int type_id, char name***
<xsl:apply-templates select="data_list[@data_class_prepend='site_']" mode="site_param_spec"/>
***END SITE_PARAM_SPEC.DEL-->
<!--***END SITE_PARAM_SPEC.DEL-->

<!--***BEGIN BOND.DEL: int bond_id, int bond_type***-->
***BEGIN BOND.DEL: int bond_id, int bond_type***
<xsl:apply-templates select="data_list[@data_class_prepend='bond_']" mode="bond"/>
***END BOND.DEL***-->
<!--***END BOND.DEL***-->

<!-- ***BEGIN PARAMTUPLELIST.DEL: int param_tuple_list_id, char param_tuple_desc -->
***BEGIN PARAMTUPLELIST.DEL: int param_tuple_list_id, char param_tuple_desc
<xsl:apply-templates select="/ffparams" mode="paramTupleList"/>
***END PARAMTUPLELIST -->
<!-- ***END PARAMTUPLELIST.DEL -->

<!-- ***BEGIN PARAMTUPLESPEC.DEL: int param_tuple_list_id, int param_id, int param_type, char param_name -->
***BEGIN PARAMTUPLESPEC.DEL: int param_tuple_list_id, int param_id, int param_type, char param_name
<xsl:apply-templates select="/ffparams" mode="paramTupleSpec"/>
***END PARAMTUPLESPEC.DEL -->
<!-- ***END PARAMTUPLESPEC.DEL -->

<!-- ***BEGIN PARAMTUPLE.DEL: int param_tuple_list_id, int param_tuple_id -->
***BEGIN PARAMTUPLE.DEL: int param_tuple_list_id, int param_tuple_id
<xsl:apply-templates select="/ffparams" mode="paramTuple"/>
***END PARAMTUPLE.DEL
<!-- ***END PARAM_TUPLE.DEL -->

<!-- ***BEGIN PARAMTUPLEDATA.DEL: int param_tuple_list_id, int param_tuple_id, int param_id, char param_data -->
***BEGIN PARAMTUPLEDATA.DEL: int param_tuple_list_id, int param_tuple_id, int param_id, char param_data
<xsl:apply-templates select="/ffparams" mode="paramTupleData"/>
***END PARAMTUPLEDATA.DEL
<!-- ***END PARAMTUPLEDATA.DEL -->

<!-- ***BEGIN SITETUPLELIST.DEL: int site_tuple_list_id, char site_tuple_desc -->
***BEGIN SITETUPLELIST.DEL: int site_tuple_list_id, char site_tuple_desc
<xsl:apply-templates select="/ffparams" mode="siteTupleList"/>
***END SITETUPLELIST.DEL
<!-- ***END SITETUPLELIST.DEL -->

<!-- ***BEGIN SITETUPLE.DEL: int site_tuple_list_id, int site_tuple_id -->
***BEGIN SITETUPLE.DEL: int site_tuple_list_id, int site_tuple_id
<xsl:apply-templates select="/ffparams" mode="siteTuple"/>
***END SITETUPLE.DEL
<!-- ***END SITETUPLE.DEL -->

<!-- ***BEGIN SITETUPLEDATA.DEL: int site_tuple_list_id, int site_tuple_id, int site_ordinal, char site_data -->
***BEGIN SITETUPLEDATA.DEL: int site_tuple_list_id, int site_tuple_id, int site_ordinal, char site_data
<xsl:apply-templates select="/ffparams" mode="siteTupleData"/>
***END SITETUPLEDATA.DEL -->
<!-- ***END SITETUPLEDATA.DEL -->

<!-- ***BEGIN UDF_INVOCATION_TABLE.DEL: char udf_id, int site_tuple_list_id, int param_tuple_list -->
***BEGIN UDF_INVOCATION_TABLE.DEL: char udf_id, int site_tuple_list_id, int param_tuple_list
<xsl:apply-templates select="udf_invocation_table" mode="udfInvocationTable"/>
***END UDF_INVOCATION_TABLE.DEL
<!-- ***END UDF_INVOCATION_TABLE.DEL -->

</xsl:template>


<xsl:template match="header_parameter_list">
<xsl:for-each select="header_parameter">
<xsl:text>"</xsl:text><xsl:value-of select="@param_id"/><xsl:text>"</xsl:text>
<xsl:text>, </xsl:text>
<xsl:choose>
    <xsl:when test="@type='char'">0</xsl:when>
    <xsl:when test="@type='int'">1</xsl:when>
    <xsl:when test="@type='double'">2</xsl:when>
    <xsl:otherwise>-1</xsl:otherwise></xsl:choose>
<xsl:text>, </xsl:text>
<xsl:choose>
<xsl:when test="@symbolic_constant='true'">1</xsl:when>
<xsl:otherwise>0</xsl:otherwise></xsl:choose>
<xsl:text>, "</xsl:text> 
<xsl:value-of select="."/>
<xsl:text>"
</xsl:text> 
</xsl:for-each>
</xsl:template>

<xsl:template match="data_list[@data_class_prepend='site_']" mode="site">
<!--***BEGIN SITE.DEL: int site_id, int cc_inst_id, int mol_inst_id, int lj_type, int lj14_type, int atomic_number, double atomic_mass, double charge***-->
<xsl:variable name="param_prepend" select="data_parameter_list/@param_prepend"/>
<xsl:variable name="mass_pid" select="data_parameter_list/data_parameter_spec[.='mass']/@param_id"/>
<xsl:variable name="charge_pid" select="data_parameter_list/data_parameter_spec[.='charge']/@param_id"/>
<xsl:variable name="mol_inst_pid" select="data_parameter_list/data_parameter_spec[.='mol_instance_id']/@param_id"/>
<xsl:variable name="cc_inst_pid" select="data_parameter_list/data_parameter_spec[.='cc_instance_id']/@param_id"/>
<xsl:variable name="atom_tag_pid" select="data_parameter_list/data_parameter_spec[.='atom_tag']/@param_id"/>
<xsl:variable name="epsilon_pid" select="data_parameter_list/data_parameter_spec[.='epsilon']/@param_id"/>
<xsl:variable name="epsilon14_pid" select="data_parameter_list/data_parameter_spec[.='epsilon14']/@param_id"/>
<xsl:variable name="rmin_pid" select="data_parameter_list/data_parameter_spec[.='rmin']/@param_id"/>
<xsl:variable name="rmin14_pid" select="data_parameter_list/data_parameter_spec[.='rmin14']/@param_id"/>
<xsl:variable name="element_pid" select="data_parameter_list/data_parameter_spec[.='element']/@param_id"/>
<xsl:variable name="cci_pid" select="/ffparams/data_list[@data_class_prepend='cci_']/data_parameter_list/data_parameter_spec[.='cc_id']/@param_id"/>

<xsl:for-each select="./data_record">
<xsl:variable name="cc_pid" select="data_parameter[@param_ref=$cc_inst_pid]"/>
<xsl:value-of select="substring-after(@data_id, $site_pp)-1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="$cc_pid - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$mol_inst_pid] - 1"/>
<xsl:text>, "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$epsilon_pid]"/>
<xsl:text>", "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$rmin_pid]"/>
<xsl:text>", "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$epsilon14_pid]"/>
<xsl:text>", "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$rmin14_pid]"/>
<xsl:text>", "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$mass_pid]"/>
<xsl:text>" , "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$charge_pid]"/>
<xsl:text>" , "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$atom_tag_pid]"/>
<!-- <xsl:text>C</xsl:text> -->
<xsl:text>" , "</xsl:text>

<xsl:variable name="temp" select="concat('cci_', $cc_pid)"/>
<xsl:value-of select="id($temp)/data_parameter[@param_ref=$cci_pid]"/>

<xsl:text>" , "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$element_pid]"/>
<xsl:text>"
</xsl:text>
</xsl:for-each>
<!-- ***END SITE.DEL*** -->
</xsl:template>

<xsl:template match="data_list[@data_class_prepend='site_']" mode="site_param_spec">
<xsl:variable name="param_prepend" select="data_parameter_list/@param_prepend"/>
***BEGIN SITE_PARAM_SPEC.DEL: int param_id, int type_id, char name***
<xsl:for-each select="data_parameter_list/data_parameter_spec[not(@type='idref')]">
<xsl:value-of select="substring-after(@param_id,$param_prepend)"/>, <xsl:choose>
    <xsl:when test="@type='char'">0</xsl:when>
    <xsl:when test="@type='int'">1</xsl:when>
    <xsl:when test="@type='double'">2</xsl:when>
    <xsl:otherwise>-1</xsl:otherwise></xsl:choose>, <xsl:value-of select="."/>
<xsl:text>
</xsl:text>
</xsl:for-each>***END SITE_PARAM_SPEC.DEL
***BEGIN SITE_PARAM.DEL: int site_id, int param_id, char value
<xsl:for-each select="data_parameter_list/data_parameter_spec[not(@type='idref')]">
<xsl:variable name="param_id" select="@param_id"/>
<xsl:for-each select="../../data_record">
<xsl:value-of select="substring-after(@data_id, $site_pp)"/>, <xsl:value-of select="substring-after(data_parameter[@param_ref=$param_id]/@param_ref,$param_prepend)"/>, <xsl:value-of select="data_parameter[@param_ref=$param_id]"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:for-each>***END SITE_PARAM.DEL
</xsl:template>


<xsl:variable name="bond_pp" select="'bond_'"/>

<xsl:template match="data_list[@data_class_prepend='bond_']" mode="bond">
<xsl:variable name="bond_type_id" select="data_parameter_list/data_parameter_spec[.='type']/@param_id"/>
<xsl:variable name="siteId1_rpp" select="data_parameter_list/data_parameter_spec[.='site_ref1']/@ref_prepend"/>
<xsl:variable name="siteId2_rpp" select="data_parameter_list/data_parameter_spec[.='site_ref2']/@ref_prepend"/>
<xsl:variable name="siteId1_pp" select="data_parameter_list/data_parameter_spec[.='site_ref1']/@param_id"/>
<xsl:variable name="siteId2_pp" select="data_parameter_list/data_parameter_spec[.='site_ref2']/@param_id"/>

<xsl:for-each select="./data_record">
<xsl:value-of select="substring-after(@data_id, $bond_pp) - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="substring-after(data_parameter_ref[@param_ref=$siteId1_pp]/@ref_ptr, $siteId1_rpp) - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="substring-after(data_parameter_ref[@param_ref=$siteId2_pp]/@ref_ptr, $siteId1_rpp) - 1"/>
<xsl:text>, </xsl:text>
<xsl:choose>
<xsl:when test="data_parameter[@param_ref=$bond_type_id]='single'">0</xsl:when>
<xsl:when test="data_parameter[@param_ref=$bond_type_id]='double'">1</xsl:when>
<xsl:when test="data_parameter[@param_ref=$bond_type_id]='triple'">2</xsl:when>
<xsl:otherwise>-1</xsl:otherwise>
</xsl:choose>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="paramTupleList">
<xsl:text>-1 , "empty list"</xsl:text><xsl:text>
</xsl:text>
<xsl:for-each select="param_tuple_list">
<xsl:value-of select="position() - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="@id"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="paramTupleSpec">
<xsl:for-each select="param_tuple_list">
<xsl:variable name="param_prepend" select="param_spec_list/@param_prepend"/>
<xsl:variable name="out_position" select="position()"/>
<xsl:for-each select="param_spec_list/param_spec">
<xsl:value-of select="$out_position - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="substring-after(@param_id, $param_prepend) - 1"/>
<xsl:text>, </xsl:text>
<xsl:choose>
    <xsl:when test="@type='int'">0</xsl:when>
    <xsl:when test="@type='double'">1</xsl:when>
    <xsl:otherwise>-1</xsl:otherwise>
</xsl:choose>
<xsl:text>, </xsl:text>
<xsl:value-of select="."/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="paramTuple">
<xsl:for-each select="param_tuple_list">
<xsl:variable name="param_prepend" select="param_spec_list/@param_prepend"/>
<xsl:variable name="out_position" select="position()"/>
<xsl:for-each select="param_tuple">
<xsl:value-of select="$out_position - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="position() - 1"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="paramTupleData">
<xsl:for-each select="param_tuple_list">
<xsl:variable name="param_prepend" select="param_spec_list/@param_prepend"/>
<xsl:variable name="param_list_id" select="position() - 1"/>
<xsl:for-each select="param_tuple">
<xsl:variable name="param_tuple_id" select="position() - 1"/>
<xsl:for-each select="param">
<xsl:value-of select="$param_list_id"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="$param_tuple_id"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="position() - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="."/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:for-each>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="siteTupleList">
<xsl:for-each select="site_tuple_list">
<xsl:value-of select="position() - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="@id"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="siteTuple">
<xsl:for-each select="site_tuple_list">
<xsl:variable name="out_position" select="position()"/>
<xsl:for-each select="site_tuple">
<xsl:value-of select="$out_position - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="position() - 1"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:for-each>
</xsl:template>

<xsl:template match="/ffparams" mode="siteTupleData">
<xsl:for-each select="site_tuple_list">
<xsl:variable name="site_list_id" select="position() - 1"/>
<xsl:for-each select="site_tuple">
<xsl:variable name="site_tuple_id" select="position() - 1"/>
<xsl:for-each select="site">
<xsl:value-of select="$site_list_id"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="$site_tuple_id"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="@site_ordinal - 1"/>
<xsl:text>, </xsl:text>
<xsl:value-of select="substring-after(@site_ref, 'site_') - 1"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:for-each>
</xsl:for-each>
</xsl:template>

<xsl:template match="udf_invocation_table" mode="udfInvocationTable">
<xsl:for-each select="udf_invocation_rec">
<xsl:variable name="udf_ident" select="@udf_id"/>
<xsl:variable name="site_tuple_ref" select="@site_tuple_ref"/>
<xsl:variable name="param_tuple_ref" select="@param_tuple_ref"/>
<xsl:text>"</xsl:text>
<xsl:value-of select="$udf_ident"/>
<xsl:text>" , </xsl:text>
<xsl:for-each select="/ffparams/site_tuple_list">
<xsl:choose>
    <xsl:when test="@id=$site_tuple_ref"><xsl:value-of select="position() - 1"/></xsl:when>
</xsl:choose>
</xsl:for-each>
<xsl:text> , </xsl:text>
<xsl:choose>
<xsl:when test="not($param_tuple_ref)">-1</xsl:when>
<xsl:otherwise>
<xsl:for-each select="/ffparams/param_tuple_list">
<xsl:choose>
    <xsl:when test="@id=$param_tuple_ref"><xsl:value-of select="position() - 1"/></xsl:when>
</xsl:choose>
</xsl:for-each>
</xsl:otherwise>
</xsl:choose>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:template>


<xsl:template match="data_list[@data_class_prepend='ch_']" mode="chain">
<xsl:variable name="dc_prepend" select="@data_class_prepend"/>
<xsl:variable name="name_pid" select="data_parameter_list/data_parameter_spec[.='name']/@param_id"/>

<xsl:for-each select="./data_record">
<xsl:value-of select="substring-after(@data_id, $dc_prepend) - 1"/>
<xsl:text> , "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$name_pid]"/>
<xsl:text>"
</xsl:text>
</xsl:for-each>
</xsl:template>

<xsl:template match="data_list[@data_class_prepend='cci_']" mode="cc_comp_instance">
<xsl:variable name="dc_prepend" select="@data_class_prepend"/>
<xsl:variable name="cc_id_pid" select="data_parameter_list/data_parameter_spec[.='cc_id']/@param_id"/>

<xsl:for-each select="./data_record">
<xsl:value-of select="substring-after(@data_id, $dc_prepend) - 1"/>
<xsl:text> , "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$cc_id_pid]"/>
<xsl:text>"
</xsl:text>
</xsl:for-each>
</xsl:template>


<xsl:template match="data_list[@data_class_prepend='moni_']" mode="monomer_instance">
<xsl:variable name="chain_id_prepend"       select="data_parameter_list/data_parameter_spec[.='chain_id']/@ref_prepend"/>
<xsl:variable name="cc_instance_id_prepend" select="data_parameter_list/data_parameter_spec[.='cc_instance_id']/@ref_prepend"/>
<xsl:variable name="chain_id_pid"           select="data_parameter_list/data_parameter_spec[.='chain_id']/@param_id"/>
<xsl:variable name="cc_instance_id_pid"     select="data_parameter_list/data_parameter_spec[.='cc_instance_id']/@param_id"/>
<xsl:variable name="ordinal_prepend"        select="data_parameter_list/@param_prepend" />

<xsl:for-each select="data_record">
<xsl:value-of select="substring-after(data_parameter_ref[@param_ref=$chain_id_pid]/@ref_ptr, $chain_id_prepend) - 1"/>
<xsl:text> , </xsl:text>
<xsl:value-of select="substring-after(@data_id, 'moni_') - 1"/>
<xsl:text> , </xsl:text>
<xsl:value-of select="substring-after(data_parameter_ref[@param_ref=$cc_instance_id_pid]/@ref_ptr, $cc_instance_id_prepend) - 1"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:template>


<xsl:template match="data_list[@data_class_prepend='mn_']" mode="specific_molecule">
<xsl:variable name="name_pid" select="data_parameter_list/data_parameter_spec[.='name']/@param_id"/>
<xsl:variable name="class_pid" select="data_parameter_list/data_parameter_spec[.='type']/@param_id"/>
<xsl:variable name="dc_prepend" select="@data_class_prepend"/>

<xsl:for-each select="./data_record">
<xsl:value-of select="substring-after(@data_id, $dc_prepend) - 1"/>
<xsl:text> , "</xsl:text>
<xsl:value-of select="data_parameter[@param_ref=$name_pid]"/>
<xsl:text>" , "</xsl:text>
<xsl:variable name="class_string" select="data_parameter[@param_ref=$class_pid]"/>
<xsl:value-of select="translate($class_string, 'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
<xsl:text>"</xsl:text>
<xsl:text>,"null","null"
</xsl:text>
</xsl:for-each>
</xsl:template>

<xsl:template match="data_list[@data_class_prepend='mi_']" mode="molecule_instance">
<xsl:variable name="molecule_name_pid" select="data_parameter_list/data_parameter_spec[.='molecule_name']/@param_id"/>
<xsl:variable name="molecule_name_ref_pre" select="data_parameter_list/data_parameter_spec[.='molecule_name']/@ref_prepend"/>
<xsl:variable name="dc_prepend" select="@data_class_prepend"/>

<xsl:for-each select="./data_record">
<xsl:value-of select="substring-after(@data_id, $dc_prepend) - 1"/>
<xsl:text> , </xsl:text>
<xsl:value-of select="substring-after(data_parameter_ref[@param_ref=$molecule_name_pid]/@ref_ptr, $molecule_name_ref_pre) - 1"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>
