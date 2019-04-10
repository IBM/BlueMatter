%GNUPLOT Metapost output: Thu Apr 10 15:40:51 2003


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
  %\usepackage[LY1]{fontenc}	% specify text font encoding
  %\usepackage{amsmath}
  %\usepackage[usenames,dvips]{color}
  \usepackage{times}
  \begin{document}
  etex;

color currentcolor; currentcolor=black;
boolean colorlines,dashedlines;
colorlines:=false;
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

beginfig(2);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (261.6a,145.2b)--(278.2a,145.2b);
draw (1178.4a,145.2b)--(1161.8a,145.2b);
put_text( btex  0 etex, 243.9a, 145.2b, 0, 3);
draw (261.6a,197.0b)--(278.2a,197.0b);
draw (1178.4a,197.0b)--(1161.8a,197.0b);
put_text( btex  2e-08 etex, 243.9a, 197.0b, 0, 3);
draw (261.6a,248.8b)--(278.2a,248.8b);
draw (1178.4a,248.8b)--(1161.8a,248.8b);
put_text( btex  4e-08 etex, 243.9a, 248.8b, 0, 3);
draw (261.6a,300.6b)--(278.2a,300.6b);
draw (1178.4a,300.6b)--(1161.8a,300.6b);
put_text( btex  6e-08 etex, 243.9a, 300.6b, 0, 3);
draw (261.6a,352.4b)--(278.2a,352.4b);
draw (1178.4a,352.4b)--(1161.8a,352.4b);
put_text( btex  8e-08 etex, 243.9a, 352.4b, 0, 3);
draw (261.6a,404.2b)--(278.2a,404.2b);
draw (1178.4a,404.2b)--(1161.8a,404.2b);
put_text( btex  1e-07 etex, 243.9a, 404.2b, 0, 3);
draw (261.6a,456.0b)--(278.2a,456.0b);
draw (1178.4a,456.0b)--(1161.8a,456.0b);
put_text( btex  1.2e-07 etex, 243.9a, 456.0b, 0, 3);
draw (261.6a,507.8b)--(278.2a,507.8b);
draw (1178.4a,507.8b)--(1161.8a,507.8b);
put_text( btex  1.4e-07 etex, 243.9a, 507.8b, 0, 3);
draw (261.6a,559.6b)--(278.2a,559.6b);
draw (1178.4a,559.6b)--(1161.8a,559.6b);
put_text( btex  1.6e-07 etex, 243.9a, 559.6b, 0, 3);
draw (261.6a,611.4b)--(278.2a,611.4b);
draw (1178.4a,611.4b)--(1161.8a,611.4b);
put_text( btex  1.8e-07 etex, 243.9a, 611.4b, 0, 3);
draw (261.6a,145.2b)--(261.6a,161.8b);
draw (261.6a,611.4b)--(261.6a,594.8b);
put_text( btex  0 etex, 261.6a, 108.6b, 0, 2);
draw (353.3a,145.2b)--(353.3a,161.8b);
draw (353.3a,611.4b)--(353.3a,594.8b);
put_text( btex  500 etex, 353.3a, 108.6b, 0, 2);
draw (445.0a,145.2b)--(445.0a,161.8b);
draw (445.0a,611.4b)--(445.0a,594.8b);
put_text( btex  1000 etex, 445.0a, 108.6b, 0, 2);
draw (536.6a,145.2b)--(536.6a,161.8b);
draw (536.6a,611.4b)--(536.6a,594.8b);
put_text( btex  1500 etex, 536.6a, 108.6b, 0, 2);
draw (628.3a,145.2b)--(628.3a,161.8b);
draw (628.3a,611.4b)--(628.3a,594.8b);
put_text( btex  2000 etex, 628.3a, 108.6b, 0, 2);
draw (720.0a,145.2b)--(720.0a,161.8b);
draw (720.0a,611.4b)--(720.0a,594.8b);
put_text( btex  2500 etex, 720.0a, 108.6b, 0, 2);
draw (811.7a,145.2b)--(811.7a,161.8b);
draw (811.7a,611.4b)--(811.7a,594.8b);
put_text( btex  3000 etex, 811.7a, 108.6b, 0, 2);
draw (903.4a,145.2b)--(903.4a,161.8b);
draw (903.4a,611.4b)--(903.4a,594.8b);
put_text( btex  3500 etex, 903.4a, 108.6b, 0, 2);
draw (995.0a,145.2b)--(995.0a,161.8b);
draw (995.0a,611.4b)--(995.0a,594.8b);
put_text( btex  4000 etex, 995.0a, 108.6b, 0, 2);
draw (1086.7a,145.2b)--(1086.7a,161.8b);
draw (1086.7a,611.4b)--(1086.7a,594.8b);
put_text( btex  4500 etex, 1086.7a, 108.6b, 0, 2);
draw (1178.4a,145.2b)--(1178.4a,161.8b);
draw (1178.4a,611.4b)--(1178.4a,594.8b);
put_text( btex  5000 etex, 1178.4a, 108.6b, 0, 2);
draw (261.6a,145.2b)--(1178.4a,145.2b)--(1178.4a,611.4b)--(261.6a,611.4b)--(261.6a,145.2b);
put_text( btex Computation Rate (ts/cycle) etex, 66.3a, 378.3b, 1, 2);
put_text( btex Node Count etex, 720.0a, 53.7b, 0, 2);
linetype 0;
put_text( btex 5000 atoms (gp) etex, 1037.9a, 436.3b, 0, 3);
draw (1055.6a,436.3b)--(1143.0a,436.3b);
draw (261.8a,145.9b)--(263.1a,150.3b)--(267.5a,162.8b)--(273.3a,180.3b)--(285.1a,204.6b)
--(308.5a,255.1b)--(355.5a,362.8b)--(449.4a,481.2b)--(637.1a,523.7b)--(1012.6a,577.0b)
--(1178.4a,518.8b);
linetype 1;
put_text( btex 5000 atoms (gfr) etex, 1037.9a, 399.7b, 0, 3);
draw (1055.6a,399.7b)--(1143.0a,399.7b);
draw (261.8a,146.0b)--(263.1a,150.8b)--(267.5a,164.2b)--(273.3a,183.1b)--(285.1a,208.5b)
--(308.5a,261.4b)--(355.5a,374.6b)--(449.4a,492.9b)--(637.1a,528.6b)--(1012.6a,577.7b)
--(1178.4a,519.2b);
linetype 2;
put_text( btex 10,000 atoms (gp) etex, 1037.9a, 363.1b, 0, 3);
draw (1055.6a,363.1b)--(1143.0a,363.1b);
draw (261.8a,145.8b)--(263.1a,149.5b)--(267.5a,160.4b)--(273.3a,175.6b)--(285.1a,197.6b)
--(308.5a,242.6b)--(355.5a,336.6b)--(449.4a,444.8b)--(637.1a,493.8b)--(1012.6a,549.1b)
--(1178.4a,496.3b);
linetype 3;
put_text( btex 10,000 atoms (gfr) etex, 1037.9a, 326.5b, 0, 3);
draw (1055.6a,326.5b)--(1143.0a,326.5b);
draw (261.8a,146.0b)--(263.1a,150.8b)--(267.5a,164.2b)--(273.3a,183.1b)--(285.1a,208.5b)
--(308.5a,261.4b)--(355.5a,374.6b)--(449.4a,492.9b)--(637.1a,528.6b)--(1012.6a,577.7b)
--(1178.4a,519.2b);
linetype 4;
put_text( btex 20,000 atoms (gp) etex, 1037.9a, 289.9b, 0, 3);
draw (1055.6a,289.9b)--(1143.0a,289.9b);
draw (261.8a,145.7b)--(263.1a,148.5b)--(267.5a,157.2b)--(273.3a,169.2b)--(285.1a,187.6b)
--(308.5a,224.6b)--(355.5a,299.4b)--(449.4a,391.4b)--(637.1a,446.1b)--(1012.6a,502.9b)
--(1178.4a,458.8b);
linetype 5;
put_text( btex 20,000 atoms (gfr) etex, 1037.9a, 253.3b, 0, 3);
draw (1055.6a,253.3b)--(1143.0a,253.3b);
draw (261.8a,146.0b)--(263.1a,150.8b)--(267.5a,164.2b)--(273.3a,183.1b)--(285.1a,208.5b)
--(308.5a,261.4b)--(355.5a,374.6b)--(449.4a,492.9b)--(637.1a,528.6b)--(1012.6a,577.7b)
--(1178.4a,519.2b);
linetype 6;
put_text( btex 30,000 atoms (gp) etex, 1037.9a, 216.7b, 0, 3);
draw (1055.6a,216.7b)--(1143.0a,216.7b);
draw (261.8a,145.6b)--(263.1a,147.9b)--(267.5a,155.2b)--(273.3a,165.0b)--(285.1a,180.8b)
--(308.5a,212.1b)--(355.5a,274.2b)--(449.4a,354.2b)--(637.1a,409.9b)--(1012.6a,466.1b)
--(1178.4a,428.9b);
linetype 7;
put_text( btex 30,000 atoms (gfr) etex, 1037.9a, 180.1b, 0, 3);
draw (1055.6a,180.1b)--(1143.0a,180.1b);
draw (261.8a,145.7b)--(263.1a,148.9b)--(267.5a,158.5b)--(273.3a,171.7b)--(285.1a,191.1b)
--(308.5a,229.2b)--(355.5a,304.1b)--(449.4a,387.0b)--(637.1a,428.3b)--(1012.6a,471.7b)
--(1178.4a,433.0b);
endfig;
end.
