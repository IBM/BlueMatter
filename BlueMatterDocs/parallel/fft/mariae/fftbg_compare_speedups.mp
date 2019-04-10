%GNUPLOT Metapost output: Tue Oct 14 11:50:59 2003


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
draw (178.8a,109.8b)--(195.4a,109.8b);
draw (1446.3a,109.8b)--(1429.7a,109.8b);
put_text( btex  1 etex, 161.1a, 109.8b, 0, 3);
draw (178.8a,185.5b)--(187.1a,185.5b);
draw (1446.3a,185.5b)--(1438.0a,185.5b);
draw (178.8a,229.8b)--(187.1a,229.8b);
draw (1446.3a,229.8b)--(1438.0a,229.8b);
draw (178.8a,261.3b)--(187.1a,261.3b);
draw (1446.3a,261.3b)--(1438.0a,261.3b);
draw (178.8a,285.7b)--(187.1a,285.7b);
draw (1446.3a,285.7b)--(1438.0a,285.7b);
draw (178.8a,305.6b)--(187.1a,305.6b);
draw (1446.3a,305.6b)--(1438.0a,305.6b);
draw (178.8a,322.4b)--(187.1a,322.4b);
draw (1446.3a,322.4b)--(1438.0a,322.4b);
draw (178.8a,337.0b)--(187.1a,337.0b);
draw (1446.3a,337.0b)--(1438.0a,337.0b);
draw (178.8a,349.9b)--(187.1a,349.9b);
draw (1446.3a,349.9b)--(1438.0a,349.9b);
draw (178.8a,361.4b)--(195.4a,361.4b);
draw (1446.3a,361.4b)--(1429.7a,361.4b);
put_text( btex  10 etex, 161.1a, 361.4b, 0, 3);
draw (178.8a,437.1b)--(187.1a,437.1b);
draw (1446.3a,437.1b)--(1438.0a,437.1b);
draw (178.8a,481.4b)--(187.1a,481.4b);
draw (1446.3a,481.4b)--(1438.0a,481.4b);
draw (178.8a,512.9b)--(187.1a,512.9b);
draw (1446.3a,512.9b)--(1438.0a,512.9b);
draw (178.8a,537.3b)--(187.1a,537.3b);
draw (1446.3a,537.3b)--(1438.0a,537.3b);
draw (178.8a,557.2b)--(187.1a,557.2b);
draw (1446.3a,557.2b)--(1438.0a,557.2b);
draw (178.8a,574.0b)--(187.1a,574.0b);
draw (1446.3a,574.0b)--(1438.0a,574.0b);
draw (178.8a,588.6b)--(187.1a,588.6b);
draw (1446.3a,588.6b)--(1438.0a,588.6b);
draw (178.8a,601.5b)--(187.1a,601.5b);
draw (1446.3a,601.5b)--(1438.0a,601.5b);
draw (178.8a,613.0b)--(195.4a,613.0b);
draw (1446.3a,613.0b)--(1429.7a,613.0b);
put_text( btex  100 etex, 161.1a, 613.0b, 0, 3);
draw (178.8a,688.7b)--(187.1a,688.7b);
draw (1446.3a,688.7b)--(1438.0a,688.7b);
draw (178.8a,733.0b)--(187.1a,733.0b);
draw (1446.3a,733.0b)--(1438.0a,733.0b);
draw (178.8a,764.5b)--(187.1a,764.5b);
draw (1446.3a,764.5b)--(1438.0a,764.5b);
draw (178.8a,788.9b)--(187.1a,788.9b);
draw (1446.3a,788.9b)--(1438.0a,788.9b);
draw (178.8a,808.8b)--(187.1a,808.8b);
draw (1446.3a,808.8b)--(1438.0a,808.8b);
draw (178.8a,825.6b)--(187.1a,825.6b);
draw (1446.3a,825.6b)--(1438.0a,825.6b);
draw (178.8a,840.2b)--(187.1a,840.2b);
draw (1446.3a,840.2b)--(1438.0a,840.2b);
draw (178.8a,853.1b)--(187.1a,853.1b);
draw (1446.3a,853.1b)--(1438.0a,853.1b);
draw (178.8a,864.6b)--(195.4a,864.6b);
draw (1446.3a,864.6b)--(1429.7a,864.6b);
put_text( btex  1000 etex, 161.1a, 864.6b, 0, 3);
draw (178.8a,109.8b)--(178.8a,126.4b);
draw (178.8a,864.6b)--(178.8a,848.0b);
put_text( btex  1 etex, 178.8a, 73.2b, 0, 2);
draw (306.0a,109.8b)--(306.0a,118.1b);
draw (306.0a,864.6b)--(306.0a,856.3b);
draw (380.4a,109.8b)--(380.4a,118.1b);
draw (380.4a,864.6b)--(380.4a,856.3b);
draw (433.2a,109.8b)--(433.2a,118.1b);
draw (433.2a,864.6b)--(433.2a,856.3b);
draw (474.1a,109.8b)--(474.1a,118.1b);
draw (474.1a,864.6b)--(474.1a,856.3b);
draw (507.6a,109.8b)--(507.6a,118.1b);
draw (507.6a,864.6b)--(507.6a,856.3b);
draw (535.9a,109.8b)--(535.9a,118.1b);
draw (535.9a,864.6b)--(535.9a,856.3b);
draw (560.4a,109.8b)--(560.4a,118.1b);
draw (560.4a,864.6b)--(560.4a,856.3b);
draw (582.0a,109.8b)--(582.0a,118.1b);
draw (582.0a,864.6b)--(582.0a,856.3b);
draw (601.3a,109.8b)--(601.3a,126.4b);
draw (601.3a,864.6b)--(601.3a,848.0b);
put_text( btex  10 etex, 601.3a, 73.2b, 0, 2);
draw (728.5a,109.8b)--(728.5a,118.1b);
draw (728.5a,864.6b)--(728.5a,856.3b);
draw (802.9a,109.8b)--(802.9a,118.1b);
draw (802.9a,864.6b)--(802.9a,856.3b);
draw (855.7a,109.8b)--(855.7a,118.1b);
draw (855.7a,864.6b)--(855.7a,856.3b);
draw (896.6a,109.8b)--(896.6a,118.1b);
draw (896.6a,864.6b)--(896.6a,856.3b);
draw (930.1a,109.8b)--(930.1a,118.1b);
draw (930.1a,864.6b)--(930.1a,856.3b);
draw (958.4a,109.8b)--(958.4a,118.1b);
draw (958.4a,864.6b)--(958.4a,856.3b);
draw (982.9a,109.8b)--(982.9a,118.1b);
draw (982.9a,864.6b)--(982.9a,856.3b);
draw (1004.5a,109.8b)--(1004.5a,118.1b);
draw (1004.5a,864.6b)--(1004.5a,856.3b);
draw (1023.8a,109.8b)--(1023.8a,126.4b);
draw (1023.8a,864.6b)--(1023.8a,848.0b);
put_text( btex  100 etex, 1023.8a, 73.2b, 0, 2);
draw (1151.0a,109.8b)--(1151.0a,118.1b);
draw (1151.0a,864.6b)--(1151.0a,856.3b);
draw (1225.4a,109.8b)--(1225.4a,118.1b);
draw (1225.4a,864.6b)--(1225.4a,856.3b);
draw (1278.2a,109.8b)--(1278.2a,118.1b);
draw (1278.2a,864.6b)--(1278.2a,856.3b);
draw (1319.1a,109.8b)--(1319.1a,118.1b);
draw (1319.1a,864.6b)--(1319.1a,856.3b);
draw (1352.6a,109.8b)--(1352.6a,118.1b);
draw (1352.6a,864.6b)--(1352.6a,856.3b);
draw (1380.9a,109.8b)--(1380.9a,118.1b);
draw (1380.9a,864.6b)--(1380.9a,856.3b);
draw (1405.4a,109.8b)--(1405.4a,118.1b);
draw (1405.4a,864.6b)--(1405.4a,856.3b);
draw (1427.0a,109.8b)--(1427.0a,118.1b);
draw (1427.0a,864.6b)--(1427.0a,856.3b);
draw (1446.3a,109.8b)--(1446.3a,126.4b);
draw (1446.3a,864.6b)--(1446.3a,848.0b);
put_text( btex  1000 etex, 1446.3a, 73.2b, 0, 2);
draw (178.8a,109.8b)--(1446.3a,109.8b)--(1446.3a,864.6b)--(178.8a,864.6b)--(178.8a,109.8b);
put_text( btex Speedup etex, 36.6a, 487.2b, 1, 2);
put_text( btex Task Count etex, 812.5a, 18.3b, 0, 2);
linetype 0;
put_text( btex Ideal etex, 1305.8a, 291.1b, 0, 3);
draw (1323.5a,291.1b)--(1410.9a,291.1b);
draw (178.8a,109.8b)--(560.4a,337.0b)--(814.7a,488.5b)--(941.9a,564.2b)--(1069.1a,640.0b)
--(1196.3a,715.7b)--(1323.5a,791.5b);
linetype 1;
put_text( btex $64^3$ FFTBGMPI on BG/L -qarch=440, 500MHz etex, 1305.8a, 254.5b, 0, 3);
gpdraw(0,178.8a,109.8b);
gpdraw(0,560.4a,349.3b);
gpdraw(0,814.7a,497.1b);
gpdraw(0,941.9a,554.1b);
gpdraw(0,1069.1a,628.7b);
gpdraw(0,1196.3a,684.6b);
gpdraw(0,1323.5a,707.6b);
gpdraw(0,1367.2a,254.5b);
linetype 2;
put_text( btex $128^3$ FFTBG on Power4 etex, 1305.8a, 217.9b, 0, 3);
gpdraw(1,178.8a,109.8b);
gpdraw(1,306.0a,191.8b);
gpdraw(1,433.2a,253.0b);
gpdraw(1,560.4a,317.0b);
gpdraw(1,687.5a,338.9b);
gpdraw(1,814.7a,395.7b);
gpdraw(1,814.7a,390.9b);
gpdraw(1,1069.1a,523.7b);
gpdraw(1,941.9a,476.3b);
gpdraw(1,1196.3a,579.5b);
gpdraw(1,1323.5a,607.9b);
gpdraw(1,1367.2a,217.9b);
linetype 3;
put_text( btex $64^3$ packet layer -qarch=440d, 444MHz etex, 1305.8a, 181.3b, 0, 3);
gpdraw(2,178.8a,109.8b);
gpdraw(2,306.0a,194.0b);
gpdraw(2,560.4a,345.2b);
gpdraw(2,814.7a,465.1b);
gpdraw(2,1367.2a,181.3b);
linetype 4;
put_text( btex $128^3$ packet layer -qarch=440d, 444MHz etex, 1305.8a, 144.7b, 0, 3);
gpdraw(3,178.8a,109.8b);
gpdraw(3,306.0a,183.7b);
gpdraw(3,560.4a,361.5b);
gpdraw(3,814.7a,520.5b);
gpdraw(3,1367.2a,144.7b);
endfig;
end.
