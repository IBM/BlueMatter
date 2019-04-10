%GNUPLOT Metapost output: Sun Apr 25 01:49:15 2004


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

beginfig(25);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (261.6a,145.2b)--(278.2a,145.2b);
draw (1058.4a,145.2b)--(1041.8a,145.2b);
put_text( btex  $10^3$ etex, 243.9a, 145.2b, 0, 3);
draw (261.6a,177.6b)--(269.9a,177.6b);
draw (1058.4a,177.6b)--(1050.1a,177.6b);
draw (261.6a,220.4b)--(269.9a,220.4b);
draw (1058.4a,220.4b)--(1050.1a,220.4b);
draw (261.6a,242.4b)--(269.9a,242.4b);
draw (1058.4a,242.4b)--(1050.1a,242.4b);
draw (261.6a,252.8b)--(278.2a,252.8b);
draw (1058.4a,252.8b)--(1041.8a,252.8b);
put_text( btex  $10^4$ etex, 243.9a, 252.8b, 0, 3);
draw (261.6a,285.2b)--(269.9a,285.2b);
draw (1058.4a,285.2b)--(1050.1a,285.2b);
draw (261.6a,328.1b)--(269.9a,328.1b);
draw (1058.4a,328.1b)--(1050.1a,328.1b);
draw (261.6a,350.0b)--(269.9a,350.0b);
draw (1058.4a,350.0b)--(1050.1a,350.0b);
draw (261.6a,360.5b)--(278.2a,360.5b);
draw (1058.4a,360.5b)--(1041.8a,360.5b);
put_text( btex  $10^5$ etex, 243.9a, 360.5b, 0, 3);
draw (261.6a,392.9b)--(269.9a,392.9b);
draw (1058.4a,392.9b)--(1050.1a,392.9b);
draw (261.6a,435.7b)--(269.9a,435.7b);
draw (1058.4a,435.7b)--(1050.1a,435.7b);
draw (261.6a,457.7b)--(269.9a,457.7b);
draw (1058.4a,457.7b)--(1050.1a,457.7b);
draw (261.6a,468.1b)--(278.2a,468.1b);
draw (1058.4a,468.1b)--(1041.8a,468.1b);
put_text( btex  $10^6$ etex, 243.9a, 468.1b, 0, 3);
draw (261.6a,500.5b)--(269.9a,500.5b);
draw (1058.4a,500.5b)--(1050.1a,500.5b);
draw (261.6a,543.4b)--(269.9a,543.4b);
draw (1058.4a,543.4b)--(1050.1a,543.4b);
draw (261.6a,565.3b)--(269.9a,565.3b);
draw (1058.4a,565.3b)--(1050.1a,565.3b);
draw (261.6a,575.8b)--(278.2a,575.8b);
draw (1058.4a,575.8b)--(1041.8a,575.8b);
put_text( btex  $10^7$ etex, 243.9a, 575.8b, 0, 3);
draw (261.6a,608.2b)--(269.9a,608.2b);
draw (1058.4a,608.2b)--(1050.1a,608.2b);
draw (261.6a,651.0b)--(269.9a,651.0b);
draw (1058.4a,651.0b)--(1050.1a,651.0b);
draw (261.6a,673.0b)--(269.9a,673.0b);
draw (1058.4a,673.0b)--(1050.1a,673.0b);
draw (261.6a,683.4b)--(278.2a,683.4b);
draw (1058.4a,683.4b)--(1041.8a,683.4b);
put_text( btex  $10^8$ etex, 243.9a, 683.4b, 0, 3);
draw (261.6a,145.2b)--(261.6a,161.8b);
draw (261.6a,683.4b)--(261.6a,666.8b);
put_text( btex  1 etex, 261.6a, 108.6b, 0, 2);
draw (309.6a,145.2b)--(309.6a,153.5b);
draw (309.6a,683.4b)--(309.6a,675.1b);
draw (373.0a,145.2b)--(373.0a,153.5b);
draw (373.0a,683.4b)--(373.0a,675.1b);
draw (405.5a,145.2b)--(405.5a,153.5b);
draw (405.5a,683.4b)--(405.5a,675.1b);
draw (421.0a,145.2b)--(421.0a,161.8b);
draw (421.0a,683.4b)--(421.0a,666.8b);
put_text( btex  10 etex, 421.0a, 108.6b, 0, 2);
draw (468.9a,145.2b)--(468.9a,153.5b);
draw (468.9a,683.4b)--(468.9a,675.1b);
draw (532.3a,145.2b)--(532.3a,153.5b);
draw (532.3a,683.4b)--(532.3a,675.1b);
draw (564.9a,145.2b)--(564.9a,153.5b);
draw (564.9a,683.4b)--(564.9a,675.1b);
draw (580.3a,145.2b)--(580.3a,161.8b);
draw (580.3a,683.4b)--(580.3a,666.8b);
put_text( btex  100 etex, 580.3a, 108.6b, 0, 2);
draw (628.3a,145.2b)--(628.3a,153.5b);
draw (628.3a,683.4b)--(628.3a,675.1b);
draw (691.7a,145.2b)--(691.7a,153.5b);
draw (691.7a,683.4b)--(691.7a,675.1b);
draw (724.2a,145.2b)--(724.2a,153.5b);
draw (724.2a,683.4b)--(724.2a,675.1b);
draw (739.7a,145.2b)--(739.7a,161.8b);
draw (739.7a,683.4b)--(739.7a,666.8b);
put_text( btex  1000 etex, 739.7a, 108.6b, 0, 2);
draw (787.7a,145.2b)--(787.7a,153.5b);
draw (787.7a,683.4b)--(787.7a,675.1b);
draw (851.1a,145.2b)--(851.1a,153.5b);
draw (851.1a,683.4b)--(851.1a,675.1b);
draw (883.6a,145.2b)--(883.6a,153.5b);
draw (883.6a,683.4b)--(883.6a,675.1b);
draw (899.0a,145.2b)--(899.0a,161.8b);
draw (899.0a,683.4b)--(899.0a,666.8b);
put_text( btex  10,000 etex, 899.0a, 108.6b, 0, 2);
draw (947.0a,145.2b)--(947.0a,153.5b);
draw (947.0a,683.4b)--(947.0a,675.1b);
draw (1010.4a,145.2b)--(1010.4a,153.5b);
draw (1010.4a,683.4b)--(1010.4a,675.1b);
draw (1043.0a,145.2b)--(1043.0a,153.5b);
draw (1043.0a,683.4b)--(1043.0a,675.1b);
draw (1058.4a,145.2b)--(1058.4a,161.8b);
draw (1058.4a,683.4b)--(1058.4a,666.8b);
put_text( btex  100,000 etex, 1058.4a, 108.6b, 0, 2);
draw (261.6a,145.2b)--(1058.4a,145.2b)--(1058.4a,683.4b)--(261.6a,683.4b)--(261.6a,145.2b);
put_text( btex Contributions to 3D-FFT (cycles) etex, 84.0a, 414.3b, 1, 2);
put_text( btex Node Count etex, 660.0a, 53.7b, 0, 2);
linetype 0;
put_text( btex Total etex, 633.3a, 253.3b, 0, 3);
draw (651.0a,253.3b)--(738.4a,253.3b);
draw (261.6a,677.5b)--(405.5a,643.4b)--(501.5a,604.5b)--(549.4a,572.1b)--(597.4a,560.7b)
--(645.4a,536.0b)--(693.3a,503.6b)--(741.3a,493.6b)--(789.3a,469.3b)--(837.3a,436.9b)
--(885.2a,427.6b)--(933.2a,403.5b);
linetype 1;
put_text( btex Communication (mesh) etex, 633.3a, 216.7b, 0, 3);
draw (651.0a,216.7b)--(738.4a,216.7b);
draw (405.5a,629.3b)--(501.5a,596.9b)--(549.4a,564.5b)--(597.4a,556.0b)
--(645.4a,532.1b)--(693.3a,499.7b)--(741.3a,491.2b)--(789.3a,467.3b)--(837.3a,434.9b)
--(885.2a,426.4b)--(933.2a,402.5b);
linetype 2;
put_text( btex Computation etex, 633.3a, 180.1b, 0, 3);
draw (651.0a,180.1b)--(738.4a,180.1b);
draw (261.6a,677.5b)--(405.5a,580.3b)--(501.5a,515.5b)--(549.4a,483.0b)--(597.4a,450.6b)
--(645.4a,418.2b)--(693.3a,385.8b)--(741.3a,353.4b)--(789.3a,321.0b)--(837.3a,288.6b)
--(885.2a,256.2b)--(933.2a,223.8b);
endfig;
end.
b);
endfig;
end.
