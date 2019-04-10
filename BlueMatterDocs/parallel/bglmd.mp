%GNUPLOT Metapost output: Wed Oct 15 21:12:44 2003

prologues:=2;
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

verbatimtex
  \def\imag{i}
  %\documentclass[20pt,dvips,landscape]{foils}
  \documentclass{article}
  \usepackage[LY1]{fontenc}	% specify text font encoding
  %\usepackage{amsmath}
  %\usepackage[usenames,dvips]{color}
  \usepackage[expert]{lucidabr}
  %\usepackage{times}
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
draw (196.5a,109.8b)--(213.1a,109.8b);
draw (1446.3a,109.8b)--(1429.7a,109.8b);
put_text( btex  0.001 etex, 178.8a, 109.8b, 0, 3);
draw (196.5a,185.5b)--(204.8a,185.5b);
draw (1446.3a,185.5b)--(1438.0a,185.5b);
draw (196.5a,229.8b)--(204.8a,229.8b);
draw (1446.3a,229.8b)--(1438.0a,229.8b);
draw (196.5a,261.3b)--(204.8a,261.3b);
draw (1446.3a,261.3b)--(1438.0a,261.3b);
draw (196.5a,285.7b)--(204.8a,285.7b);
draw (1446.3a,285.7b)--(1438.0a,285.7b);
draw (196.5a,305.6b)--(204.8a,305.6b);
draw (1446.3a,305.6b)--(1438.0a,305.6b);
draw (196.5a,322.4b)--(204.8a,322.4b);
draw (1446.3a,322.4b)--(1438.0a,322.4b);
draw (196.5a,337.0b)--(204.8a,337.0b);
draw (1446.3a,337.0b)--(1438.0a,337.0b);
draw (196.5a,349.9b)--(204.8a,349.9b);
draw (1446.3a,349.9b)--(1438.0a,349.9b);
draw (196.5a,361.4b)--(213.1a,361.4b);
draw (1446.3a,361.4b)--(1429.7a,361.4b);
put_text( btex  0.01 etex, 178.8a, 361.4b, 0, 3);
draw (196.5a,437.1b)--(204.8a,437.1b);
draw (1446.3a,437.1b)--(1438.0a,437.1b);
draw (196.5a,481.4b)--(204.8a,481.4b);
draw (1446.3a,481.4b)--(1438.0a,481.4b);
draw (196.5a,512.9b)--(204.8a,512.9b);
draw (1446.3a,512.9b)--(1438.0a,512.9b);
draw (196.5a,537.3b)--(204.8a,537.3b);
draw (1446.3a,537.3b)--(1438.0a,537.3b);
draw (196.5a,557.2b)--(204.8a,557.2b);
draw (1446.3a,557.2b)--(1438.0a,557.2b);
draw (196.5a,574.0b)--(204.8a,574.0b);
draw (1446.3a,574.0b)--(1438.0a,574.0b);
draw (196.5a,588.6b)--(204.8a,588.6b);
draw (1446.3a,588.6b)--(1438.0a,588.6b);
draw (196.5a,601.5b)--(204.8a,601.5b);
draw (1446.3a,601.5b)--(1438.0a,601.5b);
draw (196.5a,613.0b)--(213.1a,613.0b);
draw (1446.3a,613.0b)--(1429.7a,613.0b);
put_text( btex  0.1 etex, 178.8a, 613.0b, 0, 3);
draw (196.5a,688.7b)--(204.8a,688.7b);
draw (1446.3a,688.7b)--(1438.0a,688.7b);
draw (196.5a,733.0b)--(204.8a,733.0b);
draw (1446.3a,733.0b)--(1438.0a,733.0b);
draw (196.5a,764.5b)--(204.8a,764.5b);
draw (1446.3a,764.5b)--(1438.0a,764.5b);
draw (196.5a,788.9b)--(204.8a,788.9b);
draw (1446.3a,788.9b)--(1438.0a,788.9b);
draw (196.5a,808.8b)--(204.8a,808.8b);
draw (1446.3a,808.8b)--(1438.0a,808.8b);
draw (196.5a,825.6b)--(204.8a,825.6b);
draw (1446.3a,825.6b)--(1438.0a,825.6b);
draw (196.5a,840.2b)--(204.8a,840.2b);
draw (1446.3a,840.2b)--(1438.0a,840.2b);
draw (196.5a,853.1b)--(204.8a,853.1b);
draw (1446.3a,853.1b)--(1438.0a,853.1b);
draw (196.5a,864.6b)--(213.1a,864.6b);
draw (1446.3a,864.6b)--(1429.7a,864.6b);
put_text( btex  1 etex, 178.8a, 864.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,864.6b)--(196.5a,848.0b);
put_text( btex  1 etex, 196.5a, 73.2b, 0, 2);
draw (271.7a,109.8b)--(271.7a,118.1b);
draw (271.7a,864.6b)--(271.7a,856.3b);
draw (371.2a,109.8b)--(371.2a,118.1b);
draw (371.2a,864.6b)--(371.2a,856.3b);
draw (422.2a,109.8b)--(422.2a,118.1b);
draw (422.2a,864.6b)--(422.2a,856.3b);
draw (446.5a,109.8b)--(446.5a,126.4b);
draw (446.5a,864.6b)--(446.5a,848.0b);
put_text( btex  10 etex, 446.5a, 73.2b, 0, 2);
draw (521.7a,109.8b)--(521.7a,118.1b);
draw (521.7a,864.6b)--(521.7a,856.3b);
draw (621.2a,109.8b)--(621.2a,118.1b);
draw (621.2a,864.6b)--(621.2a,856.3b);
draw (672.2a,109.8b)--(672.2a,118.1b);
draw (672.2a,864.6b)--(672.2a,856.3b);
draw (696.4a,109.8b)--(696.4a,126.4b);
draw (696.4a,864.6b)--(696.4a,848.0b);
put_text( btex  100 etex, 696.4a, 73.2b, 0, 2);
draw (771.7a,109.8b)--(771.7a,118.1b);
draw (771.7a,864.6b)--(771.7a,856.3b);
draw (871.1a,109.8b)--(871.1a,118.1b);
draw (871.1a,864.6b)--(871.1a,856.3b);
draw (922.2a,109.8b)--(922.2a,118.1b);
draw (922.2a,864.6b)--(922.2a,856.3b);
draw (946.4a,109.8b)--(946.4a,126.4b);
draw (946.4a,864.6b)--(946.4a,848.0b);
put_text( btex  1000 etex, 946.4a, 73.2b, 0, 2);
draw (1021.6a,109.8b)--(1021.6a,118.1b);
draw (1021.6a,864.6b)--(1021.6a,856.3b);
draw (1121.1a,109.8b)--(1121.1a,118.1b);
draw (1121.1a,864.6b)--(1121.1a,856.3b);
draw (1172.1a,109.8b)--(1172.1a,118.1b);
draw (1172.1a,864.6b)--(1172.1a,856.3b);
draw (1196.3a,109.8b)--(1196.3a,126.4b);
draw (1196.3a,864.6b)--(1196.3a,848.0b);
put_text( btex  10000 etex, 1196.3a, 73.2b, 0, 2);
draw (1271.6a,109.8b)--(1271.6a,118.1b);
draw (1271.6a,864.6b)--(1271.6a,856.3b);
draw (1371.1a,109.8b)--(1371.1a,118.1b);
draw (1371.1a,864.6b)--(1371.1a,856.3b);
draw (1422.1a,109.8b)--(1422.1a,118.1b);
draw (1422.1a,864.6b)--(1422.1a,856.3b);
draw (1446.3a,109.8b)--(1446.3a,126.4b);
draw (1446.3a,864.6b)--(1446.3a,848.0b);
put_text( btex  100000 etex, 1446.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1446.3a,109.8b)--(1446.3a,864.6b)--(196.5a,864.6b)--(196.5a,109.8b);
put_text( btex Time/timestep (seconds) etex, 36.6a, 487.2b, 1, 2);
put_text( btex Task Count etex, 821.4a, 18.3b, 0, 2);
linetype 0;
put_text( btex MPI, $\beta$-hairpin (5239 atoms), -O2 -qarch=440, $64^3$ FFT etex, 1329.3a, 217.9b, 0, 3);
gpdraw(0,572.7a,765.8b);
gpdraw(0,723.2a,622.4b);
gpdraw(0,798.5a,617.3b);
gpdraw(0,873.7a,632.0b);
gpdraw(0,1390.7a,217.9b);
linetype 1;
put_text( btex Blade, $\beta$-hairpin (5239 atoms), -O? -qarch=440, $64^3$ FFT etex, 1329.3a, 181.3b, 0, 3);
gpdraw(1,572.7a,686.0b);
gpdraw(1,1390.7a,181.3b);
linetype 2;
put_text( btex Estimate for position globalization, $64^3$ FFT, 5239 atoms etex, 1329.3a, 144.7b, 0, 3);
draw (1347.0a,144.7b)--(1434.4a,144.7b);
draw (422.2a,689.0b)--(572.7a,545.4b)--(648.0a,470.4b)--(723.2a,405.0b)--(798.5a,336.0b)
--(873.7a,286.1b)--(949.0a,323.6b)--(1024.2a,338.5b)--(1099.4a,333.5b)--(1174.7a,445.9b)
--(1249.9a,476.3b);
endfig;
end.
