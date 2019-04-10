%GNUPLOT Metapost output: Sat Jul 29 13:17:00 2006


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
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1746.3a,109.8b)--(1729.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,250.5b)--(222.5a,250.5b);
draw (1746.3a,250.5b)--(1738.0a,250.5b);
draw (214.2a,332.8b)--(222.5a,332.8b);
draw (1746.3a,332.8b)--(1738.0a,332.8b);
draw (214.2a,391.2b)--(222.5a,391.2b);
draw (1746.3a,391.2b)--(1738.0a,391.2b);
draw (214.2a,436.5b)--(222.5a,436.5b);
draw (1746.3a,436.5b)--(1738.0a,436.5b);
draw (214.2a,473.5b)--(222.5a,473.5b);
draw (1746.3a,473.5b)--(1738.0a,473.5b);
draw (214.2a,504.8b)--(222.5a,504.8b);
draw (1746.3a,504.8b)--(1738.0a,504.8b);
draw (214.2a,531.9b)--(222.5a,531.9b);
draw (1746.3a,531.9b)--(1738.0a,531.9b);
draw (214.2a,555.8b)--(222.5a,555.8b);
draw (1746.3a,555.8b)--(1738.0a,555.8b);
draw (214.2a,577.2b)--(230.8a,577.2b);
draw (1746.3a,577.2b)--(1729.7a,577.2b);
put_text( btex  0.001 etex, 196.5a, 577.2b, 0, 3);
draw (214.2a,717.9b)--(222.5a,717.9b);
draw (1746.3a,717.9b)--(1738.0a,717.9b);
draw (214.2a,800.2b)--(222.5a,800.2b);
draw (1746.3a,800.2b)--(1738.0a,800.2b);
draw (214.2a,858.6b)--(222.5a,858.6b);
draw (1746.3a,858.6b)--(1738.0a,858.6b);
draw (214.2a,903.9b)--(222.5a,903.9b);
draw (1746.3a,903.9b)--(1738.0a,903.9b);
draw (214.2a,940.9b)--(222.5a,940.9b);
draw (1746.3a,940.9b)--(1738.0a,940.9b);
draw (214.2a,972.2b)--(222.5a,972.2b);
draw (1746.3a,972.2b)--(1738.0a,972.2b);
draw (214.2a,999.3b)--(222.5a,999.3b);
draw (1746.3a,999.3b)--(1738.0a,999.3b);
draw (214.2a,1023.2b)--(222.5a,1023.2b);
draw (1746.3a,1023.2b)--(1738.0a,1023.2b);
draw (214.2a,1044.6b)--(230.8a,1044.6b);
draw (1746.3a,1044.6b)--(1729.7a,1044.6b);
put_text( btex  0.01 etex, 196.5a, 1044.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,118.1b);
draw (214.2a,1044.6b)--(214.2a,1036.3b);
draw (304.1a,109.8b)--(304.1a,118.1b);
draw (304.1a,1044.6b)--(304.1a,1036.3b);
draw (367.9a,109.8b)--(367.9a,118.1b);
draw (367.9a,1044.6b)--(367.9a,1036.3b);
draw (417.4a,109.8b)--(417.4a,118.1b);
draw (417.4a,1044.6b)--(417.4a,1036.3b);
draw (457.9a,109.8b)--(457.9a,118.1b);
draw (457.9a,1044.6b)--(457.9a,1036.3b);
draw (492.1a,109.8b)--(492.1a,118.1b);
draw (492.1a,1044.6b)--(492.1a,1036.3b);
draw (521.7a,109.8b)--(521.7a,118.1b);
draw (521.7a,1044.6b)--(521.7a,1036.3b);
draw (547.8a,109.8b)--(547.8a,118.1b);
draw (547.8a,1044.6b)--(547.8a,1036.3b);
draw (571.2a,109.8b)--(571.2a,126.4b);
draw (571.2a,1044.6b)--(571.2a,1028.0b);
put_text( btex  1000 etex, 571.2a, 73.2b, 0, 2);
draw (724.9a,109.8b)--(724.9a,118.1b);
draw (724.9a,1044.6b)--(724.9a,1036.3b);
draw (814.8a,109.8b)--(814.8a,118.1b);
draw (814.8a,1044.6b)--(814.8a,1036.3b);
draw (878.6a,109.8b)--(878.6a,118.1b);
draw (878.6a,1044.6b)--(878.6a,1036.3b);
draw (928.1a,109.8b)--(928.1a,118.1b);
draw (928.1a,1044.6b)--(928.1a,1036.3b);
draw (968.6a,109.8b)--(968.6a,118.1b);
draw (968.6a,1044.6b)--(968.6a,1036.3b);
draw (1002.8a,109.8b)--(1002.8a,118.1b);
draw (1002.8a,1044.6b)--(1002.8a,1036.3b);
draw (1032.4a,109.8b)--(1032.4a,118.1b);
draw (1032.4a,1044.6b)--(1032.4a,1036.3b);
draw (1058.5a,109.8b)--(1058.5a,118.1b);
draw (1058.5a,1044.6b)--(1058.5a,1036.3b);
draw (1081.9a,109.8b)--(1081.9a,126.4b);
draw (1081.9a,1044.6b)--(1081.9a,1028.0b);
put_text( btex  10000 etex, 1081.9a, 73.2b, 0, 2);
draw (1235.6a,109.8b)--(1235.6a,118.1b);
draw (1235.6a,1044.6b)--(1235.6a,1036.3b);
draw (1325.5a,109.8b)--(1325.5a,118.1b);
draw (1325.5a,1044.6b)--(1325.5a,1036.3b);
draw (1389.3a,109.8b)--(1389.3a,118.1b);
draw (1389.3a,1044.6b)--(1389.3a,1036.3b);
draw (1438.8a,109.8b)--(1438.8a,118.1b);
draw (1438.8a,1044.6b)--(1438.8a,1036.3b);
draw (1479.3a,109.8b)--(1479.3a,118.1b);
draw (1479.3a,1044.6b)--(1479.3a,1036.3b);
draw (1513.5a,109.8b)--(1513.5a,118.1b);
draw (1513.5a,1044.6b)--(1513.5a,1036.3b);
draw (1543.1a,109.8b)--(1543.1a,118.1b);
draw (1543.1a,1044.6b)--(1543.1a,1036.3b);
draw (1569.2a,109.8b)--(1569.2a,118.1b);
draw (1569.2a,1044.6b)--(1569.2a,1036.3b);
draw (1592.6a,109.8b)--(1592.6a,126.4b);
draw (1592.6a,1044.6b)--(1592.6a,1028.0b);
put_text( btex  100000 etex, 1592.6a, 73.2b, 0, 2);
draw (1746.3a,109.8b)--(1746.3a,118.1b);
draw (1746.3a,1044.6b)--(1746.3a,1036.3b);
draw (214.2a,109.8b)--(1746.3a,109.8b)--(1746.3a,1044.6b)--(214.2a,1044.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time/Time-step (seconds) etex, 36.6a, 577.2b, 1, 2);
put_text( btex Node/CPU Count etex, 980.2a, 18.3b, 0, 2);
linetype 1;
linetype -2;
put_text( btex Local Bcast ApoA1 MPI etex, 1605.8a, 1009.7b, 0, 3);
linetype 1;
currentcolor:=red;
draw (1623.5a,1009.7b)--(1710.9a,1009.7b);
draw (422.7a,587.0b)--(576.4a,581.6b)--(730.2a,586.5b)--(730.2a,603.4b)--(883.9a,641.7b)
--(883.9a,681.6b)--(1037.6a,738.8b)--(1191.4a,897.1b);
gpdraw(9,422.7a,587.0b);
gpdraw(9,576.4a,581.6b);
gpdraw(9,730.2a,586.5b);
gpdraw(9,730.2a,603.4b);
gpdraw(9,883.9a,641.7b);
gpdraw(9,883.9a,681.6b);
gpdraw(9,1037.6a,738.8b);
gpdraw(9,1191.4a,897.1b);
gpdraw(9,1667.2a,1009.7b);
linetype -2;
put_text( btex Local Bcast Rhod MPI etex, 1605.8a, 973.1b, 0, 3);
linetype 1;
currentcolor:=red;
draw (1623.5a,973.1b)--(1710.9a,973.1b);
draw (422.7a,523.1b)--(576.4a,524.8b)--(730.2a,565.6b)--(730.2a,566.0b)--(883.9a,649.0b)
--(883.9a,651.5b)--(1037.6a,760.7b)--(1191.4a,915.7b);
gpdraw(8,422.7a,523.1b);
gpdraw(8,576.4a,524.8b);
gpdraw(8,730.2a,565.6b);
gpdraw(8,730.2a,566.0b);
gpdraw(8,883.9a,649.0b);
gpdraw(8,883.9a,651.5b);
gpdraw(8,1037.6a,760.7b);
gpdraw(8,1191.4a,915.7b);
gpdraw(8,1667.2a,973.1b);
linetype -2;
put_text( btex Local Bcast SOPE MPI etex, 1605.8a, 936.5b, 0, 3);
linetype 1;
currentcolor:=red;
draw (1623.5a,936.5b)--(1710.9a,936.5b);
draw (422.7a,457.8b)--(576.4a,483.5b)--(730.2a,551.0b)--(730.2a,550.7b)--(883.9a,645.0b)
--(883.9a,643.0b)--(1037.6a,762.0b)--(1191.4a,899.1b);
gpdraw(7,422.7a,457.8b);
gpdraw(7,576.4a,483.5b);
gpdraw(7,730.2a,551.0b);
gpdraw(7,730.2a,550.7b);
gpdraw(7,883.9a,645.0b);
gpdraw(7,883.9a,643.0b);
gpdraw(7,1037.6a,762.0b);
gpdraw(7,1191.4a,899.1b);
gpdraw(7,1667.2a,936.5b);
linetype -2;
put_text( btex Local Reduce ApoA1 MPI etex, 1605.8a, 899.9b, 0, 3);
linetype 1;
currentcolor:=red;
draw (1623.5a,899.9b)--(1710.9a,899.9b);
draw (422.7a,498.0b)--(576.4a,508.3b)--(730.2a,542.2b)--(730.2a,550.3b)--(883.9a,618.9b)
--(883.9a,664.4b)--(1037.6a,732.8b)--(1191.4a,895.4b);
gpdraw(5,422.7a,498.0b);
gpdraw(5,576.4a,508.3b);
gpdraw(5,730.2a,542.2b);
gpdraw(5,730.2a,550.3b);
gpdraw(5,883.9a,618.9b);
gpdraw(5,883.9a,664.4b);
gpdraw(5,1037.6a,732.8b);
gpdraw(5,1191.4a,895.4b);
gpdraw(5,1667.2a,899.9b);
linetype -2;
put_text( btex Local Reduce Rhod MPI etex, 1605.8a, 863.3b, 0, 3);
linetype 1;
currentcolor:=red;
draw (1623.5a,863.3b)--(1710.9a,863.3b);
draw (422.7a,450.5b)--(576.4a,469.6b)--(730.2a,519.9b)--(730.2a,524.2b)--(883.9a,607.2b)
--(883.9a,619.4b)--(1037.6a,722.9b)--(1191.4a,883.5b);
gpdraw(4,422.7a,450.5b);
gpdraw(4,576.4a,469.6b);
gpdraw(4,730.2a,519.9b);
gpdraw(4,730.2a,524.2b);
gpdraw(4,883.9a,607.2b);
gpdraw(4,883.9a,619.4b);
gpdraw(4,1037.6a,722.9b);
gpdraw(4,1191.4a,883.5b);
gpdraw(4,1667.2a,863.3b);
linetype -2;
put_text( btex Local Reduce SOPE MPI etex, 1605.8a, 826.7b, 0, 3);
linetype 1;
currentcolor:=red;
draw (1623.5a,826.7b)--(1710.9a,826.7b);
draw (422.7a,411.3b)--(576.4a,446.5b)--(730.2a,525.9b)--(730.2a,546.1b)--(883.9a,631.6b)
--(883.9a,634.9b)--(1037.6a,738.0b)--(1191.4a,873.2b);
gpdraw(2,422.7a,411.3b);
gpdraw(2,576.4a,446.5b);
gpdraw(2,730.2a,525.9b);
gpdraw(2,730.2a,546.1b);
gpdraw(2,883.9a,631.6b);
gpdraw(2,883.9a,634.9b);
gpdraw(2,1037.6a,738.0b);
gpdraw(2,1191.4a,873.2b);
gpdraw(2,1667.2a,826.7b);
linetype 0;
linetype -2;
put_text( btex Local Bcast ApoA1 SPI etex, 1605.8a, 790.1b, 0, 3);
linetype 0;
currentcolor:=green;
draw (1623.5a,790.1b)--(1710.9a,790.1b);
draw (422.7a,592.1b)--(576.4a,516.0b)--(730.2a,472.6b)--(730.2a,472.5b)--(883.9a,437.4b)
--(883.9a,440.2b)--(1037.6a,406.7b)--(1191.4a,389.6b);
gpdraw(9,422.7a,592.1b);
gpdraw(9,576.4a,516.0b);
gpdraw(9,730.2a,472.6b);
gpdraw(9,730.2a,472.5b);
gpdraw(9,883.9a,437.4b);
gpdraw(9,883.9a,440.2b);
gpdraw(9,1037.6a,406.7b);
gpdraw(9,1191.4a,389.6b);
gpdraw(9,1667.2a,790.1b);
linetype -2;
put_text( btex Local Bcast Rhod SPI etex, 1605.8a, 753.5b, 0, 3);
linetype 0;
currentcolor:=green;
draw (1623.5a,753.5b)--(1710.9a,753.5b);
draw (422.7a,426.0b)--(576.4a,383.8b)--(730.2a,362.2b)--(730.2a,361.9b)--(883.9a,321.1b)
--(883.9a,323.2b)--(1037.6a,290.0b)--(1191.4a,296.6b);
gpdraw(8,422.7a,426.0b);
gpdraw(8,576.4a,383.8b);
gpdraw(8,730.2a,362.2b);
gpdraw(8,730.2a,361.9b);
gpdraw(8,883.9a,321.1b);
gpdraw(8,883.9a,323.2b);
gpdraw(8,1037.6a,290.0b);
gpdraw(8,1191.4a,296.6b);
gpdraw(8,1667.2a,753.5b);
linetype -2;
put_text( btex Local Bcast SOPE SPI etex, 1605.8a, 716.9b, 0, 3);
linetype 0;
currentcolor:=green;
draw (1623.5a,716.9b)--(1710.9a,716.9b);
draw (422.7a,366.1b)--(576.4a,338.0b)--(730.2a,294.5b)--(730.2a,294.1b)--(883.9a,272.5b)
--(883.9a,293.8b)--(1037.6a,276.2b)--(1191.4a,273.7b);
gpdraw(7,422.7a,366.1b);
gpdraw(7,576.4a,338.0b);
gpdraw(7,730.2a,294.5b);
gpdraw(7,730.2a,294.1b);
gpdraw(7,883.9a,272.5b);
gpdraw(7,883.9a,293.8b);
gpdraw(7,1037.6a,276.2b);
gpdraw(7,1191.4a,273.7b);
gpdraw(7,1667.2a,716.9b);
linetype -2;
put_text( btex Local Reduce ApoA1 SPI etex, 1605.8a, 680.3b, 0, 3);
linetype 0;
currentcolor:=green;
draw (1623.5a,680.3b)--(1710.9a,680.3b);
draw (422.7a,571.2b)--(576.4a,520.8b)--(730.2a,458.7b)--(730.2a,456.2b)--(883.9a,427.7b)
--(883.9a,421.6b)--(1037.6a,382.7b)--(1191.4a,360.3b);
gpdraw(5,422.7a,571.2b);
gpdraw(5,576.4a,520.8b);
gpdraw(5,730.2a,458.7b);
gpdraw(5,730.2a,456.2b);
gpdraw(5,883.9a,427.7b);
gpdraw(5,883.9a,421.6b);
gpdraw(5,1037.6a,382.7b);
gpdraw(5,1191.4a,360.3b);
gpdraw(5,1667.2a,680.3b);
linetype -2;
put_text( btex Local Reduce Rhod SPI etex, 1605.8a, 643.7b, 0, 3);
linetype 0;
currentcolor:=green;
draw (1623.5a,643.7b)--(1710.9a,643.7b);
draw (422.7a,439.5b)--(576.4a,384.2b)--(730.2a,354.0b)--(730.2a,358.0b)--(883.9a,301.3b)
--(883.9a,313.8b)--(1037.6a,275.3b)--(1191.4a,288.2b);
gpdraw(4,422.7a,439.5b);
gpdraw(4,576.4a,384.2b);
gpdraw(4,730.2a,354.0b);
gpdraw(4,730.2a,358.0b);
gpdraw(4,883.9a,301.3b);
gpdraw(4,883.9a,313.8b);
gpdraw(4,1037.6a,275.3b);
gpdraw(4,1191.4a,288.2b);
gpdraw(4,1667.2a,643.7b);
linetype -2;
put_text( btex Local Reduce SOPE SPI etex, 1605.8a, 607.1b, 0, 3);
linetype 0;
currentcolor:=green;
draw (1623.5a,607.1b)--(1710.9a,607.1b);
draw (422.7a,364.7b)--(576.4a,331.6b)--(730.2a,278.7b)--(730.2a,280.2b)--(883.9a,263.9b)
--(883.9a,285.6b)--(1037.6a,259.9b)--(1191.4a,255.2b);
gpdraw(2,422.7a,364.7b);
gpdraw(2,576.4a,331.6b);
gpdraw(2,730.2a,278.7b);
gpdraw(2,730.2a,280.2b);
gpdraw(2,883.9a,263.9b);
gpdraw(2,883.9a,285.6b);
gpdraw(2,1037.6a,259.9b);
gpdraw(2,1191.4a,255.2b);
gpdraw(2,1667.2a,607.1b);
linetype -2;
draw (214.2a,109.8b)--(1746.3a,109.8b)--(1746.3a,1044.6b)--(214.2a,1044.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
