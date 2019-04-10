%GNUPLOT Metapost output: Fri Nov 04 17:15:35 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage{textcomp}
\usepackage{mathptmx}
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

beginfig(0);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,109.8b)--(213.1a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0.001 etex, 178.8a, 109.8b, 0, 3);
draw (196.5a,160.0b)--(204.8a,160.0b);
draw (1146.3a,160.0b)--(1138.0a,160.0b);
draw (196.5a,189.4b)--(204.8a,189.4b);
draw (1146.3a,189.4b)--(1138.0a,189.4b);
draw (196.5a,210.2b)--(204.8a,210.2b);
draw (1146.3a,210.2b)--(1138.0a,210.2b);
draw (196.5a,226.4b)--(204.8a,226.4b);
draw (1146.3a,226.4b)--(1138.0a,226.4b);
draw (196.5a,239.6b)--(204.8a,239.6b);
draw (1146.3a,239.6b)--(1138.0a,239.6b);
draw (196.5a,250.8b)--(204.8a,250.8b);
draw (1146.3a,250.8b)--(1138.0a,250.8b);
draw (196.5a,260.4b)--(204.8a,260.4b);
draw (1146.3a,260.4b)--(1138.0a,260.4b);
draw (196.5a,269.0b)--(204.8a,269.0b);
draw (1146.3a,269.0b)--(1138.0a,269.0b);
draw (196.5a,276.6b)--(213.1a,276.6b);
draw (1146.3a,276.6b)--(1129.7a,276.6b);
put_text( btex  0.01 etex, 178.8a, 276.6b, 0, 3);
draw (196.5a,326.8b)--(204.8a,326.8b);
draw (1146.3a,326.8b)--(1138.0a,326.8b);
draw (196.5a,356.2b)--(204.8a,356.2b);
draw (1146.3a,356.2b)--(1138.0a,356.2b);
draw (196.5a,377.0b)--(204.8a,377.0b);
draw (1146.3a,377.0b)--(1138.0a,377.0b);
draw (196.5a,393.2b)--(204.8a,393.2b);
draw (1146.3a,393.2b)--(1138.0a,393.2b);
draw (196.5a,406.4b)--(204.8a,406.4b);
draw (1146.3a,406.4b)--(1138.0a,406.4b);
draw (196.5a,417.6b)--(204.8a,417.6b);
draw (1146.3a,417.6b)--(1138.0a,417.6b);
draw (196.5a,427.2b)--(204.8a,427.2b);
draw (1146.3a,427.2b)--(1138.0a,427.2b);
draw (196.5a,435.8b)--(204.8a,435.8b);
draw (1146.3a,435.8b)--(1138.0a,435.8b);
draw (196.5a,443.4b)--(213.1a,443.4b);
draw (1146.3a,443.4b)--(1129.7a,443.4b);
put_text( btex  0.1 etex, 178.8a, 443.4b, 0, 3);
draw (196.5a,493.6b)--(204.8a,493.6b);
draw (1146.3a,493.6b)--(1138.0a,493.6b);
draw (196.5a,523.0b)--(204.8a,523.0b);
draw (1146.3a,523.0b)--(1138.0a,523.0b);
draw (196.5a,543.8b)--(204.8a,543.8b);
draw (1146.3a,543.8b)--(1138.0a,543.8b);
draw (196.5a,560.0b)--(204.8a,560.0b);
draw (1146.3a,560.0b)--(1138.0a,560.0b);
draw (196.5a,573.2b)--(204.8a,573.2b);
draw (1146.3a,573.2b)--(1138.0a,573.2b);
draw (196.5a,584.4b)--(204.8a,584.4b);
draw (1146.3a,584.4b)--(1138.0a,584.4b);
draw (196.5a,594.0b)--(204.8a,594.0b);
draw (1146.3a,594.0b)--(1138.0a,594.0b);
draw (196.5a,602.6b)--(204.8a,602.6b);
draw (1146.3a,602.6b)--(1138.0a,602.6b);
draw (196.5a,610.2b)--(213.1a,610.2b);
draw (1146.3a,610.2b)--(1129.7a,610.2b);
put_text( btex  1 etex, 178.8a, 610.2b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
put_text( btex  10 etex, 196.5a, 73.2b, 0, 2);
draw (302.4a,109.8b)--(302.4a,118.1b);
draw (364.4a,109.8b)--(364.4a,118.1b);
draw (408.4a,109.8b)--(408.4a,118.1b);
draw (442.5a,109.8b)--(442.5a,118.1b);
draw (470.3a,109.8b)--(470.3a,118.1b);
draw (493.9a,109.8b)--(493.9a,118.1b);
draw (514.3a,109.8b)--(514.3a,118.1b);
draw (532.3a,109.8b)--(532.3a,118.1b);
draw (548.4a,109.8b)--(548.4a,126.4b);
put_text( btex  100 etex, 548.4a, 73.2b, 0, 2);
draw (654.3a,109.8b)--(654.3a,118.1b);
draw (716.3a,109.8b)--(716.3a,118.1b);
draw (760.3a,109.8b)--(760.3a,118.1b);
draw (794.4a,109.8b)--(794.4a,118.1b);
draw (822.3a,109.8b)--(822.3a,118.1b);
draw (845.8a,109.8b)--(845.8a,118.1b);
draw (866.2a,109.8b)--(866.2a,118.1b);
draw (884.2a,109.8b)--(884.2a,118.1b);
draw (900.3a,109.8b)--(900.3a,126.4b);
put_text( btex  1000 etex, 900.3a, 73.2b, 0, 2);
draw (1006.3a,109.8b)--(1006.3a,118.1b);
draw (1068.2a,109.8b)--(1068.2a,118.1b);
draw (1112.2a,109.8b)--(1112.2a,118.1b);
draw (1146.3a,109.8b)--(1146.3a,118.1b);
draw (1131.9a,610.2b)--(1131.9a,601.9b);
draw (1115.8a,610.2b)--(1115.8a,593.6b);
put_text( btex  1 etex, 1115.8a, 646.8b, 0, 2);
draw (1009.9a,610.2b)--(1009.9a,601.9b);
draw (947.9a,610.2b)--(947.9a,601.9b);
draw (903.9a,610.2b)--(903.9a,601.9b);
draw (869.8a,610.2b)--(869.8a,601.9b);
draw (842.0a,610.2b)--(842.0a,601.9b);
draw (818.4a,610.2b)--(818.4a,601.9b);
draw (798.0a,610.2b)--(798.0a,601.9b);
draw (780.0a,610.2b)--(780.0a,601.9b);
draw (763.9a,610.2b)--(763.9a,593.6b);
put_text( btex  10 etex, 763.9a, 646.8b, 0, 2);
draw (658.0a,610.2b)--(658.0a,601.9b);
draw (596.0a,610.2b)--(596.0a,601.9b);
draw (552.0a,610.2b)--(552.0a,601.9b);
draw (517.9a,610.2b)--(517.9a,601.9b);
draw (490.1a,610.2b)--(490.1a,601.9b);
draw (466.5a,610.2b)--(466.5a,601.9b);
draw (446.1a,610.2b)--(446.1a,601.9b);
draw (428.1a,610.2b)--(428.1a,601.9b);
draw (412.0a,610.2b)--(412.0a,593.6b);
put_text( btex  100 etex, 412.0a, 646.8b, 0, 2);
draw (306.1a,610.2b)--(306.1a,601.9b);
draw (244.1a,610.2b)--(244.1a,601.9b);
draw (200.1a,610.2b)--(200.1a,601.9b);
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,610.2b)--(196.5a,610.2b)--(196.5a,109.8b);
put_text( btex Time/Time-step (sec.) etex, 36.6a, 360.0b, 1, 2);
put_text( btex Nodes/Replica etex, 671.4a, 18.3b, 0, 2);
put_text( btex Replica Count etex, 671.4a, 701.6b, 0, 2);
linetype 0;
linetype -2;
put_text( btex REMD etex, 1005.8a, 575.3b, 0, 3);
linetype 0;
gpdraw(0,1115.8a,262.3b);
gpdraw(0,903.9a,296.4b);
gpdraw(0,798.0a,327.0b);
gpdraw(0,692.1a,365.3b);
gpdraw(0,586.1a,408.2b);
gpdraw(0,480.2a,447.3b);
gpdraw(0,374.3a,498.5b);
gpdraw(0,1067.2a,575.3b);
linetype 1;
linetype -2;
put_text( btex Independent etex, 1005.8a, 538.7b, 0, 3);
linetype 1;
gpdraw(1,798.0a,322.2b);
gpdraw(1,798.0a,322.1b);
gpdraw(1,798.0a,322.6b);
gpdraw(1,903.9a,301.4b);
gpdraw(1,903.9a,301.9b);
gpdraw(1,1009.9a,277.2b);
gpdraw(1,1009.9a,276.0b);
gpdraw(1,1009.9a,276.3b);
gpdraw(1,1067.2a,538.7b);
linetype -2;
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,610.2b)--(196.5a,610.2b)--(196.5a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
