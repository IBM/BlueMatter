%GNUPLOT Metapost output: Fri May 13 12:27:27 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage[mtbold]{mathtime}
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

beginfig(1);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  1e-005 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,153.1b)--(222.5a,153.1b);
draw (1386.3a,153.1b)--(1378.0a,153.1b);
draw (214.2a,210.3b)--(222.5a,210.3b);
draw (1386.3a,210.3b)--(1378.0a,210.3b);
draw (214.2a,239.6b)--(222.5a,239.6b);
draw (1386.3a,239.6b)--(1378.0a,239.6b);
draw (214.2a,253.6b)--(230.8a,253.6b);
draw (1386.3a,253.6b)--(1369.7a,253.6b);
put_text( btex  0.0001 etex, 196.5a, 253.6b, 0, 3);
draw (214.2a,296.8b)--(222.5a,296.8b);
draw (1386.3a,296.8b)--(1378.0a,296.8b);
draw (214.2a,354.0b)--(222.5a,354.0b);
draw (1386.3a,354.0b)--(1378.0a,354.0b);
draw (214.2a,383.4b)--(222.5a,383.4b);
draw (1386.3a,383.4b)--(1378.0a,383.4b);
draw (214.2a,397.3b)--(230.8a,397.3b);
draw (1386.3a,397.3b)--(1369.7a,397.3b);
put_text( btex  0.001 etex, 196.5a, 397.3b, 0, 3);
draw (214.2a,440.6b)--(222.5a,440.6b);
draw (1386.3a,440.6b)--(1378.0a,440.6b);
draw (214.2a,497.8b)--(222.5a,497.8b);
draw (1386.3a,497.8b)--(1378.0a,497.8b);
draw (214.2a,527.1b)--(222.5a,527.1b);
draw (1386.3a,527.1b)--(1378.0a,527.1b);
draw (214.2a,541.1b)--(230.8a,541.1b);
draw (1386.3a,541.1b)--(1369.7a,541.1b);
put_text( btex  0.01 etex, 196.5a, 541.1b, 0, 3);
draw (214.2a,584.4b)--(222.5a,584.4b);
draw (1386.3a,584.4b)--(1378.0a,584.4b);
draw (214.2a,641.6b)--(222.5a,641.6b);
draw (1386.3a,641.6b)--(1378.0a,641.6b);
draw (214.2a,670.9b)--(222.5a,670.9b);
draw (1386.3a,670.9b)--(1378.0a,670.9b);
draw (214.2a,684.8b)--(230.8a,684.8b);
draw (1386.3a,684.8b)--(1369.7a,684.8b);
put_text( btex  0.1 etex, 196.5a, 684.8b, 0, 3);
draw (214.2a,728.1b)--(222.5a,728.1b);
draw (1386.3a,728.1b)--(1378.0a,728.1b);
draw (214.2a,785.3b)--(222.5a,785.3b);
draw (1386.3a,785.3b)--(1378.0a,785.3b);
draw (214.2a,814.7b)--(222.5a,814.7b);
draw (1386.3a,814.7b)--(1378.0a,814.7b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1386.3a,828.6b)--(1369.7a,828.6b);
put_text( btex  1 etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
draw (309.6a,109.8b)--(309.6a,118.1b);
draw (309.6a,828.6b)--(309.6a,820.3b);
draw (365.4a,109.8b)--(365.4a,118.1b);
draw (365.4a,828.6b)--(365.4a,820.3b);
draw (405.0a,109.8b)--(405.0a,118.1b);
draw (405.0a,828.6b)--(405.0a,820.3b);
draw (435.7a,109.8b)--(435.7a,118.1b);
draw (435.7a,828.6b)--(435.7a,820.3b);
draw (460.8a,109.8b)--(460.8a,118.1b);
draw (460.8a,828.6b)--(460.8a,820.3b);
draw (482.0a,109.8b)--(482.0a,118.1b);
draw (482.0a,828.6b)--(482.0a,820.3b);
draw (500.4a,109.8b)--(500.4a,118.1b);
draw (500.4a,828.6b)--(500.4a,820.3b);
draw (516.6a,109.8b)--(516.6a,118.1b);
draw (516.6a,828.6b)--(516.6a,820.3b);
draw (531.1a,109.8b)--(531.1a,126.4b);
draw (531.1a,828.6b)--(531.1a,812.0b);
put_text( btex  10 etex, 531.1a, 73.2b, 0, 2);
draw (626.5a,109.8b)--(626.5a,118.1b);
draw (626.5a,828.6b)--(626.5a,820.3b);
draw (682.3a,109.8b)--(682.3a,118.1b);
draw (682.3a,828.6b)--(682.3a,820.3b);
draw (721.8a,109.8b)--(721.8a,118.1b);
draw (721.8a,828.6b)--(721.8a,820.3b);
draw (752.6a,109.8b)--(752.6a,118.1b);
draw (752.6a,828.6b)--(752.6a,820.3b);
draw (777.6a,109.8b)--(777.6a,118.1b);
draw (777.6a,828.6b)--(777.6a,820.3b);
draw (798.9a,109.8b)--(798.9a,118.1b);
draw (798.9a,828.6b)--(798.9a,820.3b);
draw (817.2a,109.8b)--(817.2a,118.1b);
draw (817.2a,828.6b)--(817.2a,820.3b);
draw (833.4a,109.8b)--(833.4a,118.1b);
draw (833.4a,828.6b)--(833.4a,820.3b);
draw (847.9a,109.8b)--(847.9a,126.4b);
draw (847.9a,828.6b)--(847.9a,812.0b);
put_text( btex  100 etex, 847.9a, 73.2b, 0, 2);
draw (943.3a,109.8b)--(943.3a,118.1b);
draw (943.3a,828.6b)--(943.3a,820.3b);
draw (999.1a,109.8b)--(999.1a,118.1b);
draw (999.1a,828.6b)--(999.1a,820.3b);
draw (1038.7a,109.8b)--(1038.7a,118.1b);
draw (1038.7a,828.6b)--(1038.7a,820.3b);
draw (1069.4a,109.8b)--(1069.4a,118.1b);
draw (1069.4a,828.6b)--(1069.4a,820.3b);
draw (1094.5a,109.8b)--(1094.5a,118.1b);
draw (1094.5a,828.6b)--(1094.5a,820.3b);
draw (1115.7a,109.8b)--(1115.7a,118.1b);
draw (1115.7a,828.6b)--(1115.7a,820.3b);
draw (1134.1a,109.8b)--(1134.1a,118.1b);
draw (1134.1a,828.6b)--(1134.1a,820.3b);
draw (1150.3a,109.8b)--(1150.3a,118.1b);
draw (1150.3a,828.6b)--(1150.3a,820.3b);
draw (1164.8a,109.8b)--(1164.8a,126.4b);
draw (1164.8a,828.6b)--(1164.8a,812.0b);
put_text( btex  1000 etex, 1164.8a, 73.2b, 0, 2);
draw (1260.2a,109.8b)--(1260.2a,118.1b);
draw (1260.2a,828.6b)--(1260.2a,820.3b);
draw (1316.0a,109.8b)--(1316.0a,118.1b);
draw (1316.0a,828.6b)--(1316.0a,820.3b);
draw (1355.6a,109.8b)--(1355.6a,118.1b);
draw (1355.6a,828.6b)--(1355.6a,820.3b);
draw (1386.3a,109.8b)--(1386.3a,118.1b);
draw (1386.3a,828.6b)--(1386.3a,820.3b);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Time (sec.) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Task Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $64^3$ MPI Collective (MPICH2) etex, 1245.8a, 793.7b, 0, 3);
linetype 0;
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (214.2a,687.5b)--(309.6a,652.4b)--(405.0a,617.1b)--(500.4a,594.8b)--(595.8a,548.4b)
--(691.1a,508.3b)--(786.5a,470.3b)--(881.9a,437.2b)--(977.3a,409.4b)--(1072.7a,404.1b)
--(1168.1a,416.3b);
gpdraw(0,214.2a,687.5b);
gpdraw(0,309.6a,652.4b);
gpdraw(0,405.0a,617.1b);
gpdraw(0,500.4a,594.8b);
gpdraw(0,595.8a,548.4b);
gpdraw(0,691.1a,508.3b);
gpdraw(0,786.5a,470.3b);
gpdraw(0,881.9a,437.2b);
gpdraw(0,977.3a,409.4b);
gpdraw(0,1072.7a,404.1b);
gpdraw(0,1168.1a,416.3b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
linetype -2;
put_text( btex $64^3$ MPI Collective (optimized) etex, 1245.8a, 757.1b, 0, 3);
linetype 1;
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (214.2a,682.2b)--(500.4a,590.1b)--(786.5a,460.7b)--(881.9a,439.4b)--(1072.7a,364.4b)
--(1168.1a,354.0b)--(1263.5a,352.0b)--(1358.9a,352.7b);
gpdraw(1,214.2a,682.2b);
gpdraw(1,500.4a,590.1b);
gpdraw(1,786.5a,460.7b);
gpdraw(1,881.9a,439.4b);
gpdraw(1,1072.7a,364.4b);
gpdraw(1,1168.1a,354.0b);
gpdraw(1,1263.5a,352.0b);
gpdraw(1,1358.9a,352.7b);
gpdraw(1,1307.2a,757.1b);
linetype 2;
linetype -2;
put_text( btex $64^3$ BG/L ADE Single Core etex, 1245.8a, 720.5b, 0, 3);
linetype 2;
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (1072.7a,338.5b)--(1168.1a,327.3b)--(1263.5a,304.5b)--(1358.9a,315.8b);
gpdraw(2,1072.7a,338.5b);
gpdraw(2,1168.1a,327.3b);
gpdraw(2,1263.5a,304.5b);
gpdraw(2,1358.9a,315.8b);
gpdraw(2,1307.2a,720.5b);
linetype 3;
linetype -2;
put_text( btex $64^3$ Model etex, 1245.8a, 683.9b, 0, 3);
linetype 3;
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (214.2a,559.7b)--(500.4a,521.5b)--(691.1a,470.5b)--(786.5a,427.2b)--(881.9a,412.5b)
--(977.3a,379.8b)--(1072.7a,297.4b)--(1168.1a,282.7b)--(1263.5a,249.9b)--(1358.9a,206.7b);
linetype -2;
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
