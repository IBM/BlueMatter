#!/usr/bin/env python


"""
  Simple demo of the plotting environment
"""

from Tkinter import *
from plot import *
import Pmw
import sys

root = Tk()

gb = GraphBase(root, 500, 400, relief=SUNKEN, border=2)

# Here is where you would fill in data for the line, from a file or whatever
l  = GraphLine([(0,0),(1,1),(2,4)], 'stuff', smooth=0)

go = GraphObjects([l])
gb.pack(side=TOP, fill=BOTH, expand=YES)
gb.draw(go, 'automatic', 'automatic')

root.mainloop()
