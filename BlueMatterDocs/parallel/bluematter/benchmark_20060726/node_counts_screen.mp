%GNUPLOT Metapost output: Thu Aug 03 14:01:25 2006


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

beginfig(0);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (143.4a,109.8b)--(160.0a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  0 etex, 125.7a, 109.8b, 0, 3);
draw (143.4a,215.2b)--(160.0a,215.2b);
draw (1506.3a,215.2b)--(1489.7a,215.2b);
put_text( btex  2 etex, 125.7a, 215.2b, 0, 3);
draw (143.4a,320.7b)--(160.0a,320.7b);
draw (1506.3a,320.7b)--(1489.7a,320.7b);
put_text( btex  4 etex, 125.7a, 320.7b, 0, 3);
draw (143.4a,426.1b)--(160.0a,426.1b);
draw (1506.3a,426.1b)--(1489.7a,426.1b);
put_text( btex  6 etex, 125.7a, 426.1b, 0, 3);
draw (143.4a,531.6b)--(160.0a,531.6b);
draw (1506.3a,531.6b)--(1489.7a,531.6b);
put_text( btex  8 etex, 125.7a, 531.6b, 0, 3);
draw (143.4a,637.0b)--(160.0a,637.0b);
draw (1506.3a,637.0b)--(1489.7a,637.0b);
put_text( btex  10 etex, 125.7a, 637.0b, 0, 3);
draw (143.4a,742.4b)--(160.0a,742.4b);
draw (1506.3a,742.4b)--(1489.7a,742.4b);
put_text( btex  12 etex, 125.7a, 742.4b, 0, 3);
draw (143.4a,847.9b)--(160.0a,847.9b);
draw (1506.3a,847.9b)--(1489.7a,847.9b);
put_text( btex  14 etex, 125.7a, 847.9b, 0, 3);
draw (143.4a,109.8b)--(143.4a,126.4b);
draw (143.4a,900.6b)--(143.4a,884.0b);
put_text( btex  0 etex, 143.4a, 73.2b, 0, 2);
draw (294.8a,109.8b)--(294.8a,126.4b);
draw (294.8a,900.6b)--(294.8a,884.0b);
put_text( btex  2000 etex, 294.8a, 73.2b, 0, 2);
draw (446.3a,109.8b)--(446.3a,126.4b);
draw (446.3a,900.6b)--(446.3a,884.0b);
put_text( btex  4000 etex, 446.3a, 73.2b, 0, 2);
draw (597.7a,109.8b)--(597.7a,126.4b);
draw (597.7a,900.6b)--(597.7a,884.0b);
put_text( btex  6000 etex, 597.7a, 73.2b, 0, 2);
draw (749.1a,109.8b)--(749.1a,126.4b);
draw (749.1a,900.6b)--(749.1a,884.0b);
put_text( btex  8000 etex, 749.1a, 73.2b, 0, 2);
draw (900.6a,109.8b)--(900.6a,126.4b);
draw (900.6a,900.6b)--(900.6a,884.0b);
put_text( btex  10000 etex, 900.6a, 73.2b, 0, 2);
draw (1052.0a,109.8b)--(1052.0a,126.4b);
draw (1052.0a,900.6b)--(1052.0a,884.0b);
put_text( btex  12000 etex, 1052.0a, 73.2b, 0, 2);
draw (1203.4a,109.8b)--(1203.4a,126.4b);
draw (1203.4a,900.6b)--(1203.4a,884.0b);
put_text( btex  14000 etex, 1203.4a, 73.2b, 0, 2);
draw (1354.9a,109.8b)--(1354.9a,126.4b);
draw (1354.9a,900.6b)--(1354.9a,884.0b);
put_text( btex  16000 etex, 1354.9a, 73.2b, 0, 2);
draw (1506.3a,109.8b)--(1506.3a,126.4b);
draw (1506.3a,900.6b)--(1506.3a,884.0b);
put_text( btex  18000 etex, 1506.3a, 73.2b, 0, 2);
draw (143.4a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(143.4a,900.6b)--(143.4a,109.8b);
put_text( btex Communicating Partners/$\sqrt{p}$ etex, 36.6a, 505.2b, 1, 2);
put_text( btex Partition Size (p) etex, 824.8a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Hairpin (V4) etex, 1365.8a, 865.7b, 0, 3);
linetype 0;
draw (1383.5a,865.7b)--(1470.9a,865.7b);
draw (182.2a,401.0b)--(220.9a,467.3b)--(298.5a,558.3b)--(453.5a,614.8b)--(453.5a,637.8b)
--(763.7a,723.2b);
gpdraw(0,182.2a,401.0b);
gpdraw(0,220.9a,467.3b);
gpdraw(0,298.5a,558.3b);
gpdraw(0,453.5a,614.8b);
gpdraw(0,453.5a,637.8b);
gpdraw(0,763.7a,723.2b);
gpdraw(0,1427.2a,865.7b);
linetype 1;
linetype -2;
put_text( btex Hairpin (V5 Full Skin) etex, 1365.8a, 829.1b, 0, 3);
linetype 1;
draw (1383.5a,829.1b)--(1470.9a,829.1b);
draw (182.2a,338.1b)--(220.9a,376.7b)--(298.5a,429.0b)--(453.5a,408.0b)--(453.5a,447.5b)
--(763.7a,604.9b);
gpdraw(1,182.2a,338.1b);
gpdraw(1,220.9a,376.7b);
gpdraw(1,298.5a,429.0b);
gpdraw(1,453.5a,408.0b);
gpdraw(1,453.5a,447.5b);
gpdraw(1,763.7a,604.9b);
gpdraw(1,1427.2a,829.1b);
linetype 2;
linetype -2;
put_text( btex Hairpin (V5 Sparse Skin) etex, 1365.8a, 792.5b, 0, 3);
linetype 2;
draw (1383.5a,792.5b)--(1470.9a,792.5b);
draw (182.2a,210.0b)--(220.9a,213.6b)--(298.5a,215.8b)--(453.5a,213.6b)--(453.5a,216.9b)
--(763.7a,168.0b);
gpdraw(2,182.2a,210.0b);
gpdraw(2,220.9a,213.6b);
gpdraw(2,298.5a,215.8b);
gpdraw(2,453.5a,213.6b);
gpdraw(2,453.5a,216.9b);
gpdraw(2,763.7a,168.0b);
gpdraw(2,1427.2a,792.5b);
linetype 3;
linetype -2;
put_text( btex SOPE (V4) etex, 1365.8a, 755.9b, 0, 3);
linetype 3;
draw (1383.5a,755.9b)--(1470.9a,755.9b);
draw (182.2a,326.5b)--(220.9a,332.2b)--(298.5a,374.2b)--(453.5a,319.9b)--(453.5a,364.3b)
--(763.7a,358.5b)--(1383.9a,413.4b);
gpdraw(3,182.2a,326.5b);
gpdraw(3,220.9a,332.2b);
gpdraw(3,298.5a,374.2b);
gpdraw(3,453.5a,319.9b);
gpdraw(3,453.5a,364.3b);
gpdraw(3,763.7a,358.5b);
gpdraw(3,1383.9a,413.4b);
gpdraw(3,1427.2a,755.9b);
linetype 4;
linetype -2;
put_text( btex SOPE (V5 Full Skin) etex, 1365.8a, 719.3b, 0, 3);
linetype 4;
draw (1383.5a,719.3b)--(1470.9a,719.3b);
draw (182.2a,300.9b)--(220.9a,297.6b)--(298.5a,317.2b)--(453.5a,263.0b)--(453.5a,296.0b)
--(763.7a,274.1b)--(1383.9a,291.8b);
gpdraw(4,182.2a,300.9b);
gpdraw(4,220.9a,297.6b);
gpdraw(4,298.5a,317.2b);
gpdraw(4,453.5a,263.0b);
gpdraw(4,453.5a,296.0b);
gpdraw(4,763.7a,274.1b);
gpdraw(4,1383.9a,291.8b);
gpdraw(4,1427.2a,719.3b);
linetype 5;
linetype -2;
put_text( btex SOPE (V5 Sparse Skin) etex, 1365.8a, 682.7b, 0, 3);
linetype 5;
draw (1383.5a,682.7b)--(1470.9a,682.7b);
draw (182.2a,184.4b)--(220.9a,180.6b)--(298.5a,182.0b)--(453.5a,173.2b)--(453.5a,175.7b)
--(763.7a,171.5b)--(1383.9a,172.8b);
gpdraw(5,182.2a,184.4b);
gpdraw(5,220.9a,180.6b);
gpdraw(5,298.5a,182.0b);
gpdraw(5,453.5a,173.2b);
gpdraw(5,453.5a,175.7b);
gpdraw(5,763.7a,171.5b);
gpdraw(5,1383.9a,172.8b);
gpdraw(5,1427.2a,682.7b);
linetype 6;
linetype -2;
put_text( btex Rhodopsin (V4) etex, 1365.8a, 646.1b, 0, 3);
linetype 6;
draw (1383.5a,646.1b)--(1470.9a,646.1b);
draw (182.2a,214.6b)--(220.9a,213.6b)--(298.5a,232.1b)--(453.5a,230.9b)--(763.7a,231.5b)
--(1383.9a,253.5b);
gpdraw(6,182.2a,214.6b);
gpdraw(6,220.9a,213.6b);
gpdraw(6,298.5a,232.1b);
gpdraw(6,453.5a,230.9b);
gpdraw(6,453.5a,230.9b);
gpdraw(6,763.7a,231.5b);
gpdraw(6,1383.9a,253.5b);
gpdraw(6,1427.2a,646.1b);
linetype 7;
linetype -2;
put_text( btex Rhodopsin (V5 Full Skin) etex, 1365.8a, 609.5b, 0, 3);
linetype 7;
draw (1383.5a,609.5b)--(1470.9a,609.5b);
draw (182.2a,207.7b)--(220.9a,205.4b)--(298.5a,214.6b)--(453.5a,210.3b)--(763.7a,204.2b)
--(1383.9a,209.5b);
gpdraw(7,182.2a,207.7b);
gpdraw(7,220.9a,205.4b);
gpdraw(7,298.5a,214.6b);
gpdraw(7,453.5a,210.3b);
gpdraw(7,453.5a,210.3b);
gpdraw(7,763.7a,204.2b);
gpdraw(7,1383.9a,209.5b);
gpdraw(7,1427.2a,609.5b);
linetype 0;
linetype -2;
put_text( btex Rhodopsin (V5 Sparse Skin) etex, 1365.8a, 572.9b, 0, 3);
linetype 0;
draw (1383.5a,572.9b)--(1470.9a,572.9b);
draw (182.2a,161.1b)--(220.9a,154.3b)--(298.5a,154.1b)--(453.5a,151.0b)--(453.5a,151.8b)
--(763.7a,147.7b)--(1383.9a,146.5b);
gpdraw(8,182.2a,161.1b);
gpdraw(8,220.9a,154.3b);
gpdraw(8,298.5a,154.1b);
gpdraw(8,453.5a,151.0b);
gpdraw(8,453.5a,151.8b);
gpdraw(8,763.7a,147.7b);
gpdraw(8,1383.9a,146.5b);
gpdraw(8,1427.2a,572.9b);
linetype 1;
linetype -2;
put_text( btex ApoA1 (V4) etex, 1365.8a, 536.3b, 0, 3);
linetype 1;
draw (1383.5a,536.3b)--(1470.9a,536.3b);
draw (182.2a,214.6b)--(220.9a,213.6b)--(298.5a,218.1b)--(453.5a,221.0b)--(453.5a,211.1b)
--(763.7a,217.6b)--(1383.9a,214.8b);
gpdraw(9,182.2a,214.6b);
gpdraw(9,220.9a,213.6b);
gpdraw(9,298.5a,218.1b);
gpdraw(9,453.5a,221.0b);
gpdraw(9,453.5a,211.1b);
gpdraw(9,763.7a,217.6b);
gpdraw(9,1383.9a,214.8b);
gpdraw(9,1427.2a,536.3b);
linetype 2;
linetype -2;
put_text( btex ApoA1 (V5 Full Skin) etex, 1365.8a, 499.7b, 0, 3);
linetype 2;
draw (1383.5a,499.7b)--(1470.9a,499.7b);
draw (182.2a,207.7b)--(220.9a,205.4b)--(298.5a,205.3b)--(453.5a,203.7b)--(453.5a,197.1b)
--(763.7a,194.8b)--(1383.9a,186.4b);
gpdraw(0,182.2a,207.7b);
gpdraw(0,220.9a,205.4b);
gpdraw(0,298.5a,205.3b);
gpdraw(0,453.5a,203.7b);
gpdraw(0,453.5a,197.1b);
gpdraw(0,763.7a,194.8b);
gpdraw(0,1383.9a,186.4b);
gpdraw(0,1427.2a,499.7b);
linetype 3;
linetype -2;
put_text( btex ApoA1 (V5 Sparse Skin) etex, 1365.8a, 463.1b, 0, 3);
linetype 3;
draw (1383.5a,463.1b)--(1470.9a,463.1b);
draw (182.2a,158.7b)--(220.9a,152.6b)--(298.5a,147.1b)--(453.5a,145.2b)--(453.5a,146.0b)
--(763.7a,143.0b)--(1383.9a,141.9b);
gpdraw(1,182.2a,158.7b);
gpdraw(1,220.9a,152.6b);
gpdraw(1,298.5a,147.1b);
gpdraw(1,453.5a,145.2b);
gpdraw(1,453.5a,146.0b);
gpdraw(1,763.7a,143.0b);
gpdraw(1,1383.9a,141.9b);
gpdraw(1,1427.2a,463.1b);
linetype -2;
draw (143.4a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(143.4a,900.6b)--(143.4a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
