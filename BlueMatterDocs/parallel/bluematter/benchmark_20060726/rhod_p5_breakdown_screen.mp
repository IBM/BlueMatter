%GNUPLOT Metapost output: Sat Aug 26 01:00:44 2006


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage[mtbold]{mathtime}
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
col[-2]:=col[-1]:=col0:=white;
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

beginfig(17);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,402.6b)--(230.8a,402.6b);
draw (1746.3a,402.6b)--(1729.7a,402.6b);
put_text( btex  $10^4$ etex, 196.5a, 402.6b, 0, 3);
draw (214.2a,478.0b)--(222.5a,478.0b);
draw (1746.3a,478.0b)--(1738.0a,478.0b);
draw (214.2a,522.1b)--(222.5a,522.1b);
draw (1746.3a,522.1b)--(1738.0a,522.1b);
draw (214.2a,553.4b)--(222.5a,553.4b);
draw (1746.3a,553.4b)--(1738.0a,553.4b);
draw (214.2a,577.7b)--(222.5a,577.7b);
draw (1746.3a,577.7b)--(1738.0a,577.7b);
draw (214.2a,597.5b)--(222.5a,597.5b);
draw (1746.3a,597.5b)--(1738.0a,597.5b);
draw (214.2a,614.3b)--(222.5a,614.3b);
draw (1746.3a,614.3b)--(1738.0a,614.3b);
draw (214.2a,628.8b)--(222.5a,628.8b);
draw (1746.3a,628.8b)--(1738.0a,628.8b);
draw (214.2a,641.6b)--(222.5a,641.6b);
draw (1746.3a,641.6b)--(1738.0a,641.6b);
draw (214.2a,653.1b)--(230.8a,653.1b);
draw (1746.3a,653.1b)--(1729.7a,653.1b);
put_text( btex  $10^5$ etex, 196.5a, 653.1b, 0, 3);
draw (214.2a,728.5b)--(222.5a,728.5b);
draw (1746.3a,728.5b)--(1738.0a,728.5b);
draw (214.2a,772.6b)--(222.5a,772.6b);
draw (1746.3a,772.6b)--(1738.0a,772.6b);
draw (214.2a,803.9b)--(222.5a,803.9b);
draw (1746.3a,803.9b)--(1738.0a,803.9b);
draw (214.2a,828.2b)--(222.5a,828.2b);
draw (1746.3a,828.2b)--(1738.0a,828.2b);
draw (214.2a,848.0b)--(222.5a,848.0b);
draw (1746.3a,848.0b)--(1738.0a,848.0b);
draw (214.2a,864.8b)--(222.5a,864.8b);
draw (1746.3a,864.8b)--(1738.0a,864.8b);
draw (214.2a,879.3b)--(222.5a,879.3b);
draw (1746.3a,879.3b)--(1738.0a,879.3b);
draw (214.2a,892.1b)--(222.5a,892.1b);
draw (1746.3a,892.1b)--(1738.0a,892.1b);
draw (214.2a,903.6b)--(230.8a,903.6b);
draw (1746.3a,903.6b)--(1729.7a,903.6b);
put_text( btex  $10^6$ etex, 196.5a, 903.6b, 0, 3);
draw (214.2a,979.0b)--(222.5a,979.0b);
draw (1746.3a,979.0b)--(1738.0a,979.0b);
draw (214.2a,1023.1b)--(222.5a,1023.1b);
draw (1746.3a,1023.1b)--(1738.0a,1023.1b);
draw (214.2a,1054.4b)--(222.5a,1054.4b);
draw (1746.3a,1054.4b)--(1738.0a,1054.4b);
draw (214.2a,1078.7b)--(222.5a,1078.7b);
draw (1746.3a,1078.7b)--(1738.0a,1078.7b);
draw (214.2a,1098.5b)--(222.5a,1098.5b);
draw (1746.3a,1098.5b)--(1738.0a,1098.5b);
draw (214.2a,1115.3b)--(222.5a,1115.3b);
draw (1746.3a,1115.3b)--(1738.0a,1115.3b);
draw (214.2a,1129.8b)--(222.5a,1129.8b);
draw (1746.3a,1129.8b)--(1738.0a,1129.8b);
draw (214.2a,1142.6b)--(222.5a,1142.6b);
draw (1746.3a,1142.6b)--(1738.0a,1142.6b);
draw (214.2a,1154.1b)--(230.8a,1154.1b);
draw (1746.3a,1154.1b)--(1729.7a,1154.1b);
put_text( btex  $10^7$ etex, 196.5a, 1154.1b, 0, 3);
draw (214.2a,1229.5b)--(222.5a,1229.5b);
draw (1746.3a,1229.5b)--(1738.0a,1229.5b);
draw (214.2a,1273.6b)--(222.5a,1273.6b);
draw (1746.3a,1273.6b)--(1738.0a,1273.6b);
draw (214.2a,1304.9b)--(222.5a,1304.9b);
draw (1746.3a,1304.9b)--(1738.0a,1304.9b);
draw (214.2a,1329.2b)--(222.5a,1329.2b);
draw (1746.3a,1329.2b)--(1738.0a,1329.2b);
draw (214.2a,1349.0b)--(222.5a,1349.0b);
draw (1746.3a,1349.0b)--(1738.0a,1349.0b);
draw (214.2a,1365.8b)--(222.5a,1365.8b);
draw (1746.3a,1365.8b)--(1738.0a,1365.8b);
draw (214.2a,1380.3b)--(222.5a,1380.3b);
draw (1746.3a,1380.3b)--(1738.0a,1380.3b);
draw (214.2a,1393.1b)--(222.5a,1393.1b);
draw (1746.3a,1393.1b)--(1738.0a,1393.1b);
draw (214.2a,1404.6b)--(230.8a,1404.6b);
draw (1746.3a,1404.6b)--(1729.7a,1404.6b);
put_text( btex  $10^8$ etex, 196.5a, 1404.6b, 0, 3);
draw (214.2a,402.6b)--(214.2a,419.2b);
draw (214.2a,1404.6b)--(214.2a,1388.0b);
put_text( btex  100 etex, 214.2a, 366.0b, 0, 2);
draw (367.9a,402.6b)--(367.9a,410.9b);
draw (367.9a,1404.6b)--(367.9a,1396.3b);
draw (457.9a,402.6b)--(457.9a,410.9b);
draw (457.9a,1404.6b)--(457.9a,1396.3b);
draw (521.7a,402.6b)--(521.7a,410.9b);
draw (521.7a,1404.6b)--(521.7a,1396.3b);
draw (571.2a,402.6b)--(571.2a,410.9b);
draw (571.2a,1404.6b)--(571.2a,1396.3b);
draw (611.6a,402.6b)--(611.6a,410.9b);
draw (611.6a,1404.6b)--(611.6a,1396.3b);
draw (645.8a,402.6b)--(645.8a,410.9b);
draw (645.8a,1404.6b)--(645.8a,1396.3b);
draw (675.4a,402.6b)--(675.4a,410.9b);
draw (675.4a,1404.6b)--(675.4a,1396.3b);
draw (701.5a,402.6b)--(701.5a,410.9b);
draw (701.5a,1404.6b)--(701.5a,1396.3b);
draw (724.9a,402.6b)--(724.9a,419.2b);
draw (724.9a,1404.6b)--(724.9a,1388.0b);
put_text( btex  1000 etex, 724.9a, 366.0b, 0, 2);
draw (878.6a,402.6b)--(878.6a,410.9b);
draw (878.6a,1404.6b)--(878.6a,1396.3b);
draw (968.6a,402.6b)--(968.6a,410.9b);
draw (968.6a,1404.6b)--(968.6a,1396.3b);
draw (1032.4a,402.6b)--(1032.4a,410.9b);
draw (1032.4a,1404.6b)--(1032.4a,1396.3b);
draw (1081.9a,402.6b)--(1081.9a,410.9b);
draw (1081.9a,1404.6b)--(1081.9a,1396.3b);
draw (1122.3a,402.6b)--(1122.3a,410.9b);
draw (1122.3a,1404.6b)--(1122.3a,1396.3b);
draw (1156.5a,402.6b)--(1156.5a,410.9b);
draw (1156.5a,1404.6b)--(1156.5a,1396.3b);
draw (1186.1a,402.6b)--(1186.1a,410.9b);
draw (1186.1a,1404.6b)--(1186.1a,1396.3b);
draw (1212.2a,402.6b)--(1212.2a,410.9b);
draw (1212.2a,1404.6b)--(1212.2a,1396.3b);
draw (1235.6a,402.6b)--(1235.6a,419.2b);
draw (1235.6a,1404.6b)--(1235.6a,1388.0b);
put_text( btex  10,000 etex, 1235.6a, 366.0b, 0, 2);
draw (1389.3a,402.6b)--(1389.3a,410.9b);
draw (1389.3a,1404.6b)--(1389.3a,1396.3b);
draw (1479.3a,402.6b)--(1479.3a,410.9b);
draw (1479.3a,1404.6b)--(1479.3a,1396.3b);
draw (1543.1a,402.6b)--(1543.1a,410.9b);
draw (1543.1a,1404.6b)--(1543.1a,1396.3b);
draw (1592.6a,402.6b)--(1592.6a,410.9b);
draw (1592.6a,1404.6b)--(1592.6a,1396.3b);
draw (1633.0a,402.6b)--(1633.0a,410.9b);
draw (1633.0a,1404.6b)--(1633.0a,1396.3b);
draw (1667.2a,402.6b)--(1667.2a,410.9b);
draw (1667.2a,1404.6b)--(1667.2a,1396.3b);
draw (1696.8a,402.6b)--(1696.8a,410.9b);
draw (1696.8a,1404.6b)--(1696.8a,1396.3b);
draw (1722.9a,402.6b)--(1722.9a,410.9b);
draw (1722.9a,1404.6b)--(1722.9a,1396.3b);
draw (1746.3a,402.6b)--(1746.3a,419.2b);
draw (1746.3a,1404.6b)--(1746.3a,1388.0b);
put_text( btex  100,000 etex, 1746.3a, 366.0b, 0, 2);
draw (214.2a,402.6b)--(1746.3a,402.6b)--(1746.3a,1404.6b)--(214.2a,1404.6b)--(214.2a,402.6b);
put_text( btex Time/Time-step (ns) etex, 36.6a, 903.6b, 1, 2);
put_text( btex Nodes etex, 980.2a, 311.1b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Total Time-step etex, 344.2a, 237.9b, 0, 1);
linetype 0;
draw (239.1a,237.9b)--(326.5a,237.9b);
draw (576.4a,1211.9b)--(730.2a,1142.6b)--(883.9a,1077.5b)--(1037.6a,1025.3b)--(1191.4a,985.6b)
--(1345.1a,971.8b);
gpdraw(0,576.4a,1211.9b);
gpdraw(0,730.2a,1142.6b);
gpdraw(0,883.9a,1077.5b);
gpdraw(0,1037.6a,1025.3b);
gpdraw(0,1191.4a,985.6b);
gpdraw(0,1345.1a,971.8b);
gpdraw(0,282.8a,237.9b);
linetype 1;
linetype -2;
put_text( btex Total K-space etex, 344.2a, 201.3b, 0, 1);
linetype 1;
draw (239.1a,201.3b)--(326.5a,201.3b);
draw (576.4a,1109.4b)--(730.2a,1056.6b)--(883.9a,1000.0b)--(1037.6a,949.7b)--(1191.4a,942.0b)
--(1345.1a,933.6b);
gpdraw(1,576.4a,1109.4b);
gpdraw(1,730.2a,1056.6b);
gpdraw(1,883.9a,1000.0b);
gpdraw(1,1037.6a,949.7b);
gpdraw(1,1191.4a,942.0b);
gpdraw(1,1345.1a,933.6b);
gpdraw(1,282.8a,201.3b);
linetype 2;
linetype -2;
put_text( btex K-space Broadcast etex, 344.2a, 164.7b, 0, 1);
linetype 2;
draw (239.1a,164.7b)--(326.5a,164.7b);
draw (576.4a,783.0b)--(730.2a,726.0b)--(883.9a,700.5b)--(1037.6a,688.2b)--(1191.4a,665.1b)
--(1345.1a,607.2b);
gpdraw(2,576.4a,783.0b);
gpdraw(2,730.2a,726.0b);
gpdraw(2,883.9a,700.5b);
gpdraw(2,1037.6a,688.2b);
gpdraw(2,1191.4a,665.1b);
gpdraw(2,1345.1a,607.2b);
gpdraw(2,282.8a,164.7b);
linetype 3;
linetype -2;
put_text( btex K-space Reduction etex, 344.2a, 128.1b, 0, 1);
linetype 3;
draw (239.1a,128.1b)--(326.5a,128.1b);
draw (576.4a,662.6b)--(730.2a,632.6b)--(883.9a,588.5b)--(1037.6a,553.0b)--(1191.4a,578.8b)
--(1345.1a,503.2b);
gpdraw(3,576.4a,662.6b);
gpdraw(3,730.2a,632.6b);
gpdraw(3,883.9a,588.5b);
gpdraw(3,1037.6a,553.0b);
gpdraw(3,1191.4a,578.8b);
gpdraw(3,1345.1a,503.2b);
gpdraw(3,282.8a,128.1b);
linetype 4;
linetype -2;
put_text( btex K-space Forward FFT etex, 344.2a, 91.5b, 0, 1);
linetype 4;
draw (239.1a,91.5b)--(326.5a,91.5b);
draw (576.4a,984.8b)--(730.2a,941.8b)--(883.9a,892.8b)--(1037.6a,837.4b)--(1191.4a,840.8b)
--(1345.1a,832.6b);
gpdraw(4,576.4a,984.8b);
gpdraw(4,730.2a,941.8b);
gpdraw(4,883.9a,892.8b);
gpdraw(4,1037.6a,837.4b);
gpdraw(4,1191.4a,840.8b);
gpdraw(4,1345.1a,832.6b);
gpdraw(4,282.8a,91.5b);
linetype 5;
linetype -2;
put_text( btex K-space Reverse FFT etex, 344.2a, 54.9b, 0, 1);
linetype 5;
draw (239.1a,54.9b)--(326.5a,54.9b);
draw (576.4a,989.2b)--(730.2a,938.6b)--(883.9a,874.8b)--(1037.6a,822.1b)--(1191.4a,823.8b)
--(1345.1a,837.9b);
gpdraw(5,576.4a,989.2b);
gpdraw(5,730.2a,938.6b);
gpdraw(5,883.9a,874.8b);
gpdraw(5,1037.6a,822.1b);
gpdraw(5,1191.4a,823.8b);
gpdraw(5,1345.1a,837.9b);
gpdraw(5,282.8a,54.9b);
linetype 6;
linetype -2;
put_text( btex K-space Mesh Charges etex, 344.2a, 18.3b, 0, 1);
linetype 6;
draw (239.1a,18.3b)--(326.5a,18.3b);
draw (576.4a,890.6b)--(730.2a,818.2b)--(730.2a,818.9b)--(883.9a,749.4b)--(1037.6a,688.5b)
--(1037.6a,682.9b)--(1037.6a,682.7b)--(1191.4a,622.9b)--(1345.1a,564.3b);
gpdraw(6,576.4a,890.6b);
gpdraw(6,576.4a,890.6b);
gpdraw(6,730.2a,818.2b);
gpdraw(6,730.2a,818.9b);
gpdraw(6,883.9a,749.4b);
gpdraw(6,883.9a,749.4b);
gpdraw(6,1037.6a,688.5b);
gpdraw(6,1037.6a,682.9b);
gpdraw(6,1037.6a,682.7b);
gpdraw(6,1191.4a,622.9b);
gpdraw(6,1345.1a,564.3b);
gpdraw(6,282.8a,18.3b);
linetype 7;
linetype -2;
put_text( btex K-space Unmesh Forces etex, 1208.8a, 237.9b, 0, 1);
linetype 7;
draw (1103.7a,237.9b)--(1191.1a,237.9b);
draw (576.4a,789.3b)--(576.4a,790.0b)--(730.2a,715.9b)--(730.2a,712.9b)--(883.9a,640.8b)
--(1037.6a,579.6b)--(1037.6a,579.5b)--(1037.6a,583.6b)--(1191.4a,520.3b)--(1345.1a,461.0b);
gpdraw(7,576.4a,789.3b);
gpdraw(7,576.4a,790.0b);
gpdraw(7,730.2a,715.9b);
gpdraw(7,730.2a,712.9b);
gpdraw(7,883.9a,640.8b);
gpdraw(7,883.9a,640.8b);
gpdraw(7,1037.6a,579.6b);
gpdraw(7,1037.6a,579.5b);
gpdraw(7,1037.6a,583.6b);
gpdraw(7,1191.4a,520.3b);
gpdraw(7,1345.1a,461.0b);
gpdraw(7,1147.4a,237.9b);
linetype 0;
linetype -2;
put_text( btex Real Space Non-bond Computation etex, 1208.8a, 201.3b, 0, 1);
linetype 0;
draw (1103.7a,201.3b)--(1191.1a,201.3b);
draw (576.4a,1159.5b)--(730.2a,1081.0b)--(883.9a,1008.2b)--(1037.6a,935.1b)--(1191.4a,861.5b)
--(1345.1a,792.2b);
gpdraw(8,576.4a,1159.5b);
gpdraw(8,730.2a,1081.0b);
gpdraw(8,883.9a,1008.2b);
gpdraw(8,1037.6a,935.1b);
gpdraw(8,1191.4a,861.5b);
gpdraw(8,1345.1a,792.2b);
gpdraw(8,1147.4a,201.3b);
linetype 1;
linetype -2;
put_text( btex Real Space Non-bond Broadcast etex, 1208.8a, 164.7b, 0, 1);
linetype 1;
draw (1103.7a,164.7b)--(1191.1a,164.7b);
draw (576.4a,861.1b)--(730.2a,807.7b)--(883.9a,778.5b)--(1037.6a,764.2b)--(1191.4a,763.1b)
--(1345.1a,784.4b);
gpdraw(9,576.4a,861.1b);
gpdraw(9,730.2a,807.7b);
gpdraw(9,883.9a,778.5b);
gpdraw(9,1037.6a,764.2b);
gpdraw(9,1191.4a,763.1b);
gpdraw(9,1345.1a,784.4b);
gpdraw(9,1147.4a,164.7b);
linetype 2;
linetype -2;
put_text( btex Real Space Non-bond Reduction etex, 1208.8a, 128.1b, 0, 1);
linetype 2;
draw (1103.7a,128.1b)--(1191.1a,128.1b);
draw (576.4a,1056.0b)--(730.2a,1003.0b)--(883.9a,941.3b)--(1037.6a,905.7b)--(1191.4a,870.6b)
--(1345.1a,868.8b);
gpdraw(0,576.4a,1056.0b);
gpdraw(0,730.2a,1003.0b);
gpdraw(0,883.9a,941.3b);
gpdraw(0,1037.6a,905.7b);
gpdraw(0,1191.4a,870.6b);
gpdraw(0,1345.1a,868.8b);
gpdraw(0,1147.4a,128.1b);
linetype 3;
linetype -2;
put_text( btex Bonded Force Broadcast etex, 1208.8a, 91.5b, 0, 1);
linetype 3;
draw (1103.7a,91.5b)--(1191.1a,91.5b);
draw (576.4a,860.5b)--(730.2a,806.7b)--(883.9a,777.3b)--(1037.6a,762.8b)--(1191.4a,761.7b)
--(1345.1a,783.3b);
gpdraw(1,576.4a,860.5b);
gpdraw(1,730.2a,806.7b);
gpdraw(1,883.9a,777.3b);
gpdraw(1,1037.6a,762.8b);
gpdraw(1,1191.4a,761.7b);
gpdraw(1,1345.1a,783.3b);
gpdraw(1,1147.4a,91.5b);
linetype 4;
linetype -2;
put_text( btex Bonded Force Reduction etex, 1208.8a, 54.9b, 0, 1);
linetype 4;
draw (1103.7a,54.9b)--(1191.1a,54.9b);
draw (576.4a,1055.9b)--(730.2a,1002.8b)--(883.9a,941.0b)--(1037.6a,905.4b)--(1191.4a,870.1b)
--(1345.1a,868.3b);
gpdraw(2,576.4a,1055.9b);
gpdraw(2,730.2a,1002.8b);
gpdraw(2,883.9a,941.0b);
gpdraw(2,1037.6a,905.4b);
gpdraw(2,1191.4a,870.1b);
gpdraw(2,1345.1a,868.3b);
gpdraw(2,1147.4a,54.9b);
linetype 5;
linetype -2;
put_text( btex Local Tuple List Driver etex, 1208.8a, 18.3b, 0, 1);
linetype 5;
draw (1103.7a,18.3b)--(1191.1a,18.3b);
draw (576.4a,919.6b)--(730.2a,847.6b)--(883.9a,778.5b)--(1037.6a,714.7b)--(1191.4a,656.3b)
--(1345.1a,602.2b);
gpdraw(3,576.4a,919.6b);
gpdraw(3,730.2a,847.6b);
gpdraw(3,883.9a,778.5b);
gpdraw(3,1037.6a,714.7b);
gpdraw(3,1191.4a,656.3b);
gpdraw(3,1345.1a,602.2b);
gpdraw(3,1147.4a,18.3b);
linetype -2;
draw (214.2a,402.6b)--(1746.3a,402.6b)--(1746.3a,1404.6b)--(214.2a,1404.6b)--(214.2a,402.6b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
