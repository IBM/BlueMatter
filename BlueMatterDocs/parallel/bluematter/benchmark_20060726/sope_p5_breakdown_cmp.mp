%GNUPLOT Metapost output: Wed Jul 18 18:20:10 2007


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

beginfig(6);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  $10^5$ etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,196.3b)--(222.5a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (214.2a,246.9b)--(222.5a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (214.2a,282.8b)--(222.5a,282.8b);
draw (1146.3a,282.8b)--(1138.0a,282.8b);
draw (214.2a,310.7b)--(222.5a,310.7b);
draw (1146.3a,310.7b)--(1138.0a,310.7b);
draw (214.2a,333.4b)--(222.5a,333.4b);
draw (1146.3a,333.4b)--(1138.0a,333.4b);
draw (214.2a,352.7b)--(222.5a,352.7b);
draw (1146.3a,352.7b)--(1138.0a,352.7b);
draw (214.2a,369.3b)--(222.5a,369.3b);
draw (1146.3a,369.3b)--(1138.0a,369.3b);
draw (214.2a,384.0b)--(222.5a,384.0b);
draw (1146.3a,384.0b)--(1138.0a,384.0b);
draw (214.2a,397.2b)--(230.8a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  $10^6$ etex, 196.5a, 397.2b, 0, 3);
draw (214.2a,483.7b)--(222.5a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (214.2a,534.3b)--(222.5a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (214.2a,570.2b)--(222.5a,570.2b);
draw (1146.3a,570.2b)--(1138.0a,570.2b);
draw (214.2a,598.1b)--(222.5a,598.1b);
draw (1146.3a,598.1b)--(1138.0a,598.1b);
draw (214.2a,620.8b)--(222.5a,620.8b);
draw (1146.3a,620.8b)--(1138.0a,620.8b);
draw (214.2a,640.1b)--(222.5a,640.1b);
draw (1146.3a,640.1b)--(1138.0a,640.1b);
draw (214.2a,656.7b)--(222.5a,656.7b);
draw (1146.3a,656.7b)--(1138.0a,656.7b);
draw (214.2a,671.4b)--(222.5a,671.4b);
draw (1146.3a,671.4b)--(1138.0a,671.4b);
draw (214.2a,684.6b)--(230.8a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  $10^7$ etex, 196.5a, 684.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,684.6b)--(214.2a,668.0b);
put_text( btex  100 etex, 214.2a, 73.2b, 0, 2);
draw (354.5a,109.8b)--(354.5a,118.1b);
draw (354.5a,684.6b)--(354.5a,676.3b);
draw (436.6a,109.8b)--(436.6a,118.1b);
draw (436.6a,684.6b)--(436.6a,676.3b);
draw (494.8a,109.8b)--(494.8a,118.1b);
draw (494.8a,684.6b)--(494.8a,676.3b);
draw (540.0a,109.8b)--(540.0a,118.1b);
draw (540.0a,684.6b)--(540.0a,676.3b);
draw (576.9a,109.8b)--(576.9a,118.1b);
draw (576.9a,684.6b)--(576.9a,676.3b);
draw (608.1a,109.8b)--(608.1a,118.1b);
draw (608.1a,684.6b)--(608.1a,676.3b);
draw (635.1a,109.8b)--(635.1a,118.1b);
draw (635.1a,684.6b)--(635.1a,676.3b);
draw (658.9a,109.8b)--(658.9a,118.1b);
draw (658.9a,684.6b)--(658.9a,676.3b);
draw (680.3a,109.8b)--(680.3a,126.4b);
draw (680.3a,684.6b)--(680.3a,668.0b);
put_text( btex  1000 etex, 680.3a, 73.2b, 0, 2);
draw (820.5a,109.8b)--(820.5a,118.1b);
draw (820.5a,684.6b)--(820.5a,676.3b);
draw (902.6a,109.8b)--(902.6a,118.1b);
draw (902.6a,684.6b)--(902.6a,676.3b);
draw (960.8a,109.8b)--(960.8a,118.1b);
draw (960.8a,684.6b)--(960.8a,676.3b);
draw (1006.0a,109.8b)--(1006.0a,118.1b);
draw (1006.0a,684.6b)--(1006.0a,676.3b);
draw (1042.9a,109.8b)--(1042.9a,118.1b);
draw (1042.9a,684.6b)--(1042.9a,676.3b);
draw (1074.1a,109.8b)--(1074.1a,118.1b);
draw (1074.1a,684.6b)--(1074.1a,676.3b);
draw (1101.1a,109.8b)--(1101.1a,118.1b);
draw (1101.1a,684.6b)--(1101.1a,676.3b);
draw (1125.0a,109.8b)--(1125.0a,118.1b);
draw (1125.0a,684.6b)--(1125.0a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  10000 etex, 1146.3a, 73.2b, 0, 2);
draw (214.2a,684.6b)--(214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b);
put_text( btex Time/Time-step (ns) etex, 36.6a, 397.2b, 90, 2);
linetype -2;
put_text( btex Nodes etex, 680.2a, 18.3b, 0, 2);
linetype -2;
linetype 0;
linetype -2;
put_text( btex Total Time-step $128^3$ etex, 639.0a, 327.7b, 0, 3);
linetype 0;
draw (656.7a,327.7b)--(744.1a,327.7b);
draw (544.8a,627.6b)--(685.1a,566.2b)--(825.3a,507.4b)--(965.6a,456.7b)--(1105.9a,447.3b);
gpdraw(0,544.8a,627.6b);
gpdraw(0,685.1a,566.2b);
gpdraw(0,825.3a,507.4b);
gpdraw(0,965.6a,456.7b);
gpdraw(0,1105.9a,447.3b);
gpdraw(0,700.4a,327.7b);
linetype 1;
linetype -2;
put_text( btex K-space $128^3$ etex, 639.0a, 291.1b, 0, 3);
linetype 1;
draw (656.7a,291.1b)--(744.1a,291.1b);
draw (544.8a,611.2b)--(685.1a,550.9b)--(825.3a,488.8b)--(965.6a,432.3b)--(1105.9a,424.4b);
gpdraw(1,544.8a,611.2b);
gpdraw(1,685.1a,550.9b);
gpdraw(1,825.3a,488.8b);
gpdraw(1,965.6a,432.3b);
gpdraw(1,1105.9a,424.4b);
gpdraw(1,700.4a,291.1b);
linetype 2;
linetype -2;
put_text( btex Real Space $128^3$ etex, 639.0a, 254.5b, 0, 3);
linetype 2;
draw (656.7a,254.5b)--(744.1a,254.5b);
draw (544.8a,517.3b)--(685.1a,432.9b)--(825.3a,349.8b)--(965.6a,268.7b)--(1105.9a,188.6b);
gpdraw(2,544.8a,517.3b);
gpdraw(2,685.1a,432.9b);
gpdraw(2,825.3a,349.8b);
gpdraw(2,965.6a,268.7b);
gpdraw(2,1105.9a,188.6b);
gpdraw(2,700.4a,254.5b);
linetype 3;
linetype -2;
put_text( btex Total Time-step $64^3$ etex, 639.0a, 217.9b, 0, 3);
linetype 3;
draw (656.7a,217.9b)--(744.1a,217.9b);
draw (544.8a,593.7b)--(685.1a,525.4b)--(825.3a,470.9b)--(965.6a,425.3b)--(1105.9a,393.8b);
gpdraw(3,544.8a,593.7b);
gpdraw(3,685.1a,525.4b);
gpdraw(3,825.3a,470.9b);
gpdraw(3,965.6a,425.3b);
gpdraw(3,1105.9a,393.8b);
gpdraw(3,700.4a,217.9b);
linetype 4;
linetype -2;
put_text( btex K-space $64^3$ etex, 639.0a, 181.3b, 0, 3);
linetype 4;
draw (656.7a,181.3b)--(744.1a,181.3b);
draw (544.8a,444.0b)--(685.1a,410.1b)--(825.3a,371.1b)--(965.6a,348.0b)--(1105.9a,338.6b);
gpdraw(4,544.8a,444.0b);
gpdraw(4,685.1a,410.1b);
gpdraw(4,825.3a,371.1b);
gpdraw(4,965.6a,348.0b);
gpdraw(4,1105.9a,338.6b);
gpdraw(4,700.4a,181.3b);
linetype 5;
linetype -2;
put_text( btex Real Space $64^3$ etex, 639.0a, 144.7b, 0, 3);
linetype 5;
draw (656.7a,144.7b)--(744.1a,144.7b);
draw (544.8a,515.8b)--(685.1a,431.7b)--(825.3a,348.7b)--(965.6a,267.7b)--(1105.9a,187.6b);
gpdraw(5,544.8a,515.8b);
gpdraw(5,685.1a,431.7b);
gpdraw(5,825.3a,348.7b);
gpdraw(5,965.6a,267.7b);
gpdraw(5,1105.9a,187.6b);
gpdraw(5,700.4a,144.7b);
linetype -2;
draw (214.2a,684.6b)--(214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
