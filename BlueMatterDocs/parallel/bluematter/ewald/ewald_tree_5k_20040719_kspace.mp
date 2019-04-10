%GNUPLOT Metapost output: Thu Jul 22 00:45:19 2004


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

beginfig(18);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  10000 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,169.3b)--(222.5a,169.3b);
draw (1506.3a,169.3b)--(1498.0a,169.3b);
draw (214.2a,204.1b)--(222.5a,204.1b);
draw (1506.3a,204.1b)--(1498.0a,204.1b);
draw (214.2a,228.8b)--(222.5a,228.8b);
draw (1506.3a,228.8b)--(1498.0a,228.8b);
draw (214.2a,248.0b)--(222.5a,248.0b);
draw (1506.3a,248.0b)--(1498.0a,248.0b);
draw (214.2a,263.6b)--(222.5a,263.6b);
draw (1506.3a,263.6b)--(1498.0a,263.6b);
draw (214.2a,276.9b)--(222.5a,276.9b);
draw (1506.3a,276.9b)--(1498.0a,276.9b);
draw (214.2a,288.3b)--(222.5a,288.3b);
draw (1506.3a,288.3b)--(1498.0a,288.3b);
draw (214.2a,298.5b)--(222.5a,298.5b);
draw (1506.3a,298.5b)--(1498.0a,298.5b);
draw (214.2a,307.5b)--(230.8a,307.5b);
draw (1506.3a,307.5b)--(1489.7a,307.5b);
put_text( btex  100000 etex, 196.5a, 307.5b, 0, 3);
draw (214.2a,367.0b)--(222.5a,367.0b);
draw (1506.3a,367.0b)--(1498.0a,367.0b);
draw (214.2a,401.8b)--(222.5a,401.8b);
draw (1506.3a,401.8b)--(1498.0a,401.8b);
draw (214.2a,426.5b)--(222.5a,426.5b);
draw (1506.3a,426.5b)--(1498.0a,426.5b);
draw (214.2a,445.7b)--(222.5a,445.7b);
draw (1506.3a,445.7b)--(1498.0a,445.7b);
draw (214.2a,461.3b)--(222.5a,461.3b);
draw (1506.3a,461.3b)--(1498.0a,461.3b);
draw (214.2a,474.6b)--(222.5a,474.6b);
draw (1506.3a,474.6b)--(1498.0a,474.6b);
draw (214.2a,486.0b)--(222.5a,486.0b);
draw (1506.3a,486.0b)--(1498.0a,486.0b);
draw (214.2a,496.2b)--(222.5a,496.2b);
draw (1506.3a,496.2b)--(1498.0a,496.2b);
draw (214.2a,505.2b)--(230.8a,505.2b);
draw (1506.3a,505.2b)--(1489.7a,505.2b);
put_text( btex  1e+006 etex, 196.5a, 505.2b, 0, 3);
draw (214.2a,564.7b)--(222.5a,564.7b);
draw (1506.3a,564.7b)--(1498.0a,564.7b);
draw (214.2a,599.5b)--(222.5a,599.5b);
draw (1506.3a,599.5b)--(1498.0a,599.5b);
draw (214.2a,624.2b)--(222.5a,624.2b);
draw (1506.3a,624.2b)--(1498.0a,624.2b);
draw (214.2a,643.4b)--(222.5a,643.4b);
draw (1506.3a,643.4b)--(1498.0a,643.4b);
draw (214.2a,659.0b)--(222.5a,659.0b);
draw (1506.3a,659.0b)--(1498.0a,659.0b);
draw (214.2a,672.3b)--(222.5a,672.3b);
draw (1506.3a,672.3b)--(1498.0a,672.3b);
draw (214.2a,683.7b)--(222.5a,683.7b);
draw (1506.3a,683.7b)--(1498.0a,683.7b);
draw (214.2a,693.9b)--(222.5a,693.9b);
draw (1506.3a,693.9b)--(1498.0a,693.9b);
draw (214.2a,702.9b)--(230.8a,702.9b);
draw (1506.3a,702.9b)--(1489.7a,702.9b);
put_text( btex  1e+007 etex, 196.5a, 702.9b, 0, 3);
draw (214.2a,762.4b)--(222.5a,762.4b);
draw (1506.3a,762.4b)--(1498.0a,762.4b);
draw (214.2a,797.2b)--(222.5a,797.2b);
draw (1506.3a,797.2b)--(1498.0a,797.2b);
draw (214.2a,821.9b)--(222.5a,821.9b);
draw (1506.3a,821.9b)--(1498.0a,821.9b);
draw (214.2a,841.1b)--(222.5a,841.1b);
draw (1506.3a,841.1b)--(1498.0a,841.1b);
draw (214.2a,856.7b)--(222.5a,856.7b);
draw (1506.3a,856.7b)--(1498.0a,856.7b);
draw (214.2a,870.0b)--(222.5a,870.0b);
draw (1506.3a,870.0b)--(1498.0a,870.0b);
draw (214.2a,881.4b)--(222.5a,881.4b);
draw (1506.3a,881.4b)--(1498.0a,881.4b);
draw (214.2a,891.6b)--(222.5a,891.6b);
draw (1506.3a,891.6b)--(1498.0a,891.6b);
draw (214.2a,900.6b)--(230.8a,900.6b);
draw (1506.3a,900.6b)--(1489.7a,900.6b);
put_text( btex  1e+008 etex, 196.5a, 900.6b, 0, 3);
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
linetype 7;
linetype -2;
put_text( btex Non-bond (KS) etex, 1365.8a, 865.7b, 0, 3);
linetype 7;
draw (1383.5a,865.7b)--(1470.9a,865.7b);
draw (1383.5a,874.0b)--(1383.5a,857.4b);
draw (1470.9a,874.0b)--(1470.9a,857.4b);
draw (431.8a,833.2b)--(431.8a,852.4b);
draw (423.5a,833.2b)--(440.1a,833.2b);
draw (423.5a,852.4b)--(440.1a,852.4b);
draw (691.1a,717.6b)--(691.1a,752.2b);
draw (682.8a,717.6b)--(699.4a,717.6b);
draw (682.8a,752.2b)--(699.4a,752.2b);
draw (950.4a,637.1b)--(950.4a,701.0b);
draw (942.1a,637.1b)--(958.7a,637.1b);
draw (942.1a,701.0b)--(958.7a,701.0b);
draw (1080.0a,625.9b)--(1080.0a,712.1b);
draw (1071.7a,625.9b)--(1088.3a,625.9b);
draw (1071.7a,712.1b)--(1088.3a,712.1b);
gpdraw(7,431.8a,839.8b);
gpdraw(7,691.1a,732.0b);
gpdraw(7,950.4a,660.0b);
gpdraw(7,1080.0a,689.0b);
gpdraw(7,1427.2a,865.7b);
linetype 0;
linetype -2;
put_text( btex AllReduce $S(k)$ etex, 1365.8a, 829.1b, 0, 3);
linetype 0;
draw (1383.5a,829.1b)--(1470.9a,829.1b);
draw (1383.5a,837.4b)--(1383.5a,820.8b);
draw (1470.9a,837.4b)--(1470.9a,820.8b);
draw (431.8a,519.3b)--(431.8a,718.2b);
draw (423.5a,519.3b)--(440.1a,519.3b);
draw (423.5a,718.2b)--(440.1a,718.2b);
draw (691.1a,531.0b)--(691.1a,673.4b);
draw (682.8a,531.0b)--(699.4a,531.0b);
draw (682.8a,673.4b)--(699.4a,673.4b);
draw (950.4a,534.5b)--(950.4a,668.3b);
draw (942.1a,534.5b)--(958.7a,534.5b);
draw (942.1a,668.3b)--(958.7a,668.3b);
draw (1080.0a,560.9b)--(1080.0a,695.4b);
draw (1071.7a,560.9b)--(1088.3a,560.9b);
draw (1071.7a,695.4b)--(1088.3a,695.4b);
gpdraw(0,431.8a,629.7b);
gpdraw(0,691.1a,610.0b);
gpdraw(0,950.4a,597.4b);
gpdraw(0,1080.0a,666.1b);
gpdraw(0,1427.2a,829.1b);
linetype 2;
linetype -2;
put_text( btex Compute Forces etex, 1365.8a, 792.5b, 0, 3);
linetype 2;
draw (1383.5a,792.5b)--(1470.9a,792.5b);
draw (1383.5a,800.8b)--(1383.5a,784.2b);
draw (1470.9a,800.8b)--(1470.9a,784.2b);
draw (431.8a,731.1b)--(431.8a,739.0b);
draw (423.5a,731.1b)--(440.1a,731.1b);
draw (423.5a,739.0b)--(440.1a,739.0b);
draw (691.1a,610.8b)--(691.1a,613.9b);
draw (682.8a,610.8b)--(699.4a,610.8b);
draw (682.8a,613.9b)--(699.4a,613.9b);
draw (950.4a,496.0b)--(950.4a,507.7b);
draw (942.1a,496.0b)--(958.7a,496.0b);
draw (942.1a,507.7b)--(958.7a,507.7b);
draw (1080.0a,456.6b)--(1080.0a,464.3b);
draw (1071.7a,456.6b)--(1088.3a,456.6b);
draw (1071.7a,464.3b)--(1088.3a,464.3b);
gpdraw(2,431.8a,735.7b);
gpdraw(2,691.1a,613.2b);
gpdraw(2,950.4a,499.6b);
gpdraw(2,1080.0a,458.1b);
gpdraw(2,1427.2a,792.5b);
linetype 4;
linetype -2;
put_text( btex LocalReduce $S(k)$ etex, 1365.8a, 755.9b, 0, 3);
linetype 4;
draw (1383.5a,755.9b)--(1470.9a,755.9b);
draw (1383.5a,764.2b)--(1383.5a,747.6b);
draw (1470.9a,764.2b)--(1470.9a,747.6b);
draw (431.8a,719.8b)--(431.8a,720.6b);
draw (423.5a,719.8b)--(440.1a,719.8b);
draw (423.5a,720.6b)--(440.1a,720.6b);
draw (691.1a,605.2b)--(691.1a,607.6b);
draw (682.8a,605.2b)--(699.4a,605.2b);
draw (682.8a,607.6b)--(699.4a,607.6b);
draw (950.4a,507.7b)--(950.4a,514.6b);
draw (942.1a,507.7b)--(958.7a,507.7b);
draw (942.1a,514.6b)--(958.7a,514.6b);
draw (1080.0a,470.4b)--(1080.0a,479.6b);
draw (1071.7a,470.4b)--(1088.3a,470.4b);
draw (1071.7a,479.6b)--(1088.3a,479.6b);
gpdraw(4,431.8a,720.2b);
gpdraw(4,691.1a,607.3b);
gpdraw(4,950.4a,510.0b);
gpdraw(4,1080.0a,471.6b);
gpdraw(4,1427.2a,755.9b);
linetype 1;
linetype -2;
put_text( btex Exp. Factors etex, 1365.8a, 719.3b, 0, 3);
linetype 1;
draw (1383.5a,719.3b)--(1470.9a,719.3b);
draw (1383.5a,727.6b)--(1383.5a,711.0b);
draw (1470.9a,727.6b)--(1470.9a,711.0b);
draw (431.8a,385.5b)--(431.8a,412.1b);
draw (423.5a,385.5b)--(440.1a,385.5b);
draw (423.5a,412.1b)--(440.1a,412.1b);
draw (691.1a,265.1b)--(691.1a,308.4b);
draw (682.8a,265.1b)--(699.4a,265.1b);
draw (682.8a,308.4b)--(699.4a,308.4b);
draw (950.4a,155.4b)--(950.4a,224.4b);
draw (942.1a,155.4b)--(958.7a,155.4b);
draw (942.1a,224.4b)--(958.7a,224.4b);
draw (1080.0a,118.0b)--(1080.0a,195.1b);
draw (1071.7a,118.0b)--(1088.3a,118.0b);
draw (1071.7a,195.1b)--(1088.3a,195.1b);
gpdraw(1,431.8a,401.8b);
gpdraw(1,691.1a,290.2b);
gpdraw(1,950.4a,191.5b);
gpdraw(1,1080.0a,152.9b);
gpdraw(1,1427.2a,719.3b);
linetype 0;
draw (431.8a,629.7b)--(691.1a,610.0b)--(950.4a,597.4b)--(1080.0a,666.1b);
linetype 1;
draw (431.8a,401.8b)--(691.1a,290.2b)--(950.4a,191.5b)--(1080.0a,152.9b);
linetype 2;
draw (431.8a,735.7b)--(691.1a,613.2b)--(950.4a,499.6b)--(1080.0a,458.1b);
linetype 4;
draw (431.8a,720.2b)--(691.1a,607.3b)--(950.4a,510.0b)--(1080.0a,471.6b);
linetype 7;
draw (431.8a,839.8b)--(691.1a,732.0b)--(950.4a,660.0b)--(1080.0a,689.0b);
linetype -2;
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
