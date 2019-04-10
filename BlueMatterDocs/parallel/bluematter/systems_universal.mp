%GNUPLOT Metapost output: Thu Feb 23 01:49:46 2006


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
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

beginfig(2);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (161.1a,109.8b)--(177.7a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  0 etex, 143.4a, 109.8b, 0, 3);
draw (161.1a,208.6b)--(177.7a,208.6b);
draw (1506.3a,208.6b)--(1489.7a,208.6b);
put_text( btex  100 etex, 143.4a, 208.6b, 0, 3);
draw (161.1a,307.5b)--(177.7a,307.5b);
draw (1506.3a,307.5b)--(1489.7a,307.5b);
put_text( btex  200 etex, 143.4a, 307.5b, 0, 3);
draw (161.1a,406.3b)--(177.7a,406.3b);
draw (1506.3a,406.3b)--(1489.7a,406.3b);
put_text( btex  300 etex, 143.4a, 406.3b, 0, 3);
draw (161.1a,505.2b)--(177.7a,505.2b);
draw (1506.3a,505.2b)--(1489.7a,505.2b);
put_text( btex  400 etex, 143.4a, 505.2b, 0, 3);
draw (161.1a,604.0b)--(177.7a,604.0b);
draw (1506.3a,604.0b)--(1489.7a,604.0b);
put_text( btex  500 etex, 143.4a, 604.0b, 0, 3);
draw (161.1a,702.9b)--(177.7a,702.9b);
draw (1506.3a,702.9b)--(1489.7a,702.9b);
put_text( btex  600 etex, 143.4a, 702.9b, 0, 3);
draw (161.1a,801.7b)--(177.7a,801.7b);
draw (1506.3a,801.7b)--(1489.7a,801.7b);
put_text( btex  700 etex, 143.4a, 801.7b, 0, 3);
draw (161.1a,900.6b)--(177.7a,900.6b);
draw (1506.3a,900.6b)--(1489.7a,900.6b);
put_text( btex  800 etex, 143.4a, 900.6b, 0, 3);
draw (1506.3a,109.8b)--(1506.3a,126.4b);
draw (1506.3a,900.6b)--(1506.3a,884.0b);
put_text( btex  0.1 etex, 1506.3a, 73.2b, 0, 2);
draw (1383.6a,109.8b)--(1383.6a,118.1b);
draw (1383.6a,900.6b)--(1383.6a,892.3b);
draw (1311.9a,109.8b)--(1311.9a,118.1b);
draw (1311.9a,900.6b)--(1311.9a,892.3b);
draw (1261.0a,109.8b)--(1261.0a,118.1b);
draw (1261.0a,900.6b)--(1261.0a,892.3b);
draw (1221.5a,109.8b)--(1221.5a,118.1b);
draw (1221.5a,900.6b)--(1221.5a,892.3b);
draw (1189.2a,109.8b)--(1189.2a,118.1b);
draw (1189.2a,900.6b)--(1189.2a,892.3b);
draw (1161.9a,109.8b)--(1161.9a,118.1b);
draw (1161.9a,900.6b)--(1161.9a,892.3b);
draw (1138.3a,109.8b)--(1138.3a,118.1b);
draw (1138.3a,900.6b)--(1138.3a,892.3b);
draw (1117.4a,109.8b)--(1117.4a,118.1b);
draw (1117.4a,900.6b)--(1117.4a,892.3b);
draw (1098.8a,109.8b)--(1098.8a,126.4b);
draw (1098.8a,900.6b)--(1098.8a,884.0b);
put_text( btex  1 etex, 1098.8a, 73.2b, 0, 2);
draw (976.1a,109.8b)--(976.1a,118.1b);
draw (976.1a,900.6b)--(976.1a,892.3b);
draw (904.4a,109.8b)--(904.4a,118.1b);
draw (904.4a,900.6b)--(904.4a,892.3b);
draw (853.4a,109.8b)--(853.4a,118.1b);
draw (853.4a,900.6b)--(853.4a,892.3b);
draw (814.0a,109.8b)--(814.0a,118.1b);
draw (814.0a,900.6b)--(814.0a,892.3b);
draw (781.7a,109.8b)--(781.7a,118.1b);
draw (781.7a,900.6b)--(781.7a,892.3b);
draw (754.4a,109.8b)--(754.4a,118.1b);
draw (754.4a,900.6b)--(754.4a,892.3b);
draw (730.8a,109.8b)--(730.8a,118.1b);
draw (730.8a,900.6b)--(730.8a,892.3b);
draw (709.9a,109.8b)--(709.9a,118.1b);
draw (709.9a,900.6b)--(709.9a,892.3b);
draw (691.3a,109.8b)--(691.3a,126.4b);
draw (691.3a,900.6b)--(691.3a,884.0b);
put_text( btex  10 etex, 691.3a, 73.2b, 0, 2);
draw (568.6a,109.8b)--(568.6a,118.1b);
draw (568.6a,900.6b)--(568.6a,892.3b);
draw (496.9a,109.8b)--(496.9a,118.1b);
draw (496.9a,900.6b)--(496.9a,892.3b);
draw (445.9a,109.8b)--(445.9a,118.1b);
draw (445.9a,900.6b)--(445.9a,892.3b);
draw (406.4a,109.8b)--(406.4a,118.1b);
draw (406.4a,900.6b)--(406.4a,892.3b);
draw (374.2a,109.8b)--(374.2a,118.1b);
draw (374.2a,900.6b)--(374.2a,892.3b);
draw (346.9a,109.8b)--(346.9a,118.1b);
draw (346.9a,900.6b)--(346.9a,892.3b);
draw (323.3a,109.8b)--(323.3a,118.1b);
draw (323.3a,900.6b)--(323.3a,892.3b);
draw (302.4a,109.8b)--(302.4a,118.1b);
draw (302.4a,900.6b)--(302.4a,892.3b);
draw (283.8a,109.8b)--(283.8a,126.4b);
draw (283.8a,900.6b)--(283.8a,884.0b);
put_text( btex  100 etex, 283.8a, 73.2b, 0, 2);
draw (161.1a,109.8b)--(161.1a,118.1b);
draw (161.1a,900.6b)--(161.1a,892.3b);
draw (161.1a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(161.1a,900.6b)--(161.1a,109.8b);
put_text( btex Computation Rate (time-steps/second) etex, 36.6a, 505.2b, 1, 2);
put_text( btex Atoms/Node etex, 833.7a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex SOPE SPI etex, 1349.1a, 777.0b, 0, 3);
linetype 0;
draw (1366.8a,777.0b)--(1454.2a,777.0b);
draw (516.3a,254.9b)--(639.0a,339.8b)--(761.7a,462.0b)--(761.7a,460.5b)--(884.4a,617.7b)
--(1007.0a,633.5b)--(1129.7a,645.2b);
gpdraw(0,516.3a,254.9b);
gpdraw(0,639.0a,339.8b);
gpdraw(0,761.7a,462.0b);
gpdraw(0,761.7a,460.5b);
gpdraw(0,884.4a,617.7b);
gpdraw(0,1007.0a,633.5b);
gpdraw(0,1129.7a,645.2b);
gpdraw(0,1410.5a,777.0b);
linetype 1;
linetype -2;
put_text( btex Hairpin SPI etex, 1349.1a, 740.4b, 0, 3);
linetype 1;
draw (1366.8a,740.4b)--(1454.2a,740.4b);
draw (687.2a,438.2b)--(809.9a,599.2b)--(932.6a,774.4b)--(932.6a,779.0b)--(1055.2a,892.1b);
gpdraw(1,687.2a,438.2b);
gpdraw(1,809.9a,599.2b);
gpdraw(1,932.6a,774.4b);
gpdraw(1,932.6a,779.0b);
gpdraw(1,1055.2a,892.1b);
gpdraw(1,1410.5a,740.4b);
linetype 2;
linetype -2;
put_text( btex Rhodopsin SPI etex, 1349.1a, 703.8b, 0, 3);
linetype 2;
draw (1366.8a,703.8b)--(1454.2a,703.8b);
draw (313.8a,168.6b)--(436.4a,213.9b)--(559.1a,287.1b)--(559.1a,285.9b)--(681.8a,388.3b)
--(681.8a,394.4b)--(804.4a,503.8b)--(927.1a,543.7b);
gpdraw(2,313.8a,168.6b);
gpdraw(2,436.4a,213.9b);
gpdraw(2,559.1a,287.1b);
gpdraw(2,559.1a,285.9b);
gpdraw(2,681.8a,388.3b);
gpdraw(2,681.8a,394.4b);
gpdraw(2,804.4a,503.8b);
gpdraw(2,927.1a,543.7b);
gpdraw(2,1410.5a,703.8b);
linetype 3;
linetype -2;
put_text( btex ApoA1 Respa SPI etex, 1349.1a, 667.2b, 0, 3);
linetype 3;
draw (1366.8a,667.2b)--(1454.2a,667.2b);
draw (179.6a,137.0b)--(302.3a,161.4b)--(425.0a,202.1b)--(425.0a,202.5b)--(547.6a,275.5b)
--(547.6a,266.3b)--(670.3a,379.1b)--(793.0a,503.9b);
gpdraw(3,179.6a,137.0b);
gpdraw(3,302.3a,161.4b);
gpdraw(3,425.0a,202.1b);
gpdraw(3,425.0a,202.5b);
gpdraw(3,547.6a,275.5b);
gpdraw(3,547.6a,266.3b);
gpdraw(3,670.3a,379.1b);
gpdraw(3,793.0a,503.9b);
gpdraw(3,1410.5a,667.2b);
linetype 4;
linetype -2;
put_text( btex ApoA1 SPI etex, 1349.1a, 630.6b, 0, 3);
linetype 4;
draw (1366.8a,630.6b)--(1454.2a,630.6b);
draw (179.6a,137.0b)--(302.3a,161.3b)--(425.0a,202.2b)--(425.0a,202.4b)--(547.6a,275.4b)
--(547.6a,266.1b)--(670.3a,378.2b)--(793.0a,493.9b);
gpdraw(4,179.6a,137.0b);
gpdraw(4,302.3a,161.3b);
gpdraw(4,425.0a,202.2b);
gpdraw(4,425.0a,202.4b);
gpdraw(4,547.6a,275.4b);
gpdraw(4,547.6a,266.1b);
gpdraw(4,670.3a,378.2b);
gpdraw(4,793.0a,493.9b);
gpdraw(4,1410.5a,630.6b);
linetype 5;
linetype -2;
put_text( btex SOPE MPI etex, 1349.1a, 594.0b, 0, 3);
linetype 5;
draw (1366.8a,594.0b)--(1454.2a,594.0b);
draw (516.3a,242.2b)--(639.0a,298.3b)--(761.7a,326.7b)--(761.7a,321.8b)--(884.4a,304.5b)
--(884.4a,285.9b)--(1007.0a,245.0b)--(1129.7a,188.8b);
gpdraw(5,516.3a,242.2b);
gpdraw(5,639.0a,298.3b);
gpdraw(5,761.7a,326.7b);
gpdraw(5,761.7a,321.8b);
gpdraw(5,884.4a,304.5b);
gpdraw(5,884.4a,285.9b);
gpdraw(5,1007.0a,245.0b);
gpdraw(5,1129.7a,188.8b);
gpdraw(5,1410.5a,594.0b);
linetype 6;
linetype -2;
put_text( btex Rhodopsin MPI etex, 1349.1a, 557.4b, 0, 3);
linetype 6;
draw (1366.8a,557.4b)--(1454.2a,557.4b);
draw (313.8a,168.7b)--(436.4a,214.7b)--(559.1a,264.1b)--(559.1a,262.8b)--(681.8a,287.5b)
--(681.8a,279.5b)--(804.4a,247.6b)--(927.1a,186.5b);
gpdraw(6,313.8a,168.7b);
gpdraw(6,436.4a,214.7b);
gpdraw(6,559.1a,264.1b);
gpdraw(6,559.1a,262.8b);
gpdraw(6,681.8a,287.5b);
gpdraw(6,681.8a,279.5b);
gpdraw(6,804.4a,247.6b);
gpdraw(6,927.1a,186.5b);
gpdraw(6,1410.5a,557.4b);
linetype 7;
linetype -2;
put_text( btex ApoA1 Respa MPI etex, 1349.1a, 520.8b, 0, 3);
linetype 7;
draw (1366.8a,520.8b)--(1454.2a,520.8b);
draw (179.6a,137.6b)--(302.3a,161.1b)--(425.0a,196.9b)--(425.0a,195.9b)--(547.6a,241.1b)
--(547.6a,225.1b)--(670.3a,244.7b)--(793.0a,193.4b);
gpdraw(7,179.6a,137.6b);
gpdraw(7,302.3a,161.1b);
gpdraw(7,425.0a,196.9b);
gpdraw(7,425.0a,195.9b);
gpdraw(7,547.6a,241.1b);
gpdraw(7,547.6a,225.1b);
gpdraw(7,670.3a,244.7b);
gpdraw(7,793.0a,193.4b);
gpdraw(7,1410.5a,520.8b);
linetype 0;
linetype -2;
put_text( btex ApoA1 MPI etex, 1349.1a, 484.2b, 0, 3);
linetype 0;
draw (1366.8a,484.2b)--(1454.2a,484.2b);
draw (179.6a,137.6b)--(302.3a,161.0b)--(425.0a,196.8b)--(425.0a,195.9b)--(547.6a,240.7b)
--(547.6a,225.1b)--(670.3a,244.5b)--(793.0a,188.4b);
gpdraw(8,179.6a,137.6b);
gpdraw(8,302.3a,161.0b);
gpdraw(8,425.0a,196.8b);
gpdraw(8,425.0a,195.9b);
gpdraw(8,547.6a,240.7b);
gpdraw(8,547.6a,225.1b);
gpdraw(8,670.3a,244.5b);
gpdraw(8,793.0a,188.4b);
gpdraw(8,1410.5a,484.2b);
linetype 1;
linetype -2;
put_text( btex ApoA1 NAMD Msging Layer etex, 1349.1a, 447.6b, 0, 3);
linetype 1;
draw (1366.8a,447.6b)--(1454.2a,447.6b);
draw (161.1a,149.1b)--(179.6a,151.5b)--(302.3a,181.4b)--(425.0a,224.7b)--(547.6a,269.2b)
--(670.3a,299.9b);
gpdraw(9,179.6a,151.5b);
gpdraw(9,302.3a,181.4b);
gpdraw(9,425.0a,224.7b);
gpdraw(9,547.6a,269.2b);
gpdraw(9,670.3a,299.9b);
gpdraw(9,1410.5a,447.6b);
linetype 2;
linetype -2;
put_text( btex ApoA1 NAMD MPI etex, 1349.1a, 411.0b, 0, 3);
linetype 2;
draw (1366.8a,411.0b)--(1454.2a,411.0b);
draw (179.6a,145.4b)--(302.3a,166.9b)--(425.0a,206.7b)--(547.6a,245.2b);
gpdraw(0,179.6a,145.4b);
gpdraw(0,302.3a,166.9b);
gpdraw(0,425.0a,206.7b);
gpdraw(0,547.6a,245.2b);
gpdraw(0,1410.5a,411.0b);
linetype 3;
linetype -2;
put_text( btex ApoA1 NAMD on Lemieux etex, 1349.1a, 374.4b, 0, 3);
linetype 3;
draw (1366.8a,374.4b)--(1454.2a,374.4b);
draw (161.1a,149.6b)--(178.9a,152.2b)--(302.1a,180.4b)--(374.1a,188.3b)--(441.6a,187.0b);
gpdraw(1,178.9a,152.2b);
gpdraw(1,302.1a,180.4b);
gpdraw(1,374.1a,188.3b);
gpdraw(1,441.6a,187.0b);
gpdraw(1,1410.5a,374.4b);
linetype -2;
draw (161.1a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(161.1a,900.6b)--(161.1a,109.8b);
endfig;
verbatimtex
 \end{document}
etex
% ENDPOST
end.
