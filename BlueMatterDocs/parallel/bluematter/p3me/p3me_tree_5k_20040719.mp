%GNUPLOT Metapost output: Thu Jul 22 00:30:37 2004


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

beginfig(16);
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
linetype 5;
linetype -2;
put_text( btex Total Time etex, 1365.8a, 865.7b, 0, 3);
linetype 5;
draw (1383.5a,865.7b)--(1470.9a,865.7b);
draw (1383.5a,874.0b)--(1383.5a,857.4b);
draw (1470.9a,874.0b)--(1470.9a,857.4b);
draw (431.8a,789.4b)--(431.8a,811.0b);
draw (423.5a,789.4b)--(440.1a,789.4b);
draw (423.5a,811.0b)--(440.1a,811.0b);
draw (691.1a,707.1b)--(691.1a,730.2b);
draw (682.8a,707.1b)--(699.4a,707.1b);
draw (682.8a,730.2b)--(699.4a,730.2b);
draw (950.4a,635.9b)--(950.4a,663.8b);
draw (942.1a,635.9b)--(958.7a,635.9b);
draw (942.1a,663.8b)--(958.7a,663.8b);
draw (1080.0a,623.3b)--(1080.0a,653.0b);
draw (1071.7a,623.3b)--(1088.3a,623.3b);
draw (1071.7a,653.0b)--(1088.3a,653.0b);
gpdraw(5,431.8a,795.2b);
gpdraw(5,691.1a,713.1b);
gpdraw(5,950.4a,642.5b);
gpdraw(5,1080.0a,630.7b);
gpdraw(5,1427.2a,865.7b);
linetype 2;
linetype -2;
put_text( btex Non-bond (RS) etex, 1365.8a, 829.1b, 0, 3);
linetype 2;
draw (1383.5a,829.1b)--(1470.9a,829.1b);
draw (1383.5a,837.4b)--(1383.5a,820.8b);
draw (1470.9a,837.4b)--(1470.9a,820.8b);
draw (431.8a,762.2b)--(431.8a,793.8b);
draw (423.5a,762.2b)--(440.1a,762.2b);
draw (423.5a,793.8b)--(440.1a,793.8b);
draw (691.1a,663.4b)--(691.1a,706.2b);
draw (682.8a,663.4b)--(699.4a,663.4b);
draw (682.8a,706.2b)--(699.4a,706.2b);
draw (950.4a,562.6b)--(950.4a,631.9b);
draw (942.1a,562.6b)--(958.7a,562.6b);
draw (942.1a,631.9b)--(958.7a,631.9b);
draw (1080.0a,508.1b)--(1080.0a,620.8b);
draw (1071.7a,508.1b)--(1088.3a,508.1b);
draw (1071.7a,620.8b)--(1088.3a,620.8b);
gpdraw(2,431.8a,771.7b);
gpdraw(2,691.1a,677.9b);
gpdraw(2,950.4a,587.2b);
gpdraw(2,1080.0a,543.4b);
gpdraw(2,1427.2a,829.1b);
linetype 1;
linetype -2;
put_text( btex Non-bond (KS) etex, 1365.8a, 792.5b, 0, 3);
linetype 1;
draw (1383.5a,792.5b)--(1470.9a,792.5b);
draw (1383.5a,800.8b)--(1383.5a,784.2b);
draw (1470.9a,800.8b)--(1470.9a,784.2b);
draw (431.8a,697.2b)--(431.8a,712.1b);
draw (423.5a,697.2b)--(440.1a,697.2b);
draw (423.5a,712.1b)--(440.1a,712.1b);
draw (691.1a,615.4b)--(691.1a,660.5b);
draw (682.8a,615.4b)--(699.4a,615.4b);
draw (682.8a,660.5b)--(699.4a,660.5b);
draw (950.4a,541.2b)--(950.4a,592.1b);
draw (942.1a,541.2b)--(958.7a,541.2b);
draw (942.1a,592.1b)--(958.7a,592.1b);
draw (1080.0a,515.1b)--(1080.0a,587.4b);
draw (1071.7a,515.1b)--(1088.3a,515.1b);
draw (1071.7a,587.4b)--(1088.3a,587.4b);
gpdraw(1,431.8a,700.8b);
gpdraw(1,691.1a,630.5b);
gpdraw(1,950.4a,556.8b);
gpdraw(1,1080.0a,569.2b);
gpdraw(1,1427.2a,792.5b);
linetype 3;
linetype -2;
put_text( btex ReduceForces etex, 1365.8a, 755.9b, 0, 3);
linetype 3;
draw (1383.5a,755.9b)--(1470.9a,755.9b);
draw (1383.5a,764.2b)--(1383.5a,747.6b);
draw (1470.9a,764.2b)--(1470.9a,747.6b);
draw (431.8a,503.6b)--(431.8a,521.7b);
draw (423.5a,503.6b)--(440.1a,503.6b);
draw (423.5a,521.7b)--(440.1a,521.7b);
draw (691.1a,513.8b)--(691.1a,525.1b);
draw (682.8a,513.8b)--(699.4a,513.8b);
draw (682.8a,525.1b)--(699.4a,525.1b);
draw (950.4a,515.6b)--(950.4a,525.2b);
draw (942.1a,515.6b)--(958.7a,515.6b);
draw (942.1a,525.2b)--(958.7a,525.2b);
draw (1080.0a,514.6b)--(1080.0a,526.4b);
draw (1071.7a,514.6b)--(1088.3a,514.6b);
draw (1071.7a,526.4b)--(1088.3a,526.4b);
gpdraw(1,431.8a,511.5b);
gpdraw(1,691.1a,520.3b);
gpdraw(1,950.4a,520.9b);
gpdraw(1,1080.0a,520.5b);
gpdraw(1,1427.2a,755.9b);
linetype 0;
linetype -2;
put_text( btex GlobalizePositions etex, 1365.8a, 719.3b, 0, 3);
linetype 0;
draw (1383.5a,719.3b)--(1470.9a,719.3b);
draw (1383.5a,727.6b)--(1383.5a,711.0b);
draw (1470.9a,727.6b)--(1470.9a,711.0b);
draw (431.8a,411.6b)--(431.8a,432.0b);
draw (423.5a,411.6b)--(440.1a,411.6b);
draw (423.5a,432.0b)--(440.1a,432.0b);
draw (691.1a,412.0b)--(691.1a,438.0b);
draw (682.8a,412.0b)--(699.4a,412.0b);
draw (682.8a,438.0b)--(699.4a,438.0b);
draw (950.4a,412.1b)--(950.4a,438.0b);
draw (942.1a,412.1b)--(958.7a,412.1b);
draw (942.1a,438.0b)--(958.7a,438.0b);
draw (1080.0a,412.1b)--(1080.0a,441.0b);
draw (1071.7a,412.1b)--(1088.3a,412.1b);
draw (1071.7a,441.0b)--(1088.3a,441.0b);
gpdraw(0,431.8a,420.7b);
gpdraw(0,691.1a,421.6b);
gpdraw(0,950.4a,422.9b);
gpdraw(0,1080.0a,424.9b);
gpdraw(0,1427.2a,719.3b);
linetype 3;
linetype -2;
put_text( btex Rattle etex, 1365.8a, 682.7b, 0, 3);
linetype 3;
draw (1383.5a,682.7b)--(1470.9a,682.7b);
draw (1383.5a,691.0b)--(1383.5a,674.4b);
draw (1470.9a,691.0b)--(1470.9a,674.4b);
draw (431.8a,344.3b)--(431.8a,367.5b);
draw (423.5a,344.3b)--(440.1a,344.3b);
draw (423.5a,367.5b)--(440.1a,367.5b);
draw (691.1a,270.0b)--(691.1a,308.3b);
draw (682.8a,270.0b)--(699.4a,270.0b);
draw (682.8a,308.3b)--(699.4a,308.3b);
draw (950.4a,228.1b)--(950.4a,277.0b);
draw (942.1a,228.1b)--(958.7a,228.1b);
draw (942.1a,277.0b)--(958.7a,277.0b);
draw (1080.0a,206.7b)--(1080.0a,271.3b);
draw (1071.7a,206.7b)--(1088.3a,206.7b);
draw (1071.7a,271.3b)--(1088.3a,271.3b);
gpdraw(3,431.8a,356.4b);
gpdraw(3,691.1a,283.5b);
gpdraw(3,950.4a,241.0b);
gpdraw(3,1080.0a,227.6b);
gpdraw(3,1427.2a,682.7b);
linetype 4;
linetype -2;
put_text( btex Shake etex, 1365.8a, 646.1b, 0, 3);
linetype 4;
draw (1383.5a,646.1b)--(1470.9a,646.1b);
draw (1383.5a,654.4b)--(1383.5a,637.8b);
draw (1470.9a,654.4b)--(1470.9a,637.8b);
draw (431.8a,398.5b)--(431.8a,405.8b);
draw (423.5a,398.5b)--(440.1a,398.5b);
draw (423.5a,405.8b)--(440.1a,405.8b);
draw (691.1a,308.3b)--(691.1a,323.7b);
draw (682.8a,308.3b)--(699.4a,308.3b);
draw (682.8a,323.7b)--(699.4a,323.7b);
draw (950.4a,245.4b)--(950.4a,267.3b);
draw (942.1a,245.4b)--(958.7a,245.4b);
draw (942.1a,267.3b)--(958.7a,267.3b);
draw (1080.0a,206.7b)--(1080.0a,245.4b);
draw (1071.7a,206.7b)--(1088.3a,206.7b);
draw (1071.7a,245.4b)--(1088.3a,245.4b);
gpdraw(4,431.8a,401.7b);
gpdraw(4,691.1a,319.1b);
gpdraw(4,950.4a,260.2b);
gpdraw(4,1080.0a,239.9b);
gpdraw(4,1427.2a,646.1b);
linetype 1;
linetype -2;
put_text( btex WaterSPCRattle etex, 1365.8a, 609.5b, 0, 3);
linetype 1;
draw (1383.5a,609.5b)--(1470.9a,609.5b);
draw (1383.5a,617.8b)--(1383.5a,601.2b);
draw (1470.9a,617.8b)--(1470.9a,601.2b);
draw (431.8a,343.0b)--(431.8a,366.5b);
draw (423.5a,343.0b)--(440.1a,343.0b);
draw (423.5a,366.5b)--(440.1a,366.5b);
draw (691.1a,265.9b)--(691.1a,306.0b);
draw (682.8a,265.9b)--(699.4a,265.9b);
draw (682.8a,306.0b)--(699.4a,306.0b);
draw (950.4a,220.3b)--(950.4a,272.6b);
draw (942.1a,220.3b)--(958.7a,220.3b);
draw (942.1a,272.6b)--(958.7a,272.6b);
draw (1080.0a,195.8b)--(1080.0a,266.6b);
draw (1071.7a,195.8b)--(1088.3a,195.8b);
draw (1071.7a,266.6b)--(1088.3a,266.6b);
gpdraw(7,431.8a,355.2b);
gpdraw(7,691.1a,280.0b);
gpdraw(7,950.4a,234.1b);
gpdraw(7,1080.0a,219.5b);
gpdraw(7,1427.2a,609.5b);
linetype 2;
linetype -2;
put_text( btex WaterSPCShake etex, 1365.8a, 572.9b, 0, 3);
linetype 2;
draw (1383.5a,572.9b)--(1470.9a,572.9b);
draw (1383.5a,581.2b)--(1383.5a,564.6b);
draw (1470.9a,581.2b)--(1470.9a,564.6b);
draw (431.8a,397.8b)--(431.8a,405.2b);
draw (423.5a,397.8b)--(440.1a,397.8b);
draw (423.5a,405.2b)--(440.1a,405.2b);
draw (691.1a,305.6b)--(691.1a,321.8b);
draw (682.8a,305.6b)--(699.4a,305.6b);
draw (682.8a,321.8b)--(699.4a,321.8b);
draw (950.4a,238.4b)--(950.4a,262.2b);
draw (942.1a,238.4b)--(958.7a,238.4b);
draw (942.1a,262.2b)--(958.7a,262.2b);
draw (1080.0a,193.9b)--(1080.0a,238.4b);
draw (1071.7a,193.9b)--(1088.3a,193.9b);
draw (1071.7a,238.4b)--(1088.3a,238.4b);
gpdraw(8,431.8a,401.1b);
gpdraw(8,691.1a,316.9b);
gpdraw(8,950.4a,254.6b);
gpdraw(8,1080.0a,232.7b);
gpdraw(8,1427.2a,572.9b);
linetype 6;
linetype -2;
put_text( btex UpdatePosition etex, 1365.8a, 536.3b, 0, 3);
linetype 6;
draw (1383.5a,536.3b)--(1470.9a,536.3b);
draw (1383.5a,544.6b)--(1383.5a,528.0b);
draw (1470.9a,544.6b)--(1470.9a,528.0b);
draw (431.8a,324.3b)--(431.8a,327.2b);
draw (423.5a,324.3b)--(440.1a,324.3b);
draw (423.5a,327.2b)--(440.1a,327.2b);
draw (691.1a,289.6b)--(691.1a,292.5b);
draw (682.8a,289.6b)--(699.4a,289.6b);
draw (682.8a,292.5b)--(699.4a,292.5b);
draw (950.4a,278.2b)--(950.4a,281.1b);
draw (942.1a,278.2b)--(958.7a,278.2b);
draw (942.1a,281.1b)--(958.7a,281.1b);
draw (1080.0a,275.7b)--(1080.0a,278.2b);
draw (1071.7a,275.7b)--(1088.3a,275.7b);
draw (1071.7a,278.2b)--(1088.3a,278.2b);
gpdraw(4,431.8a,325.3b);
gpdraw(4,691.1a,291.5b);
gpdraw(4,950.4a,279.7b);
gpdraw(4,1080.0a,277.4b);
gpdraw(4,1427.2a,536.3b);
linetype 7;
linetype -2;
put_text( btex UpdateVelocity etex, 1365.8a, 499.7b, 0, 3);
linetype 7;
draw (1383.5a,499.7b)--(1470.9a,499.7b);
draw (1383.5a,508.0b)--(1383.5a,491.4b);
draw (1470.9a,508.0b)--(1470.9a,491.4b);
draw (431.8a,388.5b)--(431.8a,392.5b);
draw (423.5a,388.5b)--(440.1a,388.5b);
draw (423.5a,392.5b)--(440.1a,392.5b);
draw (691.1a,343.2b)--(691.1a,348.3b);
draw (682.8a,343.2b)--(699.4a,343.2b);
draw (682.8a,348.3b)--(699.4a,348.3b);
draw (950.4a,326.6b)--(950.4a,331.4b);
draw (942.1a,326.6b)--(958.7a,326.6b);
draw (942.1a,331.4b)--(958.7a,331.4b);
draw (1080.0a,323.4b)--(1080.0a,327.5b);
draw (1071.7a,323.4b)--(1088.3a,323.4b);
draw (1071.7a,327.5b)--(1088.3a,327.5b);
gpdraw(5,431.8a,389.9b);
gpdraw(5,691.1a,345.6b);
gpdraw(5,950.4a,329.0b);
gpdraw(5,1080.0a,325.6b);
gpdraw(5,1427.2a,499.7b);
linetype 5;
linetype -2;
put_text( btex Bonded Forces etex, 1365.8a, 463.1b, 0, 3);
linetype 5;
draw (1383.5a,463.1b)--(1470.9a,463.1b);
draw (1383.5a,471.4b)--(1383.5a,454.8b);
draw (1470.9a,471.4b)--(1470.9a,454.8b);
draw (431.8a,383.0b)--(431.8a,452.9b);
draw (423.5a,383.0b)--(440.1a,383.0b);
draw (423.5a,452.9b)--(440.1a,452.9b);
draw (691.1a,299.8b)--(691.1a,403.2b);
draw (682.8a,299.8b)--(699.4a,299.8b);
draw (682.8a,403.2b)--(699.4a,403.2b);
draw (950.4a,250.0b)--(950.4a,354.8b);
draw (942.1a,250.0b)--(958.7a,250.0b);
draw (942.1a,354.8b)--(958.7a,354.8b);
draw (1080.0a,240.5b)--(1080.0a,349.6b);
draw (1071.7a,240.5b)--(1088.3a,240.5b);
draw (1071.7a,349.6b)--(1088.3a,349.6b);
gpdraw(3,431.8a,421.3b);
gpdraw(3,691.1a,339.3b);
gpdraw(3,950.4a,279.6b);
gpdraw(3,1080.0a,259.5b);
gpdraw(3,1427.2a,463.1b);
linetype 0;
draw (431.8a,420.7b)--(691.1a,421.6b)--(950.4a,422.9b)--(1080.0a,424.9b);
linetype 1;
draw (431.8a,700.8b)--(691.1a,630.5b)--(950.4a,556.8b)--(1080.0a,569.2b);
linetype 2;
draw (431.8a,771.7b)--(691.1a,677.9b)--(950.4a,587.2b)--(1080.0a,543.4b);
linetype 4;
draw (431.8a,401.7b)--(691.1a,319.1b)--(950.4a,260.2b)--(1080.0a,239.9b);
linetype 5;
draw (431.8a,795.2b)--(691.1a,713.1b)--(950.4a,642.5b)--(1080.0a,630.7b);
linetype 3;
draw (431.8a,511.5b)--(691.1a,520.3b)--(950.4a,520.9b)--(1080.0a,520.5b);
linetype 5;
draw (431.8a,421.3b)--(691.1a,339.3b)--(950.4a,279.6b)--(1080.0a,259.5b);
linetype 6;
draw (431.8a,325.3b)--(691.1a,291.5b)--(950.4a,279.7b)--(1080.0a,277.4b);
linetype 7;
draw (431.8a,389.9b)--(691.1a,345.6b)--(950.4a,329.0b)--(1080.0a,325.6b);
linetype 1;
draw (431.8a,355.2b)--(691.1a,280.0b)--(950.4a,234.1b)--(1080.0a,219.5b);
linetype 2;
draw (431.8a,401.1b)--(691.1a,316.9b)--(950.4a,254.6b)--(1080.0a,232.7b);
linetype -2;
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
