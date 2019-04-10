#!/usr/bin/env python
import glob
import sys
import bgutils

if len(sys.argv) < 2:
    print sys.argv[0], " optSetting"
    sys.exit(-1)

optSetting = sys.argv[1]
print "installbase", bgutils.installbase()
#print "ffinfo", bgutils.ffinfo()

info = bgutils.PlatformInfo("mdsetup")
#print info.choices()

print "Implementation Parameters\n"
info.implParams(info.choices()[optSetting][0])

print "\n\nPlatform Parameters\n"
info.platformParams(info.choices()[optSetting][1])
