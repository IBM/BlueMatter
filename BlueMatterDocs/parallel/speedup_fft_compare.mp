%GNUPLOT Metapost output: Wed Jul 23 13:51:10 2003


%% prologues:=2;
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

beginfig(0);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (261.6a,145.2b)--(278.2a,145.2b);
draw (1058.4a,145.2b)--(1041.8a,145.2b);
put_text( btex  1 etex, 243.9a, 145.2b, 0, 3);
draw (261.6a,177.6b)--(269.9a,177.6b);
draw (1058.4a,177.6b)--(1050.1a,177.6b);
draw (261.6a,220.4b)--(269.9a,220.4b);
draw (1058.4a,220.4b)--(1050.1a,220.4b);
draw (261.6a,242.4b)--(269.9a,242.4b);
draw (1058.4a,242.4b)--(1050.1a,242.4b);
draw (261.6a,252.8b)--(278.2a,252.8b);
draw (1058.4a,252.8b)--(1041.8a,252.8b);
put_text( btex  10 etex, 243.9a, 252.8b, 0, 3);
draw (261.6a,285.2b)--(269.9a,285.2b);
draw (1058.4a,285.2b)--(1050.1a,285.2b);
draw (261.6a,328.1b)--(269.9a,328.1b);
draw (1058.4a,328.1b)--(1050.1a,328.1b);
draw (261.6a,350.0b)--(269.9a,350.0b);
draw (1058.4a,350.0b)--(1050.1a,350.0b);
draw (261.6a,360.5b)--(278.2a,360.5b);
draw (1058.4a,360.5b)--(1041.8a,360.5b);
put_text( btex  100 etex, 243.9a, 360.5b, 0, 3);
draw (261.6a,392.9b)--(269.9a,392.9b);
draw (1058.4a,392.9b)--(1050.1a,392.9b);
draw (261.6a,435.7b)--(269.9a,435.7b);
draw (1058.4a,435.7b)--(1050.1a,435.7b);
draw (261.6a,457.7b)--(269.9a,457.7b);
draw (1058.4a,457.7b)--(1050.1a,457.7b);
draw (261.6a,468.1b)--(278.2a,468.1b);
draw (1058.4a,468.1b)--(1041.8a,468.1b);
put_text( btex  1000 etex, 243.9a, 468.1b, 0, 3);
draw (261.6a,500.5b)--(269.9a,500.5b);
draw (1058.4a,500.5b)--(1050.1a,500.5b);
draw (261.6a,543.4b)--(269.9a,543.4b);
draw (1058.4a,543.4b)--(1050.1a,543.4b);
draw (261.6a,565.3b)--(269.9a,565.3b);
draw (1058.4a,565.3b)--(1050.1a,565.3b);
draw (261.6a,575.8b)--(278.2a,575.8b);
draw (1058.4a,575.8b)--(1041.8a,575.8b);
put_text( btex  10000 etex, 243.9a, 575.8b, 0, 3);
draw (261.6a,608.2b)--(269.9a,608.2b);
draw (1058.4a,608.2b)--(1050.1a,608.2b);
draw (261.6a,651.0b)--(269.9a,651.0b);
draw (1058.4a,651.0b)--(1050.1a,651.0b);
draw (261.6a,673.0b)--(269.9a,673.0b);
draw (1058.4a,673.0b)--(1050.1a,673.0b);
draw (261.6a,683.4b)--(278.2a,683.4b);
draw (1058.4a,683.4b)--(1041.8a,683.4b);
put_text( btex  100000 etex, 243.9a, 683.4b, 0, 3);
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
put_text( btex  10000 etex, 899.0a, 108.6b, 0, 2);
draw (947.0a,145.2b)--(947.0a,153.5b);
draw (947.0a,683.4b)--(947.0a,675.1b);
draw (1010.4a,145.2b)--(1010.4a,153.5b);
draw (1010.4a,683.4b)--(1010.4a,675.1b);
draw (1043.0a,145.2b)--(1043.0a,153.5b);
draw (1043.0a,683.4b)--(1043.0a,675.1b);
draw (1058.4a,145.2b)--(1058.4a,161.8b);
draw (1058.4a,683.4b)--(1058.4a,666.8b);
put_text( btex  100000 etex, 1058.4a, 108.6b, 0, 2);
draw (261.6a,145.2b)--(1058.4a,145.2b)--(1058.4a,683.4b)--(261.6a,683.4b)--(261.6a,145.2b);
put_text( btex Speedup relative to Single Node etex, 84.0a, 414.3b, 1, 2);
put_text( btex Node Count etex, 660.0a, 53.7b, 0, 2);
linetype 0;
put_text( btex ideal speedup etex, 721.8a, 648.5b, 0, 3);
draw (739.5a,648.5b)--(826.9a,648.5b);
draw (405.5a,242.4b)--(501.5a,307.2b)--(549.4a,339.6b)--(597.4a,372.0b)--(645.4a,404.4b)
--(693.3a,436.8b)--(741.3a,469.2b)--(789.3a,501.6b)--(837.3a,534.0b)--(885.2a,566.4b)
--(933.2a,598.8b)--(981.2a,631.2b)--(1029.2a,663.6b);
linetype 1;
put_text( btex est. speedup for $1024^3$ etex, 721.8a, 611.9b, 0, 3);
draw (739.5a,611.9b)--(826.9a,611.9b);
draw (405.5a,235.4b)--(501.5a,294.2b)--(549.4a,326.6b)--(597.4a,352.0b)--(645.4a,381.2b)
--(693.3a,413.6b)--(741.3a,435.2b)--(789.3a,463.0b)--(837.3a,495.4b)--(885.2a,512.9b)
--(933.2a,539.3b)--(981.2a,571.7b)--(1029.2a,585.8b);
linetype 2;
put_text( btex est. speedup for $512^3$ etex, 721.8a, 575.3b, 0, 3);
draw (739.5a,575.3b)--(826.9a,575.3b);
draw (405.5a,234.7b)--(501.5a,292.9b)--(549.4a,325.3b)--(597.4a,350.2b)--(645.4a,379.2b)
--(693.3a,411.6b)--(741.3a,432.6b)--(789.3a,460.1b)--(837.3a,492.6b)--(885.2a,509.5b)
--(933.2a,535.7b)--(981.2a,568.1b)--(1029.2a,528.2b);
linetype 3;
put_text( btex est. speedup for $256^3$ etex, 721.8a, 538.7b, 0, 3);
draw (739.5a,538.7b)--(826.9a,538.7b);
draw (405.5a,233.9b)--(501.5a,291.4b)--(549.4a,323.8b)--(597.4a,348.0b)--(645.4a,376.8b)
--(693.3a,409.3b)--(741.3a,429.5b)--(789.3a,456.8b)--(837.3a,489.2b)--(885.2a,484.1b)
--(933.2a,475.9b)--(981.2a,477.7b)--(1029.2a,427.8b);
linetype 4;
put_text( btex est. speedup for $128^3$ etex, 721.8a, 502.1b, 0, 3);
draw (739.5a,502.1b)--(826.9a,502.1b);
draw (405.5a,232.7b)--(501.5a,289.6b)--(549.4a,321.9b)--(597.4a,345.4b)--(645.4a,374.0b)
--(693.3a,406.4b)--(741.3a,425.8b)--(789.3a,429.5b)--(837.3a,434.8b)--(885.2a,388.4b)
--(933.2a,375.6b);
endfig;
end.
