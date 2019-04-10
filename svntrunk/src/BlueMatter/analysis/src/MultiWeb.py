#!/usr/bin/env python
import os
import sys
import glob
import shutil
import time

short = {'OuterTimeStepInPicoSeconds':'TStep', 'VelocityResampleTargetTemperature':'Temp',
         'NumberOfConstraints':'NConst','NumberOfOuterTimeSteps':'NSteps',
         'VelocityResamplePeriodInOTS':'ResamPer', 'LRFMethod':'LRF', 'Ensemble':'Ensem'}

localdir = os.getcwd()
localtime = time.asctime(time.localtime())
title = "%s %s" % (localtime, localdir)

f = open("RunListAll.txt", "r")
if not os.path.exists("web"):
  os.mkdir("web")
  os.chmod("web", 511)
h = open("web/index.html", "w")
h.write("""
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
<HEAD>
<TITLE>Results</TITLE>
</HEAD>
<BODY>
""")
h.write("<H1>%s</H1>\n" % title)
h.write("<br>\n")
h.write("<TABLE>\n")
h.write("<CAPTION><h2>%s</h2></CAPTION>\n" % 'Runs')

l = f.readline().strip()
h.write("<TR>\n")
for tok in l.split():
  hd = tok
  if tok in short.keys():
    hd = short[tok]
  h.write("<TH ALIGN=left>%s</TH>\n" % hd)
h.write("</TR>\n")

for l in f.readlines():
  dir = l.split()[0]
  po = os.popen("MDGraf.py %s" % dir)
  outlines = po.readlines()
  print outlines
  po.close()
  shutil.copyfile("%s/Run.png" % dir, "web/%s.png" % dir)
  h.write('<TR><TD ALIGN=left><A HREF="%s.png">%s</TD>\n' % (dir, dir))
  
  for tok in l.split()[1:]:
    h.write("<TD ALIGN=left>%s</TD>\n" % tok)
  h.write("</TR>\n")
h.write("</TABLE>")
f.close()

h.write("""
</BODY>
</HTML>
""")

h.close()

