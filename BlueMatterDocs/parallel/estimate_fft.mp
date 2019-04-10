%GNUPLOT Metapost output: Thu Apr 10 15:25:29 2003

prologues:=2;
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

beginfig(1);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (261.6a,145.2b)--(278.2a,145.2b);
draw (1058.4a,145.2b)--(1041.8a,145.2b);
put_text( btex  ${\rm 10}^{\rm 4}$ etex, 243.9a, 145.2b, 0, 3);
draw (261.6a,185.7b)--(269.9a,185.7b);
draw (1058.4a,185.7b)--(1050.1a,185.7b);
draw (261.6a,209.4b)--(269.9a,209.4b);
draw (1058.4a,209.4b)--(1050.1a,209.4b);
draw (261.6a,226.2b)--(269.9a,226.2b);
draw (1058.4a,226.2b)--(1050.1a,226.2b);
draw (261.6a,239.2b)--(269.9a,239.2b);
draw (1058.4a,239.2b)--(1050.1a,239.2b);
draw (261.6a,249.9b)--(269.9a,249.9b);
draw (1058.4a,249.9b)--(1050.1a,249.9b);
draw (261.6a,258.9b)--(269.9a,258.9b);
draw (1058.4a,258.9b)--(1050.1a,258.9b);
draw (261.6a,266.7b)--(269.9a,266.7b);
draw (1058.4a,266.7b)--(1050.1a,266.7b);
draw (261.6a,273.6b)--(269.9a,273.6b);
draw (1058.4a,273.6b)--(1050.1a,273.6b);
draw (261.6a,279.8b)--(278.2a,279.8b);
draw (1058.4a,279.8b)--(1041.8a,279.8b);
put_text( btex  ${\rm 10}^{\rm 5}$ etex, 243.9a, 279.8b, 0, 3);
draw (261.6a,320.3b)--(269.9a,320.3b);
draw (1058.4a,320.3b)--(1050.1a,320.3b);
draw (261.6a,343.9b)--(269.9a,343.9b);
draw (1058.4a,343.9b)--(1050.1a,343.9b);
draw (261.6a,360.8b)--(269.9a,360.8b);
draw (1058.4a,360.8b)--(1050.1a,360.8b);
draw (261.6a,373.8b)--(269.9a,373.8b);
draw (1058.4a,373.8b)--(1050.1a,373.8b);
draw (261.6a,384.5b)--(269.9a,384.5b);
draw (1058.4a,384.5b)--(1050.1a,384.5b);
draw (261.6a,393.5b)--(269.9a,393.5b);
draw (1058.4a,393.5b)--(1050.1a,393.5b);
draw (261.6a,401.3b)--(269.9a,401.3b);
draw (1058.4a,401.3b)--(1050.1a,401.3b);
draw (261.6a,408.1b)--(269.9a,408.1b);
draw (1058.4a,408.1b)--(1050.1a,408.1b);
draw (261.6a,414.3b)--(278.2a,414.3b);
draw (1058.4a,414.3b)--(1041.8a,414.3b);
put_text( btex  ${\rm 10}^{\rm 6}$ etex, 243.9a, 414.3b, 0, 3);
draw (261.6a,454.8b)--(269.9a,454.8b);
draw (1058.4a,454.8b)--(1050.1a,454.8b);
draw (261.6a,478.5b)--(269.9a,478.5b);
draw (1058.4a,478.5b)--(1050.1a,478.5b);
draw (261.6a,495.3b)--(269.9a,495.3b);
draw (1058.4a,495.3b)--(1050.1a,495.3b);
draw (261.6a,508.3b)--(269.9a,508.3b);
draw (1058.4a,508.3b)--(1050.1a,508.3b);
draw (261.6a,519.0b)--(269.9a,519.0b);
draw (1058.4a,519.0b)--(1050.1a,519.0b);
draw (261.6a,528.0b)--(269.9a,528.0b);
draw (1058.4a,528.0b)--(1050.1a,528.0b);
draw (261.6a,535.8b)--(269.9a,535.8b);
draw (1058.4a,535.8b)--(1050.1a,535.8b);
draw (261.6a,542.7b)--(269.9a,542.7b);
draw (1058.4a,542.7b)--(1050.1a,542.7b);
draw (261.6a,548.9b)--(278.2a,548.9b);
draw (1058.4a,548.9b)--(1041.8a,548.9b);
put_text( btex  ${\rm 10}^{\rm 7}$ etex, 243.9a, 548.9b, 0, 3);
draw (261.6a,589.4b)--(269.9a,589.4b);
draw (1058.4a,589.4b)--(1050.1a,589.4b);
draw (261.6a,613.0b)--(269.9a,613.0b);
draw (1058.4a,613.0b)--(1050.1a,613.0b);
draw (261.6a,629.9b)--(269.9a,629.9b);
draw (1058.4a,629.9b)--(1050.1a,629.9b);
draw (261.6a,642.9b)--(269.9a,642.9b);
draw (1058.4a,642.9b)--(1050.1a,642.9b);
draw (261.6a,653.6b)--(269.9a,653.6b);
draw (1058.4a,653.6b)--(1050.1a,653.6b);
draw (261.6a,662.6b)--(269.9a,662.6b);
draw (1058.4a,662.6b)--(1050.1a,662.6b);
draw (261.6a,670.4b)--(269.9a,670.4b);
draw (1058.4a,670.4b)--(1050.1a,670.4b);
draw (261.6a,677.2b)--(269.9a,677.2b);
draw (1058.4a,677.2b)--(1050.1a,677.2b);
draw (261.6a,683.4b)--(278.2a,683.4b);
draw (1058.4a,683.4b)--(1041.8a,683.4b);
put_text( btex  ${\rm 10}^{\rm 8}$ etex, 243.9a, 683.4b, 0, 3);
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
put_text( btex total etex, 509.4a, 253.3b, 0, 3);
draw (527.1a,253.3b)--(614.5a,253.3b);
draw (405.5a,673.4b)--(501.5a,602.4b)--(549.4a,561.9b)--(597.4a,532.5b)--(645.4a,496.8b)
--(693.3a,456.3b)--(741.3a,432.0b)--(789.3a,427.4b)--(837.3a,420.8b)--(885.2a,478.8b)
--(933.2a,494.8b);
linetype 1;
put_text( btex communication etex, 509.4a, 216.7b, 0, 3);
draw (527.1a,216.7b)--(614.5a,216.7b);
draw (405.5a,575.3b)--(501.5a,534.8b)--(549.4a,494.3b)--(597.4a,483.7b)--(645.4a,453.8b)
--(693.3a,413.3b)--(741.3a,402.6b)--(789.3a,413.3b)--(837.3a,413.3b)--(885.2a,477.5b)
--(933.2a,494.3b);
linetype 2;
put_text( btex computation etex, 509.4a, 180.1b, 0, 3);
draw (527.1a,180.1b)--(614.5a,180.1b);
draw (405.5a,661.3b)--(501.5a,580.3b)--(549.4a,539.8b)--(597.4a,499.3b)--(645.4a,458.8b)
--(693.3a,418.3b)--(741.3a,377.8b)--(789.3a,337.3b)--(837.3a,296.8b)--(885.2a,256.3b)
--(933.2a,215.8b);
endfig;
end.
