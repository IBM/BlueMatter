%GNUPLOT Metapost output: Tue Oct 14 11:50:06 2003


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
verbatimtex
\def\setfont#1#2{%.
  \font\gpfont=#1 at #2pt
\gpfont}
\setfont{lsr}{10.00}
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

beginfig(1);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,109.8b)--(213.1a,109.8b);
draw (1446.3a,109.8b)--(1429.7a,109.8b);
put_text( btex  0.001 etex, 178.8a, 109.8b, 0, 3);
draw (196.5a,166.6b)--(204.8a,166.6b);
draw (1446.3a,166.6b)--(1438.0a,166.6b);
draw (196.5a,199.8b)--(204.8a,199.8b);
draw (1446.3a,199.8b)--(1438.0a,199.8b);
draw (196.5a,223.4b)--(204.8a,223.4b);
draw (1446.3a,223.4b)--(1438.0a,223.4b);
draw (196.5a,241.7b)--(204.8a,241.7b);
draw (1446.3a,241.7b)--(1438.0a,241.7b);
draw (196.5a,256.6b)--(204.8a,256.6b);
draw (1446.3a,256.6b)--(1438.0a,256.6b);
draw (196.5a,269.3b)--(204.8a,269.3b);
draw (1446.3a,269.3b)--(1438.0a,269.3b);
draw (196.5a,280.2b)--(204.8a,280.2b);
draw (1446.3a,280.2b)--(1438.0a,280.2b);
draw (196.5a,289.9b)--(204.8a,289.9b);
draw (1446.3a,289.9b)--(1438.0a,289.9b);
draw (196.5a,298.5b)--(213.1a,298.5b);
draw (1446.3a,298.5b)--(1429.7a,298.5b);
put_text( btex  0.01 etex, 178.8a, 298.5b, 0, 3);
draw (196.5a,355.3b)--(204.8a,355.3b);
draw (1446.3a,355.3b)--(1438.0a,355.3b);
draw (196.5a,388.5b)--(204.8a,388.5b);
draw (1446.3a,388.5b)--(1438.0a,388.5b);
draw (196.5a,412.1b)--(204.8a,412.1b);
draw (1446.3a,412.1b)--(1438.0a,412.1b);
draw (196.5a,430.4b)--(204.8a,430.4b);
draw (1446.3a,430.4b)--(1438.0a,430.4b);
draw (196.5a,445.3b)--(204.8a,445.3b);
draw (1446.3a,445.3b)--(1438.0a,445.3b);
draw (196.5a,458.0b)--(204.8a,458.0b);
draw (1446.3a,458.0b)--(1438.0a,458.0b);
draw (196.5a,468.9b)--(204.8a,468.9b);
draw (1446.3a,468.9b)--(1438.0a,468.9b);
draw (196.5a,478.6b)--(204.8a,478.6b);
draw (1446.3a,478.6b)--(1438.0a,478.6b);
draw (196.5a,487.2b)--(213.1a,487.2b);
draw (1446.3a,487.2b)--(1429.7a,487.2b);
put_text( btex  0.1 etex, 178.8a, 487.2b, 0, 3);
draw (196.5a,544.0b)--(204.8a,544.0b);
draw (1446.3a,544.0b)--(1438.0a,544.0b);
draw (196.5a,577.2b)--(204.8a,577.2b);
draw (1446.3a,577.2b)--(1438.0a,577.2b);
draw (196.5a,600.8b)--(204.8a,600.8b);
draw (1446.3a,600.8b)--(1438.0a,600.8b);
draw (196.5a,619.1b)--(204.8a,619.1b);
draw (1446.3a,619.1b)--(1438.0a,619.1b);
draw (196.5a,634.0b)--(204.8a,634.0b);
draw (1446.3a,634.0b)--(1438.0a,634.0b);
draw (196.5a,646.7b)--(204.8a,646.7b);
draw (1446.3a,646.7b)--(1438.0a,646.7b);
draw (196.5a,657.6b)--(204.8a,657.6b);
draw (1446.3a,657.6b)--(1438.0a,657.6b);
draw (196.5a,667.3b)--(204.8a,667.3b);
draw (1446.3a,667.3b)--(1438.0a,667.3b);
draw (196.5a,675.9b)--(213.1a,675.9b);
draw (1446.3a,675.9b)--(1429.7a,675.9b);
put_text( btex  1 etex, 178.8a, 675.9b, 0, 3);
draw (196.5a,732.7b)--(204.8a,732.7b);
draw (1446.3a,732.7b)--(1438.0a,732.7b);
draw (196.5a,765.9b)--(204.8a,765.9b);
draw (1446.3a,765.9b)--(1438.0a,765.9b);
draw (196.5a,789.5b)--(204.8a,789.5b);
draw (1446.3a,789.5b)--(1438.0a,789.5b);
draw (196.5a,807.8b)--(204.8a,807.8b);
draw (1446.3a,807.8b)--(1438.0a,807.8b);
draw (196.5a,822.7b)--(204.8a,822.7b);
draw (1446.3a,822.7b)--(1438.0a,822.7b);
draw (196.5a,835.4b)--(204.8a,835.4b);
draw (1446.3a,835.4b)--(1438.0a,835.4b);
draw (196.5a,846.3b)--(204.8a,846.3b);
draw (1446.3a,846.3b)--(1438.0a,846.3b);
draw (196.5a,856.0b)--(204.8a,856.0b);
draw (1446.3a,856.0b)--(1438.0a,856.0b);
draw (196.5a,864.6b)--(213.1a,864.6b);
draw (1446.3a,864.6b)--(1429.7a,864.6b);
put_text( btex  10 etex, 178.8a, 864.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,864.6b)--(196.5a,848.0b);
put_text( btex  1 etex, 196.5a, 73.2b, 0, 2);
draw (321.9a,109.8b)--(321.9a,118.1b);
draw (321.9a,864.6b)--(321.9a,856.3b);
draw (395.3a,109.8b)--(395.3a,118.1b);
draw (395.3a,864.6b)--(395.3a,856.3b);
draw (447.3a,109.8b)--(447.3a,118.1b);
draw (447.3a,864.6b)--(447.3a,856.3b);
draw (487.7a,109.8b)--(487.7a,118.1b);
draw (487.7a,864.6b)--(487.7a,856.3b);
draw (520.7a,109.8b)--(520.7a,118.1b);
draw (520.7a,864.6b)--(520.7a,856.3b);
draw (548.6a,109.8b)--(548.6a,118.1b);
draw (548.6a,864.6b)--(548.6a,856.3b);
draw (572.7a,109.8b)--(572.7a,118.1b);
draw (572.7a,864.6b)--(572.7a,856.3b);
draw (594.0a,109.8b)--(594.0a,118.1b);
draw (594.0a,864.6b)--(594.0a,856.3b);
draw (613.1a,109.8b)--(613.1a,126.4b);
draw (613.1a,864.6b)--(613.1a,848.0b);
put_text( btex  10 etex, 613.1a, 73.2b, 0, 2);
draw (738.5a,109.8b)--(738.5a,118.1b);
draw (738.5a,864.6b)--(738.5a,856.3b);
draw (811.9a,109.8b)--(811.9a,118.1b);
draw (811.9a,864.6b)--(811.9a,856.3b);
draw (863.9a,109.8b)--(863.9a,118.1b);
draw (863.9a,864.6b)--(863.9a,856.3b);
draw (904.3a,109.8b)--(904.3a,118.1b);
draw (904.3a,864.6b)--(904.3a,856.3b);
draw (937.3a,109.8b)--(937.3a,118.1b);
draw (937.3a,864.6b)--(937.3a,856.3b);
draw (965.2a,109.8b)--(965.2a,118.1b);
draw (965.2a,864.6b)--(965.2a,856.3b);
draw (989.3a,109.8b)--(989.3a,118.1b);
draw (989.3a,864.6b)--(989.3a,856.3b);
draw (1010.6a,109.8b)--(1010.6a,118.1b);
draw (1010.6a,864.6b)--(1010.6a,856.3b);
draw (1029.7a,109.8b)--(1029.7a,126.4b);
draw (1029.7a,864.6b)--(1029.7a,848.0b);
put_text( btex  100 etex, 1029.7a, 73.2b, 0, 2);
draw (1155.1a,109.8b)--(1155.1a,118.1b);
draw (1155.1a,864.6b)--(1155.1a,856.3b);
draw (1228.5a,109.8b)--(1228.5a,118.1b);
draw (1228.5a,864.6b)--(1228.5a,856.3b);
draw (1280.5a,109.8b)--(1280.5a,118.1b);
draw (1280.5a,864.6b)--(1280.5a,856.3b);
draw (1320.9a,109.8b)--(1320.9a,118.1b);
draw (1320.9a,864.6b)--(1320.9a,856.3b);
draw (1353.9a,109.8b)--(1353.9a,118.1b);
draw (1353.9a,864.6b)--(1353.9a,856.3b);
draw (1381.8a,109.8b)--(1381.8a,118.1b);
draw (1381.8a,864.6b)--(1381.8a,856.3b);
draw (1405.9a,109.8b)--(1405.9a,118.1b);
draw (1405.9a,864.6b)--(1405.9a,856.3b);
draw (1427.2a,109.8b)--(1427.2a,118.1b);
draw (1427.2a,864.6b)--(1427.2a,856.3b);
draw (1446.3a,109.8b)--(1446.3a,126.4b);
draw (1446.3a,864.6b)--(1446.3a,848.0b);
put_text( btex  1000 etex, 1446.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1446.3a,109.8b)--(1446.3a,864.6b)--(196.5a,864.6b)--(196.5a,109.8b);
put_text( btex 3D-FFT time (seconds) etex, 36.6a, 487.2b, 1, 2);
put_text( btex Task Count etex, 821.4a, 18.3b, 0, 2);
linetype 0;
put_text( btex $64^3$ FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft etex, 1305.8a, 829.7b, 0, 3);
gpdraw(0,196.5a,628.0b);
gpdraw(0,572.7a,448.3b);
gpdraw(0,823.5a,337.5b);
gpdraw(0,949.0a,294.8b);
gpdraw(0,1074.4a,238.8b);
gpdraw(0,1199.8a,196.9b);
gpdraw(0,1325.2a,179.6b);
gpdraw(0,1367.2a,829.7b);
linetype 1;
put_text( btex $128^3$ FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft etex, 1305.8a, 793.1b, 0, 3);
gpdraw(1,572.7a,639.3b);
gpdraw(1,823.5a,513.0b);
gpdraw(1,949.0a,466.6b);
gpdraw(1,1074.4a,405.2b);
gpdraw(1,1199.8a,351.1b);
gpdraw(1,1325.2a,310.3b);
gpdraw(1,1367.2a,793.1b);
linetype 2;
put_text( btex $64^3$ packet layer -qarch=440d 444MHz etex, 1305.8a, 756.5b, 0, 3);
gpdraw(2,196.5a,570.8b);
gpdraw(2,321.9a,507.7b);
gpdraw(2,572.7a,394.3b);
gpdraw(2,823.5a,304.4b);
gpdraw(2,1367.2a,756.5b);
linetype 3;
put_text( btex $128^3$ packet layer -qarch=440d 444MHz etex, 1305.8a, 719.9b, 0, 3);
gpdraw(3,196.5a,762.3b);
gpdraw(3,321.9a,706.9b);
gpdraw(3,572.7a,573.6b);
gpdraw(3,823.5a,454.2b);
gpdraw(3,1367.2a,719.9b);
linetype 4;
put_text( btex $128^3$ FFTBG on Power4 etex, 1305.8a, 683.3b, 0, 3);
gpdraw(4,196.5a,720.2b);
gpdraw(4,321.9a,658.7b);
gpdraw(4,447.3a,612.9b);
gpdraw(4,572.7a,564.8b);
gpdraw(4,698.1a,548.4b);
gpdraw(4,823.5a,505.9b);
gpdraw(4,823.5a,509.4b);
gpdraw(4,1074.4a,409.8b);
gpdraw(4,949.0a,445.3b);
gpdraw(4,1199.8a,367.9b);
gpdraw(4,1325.2a,346.7b);
gpdraw(4,1367.2a,683.3b);
endfig;
end.
