%GNUPLOT Metapost output: Mon Apr 14 11:05:52 2003

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

beginfig(0);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (261.6a,145.2b)--(278.2a,145.2b);
draw (1418.4a,145.2b)--(1401.8a,145.2b);
put_text( btex  ${\rm 10}$ etex, 243.9a, 145.2b, 0, 3);
draw (261.6a,173.6b)--(269.9a,173.6b);
draw (1418.4a,173.6b)--(1410.1a,173.6b);
draw (261.6a,211.1b)--(269.9a,211.1b);
draw (1418.4a,211.1b)--(1410.1a,211.1b);
draw (261.6a,230.3b)--(269.9a,230.3b);
draw (1418.4a,230.3b)--(1410.1a,230.3b);
draw (261.6a,239.5b)--(278.2a,239.5b);
draw (1418.4a,239.5b)--(1401.8a,239.5b);
put_text( btex  ${\rm 10}^{\rm 2}$ etex, 243.9a, 239.5b, 0, 3);
draw (261.6a,267.9b)--(269.9a,267.9b);
draw (1418.4a,267.9b)--(1410.1a,267.9b);
draw (261.6a,305.4b)--(269.9a,305.4b);
draw (1418.4a,305.4b)--(1410.1a,305.4b);
draw (261.6a,324.6b)--(269.9a,324.6b);
draw (1418.4a,324.6b)--(1410.1a,324.6b);
draw (261.6a,333.8b)--(278.2a,333.8b);
draw (1418.4a,333.8b)--(1401.8a,333.8b);
put_text( btex  ${\rm 10}^{\rm 3}$ etex, 243.9a, 333.8b, 0, 3);
draw (261.6a,362.1b)--(269.9a,362.1b);
draw (1418.4a,362.1b)--(1410.1a,362.1b);
draw (261.6a,399.6b)--(269.9a,399.6b);
draw (1418.4a,399.6b)--(1410.1a,399.6b);
draw (261.6a,418.9b)--(269.9a,418.9b);
draw (1418.4a,418.9b)--(1410.1a,418.9b);
draw (261.6a,428.0b)--(278.2a,428.0b);
draw (1418.4a,428.0b)--(1401.8a,428.0b);
put_text( btex  ${\rm 10}^{\rm 4}$ etex, 243.9a, 428.0b, 0, 3);
draw (261.6a,456.4b)--(269.9a,456.4b);
draw (1418.4a,456.4b)--(1410.1a,456.4b);
draw (261.6a,493.9b)--(269.9a,493.9b);
draw (1418.4a,493.9b)--(1410.1a,493.9b);
draw (261.6a,513.2b)--(269.9a,513.2b);
draw (1418.4a,513.2b)--(1410.1a,513.2b);
draw (261.6a,522.3b)--(278.2a,522.3b);
draw (1418.4a,522.3b)--(1401.8a,522.3b);
put_text( btex  ${\rm 10}^{\rm 5}$ etex, 243.9a, 522.3b, 0, 3);
draw (261.6a,550.7b)--(269.9a,550.7b);
draw (1418.4a,550.7b)--(1410.1a,550.7b);
draw (261.6a,588.2b)--(269.9a,588.2b);
draw (1418.4a,588.2b)--(1410.1a,588.2b);
draw (261.6a,607.4b)--(269.9a,607.4b);
draw (1418.4a,607.4b)--(1410.1a,607.4b);
draw (261.6a,616.6b)--(278.2a,616.6b);
draw (1418.4a,616.6b)--(1401.8a,616.6b);
put_text( btex  ${\rm 10}^{\rm 6}$ etex, 243.9a, 616.6b, 0, 3);
draw (261.6a,645.0b)--(269.9a,645.0b);
draw (1418.4a,645.0b)--(1410.1a,645.0b);
draw (261.6a,682.5b)--(269.9a,682.5b);
draw (1418.4a,682.5b)--(1410.1a,682.5b);
draw (261.6a,701.7b)--(269.9a,701.7b);
draw (1418.4a,701.7b)--(1410.1a,701.7b);
draw (261.6a,710.9b)--(278.2a,710.9b);
draw (1418.4a,710.9b)--(1401.8a,710.9b);
put_text( btex  ${\rm 10}^{\rm 7}$ etex, 243.9a, 710.9b, 0, 3);
draw (261.6a,739.2b)--(269.9a,739.2b);
draw (1418.4a,739.2b)--(1410.1a,739.2b);
draw (261.6a,776.7b)--(269.9a,776.7b);
draw (1418.4a,776.7b)--(1410.1a,776.7b);
draw (261.6a,796.0b)--(269.9a,796.0b);
draw (1418.4a,796.0b)--(1410.1a,796.0b);
draw (261.6a,805.1b)--(278.2a,805.1b);
draw (1418.4a,805.1b)--(1401.8a,805.1b);
put_text( btex  ${\rm 10}^{\rm 8}$ etex, 243.9a, 805.1b, 0, 3);
draw (261.6a,833.5b)--(269.9a,833.5b);
draw (1418.4a,833.5b)--(1410.1a,833.5b);
draw (261.6a,871.0b)--(269.9a,871.0b);
draw (1418.4a,871.0b)--(1410.1a,871.0b);
draw (261.6a,890.3b)--(269.9a,890.3b);
draw (1418.4a,890.3b)--(1410.1a,890.3b);
draw (261.6a,899.4b)--(278.2a,899.4b);
draw (1418.4a,899.4b)--(1401.8a,899.4b);
put_text( btex  ${\rm 10}^{\rm 9}$ etex, 243.9a, 899.4b, 0, 3);
draw (261.6a,145.2b)--(261.6a,161.8b);
draw (261.6a,899.4b)--(261.6a,882.8b);
put_text( btex  1 etex, 261.6a, 108.6b, 0, 2);
draw (331.2a,145.2b)--(331.2a,153.5b);
draw (331.2a,899.4b)--(331.2a,891.1b);
draw (423.3a,145.2b)--(423.3a,153.5b);
draw (423.3a,899.4b)--(423.3a,891.1b);
draw (470.5a,145.2b)--(470.5a,153.5b);
draw (470.5a,899.4b)--(470.5a,891.1b);
draw (493.0a,145.2b)--(493.0a,161.8b);
draw (493.0a,899.4b)--(493.0a,882.8b);
put_text( btex  10 etex, 493.0a, 108.6b, 0, 2);
draw (562.6a,145.2b)--(562.6a,153.5b);
draw (562.6a,899.4b)--(562.6a,891.1b);
draw (654.7a,145.2b)--(654.7a,153.5b);
draw (654.7a,899.4b)--(654.7a,891.1b);
draw (701.9a,145.2b)--(701.9a,153.5b);
draw (701.9a,899.4b)--(701.9a,891.1b);
draw (724.3a,145.2b)--(724.3a,161.8b);
draw (724.3a,899.4b)--(724.3a,882.8b);
put_text( btex  100 etex, 724.3a, 108.6b, 0, 2);
draw (794.0a,145.2b)--(794.0a,153.5b);
draw (794.0a,899.4b)--(794.0a,891.1b);
draw (886.0a,145.2b)--(886.0a,153.5b);
draw (886.0a,899.4b)--(886.0a,891.1b);
draw (933.3a,145.2b)--(933.3a,153.5b);
draw (933.3a,899.4b)--(933.3a,891.1b);
draw (955.7a,145.2b)--(955.7a,161.8b);
draw (955.7a,899.4b)--(955.7a,882.8b);
put_text( btex  1000 etex, 955.7a, 108.6b, 0, 2);
draw (1025.3a,145.2b)--(1025.3a,153.5b);
draw (1025.3a,899.4b)--(1025.3a,891.1b);
draw (1117.4a,145.2b)--(1117.4a,153.5b);
draw (1117.4a,899.4b)--(1117.4a,891.1b);
draw (1164.6a,145.2b)--(1164.6a,153.5b);
draw (1164.6a,899.4b)--(1164.6a,891.1b);
draw (1187.0a,145.2b)--(1187.0a,161.8b);
draw (1187.0a,899.4b)--(1187.0a,882.8b);
put_text( btex  10,000 etex, 1187.0a, 108.6b, 0, 2);
draw (1256.7a,145.2b)--(1256.7a,153.5b);
draw (1256.7a,899.4b)--(1256.7a,891.1b);
draw (1348.8a,145.2b)--(1348.8a,153.5b);
draw (1348.8a,899.4b)--(1348.8a,891.1b);
draw (1396.0a,145.2b)--(1396.0a,153.5b);
draw (1396.0a,899.4b)--(1396.0a,891.1b);
draw (1418.4a,145.2b)--(1418.4a,161.8b);
draw (1418.4a,899.4b)--(1418.4a,882.8b);
put_text( btex  100,000 etex, 1418.4a, 108.6b, 0, 2);
draw (261.6a,145.2b)--(1418.4a,145.2b)--(1418.4a,899.4b)--(261.6a,899.4b)--(261.6a,145.2b);
put_text( btex Contributions to Time Step Duration (cycles) etex, 84.0a, 522.3b, 1, 2);
put_text( btex Node Count etex, 840.0a, 53.7b, 0, 2);
put_text( btex Position Globalization etex, 840.0a, 844.5b, 0, 2);
linetype 0;
put_text( btex Total etex, 615.6a, 326.5b, 0, 3);
draw (633.3a,326.5b)--(720.7a,326.5b);
draw (466.0a,899.4b)--(470.5a,897.6b)--(609.8a,844.3b)--(679.5a,816.1b)--(749.1a,792.1b)
--(818.8a,766.3b)--(888.4a,739.4b)--(958.1a,719.6b)--(1027.7a,710.0b)--(1097.4a,702.1b)
--(1167.0a,732.7b)--(1236.6a,742.2b);
linetype 1;
put_text( btex Verlet etex, 615.6a, 289.9b, 0, 3);
draw (633.3a,289.9b)--(720.7a,289.9b);
draw (261.6a,603.1b)--(470.5a,518.0b)--(609.8a,461.2b)--(679.5a,432.8b)--(749.1a,404.5b)
--(818.8a,376.1b)--(888.4a,347.7b)--(958.1a,319.3b)--(1027.7a,291.0b)--(1097.4a,262.6b)
--(1167.0a,234.2b)--(1236.6a,205.8b);
linetype 2;
put_text( btex Globalization etex, 615.6a, 253.3b, 0, 3);
draw (633.3a,253.3b)--(720.7a,253.3b);
draw (470.5a,631.5b)--(609.8a,631.5b)--(679.5a,631.5b)--(749.1a,631.5b)--(818.8a,631.5b)
--(888.4a,631.5b)--(958.1a,631.5b)--(1027.7a,631.5b)--(1097.4a,631.5b)--(1167.0a,631.5b)
--(1236.6a,631.5b);
linetype 3;
put_text( btex FFT etex, 615.6a, 216.7b, 0, 3);
draw (633.3a,216.7b)--(720.7a,216.7b);
draw (374.1a,899.4b)--(470.5a,864.0b)--(609.8a,814.2b)--(679.5a,785.9b)--(749.1a,765.3b)
--(818.8a,740.3b)--(888.4a,711.9b)--(958.1a,694.9b)--(1027.7a,691.6b)--(1097.4a,687.0b)
--(1167.0a,727.7b)--(1236.6a,738.9b);
linetype 4;
put_text( btex Real space non-bond etex, 615.6a, 180.1b, 0, 3);
draw (633.3a,180.1b)--(720.7a,180.1b);
draw (407.7a,899.4b)--(470.5a,873.8b)--(609.8a,817.0b)--(679.5a,788.7b)--(749.1a,760.3b)
--(818.8a,731.9b)--(888.4a,703.5b)--(958.1a,675.1b)--(1027.7a,646.8b)--(1097.4a,618.4b)
--(1167.0a,590.0b)--(1236.6a,561.6b);
linetype -2;
draw (261.6a,1009.2b)--(278.2a,1009.2b);
draw (1418.4a,1009.2b)--(1401.8a,1009.2b);
put_text( btex  ${\rm 10}$ etex, 243.9a, 1009.2b, 0, 3);
draw (261.6a,1037.6b)--(269.9a,1037.6b);
draw (1418.4a,1037.6b)--(1410.1a,1037.6b);
draw (261.6a,1075.1b)--(269.9a,1075.1b);
draw (1418.4a,1075.1b)--(1410.1a,1075.1b);
draw (261.6a,1094.3b)--(269.9a,1094.3b);
draw (1418.4a,1094.3b)--(1410.1a,1094.3b);
draw (261.6a,1103.5b)--(278.2a,1103.5b);
draw (1418.4a,1103.5b)--(1401.8a,1103.5b);
put_text( btex  ${\rm 10}^{\rm 2}$ etex, 243.9a, 1103.5b, 0, 3);
draw (261.6a,1131.9b)--(269.9a,1131.9b);
draw (1418.4a,1131.9b)--(1410.1a,1131.9b);
draw (261.6a,1169.4b)--(269.9a,1169.4b);
draw (1418.4a,1169.4b)--(1410.1a,1169.4b);
draw (261.6a,1188.6b)--(269.9a,1188.6b);
draw (1418.4a,1188.6b)--(1410.1a,1188.6b);
draw (261.6a,1197.8b)--(278.2a,1197.8b);
draw (1418.4a,1197.8b)--(1401.8a,1197.8b);
put_text( btex  ${\rm 10}^{\rm 3}$ etex, 243.9a, 1197.8b, 0, 3);
draw (261.6a,1226.1b)--(269.9a,1226.1b);
draw (1418.4a,1226.1b)--(1410.1a,1226.1b);
draw (261.6a,1263.6b)--(269.9a,1263.6b);
draw (1418.4a,1263.6b)--(1410.1a,1263.6b);
draw (261.6a,1282.9b)--(269.9a,1282.9b);
draw (1418.4a,1282.9b)--(1410.1a,1282.9b);
draw (261.6a,1292.0b)--(278.2a,1292.0b);
draw (1418.4a,1292.0b)--(1401.8a,1292.0b);
put_text( btex  ${\rm 10}^{\rm 4}$ etex, 243.9a, 1292.0b, 0, 3);
draw (261.6a,1320.4b)--(269.9a,1320.4b);
draw (1418.4a,1320.4b)--(1410.1a,1320.4b);
draw (261.6a,1357.9b)--(269.9a,1357.9b);
draw (1418.4a,1357.9b)--(1410.1a,1357.9b);
draw (261.6a,1377.2b)--(269.9a,1377.2b);
draw (1418.4a,1377.2b)--(1410.1a,1377.2b);
draw (261.6a,1386.3b)--(278.2a,1386.3b);
draw (1418.4a,1386.3b)--(1401.8a,1386.3b);
put_text( btex  ${\rm 10}^{\rm 5}$ etex, 243.9a, 1386.3b, 0, 3);
draw (261.6a,1414.7b)--(269.9a,1414.7b);
draw (1418.4a,1414.7b)--(1410.1a,1414.7b);
draw (261.6a,1452.2b)--(269.9a,1452.2b);
draw (1418.4a,1452.2b)--(1410.1a,1452.2b);
draw (261.6a,1471.4b)--(269.9a,1471.4b);
draw (1418.4a,1471.4b)--(1410.1a,1471.4b);
draw (261.6a,1480.6b)--(278.2a,1480.6b);
draw (1418.4a,1480.6b)--(1401.8a,1480.6b);
put_text( btex  ${\rm 10}^{\rm 6}$ etex, 243.9a, 1480.6b, 0, 3);
draw (261.6a,1509.0b)--(269.9a,1509.0b);
draw (1418.4a,1509.0b)--(1410.1a,1509.0b);
draw (261.6a,1546.5b)--(269.9a,1546.5b);
draw (1418.4a,1546.5b)--(1410.1a,1546.5b);
draw (261.6a,1565.7b)--(269.9a,1565.7b);
draw (1418.4a,1565.7b)--(1410.1a,1565.7b);
draw (261.6a,1574.9b)--(278.2a,1574.9b);
draw (1418.4a,1574.9b)--(1401.8a,1574.9b);
put_text( btex  ${\rm 10}^{\rm 7}$ etex, 243.9a, 1574.9b, 0, 3);
draw (261.6a,1603.2b)--(269.9a,1603.2b);
draw (1418.4a,1603.2b)--(1410.1a,1603.2b);
draw (261.6a,1640.7b)--(269.9a,1640.7b);
draw (1418.4a,1640.7b)--(1410.1a,1640.7b);
draw (261.6a,1660.0b)--(269.9a,1660.0b);
draw (1418.4a,1660.0b)--(1410.1a,1660.0b);
draw (261.6a,1669.1b)--(278.2a,1669.1b);
draw (1418.4a,1669.1b)--(1401.8a,1669.1b);
put_text( btex  ${\rm 10}^{\rm 8}$ etex, 243.9a, 1669.1b, 0, 3);
draw (261.6a,1697.5b)--(269.9a,1697.5b);
draw (1418.4a,1697.5b)--(1410.1a,1697.5b);
draw (261.6a,1735.0b)--(269.9a,1735.0b);
draw (1418.4a,1735.0b)--(1410.1a,1735.0b);
draw (261.6a,1754.3b)--(269.9a,1754.3b);
draw (1418.4a,1754.3b)--(1410.1a,1754.3b);
draw (261.6a,1763.4b)--(278.2a,1763.4b);
draw (1418.4a,1763.4b)--(1401.8a,1763.4b);
put_text( btex  ${\rm 10}^{\rm 9}$ etex, 243.9a, 1763.4b, 0, 3);
draw (261.6a,1009.2b)--(261.6a,1025.8b);
draw (261.6a,1763.4b)--(261.6a,1746.8b);
put_text( btex  1 etex, 261.6a, 972.6b, 0, 2);
draw (331.2a,1009.2b)--(331.2a,1017.5b);
draw (331.2a,1763.4b)--(331.2a,1755.1b);
draw (423.3a,1009.2b)--(423.3a,1017.5b);
draw (423.3a,1763.4b)--(423.3a,1755.1b);
draw (470.5a,1009.2b)--(470.5a,1017.5b);
draw (470.5a,1763.4b)--(470.5a,1755.1b);
draw (493.0a,1009.2b)--(493.0a,1025.8b);
draw (493.0a,1763.4b)--(493.0a,1746.8b);
put_text( btex  10 etex, 493.0a, 972.6b, 0, 2);
draw (562.6a,1009.2b)--(562.6a,1017.5b);
draw (562.6a,1763.4b)--(562.6a,1755.1b);
draw (654.7a,1009.2b)--(654.7a,1017.5b);
draw (654.7a,1763.4b)--(654.7a,1755.1b);
draw (701.9a,1009.2b)--(701.9a,1017.5b);
draw (701.9a,1763.4b)--(701.9a,1755.1b);
draw (724.3a,1009.2b)--(724.3a,1025.8b);
draw (724.3a,1763.4b)--(724.3a,1746.8b);
put_text( btex  100 etex, 724.3a, 972.6b, 0, 2);
draw (794.0a,1009.2b)--(794.0a,1017.5b);
draw (794.0a,1763.4b)--(794.0a,1755.1b);
draw (886.0a,1009.2b)--(886.0a,1017.5b);
draw (886.0a,1763.4b)--(886.0a,1755.1b);
draw (933.3a,1009.2b)--(933.3a,1017.5b);
draw (933.3a,1763.4b)--(933.3a,1755.1b);
draw (955.7a,1009.2b)--(955.7a,1025.8b);
draw (955.7a,1763.4b)--(955.7a,1746.8b);
put_text( btex  1000 etex, 955.7a, 972.6b, 0, 2);
draw (1025.3a,1009.2b)--(1025.3a,1017.5b);
draw (1025.3a,1763.4b)--(1025.3a,1755.1b);
draw (1117.4a,1009.2b)--(1117.4a,1017.5b);
draw (1117.4a,1763.4b)--(1117.4a,1755.1b);
draw (1164.6a,1009.2b)--(1164.6a,1017.5b);
draw (1164.6a,1763.4b)--(1164.6a,1755.1b);
draw (1187.0a,1009.2b)--(1187.0a,1025.8b);
draw (1187.0a,1763.4b)--(1187.0a,1746.8b);
put_text( btex  10,000 etex, 1187.0a, 972.6b, 0, 2);
draw (1256.7a,1009.2b)--(1256.7a,1017.5b);
draw (1256.7a,1763.4b)--(1256.7a,1755.1b);
draw (1348.8a,1009.2b)--(1348.8a,1017.5b);
draw (1348.8a,1763.4b)--(1348.8a,1755.1b);
draw (1396.0a,1009.2b)--(1396.0a,1017.5b);
draw (1396.0a,1763.4b)--(1396.0a,1755.1b);
draw (1418.4a,1009.2b)--(1418.4a,1025.8b);
draw (1418.4a,1763.4b)--(1418.4a,1746.8b);
put_text( btex  100,000 etex, 1418.4a, 972.6b, 0, 2);
draw (261.6a,1009.2b)--(1418.4a,1009.2b)--(1418.4a,1763.4b)--(261.6a,1763.4b)--(261.6a,1009.2b);
put_text( btex Contributions to Time Step Duration (cycles) etex, 84.0a, 1386.3b, 1, 2);
put_text( btex Node Count etex, 840.0a, 917.7b, 0, 2);
put_text( btex Global Force Reduction etex, 840.0a, 1708.5b, 0, 2);
linetype 0;
put_text( btex Total etex, 615.6a, 1190.5b, 0, 3);
draw (633.3a,1190.5b)--(720.7a,1190.5b);
draw (431.0a,1763.4b)--(470.5a,1748.3b)--(609.8a,1696.3b)--(679.5a,1668.2b)--(749.1a,1645.7b)
--(818.8a,1621.0b)--(888.4a,1594.9b)--(958.1a,1577.7b)--(1027.7a,1571.2b)--(1097.4a,1565.4b)
--(1167.0a,1597.0b)--(1236.6a,1606.7b);
linetype 1;
put_text( btex Verlet etex, 615.6a, 1153.9b, 0, 3);
draw (633.3a,1153.9b)--(720.7a,1153.9b);
draw (261.6a,1467.1b)--(470.5a,1459.4b)--(609.8a,1459.4b)--(679.5a,1402.3b)--(749.1a,1402.3b)
--(818.8a,1402.3b)--(888.4a,1359.2b)--(958.1a,1359.2b)--(1027.7a,1359.2b)--(1097.4a,1330.3b)
--(1167.0a,1330.3b)--(1236.6a,1330.3b);
linetype 2;
put_text( btex Force Reduction etex, 615.6a, 1117.3b, 0, 3);
draw (633.3a,1117.3b)--(720.7a,1117.3b);
draw (470.5a,1504.6b)--(609.8a,1504.6b)--(679.5a,1504.6b)--(749.1a,1504.6b)--(818.8a,1504.6b)
--(888.4a,1504.6b)--(958.1a,1504.6b)--(1027.7a,1504.6b)--(1097.4a,1504.6b)--(1167.0a,1504.6b)
--(1236.6a,1504.6b);
linetype 3;
put_text( btex FFT etex, 615.6a, 1080.7b, 0, 3);
draw (633.3a,1080.7b)--(720.7a,1080.7b);
draw (374.1a,1763.4b)--(470.5a,1728.0b)--(609.8a,1678.2b)--(679.5a,1649.9b)--(749.1a,1629.3b)
--(818.8a,1604.3b)--(888.4a,1575.9b)--(958.1a,1558.9b)--(1027.7a,1555.6b)--(1097.4a,1551.0b)
--(1167.0a,1591.7b)--(1236.6a,1602.9b);
linetype 4;
put_text( btex Real space non-bond etex, 615.6a, 1044.1b, 0, 3);
draw (633.3a,1044.1b)--(720.7a,1044.1b);
draw (338.1a,1763.4b)--(470.5a,1709.4b)--(609.8a,1652.7b)--(679.5a,1624.3b)--(749.1a,1595.9b)
--(818.8a,1567.5b)--(888.4a,1539.1b)--(958.1a,1510.8b)--(1027.7a,1482.4b)--(1097.4a,1454.0b)
--(1167.0a,1425.6b)--(1236.6a,1397.2b);
endfig;
end.
