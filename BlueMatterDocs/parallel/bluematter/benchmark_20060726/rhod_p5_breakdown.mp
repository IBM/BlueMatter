%GNUPLOT Metapost output: Tue Jun 12 02:47:44 2007


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


textmag:= 0.800;
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
col7:=.5*blue;

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

beginfig(11);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (170.8a,293.0b)--(187.4a,293.0b);
draw (1157.1a,293.0b)--(1140.5a,293.0b);
put_text( btex  $10,000$ etex, 156.7a, 293.0b, 0, 3);
draw (170.8a,323.0b)--(179.1a,323.0b);
draw (1157.1a,323.0b)--(1148.8a,323.0b);
draw (170.8a,340.6b)--(179.1a,340.6b);
draw (1157.1a,340.6b)--(1148.8a,340.6b);
draw (170.8a,353.0b)--(179.1a,353.0b);
draw (1157.1a,353.0b)--(1148.8a,353.0b);
draw (170.8a,362.7b)--(179.1a,362.7b);
draw (1157.1a,362.7b)--(1148.8a,362.7b);
draw (170.8a,370.6b)--(179.1a,370.6b);
draw (1157.1a,370.6b)--(1148.8a,370.6b);
draw (170.8a,377.3b)--(179.1a,377.3b);
draw (1157.1a,377.3b)--(1148.8a,377.3b);
draw (170.8a,383.0b)--(179.1a,383.0b);
draw (1157.1a,383.0b)--(1148.8a,383.0b);
draw (170.8a,388.1b)--(179.1a,388.1b);
draw (1157.1a,388.1b)--(1148.8a,388.1b);
draw (170.8a,392.7b)--(187.4a,392.7b);
draw (1157.1a,392.7b)--(1140.5a,392.7b);
put_text( btex  $100,000$ etex, 156.7a, 392.7b, 0, 3);
draw (170.8a,422.7b)--(179.1a,422.7b);
draw (1157.1a,422.7b)--(1148.8a,422.7b);
draw (170.8a,440.3b)--(179.1a,440.3b);
draw (1157.1a,440.3b)--(1148.8a,440.3b);
draw (170.8a,452.7b)--(179.1a,452.7b);
draw (1157.1a,452.7b)--(1148.8a,452.7b);
draw (170.8a,462.4b)--(179.1a,462.4b);
draw (1157.1a,462.4b)--(1148.8a,462.4b);
draw (170.8a,470.3b)--(179.1a,470.3b);
draw (1157.1a,470.3b)--(1148.8a,470.3b);
draw (170.8a,477.0b)--(179.1a,477.0b);
draw (1157.1a,477.0b)--(1148.8a,477.0b);
draw (170.8a,482.7b)--(179.1a,482.7b);
draw (1157.1a,482.7b)--(1148.8a,482.7b);
draw (170.8a,487.8b)--(179.1a,487.8b);
draw (1157.1a,487.8b)--(1148.8a,487.8b);
draw (170.8a,492.4b)--(187.4a,492.4b);
draw (1157.1a,492.4b)--(1140.5a,492.4b);
put_text( btex  $10^6$ etex, 156.7a, 492.4b, 0, 3);
draw (170.8a,522.4b)--(179.1a,522.4b);
draw (1157.1a,522.4b)--(1148.8a,522.4b);
draw (170.8a,540.0b)--(179.1a,540.0b);
draw (1157.1a,540.0b)--(1148.8a,540.0b);
draw (170.8a,552.4b)--(179.1a,552.4b);
draw (1157.1a,552.4b)--(1148.8a,552.4b);
draw (170.8a,562.1b)--(179.1a,562.1b);
draw (1157.1a,562.1b)--(1148.8a,562.1b);
draw (170.8a,570.0b)--(179.1a,570.0b);
draw (1157.1a,570.0b)--(1148.8a,570.0b);
draw (170.8a,576.7b)--(179.1a,576.7b);
draw (1157.1a,576.7b)--(1148.8a,576.7b);
draw (170.8a,582.4b)--(179.1a,582.4b);
draw (1157.1a,582.4b)--(1148.8a,582.4b);
draw (170.8a,587.5b)--(179.1a,587.5b);
draw (1157.1a,587.5b)--(1148.8a,587.5b);
draw (170.8a,592.1b)--(187.4a,592.1b);
draw (1157.1a,592.1b)--(1140.5a,592.1b);
put_text( btex  $10^7$ etex, 156.7a, 592.1b, 0, 3);
draw (170.8a,622.1b)--(179.1a,622.1b);
draw (1157.1a,622.1b)--(1148.8a,622.1b);
draw (170.8a,639.7b)--(179.1a,639.7b);
draw (1157.1a,639.7b)--(1148.8a,639.7b);
draw (170.8a,652.1b)--(179.1a,652.1b);
draw (1157.1a,652.1b)--(1148.8a,652.1b);
draw (170.8a,661.8b)--(179.1a,661.8b);
draw (1157.1a,661.8b)--(1148.8a,661.8b);
draw (170.8a,669.7b)--(179.1a,669.7b);
draw (1157.1a,669.7b)--(1148.8a,669.7b);
draw (170.8a,676.4b)--(179.1a,676.4b);
draw (1157.1a,676.4b)--(1148.8a,676.4b);
draw (170.8a,682.1b)--(179.1a,682.1b);
draw (1157.1a,682.1b)--(1148.8a,682.1b);
draw (170.8a,687.2b)--(179.1a,687.2b);
draw (1157.1a,687.2b)--(1148.8a,687.2b);
draw (170.8a,691.8b)--(187.4a,691.8b);
draw (1157.1a,691.8b)--(1140.5a,691.8b);
put_text( btex  $10^8$ etex, 156.7a, 691.8b, 0, 3);
draw (170.8a,293.0b)--(170.8a,309.6b);
draw (170.8a,691.8b)--(170.8a,675.2b);
put_text( btex  $100$ etex, 170.8a, 263.7b, 0, 2);
draw (269.8a,293.0b)--(269.8a,301.3b);
draw (269.8a,691.8b)--(269.8a,683.5b);
draw (327.7a,293.0b)--(327.7a,301.3b);
draw (327.7a,691.8b)--(327.7a,683.5b);
draw (368.7a,293.0b)--(368.7a,301.3b);
draw (368.7a,691.8b)--(368.7a,683.5b);
draw (400.6a,293.0b)--(400.6a,301.3b);
draw (400.6a,691.8b)--(400.6a,683.5b);
draw (426.6a,293.0b)--(426.6a,301.3b);
draw (426.6a,691.8b)--(426.6a,683.5b);
draw (448.6a,293.0b)--(448.6a,301.3b);
draw (448.6a,691.8b)--(448.6a,683.5b);
draw (467.7a,293.0b)--(467.7a,301.3b);
draw (467.7a,691.8b)--(467.7a,683.5b);
draw (484.5a,293.0b)--(484.5a,301.3b);
draw (484.5a,691.8b)--(484.5a,683.5b);
draw (499.6a,293.0b)--(499.6a,309.6b);
draw (499.6a,691.8b)--(499.6a,675.2b);
put_text( btex  $1000$ etex, 499.6a, 263.7b, 0, 2);
draw (598.5a,293.0b)--(598.5a,301.3b);
draw (598.5a,691.8b)--(598.5a,683.5b);
draw (656.4a,293.0b)--(656.4a,301.3b);
draw (656.4a,691.8b)--(656.4a,683.5b);
draw (697.5a,293.0b)--(697.5a,301.3b);
draw (697.5a,691.8b)--(697.5a,683.5b);
draw (729.4a,293.0b)--(729.4a,301.3b);
draw (729.4a,691.8b)--(729.4a,683.5b);
draw (755.4a,293.0b)--(755.4a,301.3b);
draw (755.4a,691.8b)--(755.4a,683.5b);
draw (777.4a,293.0b)--(777.4a,301.3b);
draw (777.4a,691.8b)--(777.4a,683.5b);
draw (796.5a,293.0b)--(796.5a,301.3b);
draw (796.5a,691.8b)--(796.5a,683.5b);
draw (813.3a,293.0b)--(813.3a,301.3b);
draw (813.3a,691.8b)--(813.3a,683.5b);
draw (828.3a,293.0b)--(828.3a,309.6b);
draw (828.3a,691.8b)--(828.3a,675.2b);
put_text( btex  $10,000$ etex, 828.3a, 263.7b, 0, 2);
draw (927.3a,293.0b)--(927.3a,301.3b);
draw (927.3a,691.8b)--(927.3a,683.5b);
draw (985.2a,293.0b)--(985.2a,301.3b);
draw (985.2a,691.8b)--(985.2a,683.5b);
draw (1026.3a,293.0b)--(1026.3a,301.3b);
draw (1026.3a,691.8b)--(1026.3a,683.5b);
draw (1058.1a,293.0b)--(1058.1a,301.3b);
draw (1058.1a,691.8b)--(1058.1a,683.5b);
draw (1084.2a,293.0b)--(1084.2a,301.3b);
draw (1084.2a,691.8b)--(1084.2a,683.5b);
draw (1106.2a,293.0b)--(1106.2a,301.3b);
draw (1106.2a,691.8b)--(1106.2a,683.5b);
draw (1125.2a,293.0b)--(1125.2a,301.3b);
draw (1125.2a,691.8b)--(1125.2a,683.5b);
draw (1142.1a,293.0b)--(1142.1a,301.3b);
draw (1142.1a,691.8b)--(1142.1a,683.5b);
draw (1157.1a,293.0b)--(1157.1a,309.6b);
draw (1157.1a,691.8b)--(1157.1a,675.2b);
put_text( btex  $100,000$ etex, 1157.1a, 263.7b, 0, 2);
draw (170.8a,691.8b)--(170.8a,293.0b)--(1157.1a,293.0b)--(1157.1a,691.8b)--(170.8a,691.8b);
put_text( btex Time/Time-step (ns) etex, 29.2a, 492.4b, 90, 2);
linetype -2;
put_text( btex Nodes etex, 663.9a, 219.8b, 0, 2);
linetype -2;
scalepen 2.000;
linetype 0;
linetype -2;
put_text( btex Total Time-step etex, 226.8a, 177.8b, 0, 1);
linetype 0;
draw (139.7a,177.8b)--(212.7a,177.8b);
draw (404.0a,615.1b)--(503.0a,587.5b)--(601.9a,561.6b)--(700.9a,540.8b)--(799.9a,525.0b)
--(898.8a,519.5b);
gpdraw(0,404.0a,615.1b);
gpdraw(0,503.0a,587.5b);
gpdraw(0,601.9a,561.6b);
gpdraw(0,700.9a,540.8b);
gpdraw(0,799.9a,525.0b);
gpdraw(0,898.8a,519.5b);
gpdraw(0,176.2a,177.8b);
scalepen 1.000;
linetype 1;
linetype -2;
put_text( btex Total K-space etex, 226.8a, 148.5b, 0, 1);
linetype 1;
draw (139.7a,148.5b)--(212.7a,148.5b);
draw (404.0a,574.3b)--(503.0a,553.3b)--(601.9a,530.8b)--(700.9a,510.7b)--(799.9a,507.7b)
--(898.8a,504.3b);
gpdraw(1,404.0a,574.3b);
gpdraw(1,503.0a,553.3b);
gpdraw(1,601.9a,530.8b);
gpdraw(1,700.9a,510.7b);
gpdraw(1,799.9a,507.7b);
gpdraw(1,898.8a,504.3b);
gpdraw(1,176.2a,148.5b);
linetype 2;
linetype -2;
put_text( btex K-space Broadcast etex, 226.8a, 119.2b, 0, 1);
linetype 2;
draw (139.7a,119.2b)--(212.7a,119.2b);
draw (404.0a,444.4b)--(503.0a,421.7b)--(601.9a,411.6b)--(700.9a,406.7b)--(799.9a,397.5b)
--(898.8a,374.4b);
gpdraw(2,404.0a,444.4b);
gpdraw(2,503.0a,421.7b);
gpdraw(2,601.9a,411.6b);
gpdraw(2,700.9a,406.7b);
gpdraw(2,799.9a,397.5b);
gpdraw(2,898.8a,374.4b);
gpdraw(2,176.2a,119.2b);
linetype 3;
linetype -2;
put_text( btex K-space Reduction etex, 226.8a, 89.9b, 0, 1);
linetype 3;
draw (139.7a,89.9b)--(212.7a,89.9b);
draw (404.0a,396.5b)--(503.0a,384.5b)--(601.9a,367.0b)--(700.9a,352.8b)--(799.9a,363.1b)
--(898.8a,333.0b);
gpdraw(3,404.0a,396.5b);
gpdraw(3,503.0a,384.5b);
gpdraw(3,601.9a,367.0b);
gpdraw(3,700.9a,352.8b);
gpdraw(3,799.9a,363.1b);
gpdraw(3,898.8a,333.0b);
gpdraw(3,176.2a,89.9b);
linetype 4;
linetype -2;
put_text( btex K-space Forward FFT etex, 226.8a, 60.6b, 0, 1);
linetype 4;
draw (139.7a,60.6b)--(212.7a,60.6b);
draw (404.0a,524.7b)--(503.0a,507.6b)--(601.9a,488.1b)--(700.9a,466.0b)--(799.9a,467.4b)
--(898.8a,464.1b);
gpdraw(4,404.0a,524.7b);
gpdraw(4,503.0a,507.6b);
gpdraw(4,601.9a,488.1b);
gpdraw(4,700.9a,466.0b);
gpdraw(4,799.9a,467.4b);
gpdraw(4,898.8a,464.1b);
gpdraw(4,176.2a,60.6b);
linetype 5;
linetype -2;
put_text( btex K-space Reverse FFT etex, 226.8a, 31.3b, 0, 1);
linetype 5;
draw (139.7a,31.3b)--(212.7a,31.3b);
draw (404.0a,526.5b)--(503.0a,506.3b)--(601.9a,480.9b)--(700.9a,460.0b)--(799.9a,460.6b)
--(898.8a,466.3b);
gpdraw(5,404.0a,526.5b);
gpdraw(5,503.0a,506.3b);
gpdraw(5,601.9a,480.9b);
gpdraw(5,700.9a,460.0b);
gpdraw(5,799.9a,460.6b);
gpdraw(5,898.8a,466.3b);
gpdraw(5,176.2a,31.3b);
linetype 6;
linetype -2;
put_text( btex K-space Mesh Charges etex, 765.1a, 177.8b, 0, 1);
linetype 6;
draw (678.0a,177.8b)--(751.0a,177.8b);
draw (404.0a,487.2b)--(503.0a,458.4b)--(503.0a,458.7b)--(601.9a,431.0b)--(700.9a,406.8b)
--(700.9a,404.5b)--(799.9a,380.7b)--(898.8a,357.3b);
gpdraw(6,404.0a,487.2b);
gpdraw(6,404.0a,487.2b);
gpdraw(6,503.0a,458.4b);
gpdraw(6,503.0a,458.7b);
gpdraw(6,601.9a,431.0b);
gpdraw(6,601.9a,431.0b);
gpdraw(6,700.9a,406.8b);
gpdraw(6,700.9a,404.5b);
gpdraw(6,700.9a,404.5b);
gpdraw(6,799.9a,380.7b);
gpdraw(6,898.8a,357.3b);
gpdraw(6,714.5a,177.8b);
linetype 7;
linetype -2;
put_text( btex K-space Unmesh Forces etex, 765.1a, 148.5b, 0, 1);
linetype 7;
draw (678.0a,148.5b)--(751.0a,148.5b);
draw (404.0a,446.9b)--(404.0a,447.2b)--(503.0a,417.7b)--(503.0a,416.5b)--(601.9a,387.8b)
--(700.9a,363.5b)--(700.9a,363.4b)--(700.9a,365.0b)--(799.9a,339.8b)--(898.8a,316.2b);
gpdraw(7,404.0a,446.9b);
gpdraw(7,404.0a,447.2b);
gpdraw(7,503.0a,417.7b);
gpdraw(7,503.0a,416.5b);
gpdraw(7,601.9a,387.8b);
gpdraw(7,601.9a,387.8b);
gpdraw(7,700.9a,363.5b);
gpdraw(7,700.9a,363.4b);
gpdraw(7,700.9a,365.0b);
gpdraw(7,799.9a,339.8b);
gpdraw(7,898.8a,316.2b);
gpdraw(7,714.5a,148.5b);
linetype 0;
linetype -2;
put_text( btex Real Space Non-bond Computation etex, 765.1a, 119.2b, 0, 1);
linetype 0;
draw (678.0a,119.2b)--(751.0a,119.2b);
draw (404.0a,594.3b)--(503.0a,563.0b)--(601.9a,534.1b)--(700.9a,504.9b)--(799.9a,475.6b)
--(898.8a,448.0b);
gpdraw(8,404.0a,594.3b);
gpdraw(8,503.0a,563.0b);
gpdraw(8,601.9a,534.1b);
gpdraw(8,700.9a,504.9b);
gpdraw(8,799.9a,475.6b);
gpdraw(8,898.8a,448.0b);
gpdraw(8,714.5a,119.2b);
linetype 1;
linetype -2;
put_text( btex Real Space Non-bond Broadcast etex, 765.1a, 89.9b, 0, 1);
linetype 1;
draw (678.0a,89.9b)--(751.0a,89.9b);
draw (404.0a,475.5b)--(503.0a,454.2b)--(601.9a,442.6b)--(700.9a,436.9b)--(799.9a,436.5b)
--(898.8a,445.0b);
gpdraw(9,404.0a,475.5b);
gpdraw(9,503.0a,454.2b);
gpdraw(9,601.9a,442.6b);
gpdraw(9,700.9a,436.9b);
gpdraw(9,799.9a,436.5b);
gpdraw(9,898.8a,445.0b);
gpdraw(9,714.5a,89.9b);
linetype 2;
linetype -2;
put_text( btex Real Space Non-bond Reduction etex, 765.1a, 60.6b, 0, 1);
linetype 2;
draw (678.0a,60.6b)--(751.0a,60.6b);
draw (404.0a,553.1b)--(503.0a,531.9b)--(601.9a,507.4b)--(700.9a,493.2b)--(799.9a,479.2b)
--(898.8a,478.6b);
gpdraw(0,404.0a,553.1b);
gpdraw(0,503.0a,531.9b);
gpdraw(0,601.9a,507.4b);
gpdraw(0,700.9a,493.2b);
gpdraw(0,799.9a,479.2b);
gpdraw(0,898.8a,478.6b);
gpdraw(0,714.5a,60.6b);
linetype -2;
draw (170.8a,691.8b)--(170.8a,293.0b)--(1157.1a,293.0b)--(1157.1a,691.8b)--(170.8a,691.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
