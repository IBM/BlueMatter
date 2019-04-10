%GNUPLOT Metapost output: Mon Feb 07 23:25:34 2005


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

beginfig(14);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,366.0b)--(213.1a,366.0b);
draw (1146.3a,366.0b)--(1129.7a,366.0b);
put_text( btex  0.001 etex, 178.8a, 366.0b, 0, 3);
draw (196.5a,400.8b)--(204.8a,400.8b);
draw (1146.3a,400.8b)--(1138.0a,400.8b);
draw (196.5a,421.2b)--(204.8a,421.2b);
draw (1146.3a,421.2b)--(1138.0a,421.2b);
draw (196.5a,435.6b)--(204.8a,435.6b);
draw (1146.3a,435.6b)--(1138.0a,435.6b);
draw (196.5a,446.8b)--(204.8a,446.8b);
draw (1146.3a,446.8b)--(1138.0a,446.8b);
draw (196.5a,456.0b)--(204.8a,456.0b);
draw (1146.3a,456.0b)--(1138.0a,456.0b);
draw (196.5a,463.7b)--(204.8a,463.7b);
draw (1146.3a,463.7b)--(1138.0a,463.7b);
draw (196.5a,470.4b)--(204.8a,470.4b);
draw (1146.3a,470.4b)--(1138.0a,470.4b);
draw (196.5a,476.4b)--(204.8a,476.4b);
draw (1146.3a,476.4b)--(1138.0a,476.4b);
draw (196.5a,481.7b)--(213.1a,481.7b);
draw (1146.3a,481.7b)--(1129.7a,481.7b);
put_text( btex  0.01 etex, 178.8a, 481.7b, 0, 3);
draw (196.5a,516.5b)--(204.8a,516.5b);
draw (1146.3a,516.5b)--(1138.0a,516.5b);
draw (196.5a,536.8b)--(204.8a,536.8b);
draw (1146.3a,536.8b)--(1138.0a,536.8b);
draw (196.5a,551.3b)--(204.8a,551.3b);
draw (1146.3a,551.3b)--(1138.0a,551.3b);
draw (196.5a,562.5b)--(204.8a,562.5b);
draw (1146.3a,562.5b)--(1138.0a,562.5b);
draw (196.5a,571.6b)--(204.8a,571.6b);
draw (1146.3a,571.6b)--(1138.0a,571.6b);
draw (196.5a,579.4b)--(204.8a,579.4b);
draw (1146.3a,579.4b)--(1138.0a,579.4b);
draw (196.5a,586.1b)--(204.8a,586.1b);
draw (1146.3a,586.1b)--(1138.0a,586.1b);
draw (196.5a,592.0b)--(204.8a,592.0b);
draw (1146.3a,592.0b)--(1138.0a,592.0b);
draw (196.5a,597.3b)--(213.1a,597.3b);
draw (1146.3a,597.3b)--(1129.7a,597.3b);
put_text( btex  0.1 etex, 178.8a, 597.3b, 0, 3);
draw (196.5a,632.1b)--(204.8a,632.1b);
draw (1146.3a,632.1b)--(1138.0a,632.1b);
draw (196.5a,652.5b)--(204.8a,652.5b);
draw (1146.3a,652.5b)--(1138.0a,652.5b);
draw (196.5a,666.9b)--(204.8a,666.9b);
draw (1146.3a,666.9b)--(1138.0a,666.9b);
draw (196.5a,678.1b)--(204.8a,678.1b);
draw (1146.3a,678.1b)--(1138.0a,678.1b);
draw (196.5a,687.3b)--(204.8a,687.3b);
draw (1146.3a,687.3b)--(1138.0a,687.3b);
draw (196.5a,695.0b)--(204.8a,695.0b);
draw (1146.3a,695.0b)--(1138.0a,695.0b);
draw (196.5a,701.7b)--(204.8a,701.7b);
draw (1146.3a,701.7b)--(1138.0a,701.7b);
draw (196.5a,707.7b)--(204.8a,707.7b);
draw (1146.3a,707.7b)--(1138.0a,707.7b);
draw (196.5a,713.0b)--(213.1a,713.0b);
draw (1146.3a,713.0b)--(1129.7a,713.0b);
put_text( btex  1 etex, 178.8a, 713.0b, 0, 3);
draw (196.5a,747.8b)--(204.8a,747.8b);
draw (1146.3a,747.8b)--(1138.0a,747.8b);
draw (196.5a,768.1b)--(204.8a,768.1b);
draw (1146.3a,768.1b)--(1138.0a,768.1b);
draw (196.5a,782.6b)--(204.8a,782.6b);
draw (1146.3a,782.6b)--(1138.0a,782.6b);
draw (196.5a,793.8b)--(204.8a,793.8b);
draw (1146.3a,793.8b)--(1138.0a,793.8b);
draw (196.5a,802.9b)--(204.8a,802.9b);
draw (1146.3a,802.9b)--(1138.0a,802.9b);
draw (196.5a,810.7b)--(204.8a,810.7b);
draw (1146.3a,810.7b)--(1138.0a,810.7b);
draw (196.5a,817.4b)--(204.8a,817.4b);
draw (1146.3a,817.4b)--(1138.0a,817.4b);
draw (196.5a,823.3b)--(204.8a,823.3b);
draw (1146.3a,823.3b)--(1138.0a,823.3b);
draw (196.5a,828.6b)--(213.1a,828.6b);
draw (1146.3a,828.6b)--(1129.7a,828.6b);
put_text( btex  10 etex, 178.8a, 828.6b, 0, 3);
draw (196.5a,366.0b)--(196.5a,382.6b);
draw (196.5a,828.6b)--(196.5a,812.0b);
put_text( btex  1 etex, 196.5a, 329.4b, 0, 2);
draw (282.8a,366.0b)--(282.8a,374.3b);
draw (282.8a,828.6b)--(282.8a,820.3b);
draw (333.3a,366.0b)--(333.3a,374.3b);
draw (333.3a,828.6b)--(333.3a,820.3b);
draw (369.2a,366.0b)--(369.2a,374.3b);
draw (369.2a,828.6b)--(369.2a,820.3b);
draw (397.0a,366.0b)--(397.0a,374.3b);
draw (397.0a,828.6b)--(397.0a,820.3b);
draw (419.7a,366.0b)--(419.7a,374.3b);
draw (419.7a,828.6b)--(419.7a,820.3b);
draw (438.9a,366.0b)--(438.9a,374.3b);
draw (438.9a,828.6b)--(438.9a,820.3b);
draw (455.5a,366.0b)--(455.5a,374.3b);
draw (455.5a,828.6b)--(455.5a,820.3b);
draw (470.2a,366.0b)--(470.2a,374.3b);
draw (470.2a,828.6b)--(470.2a,820.3b);
draw (483.3a,366.0b)--(483.3a,382.6b);
draw (483.3a,828.6b)--(483.3a,812.0b);
put_text( btex  10 etex, 483.3a, 329.4b, 0, 2);
draw (569.6a,366.0b)--(569.6a,374.3b);
draw (569.6a,828.6b)--(569.6a,820.3b);
draw (620.1a,366.0b)--(620.1a,374.3b);
draw (620.1a,828.6b)--(620.1a,820.3b);
draw (656.0a,366.0b)--(656.0a,374.3b);
draw (656.0a,828.6b)--(656.0a,820.3b);
draw (683.8a,366.0b)--(683.8a,374.3b);
draw (683.8a,828.6b)--(683.8a,820.3b);
draw (706.5a,366.0b)--(706.5a,374.3b);
draw (706.5a,828.6b)--(706.5a,820.3b);
draw (725.7a,366.0b)--(725.7a,374.3b);
draw (725.7a,828.6b)--(725.7a,820.3b);
draw (742.3a,366.0b)--(742.3a,374.3b);
draw (742.3a,828.6b)--(742.3a,820.3b);
draw (757.0a,366.0b)--(757.0a,374.3b);
draw (757.0a,828.6b)--(757.0a,820.3b);
draw (770.1a,366.0b)--(770.1a,382.6b);
draw (770.1a,828.6b)--(770.1a,812.0b);
put_text( btex  100 etex, 770.1a, 329.4b, 0, 2);
draw (856.4a,366.0b)--(856.4a,374.3b);
draw (856.4a,828.6b)--(856.4a,820.3b);
draw (906.9a,366.0b)--(906.9a,374.3b);
draw (906.9a,828.6b)--(906.9a,820.3b);
draw (942.8a,366.0b)--(942.8a,374.3b);
draw (942.8a,828.6b)--(942.8a,820.3b);
draw (970.6a,366.0b)--(970.6a,374.3b);
draw (970.6a,828.6b)--(970.6a,820.3b);
draw (993.3a,366.0b)--(993.3a,374.3b);
draw (993.3a,828.6b)--(993.3a,820.3b);
draw (1012.5a,366.0b)--(1012.5a,374.3b);
draw (1012.5a,828.6b)--(1012.5a,820.3b);
draw (1029.1a,366.0b)--(1029.1a,374.3b);
draw (1029.1a,828.6b)--(1029.1a,820.3b);
draw (1043.8a,366.0b)--(1043.8a,374.3b);
draw (1043.8a,828.6b)--(1043.8a,820.3b);
draw (1056.9a,366.0b)--(1056.9a,382.6b);
draw (1056.9a,828.6b)--(1056.9a,812.0b);
put_text( btex  1000 etex, 1056.9a, 329.4b, 0, 2);
draw (1143.2a,366.0b)--(1143.2a,374.3b);
draw (1143.2a,828.6b)--(1143.2a,820.3b);
draw (196.5a,366.0b)--(1146.3a,366.0b)--(1146.3a,828.6b)--(196.5a,828.6b)--(196.5a,366.0b);
put_text( btex (Task Count)$^{2/3} \times$ Time (sec.) etex, 36.6a, 597.3b, 1, 2);
put_text( btex Task Count etex, 671.4a, 274.5b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $128^3$ MPI Collective (optimized) etex, 920.0a, 201.3b, 0, 3);
linetype 0;
draw (937.7a,201.3b)--(1025.1a,201.3b);
draw (196.5a,717.8b)--(455.5a,697.9b)--(541.8a,685.8b)--(628.2a,667.3b)--(714.5a,659.9b)
--(800.8a,649.0b)--(887.2a,638.3b)--(973.5a,631.7b)--(1059.8a,633.9b)--(1146.2a,641.9b);
gpdraw(0,196.5a,717.8b);
gpdraw(0,455.5a,697.9b);
gpdraw(0,541.8a,685.8b);
gpdraw(0,628.2a,667.3b);
gpdraw(0,714.5a,659.9b);
gpdraw(0,800.8a,649.0b);
gpdraw(0,887.2a,638.3b);
gpdraw(0,973.5a,631.7b);
gpdraw(0,1059.8a,633.9b);
gpdraw(0,1146.2a,641.9b);
gpdraw(0,981.4a,201.3b);
linetype 1;
linetype -2;
put_text( btex $128^3$ Active Packet etex, 920.0a, 164.7b, 0, 3);
linetype 1;
draw (937.7a,164.7b)--(1025.1a,164.7b);
draw (455.5a,685.2b)--(628.2a,658.6b)--(800.8a,650.7b)--(973.5a,621.7b)--(1059.8a,633.7b);
gpdraw(1,455.5a,685.2b);
gpdraw(1,628.2a,658.6b);
gpdraw(1,800.8a,650.7b);
gpdraw(1,973.5a,621.7b);
gpdraw(1,1059.8a,633.7b);
gpdraw(1,981.4a,164.7b);
linetype 2;
linetype -2;
put_text( btex $64^3$ MPI Collective (optimized) etex, 920.0a, 128.1b, 0, 3);
linetype 2;
draw (937.7a,128.1b)--(1025.1a,128.1b);
draw (196.5a,597.1b)--(455.5a,593.2b)--(541.8a,579.8b)--(628.2a,564.1b)--(714.5a,559.4b)
--(800.8a,551.5b)--(887.2a,544.8b)--(973.5a,551.8b)--(1059.8a,563.9b)--(1146.2a,585.2b);
gpdraw(2,196.5a,597.1b);
gpdraw(2,455.5a,593.2b);
gpdraw(2,541.8a,579.8b);
gpdraw(2,628.2a,564.1b);
gpdraw(2,714.5a,559.4b);
gpdraw(2,800.8a,551.5b);
gpdraw(2,887.2a,544.8b);
gpdraw(2,973.5a,551.8b);
gpdraw(2,1059.8a,563.9b);
gpdraw(2,1146.2a,585.2b);
gpdraw(2,981.4a,128.1b);
linetype 3;
linetype -2;
put_text( btex $64^3$ Active Packet etex, 920.0a, 91.5b, 0, 3);
linetype 3;
draw (937.7a,91.5b)--(1025.1a,91.5b);
draw (196.5a,615.6b)--(455.5a,571.6b)--(628.2a,555.9b)--(800.8a,550.2b)--(973.5a,523.2b)
--(1059.8a,552.1b);
gpdraw(3,196.5a,615.6b);
gpdraw(3,455.5a,571.6b);
gpdraw(3,628.2a,555.9b);
gpdraw(3,800.8a,550.2b);
gpdraw(3,973.5a,523.2b);
gpdraw(3,1059.8a,552.1b);
gpdraw(3,981.4a,91.5b);
linetype 4;
linetype -2;
put_text( btex $32^3$ MPI Collective (optimized) etex, 920.0a, 54.9b, 0, 3);
linetype 4;
draw (937.7a,54.9b)--(1025.1a,54.9b);
draw (196.5a,481.5b)--(455.5a,475.8b)--(541.8a,470.5b)--(628.2a,462.6b)--(714.5a,461.4b)
--(800.8a,467.7b)--(887.2a,486.7b)--(973.5a,502.6b)--(1059.8a,527.5b);
gpdraw(4,196.5a,481.5b);
gpdraw(4,455.5a,475.8b);
gpdraw(4,541.8a,470.5b);
gpdraw(4,628.2a,462.6b);
gpdraw(4,714.5a,461.4b);
gpdraw(4,800.8a,467.7b);
gpdraw(4,887.2a,486.7b);
gpdraw(4,973.5a,502.6b);
gpdraw(4,1059.8a,527.5b);
gpdraw(4,981.4a,54.9b);
linetype 5;
linetype -2;
put_text( btex $32^3$ Active Packet etex, 920.0a, 18.3b, 0, 3);
linetype 5;
draw (937.7a,18.3b)--(1025.1a,18.3b);
draw (196.5a,501.3b)--(455.5a,466.5b)--(628.2a,455.1b)--(800.8a,450.1b)--(973.5a,452.0b);
gpdraw(5,196.5a,501.3b);
gpdraw(5,455.5a,466.5b);
gpdraw(5,628.2a,455.1b);
gpdraw(5,800.8a,450.1b);
gpdraw(5,973.5a,452.0b);
gpdraw(5,981.4a,18.3b);
linetype -2;
draw (196.5a,366.0b)--(1146.3a,366.0b)--(1146.3a,828.6b)--(196.5a,828.6b)--(196.5a,366.0b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
