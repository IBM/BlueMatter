%GNUPLOT Metapost output: Thu Apr 10 15:54:31 2003


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

beginfig(3);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (261.6a,145.2b)--(278.2a,145.2b);
draw (1178.4a,145.2b)--(1161.8a,145.2b);
put_text( btex 0.1 etex, 243.9a, 145.2b, 0, 3);
draw (261.6a,285.5b)--(278.2a,285.5b);
draw (1178.4a,285.5b)--(1161.8a,285.5b);
put_text( btex 0.2 etex, 243.9a, 285.5b, 0, 3);
draw (261.6a,367.6b)--(278.2a,367.6b);
draw (1178.4a,367.6b)--(1161.8a,367.6b);
put_text( btex   etex, 243.9a, 367.6b, 0, 3);
draw (261.6a,425.9b)--(278.2a,425.9b);
draw (1178.4a,425.9b)--(1161.8a,425.9b);
put_text( btex   etex, 243.9a, 425.9b, 0, 3);
draw (261.6a,471.1b)--(278.2a,471.1b);
draw (1178.4a,471.1b)--(1161.8a,471.1b);
put_text( btex 0.5 etex, 243.9a, 471.1b, 0, 3);
draw (261.6a,508.0b)--(278.2a,508.0b);
draw (1178.4a,508.0b)--(1161.8a,508.0b);
put_text( btex   etex, 243.9a, 508.0b, 0, 3);
draw (261.6a,539.2b)--(278.2a,539.2b);
draw (1178.4a,539.2b)--(1161.8a,539.2b);
put_text( btex 0.7 etex, 243.9a, 539.2b, 0, 3);
draw (261.6a,566.2b)--(278.2a,566.2b);
draw (1178.4a,566.2b)--(1161.8a,566.2b);
put_text( btex   etex, 243.9a, 566.2b, 0, 3);
draw (261.6a,590.1b)--(278.2a,590.1b);
draw (1178.4a,590.1b)--(1161.8a,590.1b);
put_text( btex   etex, 243.9a, 590.1b, 0, 3);
draw (261.6a,611.4b)--(278.2a,611.4b);
draw (1178.4a,611.4b)--(1161.8a,611.4b);
put_text( btex 1.0 etex, 243.9a, 611.4b, 0, 3);
draw (261.6a,145.2b)--(261.6a,161.8b);
draw (261.6a,611.4b)--(261.6a,594.8b);
put_text( btex  1 etex, 261.6a, 108.6b, 0, 2);
draw (330.6a,145.2b)--(330.6a,153.5b);
draw (330.6a,611.4b)--(330.6a,603.1b);
draw (371.0a,145.2b)--(371.0a,153.5b);
draw (371.0a,611.4b)--(371.0a,603.1b);
draw (399.6a,145.2b)--(399.6a,153.5b);
draw (399.6a,611.4b)--(399.6a,603.1b);
draw (421.8a,145.2b)--(421.8a,153.5b);
draw (421.8a,611.4b)--(421.8a,603.1b);
draw (440.0a,145.2b)--(440.0a,153.5b);
draw (440.0a,611.4b)--(440.0a,603.1b);
draw (455.3a,145.2b)--(455.3a,153.5b);
draw (455.3a,611.4b)--(455.3a,603.1b);
draw (468.6a,145.2b)--(468.6a,153.5b);
draw (468.6a,611.4b)--(468.6a,603.1b);
draw (480.3a,145.2b)--(480.3a,153.5b);
draw (480.3a,611.4b)--(480.3a,603.1b);
draw (490.8a,145.2b)--(490.8a,161.8b);
draw (490.8a,611.4b)--(490.8a,594.8b);
put_text( btex  10 etex, 490.8a, 108.6b, 0, 2);
draw (559.8a,145.2b)--(559.8a,153.5b);
draw (559.8a,611.4b)--(559.8a,603.1b);
draw (600.2a,145.2b)--(600.2a,153.5b);
draw (600.2a,611.4b)--(600.2a,603.1b);
draw (628.8a,145.2b)--(628.8a,153.5b);
draw (628.8a,611.4b)--(628.8a,603.1b);
draw (651.0a,145.2b)--(651.0a,153.5b);
draw (651.0a,611.4b)--(651.0a,603.1b);
draw (669.2a,145.2b)--(669.2a,153.5b);
draw (669.2a,611.4b)--(669.2a,603.1b);
draw (684.5a,145.2b)--(684.5a,153.5b);
draw (684.5a,611.4b)--(684.5a,603.1b);
draw (697.8a,145.2b)--(697.8a,153.5b);
draw (697.8a,611.4b)--(697.8a,603.1b);
draw (709.5a,145.2b)--(709.5a,153.5b);
draw (709.5a,611.4b)--(709.5a,603.1b);
draw (720.0a,145.2b)--(720.0a,161.8b);
draw (720.0a,611.4b)--(720.0a,594.8b);
put_text( btex  100 etex, 720.0a, 108.6b, 0, 2);
draw (789.0a,145.2b)--(789.0a,153.5b);
draw (789.0a,611.4b)--(789.0a,603.1b);
draw (829.4a,145.2b)--(829.4a,153.5b);
draw (829.4a,611.4b)--(829.4a,603.1b);
draw (858.0a,145.2b)--(858.0a,153.5b);
draw (858.0a,611.4b)--(858.0a,603.1b);
draw (880.2a,145.2b)--(880.2a,153.5b);
draw (880.2a,611.4b)--(880.2a,603.1b);
draw (898.4a,145.2b)--(898.4a,153.5b);
draw (898.4a,611.4b)--(898.4a,603.1b);
draw (913.7a,145.2b)--(913.7a,153.5b);
draw (913.7a,611.4b)--(913.7a,603.1b);
draw (927.0a,145.2b)--(927.0a,153.5b);
draw (927.0a,611.4b)--(927.0a,603.1b);
draw (938.7a,145.2b)--(938.7a,153.5b);
draw (938.7a,611.4b)--(938.7a,603.1b);
draw (949.2a,145.2b)--(949.2a,161.8b);
draw (949.2a,611.4b)--(949.2a,594.8b);
put_text( btex  1000 etex, 949.2a, 108.6b, 0, 2);
draw (1018.2a,145.2b)--(1018.2a,153.5b);
draw (1018.2a,611.4b)--(1018.2a,603.1b);
draw (1058.6a,145.2b)--(1058.6a,153.5b);
draw (1058.6a,611.4b)--(1058.6a,603.1b);
draw (1087.2a,145.2b)--(1087.2a,153.5b);
draw (1087.2a,611.4b)--(1087.2a,603.1b);
draw (1109.4a,145.2b)--(1109.4a,153.5b);
draw (1109.4a,611.4b)--(1109.4a,603.1b);
draw (1127.6a,145.2b)--(1127.6a,153.5b);
draw (1127.6a,611.4b)--(1127.6a,603.1b);
draw (1142.9a,145.2b)--(1142.9a,153.5b);
draw (1142.9a,611.4b)--(1142.9a,603.1b);
draw (1156.2a,145.2b)--(1156.2a,153.5b);
draw (1156.2a,611.4b)--(1156.2a,603.1b);
draw (1167.9a,145.2b)--(1167.9a,153.5b);
draw (1167.9a,611.4b)--(1167.9a,603.1b);
draw (1178.4a,145.2b)--(1178.4a,161.8b);
draw (1178.4a,611.4b)--(1178.4a,594.8b);
put_text( btex  10000 etex, 1178.4a, 108.6b, 0, 2);
draw (261.6a,145.2b)--(1178.4a,145.2b)--(1178.4a,611.4b)--(261.6a,611.4b)--(261.6a,145.2b);
put_text( btex Parallel Efficiency etex, 133.5a, 378.3b, 1, 2);
put_text( btex Node Count etex, 720.0a, 53.7b, 0, 2);
linetype 0;
put_text( btex 5000 atoms (gp) etex, 580.2a, 436.3b, 0, 3);
draw (597.9a,436.3b)--(685.3a,436.3b);
draw (261.6a,611.4b)--(468.6a,577.4b)--(606.6a,548.2b)--(675.6a,547.8b)--(744.6a,514.2b)
--(813.6a,498.5b)--(882.6a,496.4b)--(951.6a,444.0b)--(1020.6a,327.9b)--(1089.6a,214.2b)
--(1103.9a,145.2b);
linetype 1;
put_text( btex 5000 atoms (fr) etex, 580.2a, 399.7b, 0, 3);
draw (597.9a,399.7b)--(685.3a,399.7b);
draw (261.6a,611.4b)--(468.6a,573.7b)--(606.6a,541.7b)--(675.6a,541.3b)--(744.6a,504.9b)
--(813.6a,487.7b)--(882.6a,485.0b)--(951.6a,428.9b)--(1020.6a,308.4b)--(1089.6a,192.5b)
--(1099.4a,145.2b);
linetype 2;
put_text( btex 10,000 atoms (gp) etex, 580.2a, 363.1b, 0, 3);
draw (597.9a,363.1b)--(685.3a,363.1b);
draw (261.6a,611.4b)--(468.6a,582.8b)--(606.6a,557.4b)--(675.6a,556.8b)--(744.6a,526.9b)
--(813.6a,512.0b)--(882.6a,508.4b)--(951.6a,458.8b)--(1020.6a,349.2b)--(1089.6a,238.7b)
--(1109.6a,145.2b);
linetype 3;
put_text( btex 10,000 atoms (fr) etex, 580.2a, 326.5b, 0, 3);
draw (597.9a,326.5b)--(685.3a,326.5b);
draw (261.6a,611.4b)--(468.6a,573.7b)--(606.6a,541.7b)--(675.6a,541.3b)--(744.6a,504.9b)
--(813.6a,487.7b)--(882.6a,485.0b)--(951.6a,428.9b)--(1020.6a,308.4b)--(1089.6a,192.5b)
--(1099.4a,145.2b);
linetype 4;
put_text( btex 20,000 atoms (gp) etex, 580.2a, 289.9b, 0, 3);
draw (597.9a,289.9b)--(685.3a,289.9b);
draw (261.6a,611.4b)--(468.6a,589.6b)--(606.6a,569.4b)--(675.6a,568.5b)--(744.6a,543.7b)
--(813.6a,530.2b)--(882.6a,524.3b)--(951.6a,478.8b)--(1020.6a,379.0b)--(1089.6a,273.7b)
--(1118.5a,145.2b);
linetype 5;
put_text( btex 20,000 atoms (fr) etex, 580.2a, 253.3b, 0, 3);
draw (597.9a,253.3b)--(685.3a,253.3b);
draw (261.6a,611.4b)--(468.6a,573.7b)--(606.6a,541.7b)--(675.6a,541.3b)--(744.6a,504.9b)
--(813.6a,487.7b)--(882.6a,485.0b)--(951.6a,428.9b)--(1020.6a,308.4b)--(1089.6a,192.5b)
--(1099.4a,145.2b);
linetype 6;
put_text( btex 30,000 atoms (gp) etex, 580.2a, 216.7b, 0, 3);
draw (597.9a,216.7b)--(685.3a,216.7b);
draw (261.6a,611.4b)--(468.6a,593.7b)--(606.6a,576.9b)--(675.6a,575.8b)--(744.6a,554.4b)
--(813.6a,541.7b)--(882.6a,534.3b)--(951.6a,491.6b)--(1020.6a,399.1b)--(1089.6a,297.7b)
--(1125.6a,145.2b);
linetype 7;
put_text( btex 30,000 atoms (fr) etex, 580.2a, 180.1b, 0, 3);
draw (597.9a,180.1b)--(685.3a,180.1b);
draw (261.6a,611.4b)--(468.6a,586.2b)--(606.6a,562.5b)--(675.6a,560.9b)--(744.6a,531.9b)
--(813.6a,514.0b)--(882.6a,502.8b)--(951.6a,447.4b)--(1020.6a,339.0b)--(1089.6a,227.6b)
--(1108.7a,145.2b);
endfig;
end.
