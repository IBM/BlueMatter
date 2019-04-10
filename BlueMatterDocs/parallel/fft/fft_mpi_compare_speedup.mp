%GNUPLOT Metapost output: Sun Apr 25 01:36:14 2004


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

beginfig(23);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (178.8a,109.8b)--(195.4a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  1 etex, 161.1a, 109.8b, 0, 3);
draw (178.8a,181.9b)--(187.1a,181.9b);
draw (1386.3a,181.9b)--(1378.0a,181.9b);
draw (178.8a,224.1b)--(187.1a,224.1b);
draw (1386.3a,224.1b)--(1378.0a,224.1b);
draw (178.8a,254.1b)--(187.1a,254.1b);
draw (1386.3a,254.1b)--(1378.0a,254.1b);
draw (178.8a,277.3b)--(187.1a,277.3b);
draw (1386.3a,277.3b)--(1378.0a,277.3b);
draw (178.8a,296.2b)--(187.1a,296.2b);
draw (1386.3a,296.2b)--(1378.0a,296.2b);
draw (178.8a,312.3b)--(187.1a,312.3b);
draw (1386.3a,312.3b)--(1378.0a,312.3b);
draw (178.8a,326.2b)--(187.1a,326.2b);
draw (1386.3a,326.2b)--(1378.0a,326.2b);
draw (178.8a,338.4b)--(187.1a,338.4b);
draw (1386.3a,338.4b)--(1378.0a,338.4b);
draw (178.8a,349.4b)--(195.4a,349.4b);
draw (1386.3a,349.4b)--(1369.7a,349.4b);
put_text( btex  10 etex, 161.1a, 349.4b, 0, 3);
draw (178.8a,421.5b)--(187.1a,421.5b);
draw (1386.3a,421.5b)--(1378.0a,421.5b);
draw (178.8a,463.7b)--(187.1a,463.7b);
draw (1386.3a,463.7b)--(1378.0a,463.7b);
draw (178.8a,493.7b)--(187.1a,493.7b);
draw (1386.3a,493.7b)--(1378.0a,493.7b);
draw (178.8a,516.9b)--(187.1a,516.9b);
draw (1386.3a,516.9b)--(1378.0a,516.9b);
draw (178.8a,535.8b)--(187.1a,535.8b);
draw (1386.3a,535.8b)--(1378.0a,535.8b);
draw (178.8a,551.9b)--(187.1a,551.9b);
draw (1386.3a,551.9b)--(1378.0a,551.9b);
draw (178.8a,565.8b)--(187.1a,565.8b);
draw (1386.3a,565.8b)--(1378.0a,565.8b);
draw (178.8a,578.0b)--(187.1a,578.0b);
draw (1386.3a,578.0b)--(1378.0a,578.0b);
draw (178.8a,589.0b)--(195.4a,589.0b);
draw (1386.3a,589.0b)--(1369.7a,589.0b);
put_text( btex  100 etex, 161.1a, 589.0b, 0, 3);
draw (178.8a,661.1b)--(187.1a,661.1b);
draw (1386.3a,661.1b)--(1378.0a,661.1b);
draw (178.8a,703.3b)--(187.1a,703.3b);
draw (1386.3a,703.3b)--(1378.0a,703.3b);
draw (178.8a,733.3b)--(187.1a,733.3b);
draw (1386.3a,733.3b)--(1378.0a,733.3b);
draw (178.8a,756.5b)--(187.1a,756.5b);
draw (1386.3a,756.5b)--(1378.0a,756.5b);
draw (178.8a,775.4b)--(187.1a,775.4b);
draw (1386.3a,775.4b)--(1378.0a,775.4b);
draw (178.8a,791.5b)--(187.1a,791.5b);
draw (1386.3a,791.5b)--(1378.0a,791.5b);
draw (178.8a,805.4b)--(187.1a,805.4b);
draw (1386.3a,805.4b)--(1378.0a,805.4b);
draw (178.8a,817.6b)--(187.1a,817.6b);
draw (1386.3a,817.6b)--(1378.0a,817.6b);
draw (178.8a,828.6b)--(195.4a,828.6b);
draw (1386.3a,828.6b)--(1369.7a,828.6b);
put_text( btex  1000 etex, 161.1a, 828.6b, 0, 3);
draw (178.8a,109.8b)--(178.8a,126.4b);
draw (178.8a,828.6b)--(178.8a,812.0b);
put_text( btex  1 etex, 178.8a, 73.2b, 0, 2);
draw (300.0a,109.8b)--(300.0a,118.1b);
draw (300.0a,828.6b)--(300.0a,820.3b);
draw (370.8a,109.8b)--(370.8a,118.1b);
draw (370.8a,828.6b)--(370.8a,820.3b);
draw (421.1a,109.8b)--(421.1a,118.1b);
draw (421.1a,828.6b)--(421.1a,820.3b);
draw (460.1a,109.8b)--(460.1a,118.1b);
draw (460.1a,828.6b)--(460.1a,820.3b);
draw (492.0a,109.8b)--(492.0a,118.1b);
draw (492.0a,828.6b)--(492.0a,820.3b);
draw (519.0a,109.8b)--(519.0a,118.1b);
draw (519.0a,828.6b)--(519.0a,820.3b);
draw (542.3a,109.8b)--(542.3a,118.1b);
draw (542.3a,828.6b)--(542.3a,820.3b);
draw (562.9a,109.8b)--(562.9a,118.1b);
draw (562.9a,828.6b)--(562.9a,820.3b);
draw (581.3a,109.8b)--(581.3a,126.4b);
draw (581.3a,828.6b)--(581.3a,812.0b);
put_text( btex  10 etex, 581.3a, 73.2b, 0, 2);
draw (702.5a,109.8b)--(702.5a,118.1b);
draw (702.5a,828.6b)--(702.5a,820.3b);
draw (773.3a,109.8b)--(773.3a,118.1b);
draw (773.3a,828.6b)--(773.3a,820.3b);
draw (823.6a,109.8b)--(823.6a,118.1b);
draw (823.6a,828.6b)--(823.6a,820.3b);
draw (862.6a,109.8b)--(862.6a,118.1b);
draw (862.6a,828.6b)--(862.6a,820.3b);
draw (894.5a,109.8b)--(894.5a,118.1b);
draw (894.5a,828.6b)--(894.5a,820.3b);
draw (921.5a,109.8b)--(921.5a,118.1b);
draw (921.5a,828.6b)--(921.5a,820.3b);
draw (944.8a,109.8b)--(944.8a,118.1b);
draw (944.8a,828.6b)--(944.8a,820.3b);
draw (965.4a,109.8b)--(965.4a,118.1b);
draw (965.4a,828.6b)--(965.4a,820.3b);
draw (983.8a,109.8b)--(983.8a,126.4b);
draw (983.8a,828.6b)--(983.8a,812.0b);
put_text( btex  100 etex, 983.8a, 73.2b, 0, 2);
draw (1105.0a,109.8b)--(1105.0a,118.1b);
draw (1105.0a,828.6b)--(1105.0a,820.3b);
draw (1175.8a,109.8b)--(1175.8a,118.1b);
draw (1175.8a,828.6b)--(1175.8a,820.3b);
draw (1226.1a,109.8b)--(1226.1a,118.1b);
draw (1226.1a,828.6b)--(1226.1a,820.3b);
draw (1265.1a,109.8b)--(1265.1a,118.1b);
draw (1265.1a,828.6b)--(1265.1a,820.3b);
draw (1297.0a,109.8b)--(1297.0a,118.1b);
draw (1297.0a,828.6b)--(1297.0a,820.3b);
draw (1324.0a,109.8b)--(1324.0a,118.1b);
draw (1324.0a,828.6b)--(1324.0a,820.3b);
draw (1347.3a,109.8b)--(1347.3a,118.1b);
draw (1347.3a,828.6b)--(1347.3a,820.3b);
draw (1367.9a,109.8b)--(1367.9a,118.1b);
draw (1367.9a,828.6b)--(1367.9a,820.3b);
draw (1386.3a,109.8b)--(1386.3a,126.4b);
draw (1386.3a,828.6b)--(1386.3a,812.0b);
put_text( btex  1000 etex, 1386.3a, 73.2b, 0, 2);
draw (178.8a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(178.8a,828.6b)--(178.8a,109.8b);
put_text( btex Speedup etex, 36.6a, 469.2b, 1, 2);
put_text( btex Task Count etex, 782.5a, 18.3b, 0, 2);
linetype 0;
put_text( btex $128^3$ MPI Collective etex, 524.6a, 791.5b, 0, 3);
draw (542.3a,791.5b)--(629.7a,791.5b);
draw (178.8a,109.8b)--(542.3a,310.8b)--(784.6a,498.8b)--(1027.0a,636.3b)--(1148.1a,706.0b)
--(1269.3a,750.1b);
gpdraw(0,178.8a,109.8b);
gpdraw(0,542.3a,310.8b);
gpdraw(0,784.6a,498.8b);
gpdraw(0,1027.0a,636.3b);
gpdraw(0,1148.1a,706.0b);
gpdraw(0,1269.3a,750.1b);
gpdraw(0,586.0a,791.5b);
linetype 1;
put_text( btex $128^3$ Active Packet etex, 524.6a, 754.9b, 0, 3);
draw (542.3a,754.9b)--(629.7a,754.9b);
draw (542.3a,326.2b)--(784.6a,477.6b)--(1027.0a,590.1b)--(1269.3a,685.7b);
gpdraw(1,542.3a,326.2b);
gpdraw(1,784.6a,477.6b);
gpdraw(1,1027.0a,590.1b);
gpdraw(1,1269.3a,685.7b);
gpdraw(1,586.0a,754.9b);
linetype 2;
put_text( btex $128^3$ Model etex, 524.6a, 718.3b, 0, 3);
draw (542.3a,718.3b)--(629.7a,718.3b);
draw (178.8a,109.8b)--(542.3a,185.7b)--(784.6a,272.3b)--(905.8a,344.4b)--(1027.0a,369.8b)
--(1148.1a,424.6b)--(1269.3a,496.7b)--(1386.3a,518.3b);
linetype 3;
put_text( btex $64^3$ MPI Collective etex, 524.6a, 681.7b, 0, 3);
draw (542.3a,681.7b)--(629.7a,681.7b);
draw (178.8a,109.8b)--(542.3a,344.1b)--(784.6a,500.0b)--(1027.0a,586.6b)--(1148.1a,639.1b)
--(1269.3a,645.2b);
gpdraw(2,178.8a,109.8b);
gpdraw(2,542.3a,344.1b);
gpdraw(2,784.6a,500.0b);
gpdraw(2,1027.0a,586.6b);
gpdraw(2,1148.1a,639.1b);
gpdraw(2,1269.3a,645.2b);
gpdraw(2,586.0a,681.7b);
linetype 4;
put_text( btex $64^3$ Active Packet etex, 524.6a, 645.1b, 0, 3);
draw (542.3a,645.1b)--(629.7a,645.1b);
draw (178.8a,109.8b)--(542.3a,345.2b)--(784.6a,474.0b)--(1027.0a,582.0b)--(1269.3a,681.1b);
gpdraw(3,178.8a,109.8b);
gpdraw(3,542.3a,345.2b);
gpdraw(3,784.6a,474.0b);
gpdraw(3,1027.0a,582.0b);
gpdraw(3,1269.3a,681.1b);
gpdraw(3,586.0a,645.1b);
linetype 5;
put_text( btex $64^3$ Model etex, 524.6a, 608.5b, 0, 3);
draw (542.3a,608.5b)--(629.7a,608.5b);
draw (178.8a,109.8b)--(542.3a,173.6b)--(784.6a,258.5b)--(905.8a,330.6b)--(1027.0a,355.2b)
--(1148.1a,409.7b)--(1269.3a,481.9b)--(1386.3a,503.0b);
linetype 6;
put_text( btex $32^3$ MPI Collective etex, 524.6a, 571.9b, 0, 3);
draw (542.3a,571.9b)--(629.7a,571.9b);
draw (178.8a,109.8b)--(542.3a,260.2b)--(784.6a,390.5b)--(1027.0a,426.7b)--(1148.1a,422.0b);
gpdraw(4,178.8a,109.8b);
gpdraw(4,542.3a,260.2b);
gpdraw(4,784.6a,390.5b);
gpdraw(4,1027.0a,426.7b);
gpdraw(4,1148.1a,422.0b);
gpdraw(4,586.0a,571.9b);
linetype 7;
put_text( btex $32^3$ Active Packet etex, 524.6a, 535.3b, 0, 3);
draw (542.3a,535.3b)--(629.7a,535.3b);
draw (178.8a,109.8b)--(542.3a,326.2b)--(784.6a,446.0b)--(1027.0a,552.6b)--(1269.3a,597.8b);
gpdraw(5,178.8a,109.8b);
gpdraw(5,542.3a,326.2b);
gpdraw(5,784.6a,446.0b);
gpdraw(5,1027.0a,552.6b);
gpdraw(5,1269.3a,597.8b);
gpdraw(5,586.0a,535.3b);
linetype 0;
put_text( btex $32^3$ Model etex, 524.6a, 498.7b, 0, 3);
draw (542.3a,498.7b)--(629.7a,498.7b);
draw (178.8a,109.8b)--(542.3a,158.7b)--(784.6a,241.8b)--(905.8a,313.9b)--(1027.0a,337.6b)
--(1148.1a,392.0b)--(1269.3a,464.1b)--(1386.3a,484.8b);
linetype 1;
put_text( btex Ideal Speedup etex, 524.6a, 462.1b, 0, 3);
draw (542.3a,462.1b)--(629.7a,462.1b);
draw (178.8a,109.8b)--(191.0a,117.1b)--(203.2a,124.3b)--(215.4a,131.6b)--(227.6a,138.8b)
--(239.8a,146.1b)--(252.0a,153.4b)--(264.2a,160.6b)--(276.4a,167.9b)--(288.6a,175.1b)
--(300.8a,182.4b)--(313.0a,189.7b)--(325.2a,196.9b)--(337.4a,204.2b)--(349.6a,211.4b)
--(361.8a,218.7b)--(374.0a,226.0b)--(386.1a,233.2b)--(398.3a,240.5b)--(410.5a,247.8b)
--(422.7a,255.0b)--(434.9a,262.3b)--(447.1a,269.5b)--(459.3a,276.8b)--(471.5a,284.1b)
--(483.7a,291.3b)--(495.9a,298.6b)--(508.1a,305.8b)--(520.3a,313.1b)--(532.5a,320.4b)
--(544.7a,327.6b)--(556.9a,334.9b)--(569.1a,342.1b)--(581.3a,349.4b)--(593.5a,356.7b)
--(605.7a,363.9b)--(617.9a,371.2b)--(630.1a,378.4b)--(642.3a,385.7b)--(654.5a,393.0b)
--(666.7a,400.2b)--(678.9a,407.5b)--(691.1a,414.7b)--(703.3a,422.0b)--(715.5a,429.3b)
--(727.7a,436.5b)--(739.9a,443.8b)--(752.1a,451.0b)--(764.3a,458.3b)--(776.5a,465.6b)
--(788.6a,472.8b)--(800.8a,480.1b)--(813.0a,487.4b)--(825.2a,494.6b)--(837.4a,501.9b)
--(849.6a,509.1b)--(861.8a,516.4b)--(874.0a,523.7b)--(886.2a,530.9b)--(898.4a,538.2b)
--(910.6a,545.4b)--(922.8a,552.7b)--(935.0a,560.0b)--(947.2a,567.2b)--(959.4a,574.5b)
--(971.6a,581.7b)--(983.8a,589.0b)--(996.0a,596.3b)--(1008.2a,603.5b)--(1020.4a,610.8b)
--(1032.6a,618.0b)--(1044.8a,625.3b)--(1057.0a,632.6b)--(1069.2a,639.8b)--(1081.4a,647.1b)
--(1093.6a,654.3b)--(1105.8a,661.6b)--(1118.0a,668.9b)--(1130.2a,676.1b)--(1142.4a,683.4b)
--(1154.6a,690.6b)--(1166.8a,697.9b)--(1179.0a,705.2b)--(1191.1a,712.4b)--(1203.3a,719.7b)
--(1215.5a,727.0b)--(1227.7a,734.2b)--(1239.9a,741.5b)--(1252.1a,748.7b)--(1264.3a,756.0b)
--(1276.5a,763.3b)--(1288.7a,770.5b)--(1300.9a,777.8b)--(1313.1a,785.0b)--(1325.3a,792.3b)
--(1337.5a,799.6b)--(1349.7a,806.8b)--(1361.9a,814.1b)--(1374.1a,821.3b)--(1386.3a,828.6b);
endfig;
end.
.6b);
endfig;
end.
