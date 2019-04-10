%GNUPLOT Metapost output: Thu Jul 24 00:28:15 2003


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
draw (143.4a,109.8b)--(160.0a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0 etex, 125.7a, 109.8b, 0, 3);
draw (143.4a,173.7b)--(160.0a,173.7b);
draw (1146.3a,173.7b)--(1129.7a,173.7b);
put_text( btex  5 etex, 125.7a, 173.7b, 0, 3);
draw (143.4a,237.5b)--(160.0a,237.5b);
draw (1146.3a,237.5b)--(1129.7a,237.5b);
put_text( btex  10 etex, 125.7a, 237.5b, 0, 3);
draw (143.4a,301.4b)--(160.0a,301.4b);
draw (1146.3a,301.4b)--(1129.7a,301.4b);
put_text( btex  15 etex, 125.7a, 301.4b, 0, 3);
draw (143.4a,365.3b)--(160.0a,365.3b);
draw (1146.3a,365.3b)--(1129.7a,365.3b);
put_text( btex  20 etex, 125.7a, 365.3b, 0, 3);
draw (143.4a,429.1b)--(160.0a,429.1b);
draw (1146.3a,429.1b)--(1129.7a,429.1b);
put_text( btex  25 etex, 125.7a, 429.1b, 0, 3);
draw (143.4a,493.0b)--(160.0a,493.0b);
draw (1146.3a,493.0b)--(1129.7a,493.0b);
put_text( btex  30 etex, 125.7a, 493.0b, 0, 3);
draw (143.4a,556.9b)--(160.0a,556.9b);
draw (1146.3a,556.9b)--(1129.7a,556.9b);
put_text( btex  35 etex, 125.7a, 556.9b, 0, 3);
draw (143.4a,620.7b)--(160.0a,620.7b);
draw (1146.3a,620.7b)--(1129.7a,620.7b);
put_text( btex  40 etex, 125.7a, 620.7b, 0, 3);
draw (143.4a,684.6b)--(160.0a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  45 etex, 125.7a, 684.6b, 0, 3);
draw (143.4a,109.8b)--(143.4a,126.4b);
draw (143.4a,684.6b)--(143.4a,668.0b);
put_text( btex  0 etex, 143.4a, 73.2b, 0, 2);
draw (286.7a,109.8b)--(286.7a,126.4b);
draw (286.7a,684.6b)--(286.7a,668.0b);
put_text( btex  5 etex, 286.7a, 73.2b, 0, 2);
draw (429.9a,109.8b)--(429.9a,126.4b);
draw (429.9a,684.6b)--(429.9a,668.0b);
put_text( btex  10 etex, 429.9a, 73.2b, 0, 2);
draw (573.2a,109.8b)--(573.2a,126.4b);
draw (573.2a,684.6b)--(573.2a,668.0b);
put_text( btex  15 etex, 573.2a, 73.2b, 0, 2);
draw (716.5a,109.8b)--(716.5a,126.4b);
draw (716.5a,684.6b)--(716.5a,668.0b);
put_text( btex  20 etex, 716.5a, 73.2b, 0, 2);
draw (859.8a,109.8b)--(859.8a,126.4b);
draw (859.8a,684.6b)--(859.8a,668.0b);
put_text( btex  25 etex, 859.8a, 73.2b, 0, 2);
draw (1003.0a,109.8b)--(1003.0a,126.4b);
draw (1003.0a,684.6b)--(1003.0a,668.0b);
put_text( btex  30 etex, 1003.0a, 73.2b, 0, 2);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  35 etex, 1146.3a, 73.2b, 0, 2);
draw (143.4a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(143.4a,684.6b)--(143.4a,109.8b);
put_text( btex Speedup etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 644.8a, 18.3b, 0, 2);
linetype 0;
put_text( btex Ideal etex, 355.8a, 649.7b, 0, 3);
draw (373.5a,649.7b)--(460.9a,649.7b);
draw (172.1a,122.6b)--(200.7a,135.3b)--(372.6a,212.0b)--(1060.3a,518.5b);
linetype 1;
put_text( btex 64x64x64 etex, 355.8a, 613.1b, 0, 3);
gpdraw(0,172.1a,122.6b);
gpdraw(0,200.7a,137.4b);
gpdraw(0,372.6a,219.9b);
gpdraw(0,1060.3a,439.8b);
gpdraw(0,417.2a,613.1b);
linetype 2;
put_text( btex 128x128x128 etex, 355.8a, 576.5b, 0, 3);
gpdraw(1,172.1a,122.6b);
gpdraw(1,200.7a,134.9b);
gpdraw(1,372.6a,237.6b);
gpdraw(1,1060.3a,657.9b);
gpdraw(1,417.2a,576.5b);
endfig;
end.
