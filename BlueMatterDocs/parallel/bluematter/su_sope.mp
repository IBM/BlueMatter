%GNUPLOT Metapost output: Wed Jul 18 18:25:26 2007


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
color fillcolor;
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
    rotated r;
  draw GPtext shifted (x,y)
enddef;

beginfig(9);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (178.8a,109.8b)--(195.4a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  0 etex, 161.1a, 109.8b, 0, 3);
draw (178.8a,222.8b)--(195.4a,222.8b);
draw (1506.3a,222.8b)--(1489.7a,222.8b);
put_text( btex  200 etex, 161.1a, 222.8b, 0, 3);
draw (178.8a,335.7b)--(195.4a,335.7b);
draw (1506.3a,335.7b)--(1489.7a,335.7b);
put_text( btex  400 etex, 161.1a, 335.7b, 0, 3);
draw (178.8a,448.7b)--(195.4a,448.7b);
draw (1506.3a,448.7b)--(1489.7a,448.7b);
put_text( btex  600 etex, 161.1a, 448.7b, 0, 3);
draw (178.8a,561.7b)--(195.4a,561.7b);
draw (1506.3a,561.7b)--(1489.7a,561.7b);
put_text( btex  800 etex, 161.1a, 561.7b, 0, 3);
draw (178.8a,674.7b)--(195.4a,674.7b);
draw (1506.3a,674.7b)--(1489.7a,674.7b);
put_text( btex  1000 etex, 161.1a, 674.7b, 0, 3);
draw (178.8a,787.6b)--(195.4a,787.6b);
draw (1506.3a,787.6b)--(1489.7a,787.6b);
put_text( btex  1200 etex, 161.1a, 787.6b, 0, 3);
draw (178.8a,900.6b)--(195.4a,900.6b);
draw (1506.3a,900.6b)--(1489.7a,900.6b);
put_text( btex  1400 etex, 161.1a, 900.6b, 0, 3);
draw (1506.3a,109.8b)--(1506.3a,126.4b);
draw (1506.3a,900.6b)--(1506.3a,884.0b);
put_text( btex  1 etex, 1506.3a, 73.2b, 0, 2);
draw (1332.6a,109.8b)--(1332.6a,118.1b);
draw (1332.6a,900.6b)--(1332.6a,892.3b);
draw (1231.0a,109.8b)--(1231.0a,118.1b);
draw (1231.0a,900.6b)--(1231.0a,892.3b);
draw (1159.0a,109.8b)--(1159.0a,118.1b);
draw (1159.0a,900.6b)--(1159.0a,892.3b);
draw (1103.1a,109.8b)--(1103.1a,118.1b);
draw (1103.1a,900.6b)--(1103.1a,892.3b);
draw (1057.4a,109.8b)--(1057.4a,118.1b);
draw (1057.4a,900.6b)--(1057.4a,892.3b);
draw (1018.7a,109.8b)--(1018.7a,118.1b);
draw (1018.7a,900.6b)--(1018.7a,892.3b);
draw (985.3a,109.8b)--(985.3a,118.1b);
draw (985.3a,900.6b)--(985.3a,892.3b);
draw (955.8a,109.8b)--(955.8a,118.1b);
draw (955.8a,900.6b)--(955.8a,892.3b);
draw (929.4a,109.8b)--(929.4a,126.4b);
draw (929.4a,900.6b)--(929.4a,884.0b);
put_text( btex  10 etex, 929.4a, 73.2b, 0, 2);
draw (755.7a,109.8b)--(755.7a,118.1b);
draw (755.7a,900.6b)--(755.7a,892.3b);
draw (654.1a,109.8b)--(654.1a,118.1b);
draw (654.1a,900.6b)--(654.1a,892.3b);
draw (582.0a,109.8b)--(582.0a,118.1b);
draw (582.0a,900.6b)--(582.0a,892.3b);
draw (526.1a,109.8b)--(526.1a,118.1b);
draw (526.1a,900.6b)--(526.1a,892.3b);
draw (480.5a,109.8b)--(480.5a,118.1b);
draw (480.5a,900.6b)--(480.5a,892.3b);
draw (441.8a,109.8b)--(441.8a,118.1b);
draw (441.8a,900.6b)--(441.8a,892.3b);
draw (408.4a,109.8b)--(408.4a,118.1b);
draw (408.4a,900.6b)--(408.4a,892.3b);
draw (378.9a,109.8b)--(378.9a,118.1b);
draw (378.9a,900.6b)--(378.9a,892.3b);
draw (352.5a,109.8b)--(352.5a,126.4b);
draw (352.5a,900.6b)--(352.5a,884.0b);
put_text( btex  100 etex, 352.5a, 73.2b, 0, 2);
draw (178.8a,109.8b)--(178.8a,118.1b);
draw (178.8a,900.6b)--(178.8a,892.3b);
draw (178.8a,900.6b)--(178.8a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(178.8a,900.6b);
put_text( btex Computation Rate (time-steps/second) etex, 36.6a, 505.2b, 90, 2);
linetype -2;
put_text( btex Atoms/Node etex, 842.5a, 18.3b, 0, 2);
linetype -2;
linetype 2;
linetype -2;
put_text( btex $64^3$ FFT etex, 688.8a, 854.1b, 0, 3);
linetype 2;
draw (706.5a,854.1b)--(793.9a,854.1b);
draw (681.7a,226.8b)--(855.4a,312.0b)--(1029.1a,422.8b)--(1202.7a,560.7b)--(1376.4a,690.3b);
gpdraw(2,681.7a,226.8b);
gpdraw(2,855.4a,312.0b);
gpdraw(2,1029.1a,422.8b);
gpdraw(2,1202.7a,560.7b);
gpdraw(2,1376.4a,690.3b);
gpdraw(2,750.2a,854.1b);
linetype 0;
linetype -2;
put_text( btex $128^3$ FFT etex, 688.8a, 817.5b, 0, 3);
linetype 0;
draw (706.5a,817.5b)--(793.9a,817.5b);
draw (681.7a,199.0b)--(855.4a,255.6b)--(1029.1a,343.5b)--(1202.7a,460.5b)--(1376.4a,487.9b);
gpdraw(4,681.7a,199.0b);
gpdraw(4,855.4a,255.6b);
gpdraw(4,1029.1a,343.5b);
gpdraw(4,1202.7a,460.5b);
gpdraw(4,1376.4a,487.9b);
gpdraw(4,750.2a,817.5b);
linetype -2;
draw (178.8a,900.6b)--(178.8a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(178.8a,900.6b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
