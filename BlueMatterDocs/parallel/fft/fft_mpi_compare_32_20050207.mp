%GNUPLOT Metapost output: Mon Feb 07 23:02:45 2005


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

beginfig(9);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  1e-005 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,153.1b)--(222.5a,153.1b);
draw (1146.3a,153.1b)--(1138.0a,153.1b);
draw (214.2a,178.4b)--(222.5a,178.4b);
draw (1146.3a,178.4b)--(1138.0a,178.4b);
draw (214.2a,196.3b)--(222.5a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (214.2a,210.2b)--(222.5a,210.2b);
draw (1146.3a,210.2b)--(1138.0a,210.2b);
draw (214.2a,221.6b)--(222.5a,221.6b);
draw (1146.3a,221.6b)--(1138.0a,221.6b);
draw (214.2a,231.2b)--(222.5a,231.2b);
draw (1146.3a,231.2b)--(1138.0a,231.2b);
draw (214.2a,239.6b)--(222.5a,239.6b);
draw (1146.3a,239.6b)--(1138.0a,239.6b);
draw (214.2a,246.9b)--(222.5a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (214.2a,253.5b)--(230.8a,253.5b);
draw (1146.3a,253.5b)--(1129.7a,253.5b);
put_text( btex  0.0001 etex, 196.5a, 253.5b, 0, 3);
draw (214.2a,296.8b)--(222.5a,296.8b);
draw (1146.3a,296.8b)--(1138.0a,296.8b);
draw (214.2a,322.1b)--(222.5a,322.1b);
draw (1146.3a,322.1b)--(1138.0a,322.1b);
draw (214.2a,340.0b)--(222.5a,340.0b);
draw (1146.3a,340.0b)--(1138.0a,340.0b);
draw (214.2a,353.9b)--(222.5a,353.9b);
draw (1146.3a,353.9b)--(1138.0a,353.9b);
draw (214.2a,365.3b)--(222.5a,365.3b);
draw (1146.3a,365.3b)--(1138.0a,365.3b);
draw (214.2a,374.9b)--(222.5a,374.9b);
draw (1146.3a,374.9b)--(1138.0a,374.9b);
draw (214.2a,383.3b)--(222.5a,383.3b);
draw (1146.3a,383.3b)--(1138.0a,383.3b);
draw (214.2a,390.6b)--(222.5a,390.6b);
draw (1146.3a,390.6b)--(1138.0a,390.6b);
draw (214.2a,397.2b)--(230.8a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  0.001 etex, 196.5a, 397.2b, 0, 3);
draw (214.2a,440.5b)--(222.5a,440.5b);
draw (1146.3a,440.5b)--(1138.0a,440.5b);
draw (214.2a,465.8b)--(222.5a,465.8b);
draw (1146.3a,465.8b)--(1138.0a,465.8b);
draw (214.2a,483.7b)--(222.5a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (214.2a,497.6b)--(222.5a,497.6b);
draw (1146.3a,497.6b)--(1138.0a,497.6b);
draw (214.2a,509.0b)--(222.5a,509.0b);
draw (1146.3a,509.0b)--(1138.0a,509.0b);
draw (214.2a,518.6b)--(222.5a,518.6b);
draw (1146.3a,518.6b)--(1138.0a,518.6b);
draw (214.2a,527.0b)--(222.5a,527.0b);
draw (1146.3a,527.0b)--(1138.0a,527.0b);
draw (214.2a,534.3b)--(222.5a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (214.2a,540.9b)--(230.8a,540.9b);
draw (1146.3a,540.9b)--(1129.7a,540.9b);
put_text( btex  0.01 etex, 196.5a, 540.9b, 0, 3);
draw (214.2a,584.2b)--(222.5a,584.2b);
draw (1146.3a,584.2b)--(1138.0a,584.2b);
draw (214.2a,609.5b)--(222.5a,609.5b);
draw (1146.3a,609.5b)--(1138.0a,609.5b);
draw (214.2a,627.4b)--(222.5a,627.4b);
draw (1146.3a,627.4b)--(1138.0a,627.4b);
draw (214.2a,641.3b)--(222.5a,641.3b);
draw (1146.3a,641.3b)--(1138.0a,641.3b);
draw (214.2a,652.7b)--(222.5a,652.7b);
draw (1146.3a,652.7b)--(1138.0a,652.7b);
draw (214.2a,662.3b)--(222.5a,662.3b);
draw (1146.3a,662.3b)--(1138.0a,662.3b);
draw (214.2a,670.7b)--(222.5a,670.7b);
draw (1146.3a,670.7b)--(1138.0a,670.7b);
draw (214.2a,678.0b)--(222.5a,678.0b);
draw (1146.3a,678.0b)--(1138.0a,678.0b);
draw (214.2a,684.6b)--(230.8a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  0.1 etex, 196.5a, 684.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,684.6b)--(214.2a,668.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
draw (298.9a,109.8b)--(298.9a,118.1b);
draw (298.9a,684.6b)--(298.9a,676.3b);
draw (348.5a,109.8b)--(348.5a,118.1b);
draw (348.5a,684.6b)--(348.5a,676.3b);
draw (383.7a,109.8b)--(383.7a,118.1b);
draw (383.7a,684.6b)--(383.7a,676.3b);
draw (410.9a,109.8b)--(410.9a,118.1b);
draw (410.9a,684.6b)--(410.9a,676.3b);
draw (433.2a,109.8b)--(433.2a,118.1b);
draw (433.2a,684.6b)--(433.2a,676.3b);
draw (452.1a,109.8b)--(452.1a,118.1b);
draw (452.1a,684.6b)--(452.1a,676.3b);
draw (468.4a,109.8b)--(468.4a,118.1b);
draw (468.4a,684.6b)--(468.4a,676.3b);
draw (482.8a,109.8b)--(482.8a,118.1b);
draw (482.8a,684.6b)--(482.8a,676.3b);
draw (495.7a,109.8b)--(495.7a,126.4b);
draw (495.7a,684.6b)--(495.7a,668.0b);
put_text( btex  10 etex, 495.7a, 73.2b, 0, 2);
draw (580.4a,109.8b)--(580.4a,118.1b);
draw (580.4a,684.6b)--(580.4a,676.3b);
draw (629.9a,109.8b)--(629.9a,118.1b);
draw (629.9a,684.6b)--(629.9a,676.3b);
draw (665.1a,109.8b)--(665.1a,118.1b);
draw (665.1a,684.6b)--(665.1a,676.3b);
draw (692.4a,109.8b)--(692.4a,118.1b);
draw (692.4a,684.6b)--(692.4a,676.3b);
draw (714.7a,109.8b)--(714.7a,118.1b);
draw (714.7a,684.6b)--(714.7a,676.3b);
draw (733.5a,109.8b)--(733.5a,118.1b);
draw (733.5a,684.6b)--(733.5a,676.3b);
draw (749.8a,109.8b)--(749.8a,118.1b);
draw (749.8a,684.6b)--(749.8a,676.3b);
draw (764.2a,109.8b)--(764.2a,118.1b);
draw (764.2a,684.6b)--(764.2a,676.3b);
draw (777.1a,109.8b)--(777.1a,126.4b);
draw (777.1a,684.6b)--(777.1a,668.0b);
put_text( btex  100 etex, 777.1a, 73.2b, 0, 2);
draw (861.8a,109.8b)--(861.8a,118.1b);
draw (861.8a,684.6b)--(861.8a,676.3b);
draw (911.4a,109.8b)--(911.4a,118.1b);
draw (911.4a,684.6b)--(911.4a,676.3b);
draw (946.6a,109.8b)--(946.6a,118.1b);
draw (946.6a,684.6b)--(946.6a,676.3b);
draw (973.8a,109.8b)--(973.8a,118.1b);
draw (973.8a,684.6b)--(973.8a,676.3b);
draw (996.1a,109.8b)--(996.1a,118.1b);
draw (996.1a,684.6b)--(996.1a,676.3b);
draw (1015.0a,109.8b)--(1015.0a,118.1b);
draw (1015.0a,684.6b)--(1015.0a,676.3b);
draw (1031.3a,109.8b)--(1031.3a,118.1b);
draw (1031.3a,684.6b)--(1031.3a,676.3b);
draw (1045.7a,109.8b)--(1045.7a,118.1b);
draw (1045.7a,684.6b)--(1045.7a,676.3b);
draw (1058.6a,109.8b)--(1058.6a,126.4b);
draw (1058.6a,684.6b)--(1058.6a,668.0b);
put_text( btex  1000 etex, 1058.6a, 73.2b, 0, 2);
draw (1143.3a,109.8b)--(1143.3a,118.1b);
draw (1143.3a,684.6b)--(1143.3a,676.3b);
draw (214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b)--(214.2a,109.8b);
put_text( btex Time (sec.) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Task Count etex, 680.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $32^3$ MPI Collective (MPICH2) etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
draw (1023.5a,649.7b)--(1110.9a,649.7b);
draw (214.2a,538.7b)--(298.9a,504.2b)--(383.7a,476.1b)--(468.4a,458.7b)--(553.1a,419.1b)
--(637.8a,382.3b)--(722.6a,363.6b)--(807.3a,339.2b)--(892.0a,337.1b)--(976.7a,334.1b);
gpdraw(0,214.2a,538.7b);
gpdraw(0,298.9a,504.2b);
gpdraw(0,383.7a,476.1b);
gpdraw(0,468.4a,458.7b);
gpdraw(0,553.1a,419.1b);
gpdraw(0,637.8a,382.3b);
gpdraw(0,722.6a,363.6b);
gpdraw(0,807.3a,339.2b);
gpdraw(0,892.0a,337.1b);
gpdraw(0,976.7a,334.1b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
linetype -2;
put_text( btex $32^3$ MPI Collective (optimized) etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
draw (1023.5a,613.1b)--(1110.9a,613.1b);
draw (214.2a,540.7b)--(468.4a,447.1b)--(553.1a,411.7b)--(637.8a,373.0b)--(722.6a,342.8b)
--(807.3a,321.6b)--(892.0a,316.4b)--(976.7a,307.4b)--(1061.5a,309.4b);
gpdraw(1,214.2a,540.7b);
gpdraw(1,468.4a,447.1b);
gpdraw(1,553.1a,411.7b);
gpdraw(1,637.8a,373.0b);
gpdraw(1,722.6a,342.8b);
gpdraw(1,807.3a,321.6b);
gpdraw(1,892.0a,316.4b);
gpdraw(1,976.7a,307.4b);
gpdraw(1,1061.5a,309.4b);
gpdraw(1,1067.2a,613.1b);
linetype 2;
linetype -2;
put_text( btex $32^3$ Active Packet etex, 1005.8a, 576.5b, 0, 3);
linetype 2;
draw (1023.5a,576.5b)--(1110.9a,576.5b);
draw (214.2a,565.4b)--(468.4a,435.6b)--(637.8a,363.7b)--(807.3a,299.8b)--(976.7a,244.5b);
gpdraw(2,214.2a,565.4b);
gpdraw(2,468.4a,435.6b);
gpdraw(2,637.8a,363.7b);
gpdraw(2,807.3a,299.8b);
gpdraw(2,976.7a,244.5b);
gpdraw(2,1067.2a,576.5b);
linetype 3;
linetype -2;
put_text( btex $32^3$ Model etex, 1005.8a, 539.9b, 0, 3);
linetype 3;
draw (1023.5a,539.9b)--(1110.9a,539.9b);
draw (214.2a,418.4b)--(468.4a,389.1b)--(637.8a,339.2b)--(722.6a,296.0b)--(807.3a,281.8b)
--(892.0a,249.1b)--(976.7a,166.2b)--(1061.5a,152.0b);
linetype -2;
draw (214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
