%GNUPLOT Metapost output: Tue Oct 14 02:11:41 2003


warningcheck:=0;
defaultmpt:=mpt:=4;
th:=.6;
%% Have nice sharp joins on our lines
linecap:=butt;
linejoin:=mitered;

def scalepen expr n = pickup pencircle scaled (n*th) enddef;
def ptsize expr n = mpt:=n*defaultmpt enddef;


textmag:= 1.000;
vardef makepic(expr str) =
  if picture str : str scaled textmag
  % otherwise a string
  else: str infont defaultfont scaled (defaultscale*textmag)
  fi
enddef;

def infontsize(expr str, size) =
  infont str scaled (size / fontsize str)
enddef;

% latex prologue to enable use of times-roman font and other facilities
verbatimtex
  \def\imag{i}
  %\documentclass[20pt,dvips,landscape]{foils}
  \documentclass{article}
  \usepackage[LY1]{fontenc}	% specify text font encoding
  %\usepackage{amsmath}
  %\usepackage[usenames,dvips]{color}
  \usepackage[expert]{lucidabr}
  \begin{document}
  etex;

%font changes
verbatimtex
\def\setfont#1#2{%.
  \font\gpfont=#1 at #2pt
\gpfont}
\setfont{lsr}{10.00}
etex

color currentcolor; currentcolor=black;
boolean colorlines,dashedlines;
colorlines:=true;
dashedlines:=true;

def _wc = withpen currentpen withcolor currentcolor enddef;
def _ac = addto currentpicture enddef;
def _sms = scaled mpt shifted enddef;
% drawing point-types
def gpdraw (expr n, x, y) =
  if n<0: _ac contour fullcircle _sms (x,y)
  elseif (n=1) or (n=3):
    _ac doublepath ptpath[n] _sms (x,y) _wc;
    _ac doublepath ptpath[n] rotated 90 _sms (x,y) _wc
  elseif n<6: _ac doublepath ptpath[n] _sms (x,y) _wc
  else: _ac contour ptpath[n] _sms (x,y) _wc
  fi
enddef;

% the point shapes
path ptpath[];
%diamond
ptpath0 = ptpath6 = (-1/2,0)--(0,-1/2)--(1/2,0)--(0,1/2)--cycle;
% plus sign
ptpath1 = (-1/2,0)--(1/2,0);
% square
ptpath2 = ptpath7 = (-1/2,-1/2)--(1/2,-1/2)--(1/2,1/2)--(-1/2,1/2)--cycle;
% cross
ptpath3 := (-1/2,-1/2)--(1/2,1/2);
% circle:
ptpath4 = ptpath8:= fullcircle;
% triangle
ptpath5 = ptpath9 := (0,1/2)--(-1/2,-1/2)--(1/2,-1/2)--cycle;

def linetype expr n =
  currentcolor:= if colorlines : col[n] else: black fi;
  if n = -1 :
      drawoptions(withcolor currentcolor withpen (currentpen scaled .5));
  elseif n < 1 :
    drawoptions(_wc);
  else :
    drawoptions( if dashedlines: dashed lt[n] fi _wc);
  fi
enddef;

% dash patterns
picture lt[];
lt1=dashpattern(on 2 off 2); % dashes
lt2=dashpattern(on 2 off 2 on 0.2 off 2); %dash-dot
lt3=lt1 scaled 1.414;
lt4=lt2 scaled 1.414;
lt5=lt1 scaled 2;
lt6:=lt2 scaled 2;
lt7=dashpattern(on 0.2 off 2); %dots

color col[],cyan, magenta, yellow;
cyan=blue+green; magenta=red+blue;yellow=green+red;
col[-2]:=col[-1]:=col0:=black;
col1:=red;
col2:=(.2,.2,1); %blue
col3:=(1,.66,0); %orange
col4:=.85*green;
col5:=.9*magenta;
col6:=0.85*cyan;
col7:=.85*yellow;

%placing text
picture GPtext;
def put_text(expr pic, x, y, r, j) =
  GPtext:=makepic(pic);
  GPtext:=GPtext shifted
    if j = 1: (-(ulcorner GPtext + llcorner GPtext)/2)
    elseif j = 2: (-center GPtext)
    else: (-(urcorner GPtext + lrcorner GPtext)/2)
    fi
    rotated if r > 0: 90 else: 0 fi;
  draw GPtext shifted (x,y)
enddef;

beginfig(0);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (161.1a,109.8b)--(177.7a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0.1 etex, 143.4a, 109.8b, 0, 3);
draw (161.1a,282.8b)--(169.4a,282.8b);
draw (1146.3a,282.8b)--(1138.0a,282.8b);
draw (161.1a,384.0b)--(169.4a,384.0b);
draw (1146.3a,384.0b)--(1138.0a,384.0b);
draw (161.1a,455.9b)--(169.4a,455.9b);
draw (1146.3a,455.9b)--(1138.0a,455.9b);
draw (161.1a,511.6b)--(169.4a,511.6b);
draw (1146.3a,511.6b)--(1138.0a,511.6b);
draw (161.1a,557.1b)--(169.4a,557.1b);
draw (1146.3a,557.1b)--(1138.0a,557.1b);
draw (161.1a,595.6b)--(169.4a,595.6b);
draw (1146.3a,595.6b)--(1138.0a,595.6b);
draw (161.1a,628.9b)--(169.4a,628.9b);
draw (1146.3a,628.9b)--(1138.0a,628.9b);
draw (161.1a,658.3b)--(169.4a,658.3b);
draw (1146.3a,658.3b)--(1138.0a,658.3b);
draw (161.1a,684.6b)--(177.7a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  1 etex, 143.4a, 684.6b, 0, 3);
draw (161.1a,109.8b)--(161.1a,126.4b);
draw (161.1a,684.6b)--(161.1a,668.0b);
put_text( btex  10 etex, 161.1a, 73.2b, 0, 2);
draw (309.4a,109.8b)--(309.4a,118.1b);
draw (309.4a,684.6b)--(309.4a,676.3b);
draw (396.1a,109.8b)--(396.1a,118.1b);
draw (396.1a,684.6b)--(396.1a,676.3b);
draw (457.7a,109.8b)--(457.7a,118.1b);
draw (457.7a,684.6b)--(457.7a,676.3b);
draw (505.4a,109.8b)--(505.4a,118.1b);
draw (505.4a,684.6b)--(505.4a,676.3b);
draw (544.4a,109.8b)--(544.4a,118.1b);
draw (544.4a,684.6b)--(544.4a,676.3b);
draw (577.4a,109.8b)--(577.4a,118.1b);
draw (577.4a,684.6b)--(577.4a,676.3b);
draw (606.0a,109.8b)--(606.0a,118.1b);
draw (606.0a,684.6b)--(606.0a,676.3b);
draw (631.2a,109.8b)--(631.2a,118.1b);
draw (631.2a,684.6b)--(631.2a,676.3b);
draw (653.7a,109.8b)--(653.7a,126.4b);
draw (653.7a,684.6b)--(653.7a,668.0b);
put_text( btex  100 etex, 653.7a, 73.2b, 0, 2);
draw (802.0a,109.8b)--(802.0a,118.1b);
draw (802.0a,684.6b)--(802.0a,676.3b);
draw (888.7a,109.8b)--(888.7a,118.1b);
draw (888.7a,684.6b)--(888.7a,676.3b);
draw (950.3a,109.8b)--(950.3a,118.1b);
draw (950.3a,684.6b)--(950.3a,676.3b);
draw (998.0a,109.8b)--(998.0a,118.1b);
draw (998.0a,684.6b)--(998.0a,676.3b);
draw (1037.0a,109.8b)--(1037.0a,118.1b);
draw (1037.0a,684.6b)--(1037.0a,676.3b);
draw (1070.0a,109.8b)--(1070.0a,118.1b);
draw (1070.0a,684.6b)--(1070.0a,676.3b);
draw (1098.6a,109.8b)--(1098.6a,118.1b);
draw (1098.6a,684.6b)--(1098.6a,676.3b);
draw (1123.8a,109.8b)--(1123.8a,118.1b);
draw (1123.8a,684.6b)--(1123.8a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  1000 etex, 1146.3a, 73.2b, 0, 2);
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(161.1a,684.6b)--(161.1a,109.8b);
put_text( btex Time/timestep (seconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Task Count etex, 653.7a, 18.3b, 0, 2);
linetype 0;
put_text( btex $\beta$-hairpin (5239 atoms), -O2 -qarch=440, $64^3$ FFT etex, 1005.8a, 649.7b, 0, 3);
gpdraw(0,409.9a,459.0b);
gpdraw(0,706.5a,131.3b);
gpdraw(0,854.8a,119.6b);
gpdraw(0,1003.1a,153.2b);
gpdraw(0,1067.2a,649.7b);
endfig;
end.
