%GNUPLOT Metapost output: Thu Dec 04 01:23:27 2003


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
%verbatimtex
%\def\setfont#1#2{%.
%  \font\gpfont=#1 at #2pt
%\gpfont}
%\setfont{lsr}{10.00}
%etex

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
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1746.3a,109.8b)--(1729.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,166.1b)--(222.5a,166.1b);
draw (1746.3a,166.1b)--(1738.0a,166.1b);
draw (214.2a,240.5b)--(222.5a,240.5b);
draw (1746.3a,240.5b)--(1738.0a,240.5b);
draw (214.2a,278.6b)--(222.5a,278.6b);
draw (1746.3a,278.6b)--(1738.0a,278.6b);
draw (214.2a,296.8b)--(230.8a,296.8b);
draw (1746.3a,296.8b)--(1729.7a,296.8b);
put_text( btex  0.001 etex, 196.5a, 296.8b, 0, 3);
draw (214.2a,353.0b)--(222.5a,353.0b);
draw (1746.3a,353.0b)--(1738.0a,353.0b);
draw (214.2a,427.4b)--(222.5a,427.4b);
draw (1746.3a,427.4b)--(1738.0a,427.4b);
draw (214.2a,465.6b)--(222.5a,465.6b);
draw (1746.3a,465.6b)--(1738.0a,465.6b);
draw (214.2a,483.7b)--(230.8a,483.7b);
draw (1746.3a,483.7b)--(1729.7a,483.7b);
put_text( btex  0.01 etex, 196.5a, 483.7b, 0, 3);
draw (214.2a,540.0b)--(222.5a,540.0b);
draw (1746.3a,540.0b)--(1738.0a,540.0b);
draw (214.2a,614.4b)--(222.5a,614.4b);
draw (1746.3a,614.4b)--(1738.0a,614.4b);
draw (214.2a,652.6b)--(222.5a,652.6b);
draw (1746.3a,652.6b)--(1738.0a,652.6b);
draw (214.2a,670.7b)--(230.8a,670.7b);
draw (1746.3a,670.7b)--(1729.7a,670.7b);
put_text( btex  0.1 etex, 196.5a, 670.7b, 0, 3);
draw (214.2a,727.0b)--(222.5a,727.0b);
draw (1746.3a,727.0b)--(1738.0a,727.0b);
draw (214.2a,801.4b)--(222.5a,801.4b);
draw (1746.3a,801.4b)--(1738.0a,801.4b);
draw (214.2a,839.5b)--(222.5a,839.5b);
draw (1746.3a,839.5b)--(1738.0a,839.5b);
draw (214.2a,857.6b)--(230.8a,857.6b);
draw (1746.3a,857.6b)--(1729.7a,857.6b);
put_text( btex  1 etex, 196.5a, 857.6b, 0, 3);
draw (214.2a,913.9b)--(222.5a,913.9b);
draw (1746.3a,913.9b)--(1738.0a,913.9b);
draw (214.2a,988.3b)--(222.5a,988.3b);
draw (1746.3a,988.3b)--(1738.0a,988.3b);
draw (214.2a,1026.5b)--(222.5a,1026.5b);
draw (1746.3a,1026.5b)--(1738.0a,1026.5b);
draw (214.2a,1044.6b)--(230.8a,1044.6b);
draw (1746.3a,1044.6b)--(1729.7a,1044.6b);
put_text( btex  10 etex, 196.5a, 1044.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,1044.6b)--(214.2a,1028.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
draw (306.4a,109.8b)--(306.4a,118.1b);
draw (306.4a,1044.6b)--(306.4a,1036.3b);
draw (428.4a,109.8b)--(428.4a,118.1b);
draw (428.4a,1044.6b)--(428.4a,1036.3b);
draw (490.9a,109.8b)--(490.9a,118.1b);
draw (490.9a,1044.6b)--(490.9a,1036.3b);
draw (520.6a,109.8b)--(520.6a,126.4b);
draw (520.6a,1044.6b)--(520.6a,1028.0b);
put_text( btex  10 etex, 520.6a, 73.2b, 0, 2);
draw (612.9a,109.8b)--(612.9a,118.1b);
draw (612.9a,1044.6b)--(612.9a,1036.3b);
draw (734.8a,109.8b)--(734.8a,118.1b);
draw (734.8a,1044.6b)--(734.8a,1036.3b);
draw (797.3a,109.8b)--(797.3a,118.1b);
draw (797.3a,1044.6b)--(797.3a,1036.3b);
draw (827.0a,109.8b)--(827.0a,126.4b);
draw (827.0a,1044.6b)--(827.0a,1028.0b);
put_text( btex  100 etex, 827.0a, 73.2b, 0, 2);
draw (919.3a,109.8b)--(919.3a,118.1b);
draw (919.3a,1044.6b)--(919.3a,1036.3b);
draw (1041.2a,109.8b)--(1041.2a,118.1b);
draw (1041.2a,1044.6b)--(1041.2a,1036.3b);
draw (1103.8a,109.8b)--(1103.8a,118.1b);
draw (1103.8a,1044.6b)--(1103.8a,1036.3b);
draw (1133.5a,109.8b)--(1133.5a,126.4b);
draw (1133.5a,1044.6b)--(1133.5a,1028.0b);
put_text( btex  1000 etex, 1133.5a, 73.2b, 0, 2);
draw (1225.7a,109.8b)--(1225.7a,118.1b);
draw (1225.7a,1044.6b)--(1225.7a,1036.3b);
draw (1347.6a,109.8b)--(1347.6a,118.1b);
draw (1347.6a,1044.6b)--(1347.6a,1036.3b);
draw (1410.2a,109.8b)--(1410.2a,118.1b);
draw (1410.2a,1044.6b)--(1410.2a,1036.3b);
draw (1439.9a,109.8b)--(1439.9a,126.4b);
draw (1439.9a,1044.6b)--(1439.9a,1028.0b);
put_text( btex  10000 etex, 1439.9a, 73.2b, 0, 2);
draw (1532.1a,109.8b)--(1532.1a,118.1b);
draw (1532.1a,1044.6b)--(1532.1a,1036.3b);
draw (1654.1a,109.8b)--(1654.1a,118.1b);
draw (1654.1a,1044.6b)--(1654.1a,1036.3b);
draw (1716.6a,109.8b)--(1716.6a,118.1b);
draw (1716.6a,1044.6b)--(1716.6a,1036.3b);
draw (1746.3a,109.8b)--(1746.3a,126.4b);
draw (1746.3a,1044.6b)--(1746.3a,1028.0b);
put_text( btex  100000 etex, 1746.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1746.3a,109.8b)--(1746.3a,1044.6b)--(214.2a,1044.6b)--(214.2a,109.8b);
put_text( btex 3D-FFT time (seconds) etex, 36.6a, 577.2b, 1, 2);
put_text( btex Task Count etex, 980.2a, 18.3b, 0, 2);
linetype 0;
put_text( btex 128x128x128 FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft etex, 1605.8a, 1009.7b, 0, 3);
gpdraw(0,490.9a,821.4b);
gpdraw(0,675.4a,696.3b);
gpdraw(0,767.6a,650.3b);
gpdraw(0,859.9a,589.5b);
gpdraw(0,952.1a,535.8b);
gpdraw(0,1044.4a,495.5b);
gpdraw(0,1667.2a,1009.7b);
linetype 1;
put_text( btex 128x128x128 FFTBGMPI on BG/L -qarch=440d w/tjcw 1d fft etex, 1605.8a, 973.1b, 0, 3);
gpdraw(1,490.9a,815.3b);
gpdraw(1,583.2a,745.8b);
gpdraw(1,675.4a,686.0b);
gpdraw(1,767.6a,643.2b);
gpdraw(1,859.9a,580.7b);
gpdraw(1,952.1a,529.3b);
gpdraw(1,1044.4a,489.8b);
gpdraw(1,1667.2a,973.1b);
linetype 2;
put_text( btex $128^3$ packet layer -qarch=440d, 444MHz normalized to 500MHz etex, 1605.8a, 936.5b, 0, 3);
gpdraw(2,214.2a,933.6b);
gpdraw(2,306.4a,878.7b);
gpdraw(2,490.9a,746.6b);
gpdraw(2,675.4a,628.4b);
gpdraw(2,1667.2a,936.5b);
linetype 3;
put_text( btex $128^3$ mesh estimate (500MHz) etex, 1605.8a, 899.9b, 0, 3);
draw (1623.5a,899.9b)--(1710.9a,899.9b);
draw (490.9a,726.9b)--(675.4a,636.6b)--(767.6a,580.3b)--(859.9a,546.5b)--(952.1a,499.5b)
--(1044.4a,443.2b)--(1136.6a,416.1b)--(1228.9a,418.3b)--(1321.1a,413.3b)--(1413.3a,498.0b)
--(1505.6a,520.8b);
linetype 4;
put_text( btex $128^3$ torus estimate (500MHz)/tjcw fft etex, 1605.8a, 863.3b, 0, 3);
draw (1623.5a,863.3b)--(1710.9a,863.3b);
draw (490.9a,649.0b)--(675.4a,564.4b)--(767.6a,508.1b)--(859.9a,478.5b)--(952.1a,432.8b)
--(1044.4a,376.5b)--(1136.6a,352.7b)--(1228.9a,358.6b)--(1321.1a,355.2b)--(1413.3a,441.5b)
--(1505.6a,464.4b);
linetype 5;
put_text( btex 64x64x64 FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft etex, 1605.8a, 826.7b, 0, 3);
gpdraw(3,214.2a,810.2b);
gpdraw(3,490.9a,632.2b);
gpdraw(3,675.4a,522.4b);
gpdraw(3,767.6a,480.0b);
gpdraw(3,859.9a,424.6b);
gpdraw(3,952.1a,383.0b);
gpdraw(3,1044.4a,366.0b);
gpdraw(3,1667.2a,826.7b);
linetype 6;
put_text( btex 64x64x64 FFTBGMPI on BG/L -qarch=440d w/tjcw 1d fft etex, 1605.8a, 790.1b, 0, 3);
gpdraw(4,214.2a,804.6b);
gpdraw(4,306.4a,740.0b);
gpdraw(4,398.7a,665.6b);
gpdraw(4,490.9a,625.2b);
gpdraw(4,583.2a,568.6b);
gpdraw(4,675.4a,513.5b);
gpdraw(4,767.6a,473.1b);
gpdraw(4,859.9a,418.1b);
gpdraw(4,952.1a,378.3b);
gpdraw(4,1044.4a,363.1b);
gpdraw(4,1667.2a,790.1b);
linetype 7;
put_text( btex $64^3$ packet layer -qarch=440d, 444MHz normalized to 500MHz etex, 1605.8a, 753.5b, 0, 3);
gpdraw(5,214.2a,743.9b);
gpdraw(5,306.4a,681.3b);
gpdraw(5,490.9a,569.0b);
gpdraw(5,675.4a,479.9b);
gpdraw(5,1667.2a,753.5b);
linetype 0;
put_text( btex $64^3$ mesh estimate (500MHz) etex, 1605.8a, 716.9b, 0, 3);
draw (1623.5a,716.9b)--(1710.9a,716.9b);
draw (490.9a,549.7b)--(675.4a,461.5b)--(767.6a,405.2b)--(859.9a,373.0b)--(952.1a,326.5b)
--(1044.4a,312.4b)--(1136.6a,387.7b)--(1228.9a,409.1b)--(1321.1a,408.5b);
linetype 1;
put_text( btex $64^3$ torus estimate (500MHz)/tjcw fft etex, 1605.8a, 680.3b, 0, 3);
draw (1623.5a,680.3b)--(1710.9a,680.3b);
draw (490.9a,473.0b)--(675.4a,390.6b)--(767.6a,334.3b)--(859.9a,306.0b)--(952.1a,260.9b)
--(1044.4a,250.7b)--(1136.6a,330.3b)--(1228.9a,352.4b)--(1321.1a,352.1b);
endfig;
end.
