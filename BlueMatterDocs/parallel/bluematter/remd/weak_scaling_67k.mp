%GNUPLOT Metapost output: Tue Nov 01 10:51:38 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage{textcomp}
\usepackage[mtbold]{mathtime}
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
draw (161.1a,109.8b)--(177.7a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0 etex, 143.4a, 109.8b, 0, 3);
draw (161.1a,209.9b)--(177.7a,209.9b);
draw (1146.3a,209.9b)--(1129.7a,209.9b);
put_text( btex  0.1 etex, 143.4a, 209.9b, 0, 3);
draw (161.1a,310.0b)--(177.7a,310.0b);
draw (1146.3a,310.0b)--(1129.7a,310.0b);
put_text( btex  0.2 etex, 143.4a, 310.0b, 0, 3);
draw (161.1a,410.0b)--(177.7a,410.0b);
draw (1146.3a,410.0b)--(1129.7a,410.0b);
put_text( btex  0.3 etex, 143.4a, 410.0b, 0, 3);
draw (161.1a,510.1b)--(177.7a,510.1b);
draw (1146.3a,510.1b)--(1129.7a,510.1b);
put_text( btex  0.4 etex, 143.4a, 510.1b, 0, 3);
draw (161.1a,610.2b)--(177.7a,610.2b);
draw (1146.3a,610.2b)--(1129.7a,610.2b);
put_text( btex  0.5 etex, 143.4a, 610.2b, 0, 3);
draw (161.1a,109.8b)--(161.1a,126.4b);
put_text( btex  10 etex, 161.1a, 73.2b, 0, 2);
draw (260.0a,109.8b)--(260.0a,118.1b);
draw (317.8a,109.8b)--(317.8a,118.1b);
draw (358.8a,109.8b)--(358.8a,118.1b);
draw (390.6a,109.8b)--(390.6a,118.1b);
draw (416.6a,109.8b)--(416.6a,118.1b);
draw (438.6a,109.8b)--(438.6a,118.1b);
draw (457.7a,109.8b)--(457.7a,118.1b);
draw (474.5a,109.8b)--(474.5a,118.1b);
draw (489.5a,109.8b)--(489.5a,126.4b);
put_text( btex  100 etex, 489.5a, 73.2b, 0, 2);
draw (588.4a,109.8b)--(588.4a,118.1b);
draw (646.2a,109.8b)--(646.2a,118.1b);
draw (687.2a,109.8b)--(687.2a,118.1b);
draw (719.0a,109.8b)--(719.0a,118.1b);
draw (745.0a,109.8b)--(745.0a,118.1b);
draw (767.0a,109.8b)--(767.0a,118.1b);
draw (786.1a,109.8b)--(786.1a,118.1b);
draw (802.9a,109.8b)--(802.9a,118.1b);
draw (817.9a,109.8b)--(817.9a,126.4b);
put_text( btex  1000 etex, 817.9a, 73.2b, 0, 2);
draw (916.8a,109.8b)--(916.8a,118.1b);
draw (974.6a,109.8b)--(974.6a,118.1b);
draw (1015.6a,109.8b)--(1015.6a,118.1b);
draw (1047.4a,109.8b)--(1047.4a,118.1b);
draw (1073.4a,109.8b)--(1073.4a,118.1b);
draw (1095.4a,109.8b)--(1095.4a,118.1b);
draw (1114.5a,109.8b)--(1114.5a,118.1b);
draw (1131.3a,109.8b)--(1131.3a,118.1b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
put_text( btex  10000 etex, 1146.3a, 73.2b, 0, 2);
draw (196.3a,610.2b)--(196.3a,601.9b);
draw (228.1a,610.2b)--(228.1a,601.9b);
draw (254.1a,610.2b)--(254.1a,601.9b);
draw (276.1a,610.2b)--(276.1a,601.9b);
draw (295.2a,610.2b)--(295.2a,601.9b);
draw (312.0a,610.2b)--(312.0a,601.9b);
draw (327.0a,610.2b)--(327.0a,593.6b);
put_text( btex  1 etex, 327.0a, 646.8b, 0, 2);
draw (425.8a,610.2b)--(425.8a,601.9b);
draw (483.7a,610.2b)--(483.7a,601.9b);
draw (524.7a,610.2b)--(524.7a,601.9b);
draw (556.5a,610.2b)--(556.5a,601.9b);
draw (582.5a,610.2b)--(582.5a,601.9b);
draw (604.5a,610.2b)--(604.5a,601.9b);
draw (623.6a,610.2b)--(623.6a,601.9b);
draw (640.4a,610.2b)--(640.4a,601.9b);
draw (655.4a,610.2b)--(655.4a,593.6b);
put_text( btex  10 etex, 655.4a, 646.8b, 0, 2);
draw (754.2a,610.2b)--(754.2a,601.9b);
draw (812.1a,610.2b)--(812.1a,601.9b);
draw (853.1a,610.2b)--(853.1a,601.9b);
draw (884.9a,610.2b)--(884.9a,601.9b);
draw (910.9a,610.2b)--(910.9a,601.9b);
draw (932.9a,610.2b)--(932.9a,601.9b);
draw (952.0a,610.2b)--(952.0a,601.9b);
draw (968.8a,610.2b)--(968.8a,601.9b);
draw (983.8a,610.2b)--(983.8a,593.6b);
put_text( btex  100 etex, 983.8a, 646.8b, 0, 2);
draw (1082.6a,610.2b)--(1082.6a,601.9b);
draw (1140.5a,610.2b)--(1140.5a,601.9b);
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,610.2b)--(161.1a,610.2b)--(161.1a,109.8b);
put_text( btex Time/Time-step (sec.) etex, 36.6a, 360.0b, 1, 2);
put_text( btex Total Node Count etex, 653.7a, 18.3b, 0, 2);
put_text( btex Replica Count etex, 653.7a, 701.6b, 0, 2);
linetype 0;
gpdraw(0,327.0a,323.1b);
gpdraw(0,524.7a,347.8b);
gpdraw(0,722.4a,322.5b);
gpdraw(0,821.3a,334.3b);
gpdraw(0,920.1a,333.0b);
gpdraw(0,1019.0a,313.8b);
gpdraw(0,1117.9a,313.5b);
linetype -2;
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,610.2b)--(161.1a,610.2b)--(161.1a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
