#!/bin/csh -fe

# This script changes the current pwd from an automounter-based root to an nfs one.
# This can be used in conjunction with MP_REMOTEDIR to make poe work.

# You must define environment variables appropriately.  Here is an example:
# pwd currently returns automounter form:  /a/bluematter000/var/local/suits/sandboxes
# and you want it to be the nfs version: /var/clust/0/suits/sandboxes
# you would define:  BG_BAD_FILESTEM /a/bluematter000/var/local
# and:               BG_GOOD_FILESTEM /var/clust/0
# Then pwd.csh will always give the nfs version of pwd.

# To make this work for MPI/POE, simply define MP_REMOTEDIR to be pwd.csh
  
set oldpwd = `pwd`

if ($?BG_BAD_FILESTEM && $?BG_GOOD_FILESTEM) then
  echo `echo $oldpwd | sed "s.$BG_BAD_FILESTEM.$BG_GOOD_FILESTEM."`
else
  echo $oldpwd
endif

