%GNUPLOT Metapost output: Mon Jul 25 15:43:43 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
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
draw (196.5a,167.5b)--(204.8a,167.5b);
draw (1146.3a,167.5b)--(1138.0a,167.5b);
draw (196.5a,201.2b)--(204.8a,201.2b);
draw (1146.3a,201.2b)--(1138.0a,201.2b);
draw (196.5a,225.2b)--(204.8a,225.2b);
draw (1146.3a,225.2b)--(1138.0a,225.2b);
draw (196.5a,243.7b)--(204.8a,243.7b);
draw (1146.3a,243.7b)--(1138.0a,243.7b);
draw (196.5a,258.9b)--(204.8a,258.9b);
draw (1146.3a,258.9b)--(1138.0a,258.9b);
draw (196.5a,271.7b)--(204.8a,271.7b);
draw (1146.3a,271.7b)--(1138.0a,271.7b);
draw (196.5a,282.8b)--(204.8a,282.8b);
draw (1146.3a,282.8b)--(1138.0a,282.8b);
draw (196.5a,292.6b)--(204.8a,292.6b);
draw (1146.3a,292.6b)--(1138.0a,292.6b);
draw (196.5a,301.4b)--(213.1a,301.4b);
draw (1146.3a,301.4b)--(1129.7a,301.4b);
put_text( btex  0.01 etex, 178.8a, 301.4b, 0, 3);
draw (196.5a,359.1b)--(204.8a,359.1b);
draw (1146.3a,359.1b)--(1138.0a,359.1b);
draw (196.5a,392.8b)--(204.8a,392.8b);
draw (1146.3a,392.8b)--(1138.0a,392.8b);
draw (196.5a,416.8b)--(204.8a,416.8b);
draw (1146.3a,416.8b)--(1138.0a,416.8b);
draw (196.5a,435.3b)--(204.8a,435.3b);
draw (1146.3a,435.3b)--(1138.0a,435.3b);
draw (196.5a,450.5b)--(204.8a,450.5b);
draw (1146.3a,450.5b)--(1138.0a,450.5b);
draw (196.5a,463.3b)--(204.8a,463.3b);
draw (1146.3a,463.3b)--(1138.0a,463.3b);
draw (196.5a,474.4b)--(204.8a,474.4b);
draw (1146.3a,474.4b)--(1138.0a,474.4b);
draw (196.5a,484.2b)--(204.8a,484.2b);
draw (1146.3a,484.2b)--(1138.0a,484.2b);
draw (196.5a,493.0b)--(213.1a,493.0b);
draw (1146.3a,493.0b)--(1129.7a,493.0b);
put_text( btex  0.1 etex, 178.8a, 493.0b, 0, 3);
draw (196.5a,550.7b)--(204.8a,550.7b);
draw (1146.3a,550.7b)--(1138.0a,550.7b);
draw (196.5a,584.4b)--(204.8a,584.4b);
draw (1146.3a,584.4b)--(1138.0a,584.4b);
draw (196.5a,608.4b)--(204.8a,608.4b);
draw (1146.3a,608.4b)--(1138.0a,608.4b);
draw (196.5a,626.9b)--(204.8a,626.9b);
draw (1146.3a,626.9b)--(1138.0a,626.9b);
draw (196.5a,642.1b)--(204.8a,642.1b);
draw (1146.3a,642.1b)--(1138.0a,642.1b);
draw (196.5a,654.9b)--(204.8a,654.9b);
draw (1146.3a,654.9b)--(1138.0a,654.9b);
draw (196.5a,666.0b)--(204.8a,666.0b);
draw (1146.3a,666.0b)--(1138.0a,666.0b);
draw (196.5a,675.8b)--(204.8a,675.8b);
draw (1146.3a,675.8b)--(1138.0a,675.8b);
draw (196.5a,684.6b)--(213.1a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  1 etex, 178.8a, 684.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,684.6b)--(196.5a,668.0b);
put_text( btex  10 etex, 196.5a, 73.2b, 0, 2);
draw (291.8a,109.8b)--(291.8a,118.1b);
draw (291.8a,684.6b)--(291.8a,676.3b);
draw (347.6a,109.8b)--(347.6a,118.1b);
draw (347.6a,684.6b)--(347.6a,676.3b);
draw (387.1a,109.8b)--(387.1a,118.1b);
draw (387.1a,684.6b)--(387.1a,676.3b);
draw (417.8a,109.8b)--(417.8a,118.1b);
draw (417.8a,684.6b)--(417.8a,676.3b);
draw (442.9a,109.8b)--(442.9a,118.1b);
draw (442.9a,684.6b)--(442.9a,676.3b);
draw (464.1a,109.8b)--(464.1a,118.1b);
draw (464.1a,684.6b)--(464.1a,676.3b);
draw (482.4a,109.8b)--(482.4a,118.1b);
draw (482.4a,684.6b)--(482.4a,676.3b);
draw (498.6a,109.8b)--(498.6a,118.1b);
draw (498.6a,684.6b)--(498.6a,676.3b);
draw (513.1a,109.8b)--(513.1a,126.4b);
draw (513.1a,684.6b)--(513.1a,668.0b);
put_text( btex  100 etex, 513.1a, 73.2b, 0, 2);
draw (608.4a,109.8b)--(608.4a,118.1b);
draw (608.4a,684.6b)--(608.4a,676.3b);
draw (664.2a,109.8b)--(664.2a,118.1b);
draw (664.2a,684.6b)--(664.2a,676.3b);
draw (703.7a,109.8b)--(703.7a,118.1b);
draw (703.7a,684.6b)--(703.7a,676.3b);
draw (734.4a,109.8b)--(734.4a,118.1b);
draw (734.4a,684.6b)--(734.4a,676.3b);
draw (759.5a,109.8b)--(759.5a,118.1b);
draw (759.5a,684.6b)--(759.5a,676.3b);
draw (780.7a,109.8b)--(780.7a,118.1b);
draw (780.7a,684.6b)--(780.7a,676.3b);
draw (799.0a,109.8b)--(799.0a,118.1b);
draw (799.0a,684.6b)--(799.0a,676.3b);
draw (815.2a,109.8b)--(815.2a,118.1b);
draw (815.2a,684.6b)--(815.2a,676.3b);
draw (829.7a,109.8b)--(829.7a,126.4b);
draw (829.7a,684.6b)--(829.7a,668.0b);
put_text( btex  1000 etex, 829.7a, 73.2b, 0, 2);
draw (925.0a,109.8b)--(925.0a,118.1b);
draw (925.0a,684.6b)--(925.0a,676.3b);
draw (980.8a,109.8b)--(980.8a,118.1b);
draw (980.8a,684.6b)--(980.8a,676.3b);
draw (1020.3a,109.8b)--(1020.3a,118.1b);
draw (1020.3a,684.6b)--(1020.3a,676.3b);
draw (1051.0a,109.8b)--(1051.0a,118.1b);
draw (1051.0a,684.6b)--(1051.0a,676.3b);
draw (1076.1a,109.8b)--(1076.1a,118.1b);
draw (1076.1a,684.6b)--(1076.1a,676.3b);
draw (1097.3a,109.8b)--(1097.3a,118.1b);
draw (1097.3a,684.6b)--(1097.3a,676.3b);
draw (1115.6a,109.8b)--(1115.6a,118.1b);
draw (1115.6a,684.6b)--(1115.6a,676.3b);
draw (1131.8a,109.8b)--(1131.8a,118.1b);
draw (1131.8a,684.6b)--(1131.8a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  10000 etex, 1146.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
put_text( btex Elapsed Time (seconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 671.4a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex 92K atom ApoA1 etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
draw (1023.5a,649.7b)--(1110.9a,649.7b);
draw (737.7a,447.4b)--(833.0a,405.2b)--(928.3a,375.5b)--(1023.6a,329.0b);
gpdraw(0,737.7a,447.4b);
gpdraw(0,833.0a,405.2b);
gpdraw(0,928.3a,375.5b);
gpdraw(0,1023.6a,329.0b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
linetype -2;
put_text( btex 51K atom Mini FBP etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
draw (1023.5a,613.1b)--(1110.9a,613.1b);
draw (356.4a,561.1b)--(547.0a,455.3b)--(737.7a,358.4b)--(833.0a,331.7b)--(928.3a,297.0b)
--(1023.6a,281.6b);
gpdraw(1,356.4a,561.1b);
gpdraw(1,547.0a,455.3b);
gpdraw(1,737.7a,358.4b);
gpdraw(1,833.0a,331.7b);
gpdraw(1,928.3a,297.0b);
gpdraw(1,1023.6a,281.6b);
gpdraw(1,1067.2a,613.1b);
linetype 2;
linetype -2;
put_text( btex 43K atom Rhodopsin etex, 1005.8a, 576.5b, 0, 3);
linetype 2;
draw (1023.5a,576.5b)--(1110.9a,576.5b);
draw (356.4a,600.6b)--(547.0a,485.3b)--(737.7a,378.5b)--(833.0a,352.4b)--(928.3a,303.2b)
--(1023.6a,292.3b);
gpdraw(2,356.4a,600.6b);
gpdraw(2,547.0a,485.3b);
gpdraw(2,737.7a,378.5b);
gpdraw(2,833.0a,352.4b);
gpdraw(2,928.3a,303.2b);
gpdraw(2,1023.6a,292.3b);
gpdraw(2,1067.2a,576.5b);
linetype 3;
linetype -2;
put_text( btex 23K atom DHFR etex, 1005.8a, 539.9b, 0, 3);
linetype 3;
draw (1023.5a,539.9b)--(1110.9a,539.9b);
draw (356.4a,493.9b)--(547.0a,389.5b)--(737.7a,294.2b)--(833.0a,278.0b)--(928.3a,259.8b);
gpdraw(3,356.4a,493.9b);
gpdraw(3,547.0a,389.5b);
gpdraw(3,737.7a,294.2b);
gpdraw(3,833.0a,278.0b);
gpdraw(3,928.3a,259.8b);
gpdraw(3,1067.2a,539.9b);
linetype -2;
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
