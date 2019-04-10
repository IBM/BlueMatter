%GNUPLOT Metapost output: Wed Aug 03 23:43:23 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage{textcomp}
\usepackage{latexsym}
\usepackage[mtbold]{mathtime}
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

beginfig(2);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (161.1a,109.8b)--(177.7a,109.8b);
put_text( btex  0 etex, 143.4a, 109.8b, 0, 3);
draw (161.1a,224.8b)--(177.7a,224.8b);
put_text( btex  0.2 etex, 143.4a, 224.8b, 0, 3);
draw (161.1a,339.7b)--(177.7a,339.7b);
put_text( btex  0.4 etex, 143.4a, 339.7b, 0, 3);
draw (161.1a,454.7b)--(177.7a,454.7b);
put_text( btex  0.6 etex, 143.4a, 454.7b, 0, 3);
draw (161.1a,569.6b)--(177.7a,569.6b);
put_text( btex  0.8 etex, 143.4a, 569.6b, 0, 3);
draw (161.1a,684.6b)--(177.7a,684.6b);
put_text( btex  1 etex, 143.4a, 684.6b, 0, 3);
draw (161.1a,109.8b)--(161.1a,126.4b);
draw (161.1a,684.6b)--(161.1a,668.0b);
put_text( btex  256 etex, 161.1a, 73.2b, 0, 2);
draw (301.8a,109.8b)--(301.8a,126.4b);
draw (301.8a,684.6b)--(301.8a,668.0b);
put_text( btex  512 etex, 301.8a, 73.2b, 0, 2);
draw (442.6a,109.8b)--(442.6a,126.4b);
draw (442.6a,684.6b)--(442.6a,668.0b);
put_text( btex  1024 etex, 442.6a, 73.2b, 0, 2);
draw (583.3a,109.8b)--(583.3a,126.4b);
draw (583.3a,684.6b)--(583.3a,668.0b);
put_text( btex  2048 etex, 583.3a, 73.2b, 0, 2);
draw (724.1a,109.8b)--(724.1a,126.4b);
draw (724.1a,684.6b)--(724.1a,668.0b);
put_text( btex  4096 etex, 724.1a, 73.2b, 0, 2);
draw (864.8a,109.8b)--(864.8a,126.4b);
draw (864.8a,684.6b)--(864.8a,668.0b);
put_text( btex  8192 etex, 864.8a, 73.2b, 0, 2);
draw (1005.6a,109.8b)--(1005.6a,126.4b);
draw (1005.6a,684.6b)--(1005.6a,668.0b);
put_text( btex  16384 etex, 1005.6a, 73.2b, 0, 2);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  32768 etex, 1146.3a, 73.2b, 0, 2);
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(161.1a,684.6b)--(161.1a,109.8b);
put_text( btex Fraction of Total Time-step etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 653.7a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Real Space etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
fill (1023.5a,640.6b)--(1110.9a,640.6b)--(1110.9a,658.8b)--(1023.5a,658.8b)--cycle withcolor col0;
fill (228.0a,109.8b)--(228.0a,521.9b)--(298.3a,521.9b)--(298.3a,109.8b)--cycle withcolor col0;
fill (368.7a,109.8b)--(368.7a,448.2b)--(439.1a,448.2b)--(439.1a,109.8b)--cycle withcolor col0;
fill (509.4a,109.8b)--(509.4a,397.2b)--(579.8a,397.2b)--(579.8a,109.8b)--cycle withcolor col0;
fill (650.2a,109.8b)--(650.2a,377.7b)--(720.6a,377.7b)--(720.6a,109.8b)--cycle withcolor col0;
fill (790.9a,109.8b)--(790.9a,332.3b)--(861.3a,332.3b)--(861.3a,109.8b)--cycle withcolor col0;
fill (931.7a,109.8b)--(931.7a,314.8b)--(1002.0a,314.8b)--(1002.0a,109.8b)--cycle withcolor col0;
linetype 1;
linetype -2;
put_text( btex P3ME etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
fill (1023.5a,604.0b)--(1110.9a,604.0b)--(1110.9a,622.2b)--(1023.5a,622.2b)--cycle withcolor col1;
fill (247.3a,109.8b)--(247.3a,160.7b)--(317.7a,160.7b)--(317.7a,109.8b)--cycle withcolor col1;
fill (388.0a,109.8b)--(388.0a,176.2b)--(458.4a,176.2b)--(458.4a,109.8b)--cycle withcolor col1;
fill (528.8a,109.8b)--(528.8a,173.1b)--(599.2a,173.1b)--(599.2a,109.8b)--cycle withcolor col1;
fill (669.5a,109.8b)--(669.5a,163.4b)--(739.9a,163.4b)--(739.9a,109.8b)--cycle withcolor col1;
fill (810.3a,109.8b)--(810.3a,187.0b)--(880.6a,187.0b)--(880.6a,109.8b)--cycle withcolor col1;
fill (951.0a,109.8b)--(951.0a,192.4b)--(1021.4a,192.4b)--(1021.4a,109.8b)--cycle withcolor col1;
linetype 2;
linetype -2;
put_text( btex Local Force Reduction etex, 1005.8a, 576.5b, 0, 3);
linetype 2;
fill (1023.5a,567.4b)--(1110.9a,567.4b)--(1110.9a,585.6b)--(1023.5a,585.6b)--cycle withcolor col2;
fill (266.7a,109.8b)--(266.7a,242.9b)--(337.0a,242.9b)--(337.0a,109.8b)--cycle withcolor col2;
fill (407.4a,109.8b)--(407.4a,268.9b)--(477.8a,268.9b)--(477.8a,109.8b)--cycle withcolor col2;
fill (548.1a,109.8b)--(548.1a,312.8b)--(618.5a,312.8b)--(618.5a,109.8b)--cycle withcolor col2;
fill (688.9a,109.8b)--(688.9a,352.7b)--(759.3a,352.7b)--(759.3a,109.8b)--cycle withcolor col2;
fill (829.6a,109.8b)--(829.6a,372.4b)--(900.0a,372.4b)--(900.0a,109.8b)--cycle withcolor col2;
fill (970.4a,109.8b)--(970.4a,374.3b)--(1040.7a,374.3b)--(1040.7a,109.8b)--cycle withcolor col2;
linetype 3;
linetype -2;
put_text( btex Local Position Broadcast etex, 1005.8a, 539.9b, 0, 3);
linetype 3;
fill (1023.5a,530.8b)--(1110.9a,530.8b)--(1110.9a,549.0b)--(1023.5a,549.0b)--cycle withcolor col3;
fill (286.0a,109.8b)--(286.0a,139.5b)--(356.4a,139.5b)--(356.4a,109.8b)--cycle withcolor col3;
fill (426.8a,109.8b)--(426.8a,187.1b)--(497.1a,187.1b)--(497.1a,109.8b)--cycle withcolor col3;
fill (567.5a,109.8b)--(567.5a,194.2b)--(637.9a,194.2b)--(637.9a,109.8b)--cycle withcolor col3;
fill (708.2a,109.8b)--(708.2a,173.8b)--(778.6a,173.8b)--(778.6a,109.8b)--cycle withcolor col3;
fill (849.0a,109.8b)--(849.0a,199.5b)--(919.4a,199.5b)--(919.4a,109.8b)--cycle withcolor col3;
fill (989.7a,109.8b)--(989.7a,215.1b)--(1060.1a,215.1b)--(1060.1a,109.8b)--cycle withcolor col3;
linetype -2;
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(161.1a,684.6b)--(161.1a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
