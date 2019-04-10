create database mdsetup2
 automatic storage yes
  on /db2_mdsetup2
 autoconfigure using
  mem_percent 100
  isolation UR
  apply db and dbm;
