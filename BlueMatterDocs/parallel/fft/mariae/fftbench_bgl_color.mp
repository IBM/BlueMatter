%GNUPLOT Metapost output: Mon Jul 14 17:50:35 2003


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

beginfig(1);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (178.8a,109.8b)--(195.4a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0.01 etex, 161.1a, 109.8b, 0, 3);
draw (178.8a,153.1b)--(187.1a,153.1b);
draw (1146.3a,153.1b)--(1138.0a,153.1b);
draw (178.8a,178.4b)--(187.1a,178.4b);
draw (1146.3a,178.4b)--(1138.0a,178.4b);
draw (178.8a,196.3b)--(187.1a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (178.8a,210.2b)--(187.1a,210.2b);
draw (1146.3a,210.2b)--(1138.0a,210.2b);
draw (178.8a,221.6b)--(187.1a,221.6b);
draw (1146.3a,221.6b)--(1138.0a,221.6b);
draw (178.8a,231.2b)--(187.1a,231.2b);
draw (1146.3a,231.2b)--(1138.0a,231.2b);
draw (178.8a,239.6b)--(187.1a,239.6b);
draw (1146.3a,239.6b)--(1138.0a,239.6b);
draw (178.8a,246.9b)--(187.1a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (178.8a,253.5b)--(195.4a,253.5b);
draw (1146.3a,253.5b)--(1129.7a,253.5b);
put_text( btex  0.1 etex, 161.1a, 253.5b, 0, 3);
draw (178.8a,296.8b)--(187.1a,296.8b);
draw (1146.3a,296.8b)--(1138.0a,296.8b);
draw (178.8a,322.1b)--(187.1a,322.1b);
draw (1146.3a,322.1b)--(1138.0a,322.1b);
draw (178.8a,340.0b)--(187.1a,340.0b);
draw (1146.3a,340.0b)--(1138.0a,340.0b);
draw (178.8a,353.9b)--(187.1a,353.9b);
draw (1146.3a,353.9b)--(1138.0a,353.9b);
draw (178.8a,365.3b)--(187.1a,365.3b);
draw (1146.3a,365.3b)--(1138.0a,365.3b);
draw (178.8a,374.9b)--(187.1a,374.9b);
draw (1146.3a,374.9b)--(1138.0a,374.9b);
draw (178.8a,383.3b)--(187.1a,383.3b);
draw (1146.3a,383.3b)--(1138.0a,383.3b);
draw (178.8a,390.6b)--(187.1a,390.6b);
draw (1146.3a,390.6b)--(1138.0a,390.6b);
draw (178.8a,397.2b)--(195.4a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  1 etex, 161.1a, 397.2b, 0, 3);
draw (178.8a,440.5b)--(187.1a,440.5b);
draw (1146.3a,440.5b)--(1138.0a,440.5b);
draw (178.8a,465.8b)--(187.1a,465.8b);
draw (1146.3a,465.8b)--(1138.0a,465.8b);
draw (178.8a,483.7b)--(187.1a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (178.8a,497.6b)--(187.1a,497.6b);
draw (1146.3a,497.6b)--(1138.0a,497.6b);
draw (178.8a,509.0b)--(187.1a,509.0b);
draw (1146.3a,509.0b)--(1138.0a,509.0b);
draw (178.8a,518.6b)--(187.1a,518.6b);
draw (1146.3a,518.6b)--(1138.0a,518.6b);
draw (178.8a,527.0b)--(187.1a,527.0b);
draw (1146.3a,527.0b)--(1138.0a,527.0b);
draw (178.8a,534.3b)--(187.1a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (178.8a,540.9b)--(195.4a,540.9b);
draw (1146.3a,540.9b)--(1129.7a,540.9b);
put_text( btex  10 etex, 161.1a, 540.9b, 0, 3);
draw (178.8a,584.2b)--(187.1a,584.2b);
draw (1146.3a,584.2b)--(1138.0a,584.2b);
draw (178.8a,609.5b)--(187.1a,609.5b);
draw (1146.3a,609.5b)--(1138.0a,609.5b);
draw (178.8a,627.4b)--(187.1a,627.4b);
draw (1146.3a,627.4b)--(1138.0a,627.4b);
draw (178.8a,641.3b)--(187.1a,641.3b);
draw (1146.3a,641.3b)--(1138.0a,641.3b);
draw (178.8a,652.7b)--(187.1a,652.7b);
draw (1146.3a,652.7b)--(1138.0a,652.7b);
draw (178.8a,662.3b)--(187.1a,662.3b);
draw (1146.3a,662.3b)--(1138.0a,662.3b);
draw (178.8a,670.7b)--(187.1a,670.7b);
draw (1146.3a,670.7b)--(1138.0a,670.7b);
draw (178.8a,678.0b)--(187.1a,678.0b);
draw (1146.3a,678.0b)--(1138.0a,678.0b);
draw (178.8a,684.6b)--(195.4a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  100 etex, 161.1a, 684.6b, 0, 3);
draw (178.8a,109.8b)--(178.8a,126.4b);
draw (178.8a,684.6b)--(178.8a,668.0b);
put_text( btex  1 etex, 178.8a, 73.2b, 0, 2);
draw (275.9a,109.8b)--(275.9a,118.1b);
draw (275.9a,684.6b)--(275.9a,676.3b);
draw (332.7a,109.8b)--(332.7a,118.1b);
draw (332.7a,684.6b)--(332.7a,676.3b);
draw (373.0a,109.8b)--(373.0a,118.1b);
draw (373.0a,684.6b)--(373.0a,676.3b);
draw (404.2a,109.8b)--(404.2a,118.1b);
draw (404.2a,684.6b)--(404.2a,676.3b);
draw (429.8a,109.8b)--(429.8a,118.1b);
draw (429.8a,684.6b)--(429.8a,676.3b);
draw (451.3a,109.8b)--(451.3a,118.1b);
draw (451.3a,684.6b)--(451.3a,676.3b);
draw (470.0a,109.8b)--(470.0a,118.1b);
draw (470.0a,684.6b)--(470.0a,676.3b);
draw (486.5a,109.8b)--(486.5a,118.1b);
draw (486.5a,684.6b)--(486.5a,676.3b);
draw (501.3a,109.8b)--(501.3a,126.4b);
draw (501.3a,684.6b)--(501.3a,668.0b);
put_text( btex  10 etex, 501.3a, 73.2b, 0, 2);
draw (598.4a,109.8b)--(598.4a,118.1b);
draw (598.4a,684.6b)--(598.4a,676.3b);
draw (655.2a,109.8b)--(655.2a,118.1b);
draw (655.2a,684.6b)--(655.2a,676.3b);
draw (695.5a,109.8b)--(695.5a,118.1b);
draw (695.5a,684.6b)--(695.5a,676.3b);
draw (726.7a,109.8b)--(726.7a,118.1b);
draw (726.7a,684.6b)--(726.7a,676.3b);
draw (752.3a,109.8b)--(752.3a,118.1b);
draw (752.3a,684.6b)--(752.3a,676.3b);
draw (773.8a,109.8b)--(773.8a,118.1b);
draw (773.8a,684.6b)--(773.8a,676.3b);
draw (792.5a,109.8b)--(792.5a,118.1b);
draw (792.5a,684.6b)--(792.5a,676.3b);
draw (809.0a,109.8b)--(809.0a,118.1b);
draw (809.0a,684.6b)--(809.0a,676.3b);
draw (823.8a,109.8b)--(823.8a,126.4b);
draw (823.8a,684.6b)--(823.8a,668.0b);
put_text( btex  100 etex, 823.8a, 73.2b, 0, 2);
draw (920.9a,109.8b)--(920.9a,118.1b);
draw (920.9a,684.6b)--(920.9a,676.3b);
draw (977.7a,109.8b)--(977.7a,118.1b);
draw (977.7a,684.6b)--(977.7a,676.3b);
draw (1018.0a,109.8b)--(1018.0a,118.1b);
draw (1018.0a,684.6b)--(1018.0a,676.3b);
draw (1049.2a,109.8b)--(1049.2a,118.1b);
draw (1049.2a,684.6b)--(1049.2a,676.3b);
draw (1074.8a,109.8b)--(1074.8a,118.1b);
draw (1074.8a,684.6b)--(1074.8a,676.3b);
draw (1096.3a,109.8b)--(1096.3a,118.1b);
draw (1096.3a,684.6b)--(1096.3a,676.3b);
draw (1115.0a,109.8b)--(1115.0a,118.1b);
draw (1115.0a,684.6b)--(1115.0a,676.3b);
draw (1131.5a,109.8b)--(1131.5a,118.1b);
draw (1131.5a,684.6b)--(1131.5a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  1000 etex, 1146.3a, 73.2b, 0, 2);
draw (178.8a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(178.8a,684.6b)--(178.8a,109.8b);
put_text( btex 3D-FFT time (seconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Task Count etex, 662.5a, 18.3b, 0, 2);
linetype 0;
put_text( btex 128x128x128 etex, 1005.8a, 649.7b, 0, 3);
gpdraw(0,178.8a,431.0b);
gpdraw(0,275.9a,384.1b);
gpdraw(0,373.0a,349.2b);
gpdraw(0,470.0a,312.6b);
gpdraw(0,567.1a,300.1b);
gpdraw(0,664.2a,267.7b);
gpdraw(0,664.2a,270.4b);
gpdraw(0,858.4a,194.6b);
gpdraw(0,761.3a,221.6b);
gpdraw(0,955.5a,162.7b);
gpdraw(0,1052.5a,146.5b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
put_text( btex 256x256x256 etex, 1005.8a, 613.1b, 0, 3);
gpdraw(1,178.8a,577.8b);
gpdraw(1,275.9a,554.1b);
gpdraw(1,373.0a,512.4b);
gpdraw(1,470.0a,466.1b);
gpdraw(1,567.1a,439.8b);
gpdraw(1,664.2a,403.6b);
gpdraw(1,858.4a,341.4b);
gpdraw(1,761.3a,374.5b);
gpdraw(1,955.5a,307.0b);
gpdraw(1,1052.5a,244.6b);
gpdraw(1,1067.2a,613.1b);
linetype 2;
put_text( btex 512x512x512 etex, 1005.8a, 576.5b, 0, 3);
gpdraw(2,761.3a,506.7b);
gpdraw(2,858.4a,474.3b);
gpdraw(2,955.5a,433.0b);
gpdraw(2,1052.5a,415.3b);
gpdraw(2,1067.2a,576.5b);
endfig;
end.
