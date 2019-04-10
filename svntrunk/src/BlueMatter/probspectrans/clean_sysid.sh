#!/bin/sh
##############################################################################
# Cleans out probspectrans entries in DB2 for a specific system_id
#
# Usage: clean_sysid <pst_id>
##############################################################################


db2 connect to mdtest

db2 delete from mdsystem.PSTID_to_exp where pst_id=$1;
db2 delete from mdsystem.online_parameter where pst_id=$1;
db2 delete from mdsystem.rigid2_sites where pst_id=$1;
db2 delete from mdsystem.rigid3_sites where pst_id=$1;
db2 delete from mdsystem.rigid4_sites where pst_id=$1;
db2 delete from mdsystem.fragment_type where pst_id=$1;
db2 delete from mdsystem.fragment_map where pst_id=$1;
db2 delete from mdsystem.internal_external_siteids where pst_id=$1;
db2 delete from mdsystem.stdharmonicbondforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.stdharmonicangleforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.ureybradleyforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.swopetorsionforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.improperdihedralforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.oplstorsionforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.oplsimproperforce_tuplelist where pst_id=$1;
db2 delete from mdsystem.siteinfo where pst_id=$1;
db2 delete from mdsystem.site_to_siteinfo where pst_id=$1;
db2 delete from mdsystem.nsq_siteinfo where pst_id=$1;
db2 delete from mdsystem.nsq_site_to_nsq_siteinfo where pst_id=$1;
db2 delete from mdsystem.water3_sites where pst_id=$1;
db2 delete from mdsystem.exclusion_sites where pst_id=$1;
db2 delete from mdsystem.pair14_sites where pst_id=$1;
db2 commit;

db2 terminate
