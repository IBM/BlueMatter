create database mdsetup2
 automatic storage yes
  on /db2_mdsetup2
 autoconfigure using
  isolation UR
  mem_percent 25
  apply db and dbm;
