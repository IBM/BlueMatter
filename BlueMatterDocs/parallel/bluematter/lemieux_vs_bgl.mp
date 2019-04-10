%GNUPLOT Metapost output: Mon Jul 25 22:39:53 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage[latin1]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{textcomp}
\usepackage{mathptmx}
\usepackage[scaled=.92]{helvet}
\usepackage{courier}
\usepackage{latexsym}
\begin{document}
etex
% ENDPRE

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

color currentcolor; currentcolor:=black;
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

beginfig(4);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,109.8b)--(213.1a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0.001 etex, 178.8a, 109.8b, 0, 3);
draw (196.5a,196.3b)--(204.8a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (196.5a,246.9b)--(204.8a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (196.5a,282.8b)--(204.8a,282.8b);
draw (1146.3a,282.8b)--(1138.0a,282.8b);
draw (196.5a,310.7b)--(204.8a,310.7b);
draw (1146.3a,310.7b)--(1138.0a,310.7b);
draw (196.5a,333.4b)--(204.8a,333.4b);
draw (1146.3a,333.4b)--(1138.0a,333.4b);
draw (196.5a,352.7b)--(204.8a,352.7b);
draw (1146.3a,352.7b)--(1138.0a,352.7b);
draw (196.5a,369.3b)--(204.8a,369.3b);
draw (1146.3a,369.3b)--(1138.0a,369.3b);
draw (196.5a,384.0b)--(204.8a,384.0b);
draw (1146.3a,384.0b)--(1138.0a,384.0b);
draw (196.5a,397.2b)--(213.1a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  0.01 etex, 178.8a, 397.2b, 0, 3);
draw (196.5a,483.7b)--(204.8a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (196.5a,534.3b)--(204.8a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (196.5a,570.2b)--(204.8a,570.2b);
draw (1146.3a,570.2b)--(1138.0a,570.2b);
draw (196.5a,598.1b)--(204.8a,598.1b);
draw (1146.3a,598.1b)--(1138.0a,598.1b);
draw (196.5a,620.8b)--(204.8a,620.8b);
draw (1146.3a,620.8b)--(1138.0a,620.8b);
draw (196.5a,640.1b)--(204.8a,640.1b);
draw (1146.3a,640.1b)--(1138.0a,640.1b);
draw (196.5a,656.7b)--(204.8a,656.7b);
draw (1146.3a,656.7b)--(1138.0a,656.7b);
draw (196.5a,671.4b)--(204.8a,671.4b);
draw (1146.3a,671.4b)--(1138.0a,671.4b);
draw (196.5a,684.6b)--(213.1a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  0.1 etex, 178.8a, 684.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,684.6b)--(196.5a,668.0b);
put_text( btex  100 etex, 196.5a, 73.2b, 0, 2);
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
put_text( btex  1000 etex, 513.1a, 73.2b, 0, 2);
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
put_text( btex  10000 etex, 829.7a, 73.2b, 0, 2);
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
put_text( btex  100000 etex, 1146.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
put_text( btex Elapsed Time (seconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node/CPU Count etex, 671.4a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Blue Matter on BG/L (BG/L ADE SPI) etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
draw (1023.5a,649.7b)--(1110.9a,649.7b);
draw (421.1a,545.7b)--(516.4a,489.4b)--(611.7a,425.1b)--(707.0a,353.1b)--(802.3a,301.6b)
--(897.6a,240.4b);
gpdraw(0,421.1a,545.7b);
gpdraw(0,516.4a,489.4b);
gpdraw(0,611.7a,425.1b);
gpdraw(0,707.0a,353.1b);
gpdraw(0,802.3a,301.6b);
gpdraw(0,897.6a,240.4b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
linetype -2;
put_text( btex NAMD on Lemieux (Elan/Quadrics) etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
draw (1023.5a,613.1b)--(1110.9a,613.1b);
draw (196.5a,672.6b)--(230.4a,642.7b)--(325.7a,571.2b)--(420.5a,502.8b)--(516.2a,439.2b)
--(572.1a,426.0b)--(624.6a,428.0b);
gpdraw(1,230.4a,642.7b);
gpdraw(1,325.7a,571.2b);
gpdraw(1,420.5a,502.8b);
gpdraw(1,516.2a,439.2b);
gpdraw(1,572.1a,426.0b);
gpdraw(1,624.6a,428.0b);
gpdraw(1,1067.2a,613.1b);
linetype -2;
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
