%GNUPLOT Metapost output: Thu Mar 15 17:27:31 2007


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

beginfig(3);
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
put_text( btex  0.1 etex, 1506.3a, 73.2b, 0, 2);
draw (1385.2a,109.8b)--(1385.2a,118.1b);
draw (1385.2a,900.6b)--(1385.2a,892.3b);
draw (1314.4a,109.8b)--(1314.4a,118.1b);
draw (1314.4a,900.6b)--(1314.4a,892.3b);
draw (1264.2a,109.8b)--(1264.2a,118.1b);
draw (1264.2a,900.6b)--(1264.2a,892.3b);
draw (1225.2a,109.8b)--(1225.2a,118.1b);
draw (1225.2a,900.6b)--(1225.2a,892.3b);
draw (1193.4a,109.8b)--(1193.4a,118.1b);
draw (1193.4a,900.6b)--(1193.4a,892.3b);
draw (1166.4a,109.8b)--(1166.4a,118.1b);
draw (1166.4a,900.6b)--(1166.4a,892.3b);
draw (1143.1a,109.8b)--(1143.1a,118.1b);
draw (1143.1a,900.6b)--(1143.1a,892.3b);
draw (1122.6a,109.8b)--(1122.6a,118.1b);
draw (1122.6a,900.6b)--(1122.6a,892.3b);
draw (1104.2a,109.8b)--(1104.2a,126.4b);
draw (1104.2a,900.6b)--(1104.2a,884.0b);
put_text( btex  1 etex, 1104.2a, 73.2b, 0, 2);
draw (983.1a,109.8b)--(983.1a,118.1b);
draw (983.1a,900.6b)--(983.1a,892.3b);
draw (912.3a,109.8b)--(912.3a,118.1b);
draw (912.3a,900.6b)--(912.3a,892.3b);
draw (862.0a,109.8b)--(862.0a,118.1b);
draw (862.0a,900.6b)--(862.0a,892.3b);
draw (823.1a,109.8b)--(823.1a,118.1b);
draw (823.1a,900.6b)--(823.1a,892.3b);
draw (791.2a,109.8b)--(791.2a,118.1b);
draw (791.2a,900.6b)--(791.2a,892.3b);
draw (764.3a,109.8b)--(764.3a,118.1b);
draw (764.3a,900.6b)--(764.3a,892.3b);
draw (741.0a,109.8b)--(741.0a,118.1b);
draw (741.0a,900.6b)--(741.0a,892.3b);
draw (720.4a,109.8b)--(720.4a,118.1b);
draw (720.4a,900.6b)--(720.4a,892.3b);
draw (702.0a,109.8b)--(702.0a,126.4b);
draw (702.0a,900.6b)--(702.0a,884.0b);
put_text( btex  10 etex, 702.0a, 73.2b, 0, 2);
draw (580.9a,109.8b)--(580.9a,118.1b);
draw (580.9a,900.6b)--(580.9a,892.3b);
draw (510.1a,109.8b)--(510.1a,118.1b);
draw (510.1a,900.6b)--(510.1a,892.3b);
draw (459.9a,109.8b)--(459.9a,118.1b);
draw (459.9a,900.6b)--(459.9a,892.3b);
draw (420.9a,109.8b)--(420.9a,118.1b);
draw (420.9a,900.6b)--(420.9a,892.3b);
draw (389.1a,109.8b)--(389.1a,118.1b);
draw (389.1a,900.6b)--(389.1a,892.3b);
draw (362.2a,109.8b)--(362.2a,118.1b);
draw (362.2a,900.6b)--(362.2a,892.3b);
draw (338.8a,109.8b)--(338.8a,118.1b);
draw (338.8a,900.6b)--(338.8a,892.3b);
draw (318.3a,109.8b)--(318.3a,118.1b);
draw (318.3a,900.6b)--(318.3a,892.3b);
draw (299.9a,109.8b)--(299.9a,126.4b);
draw (299.9a,900.6b)--(299.9a,884.0b);
put_text( btex  100 etex, 299.9a, 73.2b, 0, 2);
draw (178.8a,109.8b)--(178.8a,118.1b);
draw (178.8a,900.6b)--(178.8a,892.3b);
draw (178.8a,900.6b)--(178.8a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(178.8a,900.6b);
put_text( btex Computation Rate (time-steps/second) etex, 36.6a, 505.2b, 90, 2);
linetype -2;
put_text( btex Atoms/Node etex, 842.5a, 18.3b, 0, 2);
linetype -2;
linetype 2;
linetype -2;
put_text( btex Hairpin SPI $64^3$ (V5) etex, 497.1a, 854.1b, 0, 3);
linetype 2;
draw (514.8a,854.1b)--(602.2a,854.1b);
draw (698.0a,361.4b)--(819.1a,500.5b)--(940.1a,673.5b)--(1061.2a,792.1b);
gpdraw(0,698.0a,361.4b);
gpdraw(0,819.1a,500.5b);
gpdraw(0,940.1a,673.5b);
gpdraw(0,1061.2a,792.1b);
gpdraw(0,558.5a,854.1b);
linetype -2;
put_text( btex SOPE SPI $64^3$ (V5) etex, 497.1a, 817.5b, 0, 3);
linetype 2;
draw (514.8a,817.5b)--(602.2a,817.5b);
draw (529.4a,226.8b)--(650.4a,312.0b)--(771.5a,422.8b)--(892.5a,551.5b)--(892.5a,560.7b)
--(1013.6a,690.3b);
gpdraw(2,529.4a,226.8b);
gpdraw(2,650.4a,312.0b);
gpdraw(2,771.5a,422.8b);
gpdraw(2,892.5a,551.5b);
gpdraw(2,892.5a,560.7b);
gpdraw(2,1013.6a,690.3b);
gpdraw(2,558.5a,817.5b);
linetype -2;
put_text( btex Hairpin SPI $64^3$ (V4) etex, 497.1a, 780.9b, 0, 3);
linetype 2;
draw (514.8a,780.9b)--(602.2a,780.9b);
draw (698.0a,297.5b)--(819.1a,389.4b)--(940.1a,489.6b)--(940.1a,492.2b)--(1061.2a,556.8b);
gpdraw(6,698.0a,297.5b);
gpdraw(6,819.1a,389.4b);
gpdraw(6,940.1a,489.6b);
gpdraw(6,940.1a,492.2b);
gpdraw(6,1061.2a,556.8b);
gpdraw(6,558.5a,780.9b);
linetype 0;
linetype -2;
put_text( btex SOPE SPI (V5) etex, 497.1a, 744.3b, 0, 3);
linetype 0;
draw (514.8a,744.3b)--(602.2a,744.3b);
draw (529.4a,200.6b)--(650.4a,254.9b)--(771.5a,340.0b)--(892.5a,461.8b)--(1013.6a,486.6b)
--(1134.7a,449.2b);
gpdraw(2,529.4a,200.6b);
gpdraw(2,650.4a,254.9b);
gpdraw(2,771.5a,340.0b);
gpdraw(2,892.5a,461.8b);
gpdraw(2,1013.6a,486.6b);
gpdraw(2,1134.7a,449.2b);
gpdraw(2,558.5a,744.3b);
linetype -2;
put_text( btex Rhodopsin SPI (V5) etex, 497.1a, 707.7b, 0, 3);
linetype 0;
draw (514.8a,707.7b)--(602.2a,707.7b);
draw (329.4a,142.0b)--(450.5a,169.4b)--(571.6a,221.2b)--(692.6a,291.6b)--(813.7a,370.8b)
--(934.7a,410.3b);
gpdraw(4,329.4a,142.0b);
gpdraw(4,450.5a,169.4b);
gpdraw(4,571.6a,221.2b);
gpdraw(4,692.6a,291.6b);
gpdraw(4,813.7a,370.8b);
gpdraw(4,934.7a,410.3b);
gpdraw(4,558.5a,707.7b);
linetype -2;
put_text( btex SOPE SPI (V4) etex, 497.1a, 671.1b, 0, 3);
linetype 0;
draw (514.8a,671.1b)--(602.2a,671.1b);
draw (529.4a,192.7b)--(650.4a,241.2b)--(771.5a,311.1b)--(771.5a,310.2b)--(892.5a,400.0b)
--(1013.6a,409.1b)--(1134.7a,415.8b);
gpdraw(7,529.4a,192.7b);
gpdraw(7,650.4a,241.2b);
gpdraw(7,771.5a,311.1b);
gpdraw(7,771.5a,310.2b);
gpdraw(7,892.5a,400.0b);
gpdraw(7,1013.6a,409.1b);
gpdraw(7,1134.7a,415.8b);
gpdraw(7,558.5a,671.1b);
linetype -2;
put_text( btex ApoA1 SPI (V5) etex, 497.1a, 634.5b, 0, 3);
linetype 0;
draw (514.8a,634.5b)--(602.2a,634.5b);
draw (197.1a,124.5b)--(318.1a,139.6b)--(439.2a,166.5b)--(560.3a,214.5b)--(681.3a,289.6b)
--(801.3a,380.7b);
gpdraw(5,197.1a,124.5b);
gpdraw(5,318.1a,139.6b);
gpdraw(5,439.2a,166.5b);
gpdraw(5,560.3a,214.5b);
gpdraw(5,681.3a,289.6b);
gpdraw(5,801.3a,380.7b);
gpdraw(5,558.5a,634.5b);
linetype -2;
put_text( btex Rhodopsin SPI (V4) etex, 497.1a, 597.9b, 0, 3);
linetype 0;
draw (514.8a,597.9b)--(602.2a,597.9b);
draw (329.4a,143.4b)--(450.5a,169.3b)--(571.6a,211.1b)--(571.6a,210.5b)--(692.6a,268.9b)
--(692.6a,272.4b)--(813.7a,334.9b)--(934.7a,357.8b);
gpdraw(8,329.4a,143.4b);
gpdraw(8,450.5a,169.3b);
gpdraw(8,571.6a,211.1b);
gpdraw(8,571.6a,210.5b);
gpdraw(8,692.6a,268.9b);
gpdraw(8,692.6a,272.4b);
gpdraw(8,813.7a,334.9b);
gpdraw(8,934.7a,357.8b);
gpdraw(8,558.5a,597.9b);
linetype -2;
put_text( btex ApoA1 SPI (V4) etex, 497.1a, 561.3b, 0, 3);
linetype 0;
draw (514.8a,561.3b)--(602.2a,561.3b);
draw (197.1a,125.3b)--(318.1a,139.3b)--(439.2a,162.6b)--(439.2a,162.7b)--(560.3a,204.4b)
--(560.3a,199.1b)--(681.3a,263.2b)--(802.4a,329.3b);
gpdraw(9,197.1a,125.3b);
gpdraw(9,318.1a,139.3b);
gpdraw(9,439.2a,162.6b);
gpdraw(9,439.2a,162.7b);
gpdraw(9,560.3a,204.4b);
gpdraw(9,560.3a,199.1b);
gpdraw(9,681.3a,263.2b);
gpdraw(9,802.4a,329.3b);
gpdraw(9,558.5a,561.3b);
linetype 1;
linetype -2;
put_text( btex SOPE MPI (V4) etex, 497.1a, 524.7b, 0, 3);
linetype 1;
draw (514.8a,524.7b)--(602.2a,524.7b);
draw (529.4a,185.4b)--(650.4a,217.5b)--(771.5a,233.7b)--(771.5a,230.9b)--(892.5a,221.0b)
--(1013.6a,187.1b)--(1134.7a,154.9b);
gpdraw(7,529.4a,185.4b);
gpdraw(7,650.4a,217.5b);
gpdraw(7,771.5a,233.7b);
gpdraw(7,771.5a,230.9b);
gpdraw(7,892.5a,221.0b);
gpdraw(7,1013.6a,187.1b);
gpdraw(7,1134.7a,154.9b);
gpdraw(7,558.5a,524.7b);
linetype -2;
put_text( btex Rhodopsin MPI (V4) etex, 497.1a, 488.1b, 0, 3);
linetype 1;
draw (514.8a,488.1b)--(602.2a,488.1b);
draw (329.4a,143.5b)--(450.5a,169.8b)--(571.6a,198.0b)--(571.6a,197.2b)--(692.6a,211.3b)
--(813.7a,188.6b)--(934.7a,153.6b);
gpdraw(8,329.4a,143.5b);
gpdraw(8,450.5a,169.8b);
gpdraw(8,571.6a,198.0b);
gpdraw(8,571.6a,197.2b);
gpdraw(8,692.6a,211.3b);
gpdraw(8,813.7a,188.6b);
gpdraw(8,934.7a,153.6b);
gpdraw(8,558.5a,488.1b);
linetype -2;
put_text( btex ApoA1 MPI (V4) etex, 497.1a, 451.5b, 0, 3);
linetype 1;
draw (514.8a,451.5b)--(602.2a,451.5b);
draw (197.1a,125.7b)--(318.1a,139.1b)--(439.2a,159.5b)--(439.2a,159.0b)--(560.3a,184.6b)
--(560.3a,175.7b)--(681.3a,186.8b)--(802.4a,154.7b);
gpdraw(9,197.1a,125.7b);
gpdraw(9,318.1a,139.1b);
gpdraw(9,439.2a,159.5b);
gpdraw(9,439.2a,159.0b);
gpdraw(9,560.3a,184.6b);
gpdraw(9,560.3a,175.7b);
gpdraw(9,681.3a,186.8b);
gpdraw(9,802.4a,154.7b);
gpdraw(9,558.5a,451.5b);
linetype -2;
draw (178.8a,900.6b)--(178.8a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(178.8a,900.6b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
