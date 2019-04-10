%GNUPLOT Metapost output: Thu Jul 22 00:33:26 2004


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

beginfig(17);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  10000 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,157.4b)--(222.5a,157.4b);
draw (1506.3a,157.4b)--(1498.0a,157.4b);
draw (214.2a,220.3b)--(222.5a,220.3b);
draw (1506.3a,220.3b)--(1498.0a,220.3b);
draw (214.2a,252.6b)--(222.5a,252.6b);
draw (1506.3a,252.6b)--(1498.0a,252.6b);
draw (214.2a,268.0b)--(230.8a,268.0b);
draw (1506.3a,268.0b)--(1489.7a,268.0b);
put_text( btex  100000 etex, 196.5a, 268.0b, 0, 3);
draw (214.2a,315.6b)--(222.5a,315.6b);
draw (1506.3a,315.6b)--(1498.0a,315.6b);
draw (214.2a,378.5b)--(222.5a,378.5b);
draw (1506.3a,378.5b)--(1498.0a,378.5b);
draw (214.2a,410.8b)--(222.5a,410.8b);
draw (1506.3a,410.8b)--(1498.0a,410.8b);
draw (214.2a,426.1b)--(230.8a,426.1b);
draw (1506.3a,426.1b)--(1489.7a,426.1b);
put_text( btex  1e+006 etex, 196.5a, 426.1b, 0, 3);
draw (214.2a,473.7b)--(222.5a,473.7b);
draw (1506.3a,473.7b)--(1498.0a,473.7b);
draw (214.2a,536.7b)--(222.5a,536.7b);
draw (1506.3a,536.7b)--(1498.0a,536.7b);
draw (214.2a,569.0b)--(222.5a,569.0b);
draw (1506.3a,569.0b)--(1498.0a,569.0b);
draw (214.2a,584.3b)--(230.8a,584.3b);
draw (1506.3a,584.3b)--(1489.7a,584.3b);
put_text( btex  1e+007 etex, 196.5a, 584.3b, 0, 3);
draw (214.2a,631.9b)--(222.5a,631.9b);
draw (1506.3a,631.9b)--(1498.0a,631.9b);
draw (214.2a,694.8b)--(222.5a,694.8b);
draw (1506.3a,694.8b)--(1498.0a,694.8b);
draw (214.2a,727.1b)--(222.5a,727.1b);
draw (1506.3a,727.1b)--(1498.0a,727.1b);
draw (214.2a,742.4b)--(230.8a,742.4b);
draw (1506.3a,742.4b)--(1489.7a,742.4b);
put_text( btex  1e+008 etex, 196.5a, 742.4b, 0, 3);
draw (214.2a,790.1b)--(222.5a,790.1b);
draw (1506.3a,790.1b)--(1498.0a,790.1b);
draw (214.2a,853.0b)--(222.5a,853.0b);
draw (1506.3a,853.0b)--(1498.0a,853.0b);
draw (214.2a,885.3b)--(222.5a,885.3b);
draw (1506.3a,885.3b)--(1498.0a,885.3b);
draw (214.2a,900.6b)--(230.8a,900.6b);
draw (1506.3a,900.6b)--(1489.7a,900.6b);
put_text( btex  1e+009 etex, 196.5a, 900.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,900.6b)--(214.2a,884.0b);
put_text( btex  10 etex, 214.2a, 73.2b, 0, 2);
draw (343.9a,109.8b)--(343.9a,118.1b);
draw (343.9a,900.6b)--(343.9a,892.3b);
draw (419.7a,109.8b)--(419.7a,118.1b);
draw (419.7a,900.6b)--(419.7a,892.3b);
draw (473.5a,109.8b)--(473.5a,118.1b);
draw (473.5a,900.6b)--(473.5a,892.3b);
draw (515.2a,109.8b)--(515.2a,118.1b);
draw (515.2a,900.6b)--(515.2a,892.3b);
draw (549.3a,109.8b)--(549.3a,118.1b);
draw (549.3a,900.6b)--(549.3a,892.3b);
draw (578.2a,109.8b)--(578.2a,118.1b);
draw (578.2a,900.6b)--(578.2a,892.3b);
draw (603.2a,109.8b)--(603.2a,118.1b);
draw (603.2a,900.6b)--(603.2a,892.3b);
draw (625.2a,109.8b)--(625.2a,118.1b);
draw (625.2a,900.6b)--(625.2a,892.3b);
draw (644.9a,109.8b)--(644.9a,126.4b);
draw (644.9a,900.6b)--(644.9a,884.0b);
put_text( btex  100 etex, 644.9a, 73.2b, 0, 2);
draw (774.6a,109.8b)--(774.6a,118.1b);
draw (774.6a,900.6b)--(774.6a,892.3b);
draw (850.4a,109.8b)--(850.4a,118.1b);
draw (850.4a,900.6b)--(850.4a,892.3b);
draw (904.2a,109.8b)--(904.2a,118.1b);
draw (904.2a,900.6b)--(904.2a,892.3b);
draw (945.9a,109.8b)--(945.9a,118.1b);
draw (945.9a,900.6b)--(945.9a,892.3b);
draw (980.0a,109.8b)--(980.0a,118.1b);
draw (980.0a,900.6b)--(980.0a,892.3b);
draw (1008.9a,109.8b)--(1008.9a,118.1b);
draw (1008.9a,900.6b)--(1008.9a,892.3b);
draw (1033.9a,109.8b)--(1033.9a,118.1b);
draw (1033.9a,900.6b)--(1033.9a,892.3b);
draw (1055.9a,109.8b)--(1055.9a,118.1b);
draw (1055.9a,900.6b)--(1055.9a,892.3b);
draw (1075.6a,109.8b)--(1075.6a,126.4b);
draw (1075.6a,900.6b)--(1075.6a,884.0b);
put_text( btex  1000 etex, 1075.6a, 73.2b, 0, 2);
draw (1205.3a,109.8b)--(1205.3a,118.1b);
draw (1205.3a,900.6b)--(1205.3a,892.3b);
draw (1281.1a,109.8b)--(1281.1a,118.1b);
draw (1281.1a,900.6b)--(1281.1a,892.3b);
draw (1334.9a,109.8b)--(1334.9a,118.1b);
draw (1334.9a,900.6b)--(1334.9a,892.3b);
draw (1376.6a,109.8b)--(1376.6a,118.1b);
draw (1376.6a,900.6b)--(1376.6a,892.3b);
draw (1410.7a,109.8b)--(1410.7a,118.1b);
draw (1410.7a,900.6b)--(1410.7a,892.3b);
draw (1439.6a,109.8b)--(1439.6a,118.1b);
draw (1439.6a,900.6b)--(1439.6a,892.3b);
draw (1464.6a,109.8b)--(1464.6a,118.1b);
draw (1464.6a,900.6b)--(1464.6a,892.3b);
draw (1486.6a,109.8b)--(1486.6a,118.1b);
draw (1486.6a,900.6b)--(1486.6a,892.3b);
draw (1506.3a,109.8b)--(1506.3a,126.4b);
draw (1506.3a,900.6b)--(1506.3a,884.0b);
put_text( btex  10000 etex, 1506.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (nanoseconds) etex, 36.6a, 505.2b, 1, 2);
put_text( btex Node Count etex, 860.2a, 18.3b, 0, 2);
linetype 3;
linetype -2;
put_text( btex Total Time etex, 1365.8a, 865.7b, 0, 3);
linetype 3;
draw (1383.5a,865.7b)--(1470.9a,865.7b);
draw (1383.5a,874.0b)--(1383.5a,857.4b);
draw (1470.9a,874.0b)--(1470.9a,857.4b);
draw (431.8a,787.5b)--(431.8a,807.5b);
draw (423.5a,787.5b)--(440.1a,787.5b);
draw (423.5a,807.5b)--(440.1a,807.5b);
draw (691.1a,700.4b)--(691.1a,722.6b);
draw (682.8a,700.4b)--(699.4a,700.4b);
draw (682.8a,722.6b)--(699.4a,722.6b);
draw (950.4a,633.7b)--(950.4a,661.1b);
draw (942.1a,633.7b)--(958.7a,633.7b);
draw (942.1a,661.1b)--(958.7a,661.1b);
draw (1080.0a,625.7b)--(1080.0a,652.7b);
draw (1071.7a,625.7b)--(1088.3a,625.7b);
draw (1071.7a,652.7b)--(1088.3a,652.7b);
gpdraw(1,431.8a,793.1b);
gpdraw(1,691.1a,706.3b);
gpdraw(1,950.4a,640.5b);
gpdraw(1,1080.0a,632.5b);
gpdraw(1,1427.2a,865.7b);
linetype 0;
linetype -2;
put_text( btex Non-bond (RS) etex, 1365.8a, 829.1b, 0, 3);
linetype 0;
draw (1383.5a,829.1b)--(1470.9a,829.1b);
draw (1383.5a,837.4b)--(1383.5a,820.8b);
draw (1470.9a,837.4b)--(1470.9a,820.8b);
draw (431.8a,760.8b)--(431.8a,791.1b);
draw (423.5a,760.8b)--(440.1a,760.8b);
draw (423.5a,791.1b)--(440.1a,791.1b);
draw (691.1a,663.6b)--(691.1a,702.4b);
draw (682.8a,663.6b)--(699.4a,663.6b);
draw (682.8a,702.4b)--(699.4a,702.4b);
draw (950.4a,560.6b)--(950.4a,627.3b);
draw (942.1a,560.6b)--(958.7a,560.6b);
draw (942.1a,627.3b)--(958.7a,627.3b);
draw (1080.0a,505.5b)--(1080.0a,618.1b);
draw (1071.7a,505.5b)--(1088.3a,505.5b);
draw (1071.7a,618.1b)--(1088.3a,618.1b);
gpdraw(8,431.8a,770.8b);
gpdraw(8,691.1a,676.5b);
gpdraw(8,950.4a,585.3b);
gpdraw(8,1080.0a,541.7b);
gpdraw(8,1427.2a,829.1b);
linetype 7;
linetype -2;
put_text( btex Non-bond (KS) etex, 1365.8a, 792.5b, 0, 3);
linetype 7;
draw (1383.5a,792.5b)--(1470.9a,792.5b);
draw (1383.5a,800.8b)--(1383.5a,784.2b);
draw (1470.9a,800.8b)--(1470.9a,784.2b);
draw (431.8a,688.5b)--(431.8a,703.8b);
draw (423.5a,688.5b)--(440.1a,688.5b);
draw (423.5a,703.8b)--(440.1a,703.8b);
draw (691.1a,596.0b)--(691.1a,623.8b);
draw (682.8a,596.0b)--(699.4a,596.0b);
draw (682.8a,623.8b)--(699.4a,623.8b);
draw (950.4a,531.7b)--(950.4a,582.8b);
draw (942.1a,531.7b)--(958.7a,531.7b);
draw (942.1a,582.8b)--(958.7a,582.8b);
draw (1080.0a,522.7b)--(1080.0a,591.7b);
draw (1071.7a,522.7b)--(1088.3a,522.7b);
draw (1071.7a,591.7b)--(1088.3a,591.7b);
gpdraw(7,431.8a,693.8b);
gpdraw(7,691.1a,607.5b);
gpdraw(7,950.4a,550.0b);
gpdraw(7,1080.0a,573.2b);
gpdraw(7,1427.2a,792.5b);
linetype 4;
linetype -2;
put_text( btex ReduceForces etex, 1365.8a, 755.9b, 0, 3);
linetype 4;
draw (1383.5a,755.9b)--(1470.9a,755.9b);
draw (1383.5a,764.2b)--(1383.5a,747.6b);
draw (1470.9a,764.2b)--(1470.9a,747.6b);
draw (431.8a,504.8b)--(431.8a,535.2b);
draw (423.5a,504.8b)--(440.1a,504.8b);
draw (423.5a,535.2b)--(440.1a,535.2b);
draw (691.1a,512.6b)--(691.1a,525.3b);
draw (682.8a,512.6b)--(699.4a,512.6b);
draw (682.8a,525.3b)--(699.4a,525.3b);
draw (950.4a,514.6b)--(950.4a,528.2b);
draw (942.1a,514.6b)--(958.7a,514.6b);
draw (942.1a,528.2b)--(958.7a,528.2b);
draw (1080.0a,514.6b)--(1080.0a,524.3b);
draw (1071.7a,514.6b)--(1088.3a,514.6b);
draw (1071.7a,524.3b)--(1088.3a,524.3b);
gpdraw(2,431.8a,519.7b);
gpdraw(2,691.1a,519.1b);
gpdraw(2,950.4a,521.3b);
gpdraw(2,1080.0a,518.7b);
gpdraw(2,1427.2a,755.9b);
linetype 6;
linetype -2;
put_text( btex GlobalizePositions etex, 1365.8a, 719.3b, 0, 3);
linetype 6;
draw (1383.5a,719.3b)--(1470.9a,719.3b);
draw (1383.5a,727.6b)--(1383.5a,711.0b);
draw (1470.9a,727.6b)--(1470.9a,711.0b);
draw (431.8a,429.6b)--(431.8a,449.2b);
draw (423.5a,429.6b)--(440.1a,429.6b);
draw (423.5a,449.2b)--(440.1a,449.2b);
draw (691.1a,429.7b)--(691.1a,452.4b);
draw (682.8a,429.7b)--(699.4a,429.7b);
draw (682.8a,452.4b)--(699.4a,452.4b);
draw (950.4a,429.7b)--(950.4a,455.0b);
draw (942.1a,429.7b)--(958.7a,429.7b);
draw (942.1a,455.0b)--(958.7a,455.0b);
draw (1080.0a,429.7b)--(1080.0a,454.7b);
draw (1071.7a,429.7b)--(1088.3a,429.7b);
draw (1071.7a,454.7b)--(1088.3a,454.7b);
gpdraw(6,431.8a,436.5b);
gpdraw(6,691.1a,437.7b);
gpdraw(6,950.4a,438.9b);
gpdraw(6,1080.0a,440.3b);
gpdraw(6,1427.2a,719.3b);
linetype 1;
linetype -2;
put_text( btex Rattle etex, 1365.8a, 682.7b, 0, 3);
linetype 1;
draw (1383.5a,682.7b)--(1470.9a,682.7b);
draw (1383.5a,691.0b)--(1383.5a,674.4b);
draw (1470.9a,691.0b)--(1470.9a,674.4b);
draw (431.8a,347.4b)--(431.8a,367.2b);
draw (423.5a,347.4b)--(440.1a,347.4b);
draw (423.5a,367.2b)--(440.1a,367.2b);
draw (691.1a,269.3b)--(691.1a,308.3b);
draw (682.8a,269.3b)--(699.4a,269.3b);
draw (682.8a,308.3b)--(699.4a,308.3b);
draw (950.4a,225.6b)--(950.4a,275.1b);
draw (942.1a,225.6b)--(958.7a,225.6b);
draw (942.1a,275.1b)--(958.7a,275.1b);
draw (1080.0a,206.7b)--(1080.0a,270.7b);
draw (1071.7a,206.7b)--(1088.3a,206.7b);
draw (1071.7a,270.7b)--(1088.3a,270.7b);
gpdraw(9,431.8a,357.2b);
gpdraw(9,691.1a,283.2b);
gpdraw(9,950.4a,238.9b);
gpdraw(9,1080.0a,227.4b);
gpdraw(9,1427.2a,682.7b);
linetype 2;
linetype -2;
put_text( btex Shake etex, 1365.8a, 646.1b, 0, 3);
linetype 2;
draw (1383.5a,646.1b)--(1470.9a,646.1b);
draw (1383.5a,654.4b)--(1383.5a,637.8b);
draw (1470.9a,654.4b)--(1470.9a,637.8b);
draw (431.8a,397.1b)--(431.8a,405.3b);
draw (423.5a,397.1b)--(440.1a,397.1b);
draw (423.5a,405.3b)--(440.1a,405.3b);
draw (691.1a,307.6b)--(691.1a,323.7b);
draw (682.8a,307.6b)--(699.4a,307.6b);
draw (682.8a,323.7b)--(699.4a,323.7b);
draw (950.4a,238.4b)--(950.4a,265.9b);
draw (942.1a,238.4b)--(958.7a,238.4b);
draw (942.1a,265.9b)--(958.7a,265.9b);
draw (1080.0a,206.7b)--(1080.0a,246.3b);
draw (1071.7a,206.7b)--(1088.3a,206.7b);
draw (1071.7a,246.3b)--(1088.3a,246.3b);
gpdraw(0,431.8a,401.6b);
gpdraw(0,691.1a,318.9b);
gpdraw(0,950.4a,258.7b);
gpdraw(0,1080.0a,240.3b);
gpdraw(0,1427.2a,646.1b);
linetype -2;
put_text( btex WaterSPCRattle etex, 1365.8a, 609.5b, 0, 3);
linetype 2;
draw (1383.5a,609.5b)--(1470.9a,609.5b);
draw (1383.5a,617.8b)--(1383.5a,601.2b);
draw (1470.9a,617.8b)--(1470.9a,601.2b);
draw (431.8a,345.9b)--(431.8a,366.0b);
draw (423.5a,345.9b)--(440.1a,345.9b);
draw (423.5a,366.0b)--(440.1a,366.0b);
draw (691.1a,264.4b)--(691.1a,305.6b);
draw (682.8a,264.4b)--(699.4a,264.4b);
draw (682.8a,305.6b)--(699.4a,305.6b);
draw (950.4a,217.5b)--(950.4a,271.3b);
draw (942.1a,217.5b)--(958.7a,217.5b);
draw (942.1a,271.3b)--(958.7a,271.3b);
draw (1080.0a,193.9b)--(1080.0a,266.6b);
draw (1071.7a,193.9b)--(1088.3a,193.9b);
draw (1071.7a,266.6b)--(1088.3a,266.6b);
gpdraw(8,431.8a,356.0b);
gpdraw(8,691.1a,279.6b);
gpdraw(8,950.4a,232.0b);
gpdraw(8,1080.0a,219.2b);
gpdraw(8,1427.2a,609.5b);
linetype 3;
linetype -2;
put_text( btex WaterSPCShake etex, 1365.8a, 572.9b, 0, 3);
linetype 3;
draw (1383.5a,572.9b)--(1470.9a,572.9b);
draw (1383.5a,581.2b)--(1383.5a,564.6b);
draw (1470.9a,581.2b)--(1470.9a,564.6b);
draw (431.8a,396.3b)--(431.8a,404.5b);
draw (423.5a,396.3b)--(440.1a,396.3b);
draw (423.5a,404.5b)--(440.1a,404.5b);
draw (691.1a,304.8b)--(691.1a,321.5b);
draw (682.8a,304.8b)--(699.4a,304.8b);
draw (682.8a,321.5b)--(699.4a,321.5b);
draw (950.4a,231.7b)--(950.4a,260.7b);
draw (942.1a,231.7b)--(958.7a,231.7b);
draw (942.1a,260.7b)--(958.7a,260.7b);
draw (1080.0a,193.9b)--(1080.0a,239.4b);
draw (1071.7a,193.9b)--(1088.3a,193.9b);
draw (1071.7a,239.4b)--(1088.3a,239.4b);
gpdraw(9,431.8a,401.0b);
gpdraw(9,691.1a,316.7b);
gpdraw(9,950.4a,253.3b);
gpdraw(9,1080.0a,233.1b);
gpdraw(9,1427.2a,572.9b);
linetype 7;
linetype -2;
put_text( btex UpdatePosition etex, 1365.8a, 536.3b, 0, 3);
linetype 7;
draw (1383.5a,536.3b)--(1470.9a,536.3b);
draw (1383.5a,544.6b)--(1383.5a,528.0b);
draw (1470.9a,544.6b)--(1470.9a,528.0b);
draw (431.8a,325.2b)--(431.8a,327.8b);
draw (423.5a,325.2b)--(440.1a,325.2b);
draw (423.5a,327.8b)--(440.1a,327.8b);
draw (691.1a,291.1b)--(691.1a,293.5b);
draw (682.8a,291.1b)--(699.4a,291.1b);
draw (682.8a,293.5b)--(699.4a,293.5b);
draw (950.4a,278.7b)--(950.4a,282.2b);
draw (942.1a,278.7b)--(958.7a,278.7b);
draw (942.1a,282.2b)--(958.7a,282.2b);
draw (1080.0a,277.0b)--(1080.0a,279.3b);
draw (1071.7a,277.0b)--(1088.3a,277.0b);
draw (1071.7a,279.3b)--(1088.3a,279.3b);
gpdraw(5,431.8a,326.6b);
gpdraw(5,691.1a,292.7b);
gpdraw(5,950.4a,280.8b);
gpdraw(5,1080.0a,278.6b);
gpdraw(5,1427.2a,536.3b);
linetype 0;
linetype -2;
put_text( btex UpdateVelocity etex, 1365.8a, 499.7b, 0, 3);
linetype 0;
draw (1383.5a,499.7b)--(1470.9a,499.7b);
draw (1383.5a,508.0b)--(1383.5a,491.4b);
draw (1470.9a,508.0b)--(1470.9a,491.4b);
draw (431.8a,389.4b)--(431.8a,393.5b);
draw (423.5a,389.4b)--(440.1a,389.4b);
draw (423.5a,393.5b)--(440.1a,393.5b);
draw (691.1a,344.6b)--(691.1a,349.1b);
draw (682.8a,344.6b)--(699.4a,344.6b);
draw (682.8a,349.1b)--(699.4a,349.1b);
draw (950.4a,327.5b)--(950.4a,332.5b);
draw (942.1a,327.5b)--(958.7a,327.5b);
draw (942.1a,332.5b)--(958.7a,332.5b);
draw (1080.0a,324.6b)--(1080.0a,328.7b);
draw (1071.7a,324.6b)--(1088.3a,324.6b);
draw (1071.7a,328.7b)--(1088.3a,328.7b);
gpdraw(6,431.8a,391.4b);
gpdraw(6,691.1a,346.7b);
gpdraw(6,950.4a,330.2b);
gpdraw(6,1080.0a,327.0b);
gpdraw(6,1427.2a,499.7b);
linetype 6;
linetype -2;
put_text( btex Bonded Forces etex, 1365.8a, 463.1b, 0, 3);
linetype 6;
draw (1383.5a,463.1b)--(1470.9a,463.1b);
draw (1383.5a,471.4b)--(1383.5a,454.8b);
draw (1470.9a,471.4b)--(1470.9a,454.8b);
draw (431.8a,388.7b)--(431.8a,447.9b);
draw (423.5a,388.7b)--(440.1a,388.7b);
draw (423.5a,447.9b)--(440.1a,447.9b);
draw (691.1a,300.2b)--(691.1a,389.2b);
draw (682.8a,300.2b)--(699.4a,300.2b);
draw (682.8a,389.2b)--(699.4a,389.2b);
draw (950.4a,248.2b)--(950.4a,352.6b);
draw (942.1a,248.2b)--(958.7a,248.2b);
draw (942.1a,352.6b)--(958.7a,352.6b);
draw (1080.0a,241.5b)--(1080.0a,348.5b);
draw (1071.7a,241.5b)--(1088.3a,241.5b);
draw (1071.7a,348.5b)--(1088.3a,348.5b);
gpdraw(4,431.8a,420.5b);
gpdraw(4,691.1a,338.8b);
gpdraw(4,950.4a,278.2b);
gpdraw(4,1080.0a,259.9b);
gpdraw(4,1427.2a,463.1b);
draw (431.8a,436.5b)--(691.1a,437.7b)--(950.4a,438.9b)--(1080.0a,440.3b);
linetype 7;
draw (431.8a,693.8b)--(691.1a,607.5b)--(950.4a,550.0b)--(1080.0a,573.2b);
linetype 0;
draw (431.8a,770.8b)--(691.1a,676.5b)--(950.4a,585.3b)--(1080.0a,541.7b);
linetype 1;
draw (431.8a,357.2b)--(691.1a,283.2b)--(950.4a,238.9b)--(1080.0a,227.4b);
linetype 2;
draw (431.8a,401.6b)--(691.1a,318.9b)--(950.4a,258.7b)--(1080.0a,240.3b);
linetype 3;
draw (431.8a,793.1b)--(691.1a,706.3b)--(950.4a,640.5b)--(1080.0a,632.5b);
linetype 4;
draw (431.8a,519.7b)--(691.1a,519.1b)--(950.4a,521.3b)--(1080.0a,518.7b);
linetype 6;
draw (431.8a,420.5b)--(691.1a,338.8b)--(950.4a,278.2b)--(1080.0a,259.9b);
linetype 7;
draw (431.8a,326.6b)--(691.1a,292.7b)--(950.4a,280.8b)--(1080.0a,278.6b);
linetype 0;
draw (431.8a,391.4b)--(691.1a,346.7b)--(950.4a,330.2b)--(1080.0a,327.0b);
linetype 3;
draw (431.8a,401.0b)--(691.1a,316.7b)--(950.4a,253.3b)--(1080.0a,233.1b);
linetype -2;
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
