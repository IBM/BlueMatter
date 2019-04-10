%GNUPLOT Metapost output: Mon Apr 26 02:01:39 2004


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
  \documentclass{article}
%  \usepackage[LY1]{fontenc}	% specify text font encoding
  %\usepackage{amsmath}
  %\usepackage[usenames,dvips]{color}
%  \usepackage[expert]{lucidabr}
  \usepackage{times}
  \begin{document}
  etex;

%font changes
verbatimtex
\def\setfont#1#2{%.
  \font\gpfont=#1 at #2pt
\gpfont}
\setfont{cmr10}{10.00}
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

beginfig(2);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,109.8b)--(213.1a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  $10^7$ etex, 178.8a, 109.8b, 0, 3);
draw (196.5a,153.1b)--(204.8a,153.1b);
draw (1146.3a,153.1b)--(1138.0a,153.1b);
draw (196.5a,178.4b)--(204.8a,178.4b);
draw (1146.3a,178.4b)--(1138.0a,178.4b);
draw (196.5a,196.3b)--(204.8a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (196.5a,210.2b)--(204.8a,210.2b);
draw (1146.3a,210.2b)--(1138.0a,210.2b);
draw (196.5a,221.6b)--(204.8a,221.6b);
draw (1146.3a,221.6b)--(1138.0a,221.6b);
draw (196.5a,231.2b)--(204.8a,231.2b);
draw (1146.3a,231.2b)--(1138.0a,231.2b);
draw (196.5a,239.6b)--(204.8a,239.6b);
draw (1146.3a,239.6b)--(1138.0a,239.6b);
draw (196.5a,246.9b)--(204.8a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (196.5a,253.5b)--(213.1a,253.5b);
draw (1146.3a,253.5b)--(1129.7a,253.5b);
put_text( btex  $10^8$ etex, 178.8a, 253.5b, 0, 3);
draw (196.5a,296.8b)--(204.8a,296.8b);
draw (1146.3a,296.8b)--(1138.0a,296.8b);
draw (196.5a,322.1b)--(204.8a,322.1b);
draw (1146.3a,322.1b)--(1138.0a,322.1b);
draw (196.5a,340.0b)--(204.8a,340.0b);
draw (1146.3a,340.0b)--(1138.0a,340.0b);
draw (196.5a,353.9b)--(204.8a,353.9b);
draw (1146.3a,353.9b)--(1138.0a,353.9b);
draw (196.5a,365.3b)--(204.8a,365.3b);
draw (1146.3a,365.3b)--(1138.0a,365.3b);
draw (196.5a,374.9b)--(204.8a,374.9b);
draw (1146.3a,374.9b)--(1138.0a,374.9b);
draw (196.5a,383.3b)--(204.8a,383.3b);
draw (1146.3a,383.3b)--(1138.0a,383.3b);
draw (196.5a,390.6b)--(204.8a,390.6b);
draw (1146.3a,390.6b)--(1138.0a,390.6b);
draw (196.5a,397.2b)--(213.1a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  $10^9$ etex, 178.8a, 397.2b, 0, 3);
draw (196.5a,440.5b)--(204.8a,440.5b);
draw (1146.3a,440.5b)--(1138.0a,440.5b);
draw (196.5a,465.8b)--(204.8a,465.8b);
draw (1146.3a,465.8b)--(1138.0a,465.8b);
draw (196.5a,483.7b)--(204.8a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (196.5a,497.6b)--(204.8a,497.6b);
draw (1146.3a,497.6b)--(1138.0a,497.6b);
draw (196.5a,509.0b)--(204.8a,509.0b);
draw (1146.3a,509.0b)--(1138.0a,509.0b);
draw (196.5a,518.6b)--(204.8a,518.6b);
draw (1146.3a,518.6b)--(1138.0a,518.6b);
draw (196.5a,527.0b)--(204.8a,527.0b);
draw (1146.3a,527.0b)--(1138.0a,527.0b);
draw (196.5a,534.3b)--(204.8a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (196.5a,540.9b)--(213.1a,540.9b);
draw (1146.3a,540.9b)--(1129.7a,540.9b);
put_text( btex  $10^{10}$ etex, 178.8a, 540.9b, 0, 3);
draw (196.5a,584.2b)--(204.8a,584.2b);
draw (1146.3a,584.2b)--(1138.0a,584.2b);
draw (196.5a,609.5b)--(204.8a,609.5b);
draw (1146.3a,609.5b)--(1138.0a,609.5b);
draw (196.5a,627.4b)--(204.8a,627.4b);
draw (1146.3a,627.4b)--(1138.0a,627.4b);
draw (196.5a,641.3b)--(204.8a,641.3b);
draw (1146.3a,641.3b)--(1138.0a,641.3b);
draw (196.5a,652.7b)--(204.8a,652.7b);
draw (1146.3a,652.7b)--(1138.0a,652.7b);
draw (196.5a,662.3b)--(204.8a,662.3b);
draw (1146.3a,662.3b)--(1138.0a,662.3b);
draw (196.5a,670.7b)--(204.8a,670.7b);
draw (1146.3a,670.7b)--(1138.0a,670.7b);
draw (196.5a,678.0b)--(204.8a,678.0b);
draw (1146.3a,678.0b)--(1138.0a,678.0b);
draw (196.5a,684.6b)--(213.1a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  $10^{11}$ etex, 178.8a, 684.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,684.6b)--(196.5a,668.0b);
put_text( btex  1 etex, 196.5a, 73.2b, 0, 2);
draw (291.8a,109.8b)--(291.8a,118.1b);
draw (291.8a,684.6b)--(291.8a,676.3b);
draw (347.6a,109.8b)--(347.6a,118.1b);
draw (347.6a,684.6b)--(347.6a,676.3b);
draw (387.1a,109.8b)--(387.1a,118.1b);
draw (387.1a,684.6b)--(387.1a,676.3b);
draw (417.8a,109.8b)--(417.8a,118.1b);
draw (417.8a,684.6b)--(417.8a,676.3b);
draw (442.9a,109.8b)--(442.9a,118.1b);
draw (442.9a,684.6b)--(442.9a,676.3b);
draw (464.1a,109.8b)--(464.1a,118.1b);
draw (464.1a,684.6b)--(464.1a,676.3b);
draw (482.4a,109.8b)--(482.4a,118.1b);
draw (482.4a,684.6b)--(482.4a,676.3b);
draw (498.6a,109.8b)--(498.6a,118.1b);
draw (498.6a,684.6b)--(498.6a,676.3b);
draw (513.1a,109.8b)--(513.1a,126.4b);
draw (513.1a,684.6b)--(513.1a,668.0b);
put_text( btex  10 etex, 513.1a, 73.2b, 0, 2);
draw (608.4a,109.8b)--(608.4a,118.1b);
draw (608.4a,684.6b)--(608.4a,676.3b);
draw (664.2a,109.8b)--(664.2a,118.1b);
draw (664.2a,684.6b)--(664.2a,676.3b);
draw (703.7a,109.8b)--(703.7a,118.1b);
draw (703.7a,684.6b)--(703.7a,676.3b);
draw (734.4a,109.8b)--(734.4a,118.1b);
draw (734.4a,684.6b)--(734.4a,676.3b);
draw (759.5a,109.8b)--(759.5a,118.1b);
draw (759.5a,684.6b)--(759.5a,676.3b);
draw (780.7a,109.8b)--(780.7a,118.1b);
draw (780.7a,684.6b)--(780.7a,676.3b);
draw (799.0a,109.8b)--(799.0a,118.1b);
draw (799.0a,684.6b)--(799.0a,676.3b);
draw (815.2a,109.8b)--(815.2a,118.1b);
draw (815.2a,684.6b)--(815.2a,676.3b);
draw (829.7a,109.8b)--(829.7a,126.4b);
draw (829.7a,684.6b)--(829.7a,668.0b);
put_text( btex  100 etex, 829.7a, 73.2b, 0, 2);
draw (925.0a,109.8b)--(925.0a,118.1b);
draw (925.0a,684.6b)--(925.0a,676.3b);
draw (980.8a,109.8b)--(980.8a,118.1b);
draw (980.8a,684.6b)--(980.8a,676.3b);
draw (1020.3a,109.8b)--(1020.3a,118.1b);
draw (1020.3a,684.6b)--(1020.3a,676.3b);
draw (1051.0a,109.8b)--(1051.0a,118.1b);
draw (1051.0a,684.6b)--(1051.0a,676.3b);
draw (1076.1a,109.8b)--(1076.1a,118.1b);
draw (1076.1a,684.6b)--(1076.1a,676.3b);
draw (1097.3a,109.8b)--(1097.3a,118.1b);
draw (1097.3a,684.6b)--(1097.3a,676.3b);
draw (1115.6a,109.8b)--(1115.6a,118.1b);
draw (1115.6a,684.6b)--(1115.6a,676.3b);
draw (1131.8a,109.8b)--(1131.8a,118.1b);
draw (1131.8a,684.6b)--(1131.8a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  1000 etex, 1146.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
put_text( btex Elapsed Time (pclocks) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 671.4a, 18.3b, 0, 2);
linetype 0;
put_text( btex P3ME-mesh-based collectives etex, 1005.8a, 649.7b, 0, 3);
draw (1023.5a,649.7b)--(1110.9a,649.7b);
draw (1023.5a,658.0b)--(1023.5a,641.4b);
draw (1110.9a,658.0b)--(1110.9a,641.4b);
draw (673.0a,222.6b)--(673.0a,227.5b);
draw (664.7a,222.6b)--(681.3a,222.6b);
draw (664.7a,227.5b)--(681.3a,227.5b);
draw (958.9a,135.2b)--(958.9a,144.5b);
draw (950.6a,135.2b)--(967.2a,135.2b);
draw (950.6a,144.5b)--(967.2a,144.5b);
draw (1054.3a,118.5b)--(1054.3a,129.7b);
draw (1046.0a,118.5b)--(1062.6a,118.5b);
draw (1046.0a,129.7b)--(1062.6a,129.7b);
gpdraw(0,673.0a,224.8b);
gpdraw(0,958.9a,139.7b);
gpdraw(0,1054.3a,123.8b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
put_text( btex Ewald etex, 1005.8a, 613.1b, 0, 3);
draw (1023.5a,613.1b)--(1110.9a,613.1b);
draw (1023.5a,621.4b)--(1023.5a,604.8b);
draw (1110.9a,621.4b)--(1110.9a,604.8b);
draw (196.5a,552.2b)--(196.5a,552.7b);
draw (188.2a,552.2b)--(204.8a,552.2b);
draw (188.2a,552.7b)--(204.8a,552.7b);
draw (482.4a,423.2b)--(482.4a,423.8b);
draw (474.1a,423.2b)--(490.7a,423.2b);
draw (474.1a,423.8b)--(490.7a,423.8b);
draw (673.0a,284.1b)--(673.0a,285.7b);
draw (664.7a,284.1b)--(681.3a,284.1b);
draw (664.7a,285.7b)--(681.3a,285.7b);
draw (863.6a,178.4b)--(863.6a,182.2b);
draw (855.3a,178.4b)--(871.9a,178.4b);
draw (855.3a,182.2b)--(871.9a,182.2b);
draw (958.9a,150.7b)--(958.9a,155.9b);
draw (950.6a,150.7b)--(967.2a,150.7b);
draw (950.6a,155.9b)--(967.2a,155.9b);
draw (1054.3a,133.0b)--(1054.3a,139.0b);
draw (1046.0a,133.0b)--(1062.6a,133.0b);
draw (1046.0a,139.0b)--(1062.6a,139.0b);
gpdraw(1,196.5a,552.4b);
gpdraw(1,482.4a,423.4b);
gpdraw(1,673.0a,284.8b);
gpdraw(1,863.6a,180.0b);
gpdraw(1,958.9a,153.0b);
gpdraw(1,1054.3a,135.4b);
gpdraw(1,1067.2a,613.1b);
linetype 0;
draw (673.0a,224.8b)--(958.9a,139.7b)--(1054.3a,123.8b);
linetype 1;
draw (196.5a,552.4b)--(482.4a,423.4b)--(673.0a,284.8b)--(863.6a,180.0b)--(958.9a,153.0b)
--(1054.3a,135.4b);
endfig;
end.
