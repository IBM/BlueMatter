%GNUPLOT Metapost output: Tue Jul 20 20:09:08 2004


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

beginfig(14);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  100000 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,189.2b)--(222.5a,189.2b);
draw (1506.3a,189.2b)--(1498.0a,189.2b);
draw (214.2a,235.6b)--(222.5a,235.6b);
draw (1506.3a,235.6b)--(1498.0a,235.6b);
draw (214.2a,268.5b)--(222.5a,268.5b);
draw (1506.3a,268.5b)--(1498.0a,268.5b);
draw (214.2a,294.0b)--(222.5a,294.0b);
draw (1506.3a,294.0b)--(1498.0a,294.0b);
draw (214.2a,314.9b)--(222.5a,314.9b);
draw (1506.3a,314.9b)--(1498.0a,314.9b);
draw (214.2a,332.6b)--(222.5a,332.6b);
draw (1506.3a,332.6b)--(1498.0a,332.6b);
draw (214.2a,347.9b)--(222.5a,347.9b);
draw (1506.3a,347.9b)--(1498.0a,347.9b);
draw (214.2a,361.3b)--(222.5a,361.3b);
draw (1506.3a,361.3b)--(1498.0a,361.3b);
draw (214.2a,373.4b)--(230.8a,373.4b);
draw (1506.3a,373.4b)--(1489.7a,373.4b);
put_text( btex  1e+006 etex, 196.5a, 373.4b, 0, 3);
draw (214.2a,452.8b)--(222.5a,452.8b);
draw (1506.3a,452.8b)--(1498.0a,452.8b);
draw (214.2a,499.2b)--(222.5a,499.2b);
draw (1506.3a,499.2b)--(1498.0a,499.2b);
draw (214.2a,532.1b)--(222.5a,532.1b);
draw (1506.3a,532.1b)--(1498.0a,532.1b);
draw (214.2a,557.6b)--(222.5a,557.6b);
draw (1506.3a,557.6b)--(1498.0a,557.6b);
draw (214.2a,578.5b)--(222.5a,578.5b);
draw (1506.3a,578.5b)--(1498.0a,578.5b);
draw (214.2a,596.2b)--(222.5a,596.2b);
draw (1506.3a,596.2b)--(1498.0a,596.2b);
draw (214.2a,611.5b)--(222.5a,611.5b);
draw (1506.3a,611.5b)--(1498.0a,611.5b);
draw (214.2a,624.9b)--(222.5a,624.9b);
draw (1506.3a,624.9b)--(1498.0a,624.9b);
draw (214.2a,637.0b)--(230.8a,637.0b);
draw (1506.3a,637.0b)--(1489.7a,637.0b);
put_text( btex  1e+007 etex, 196.5a, 637.0b, 0, 3);
draw (214.2a,716.4b)--(222.5a,716.4b);
draw (1506.3a,716.4b)--(1498.0a,716.4b);
draw (214.2a,762.8b)--(222.5a,762.8b);
draw (1506.3a,762.8b)--(1498.0a,762.8b);
draw (214.2a,795.7b)--(222.5a,795.7b);
draw (1506.3a,795.7b)--(1498.0a,795.7b);
draw (214.2a,821.2b)--(222.5a,821.2b);
draw (1506.3a,821.2b)--(1498.0a,821.2b);
draw (214.2a,842.1b)--(222.5a,842.1b);
draw (1506.3a,842.1b)--(1498.0a,842.1b);
draw (214.2a,859.8b)--(222.5a,859.8b);
draw (1506.3a,859.8b)--(1498.0a,859.8b);
draw (214.2a,875.1b)--(222.5a,875.1b);
draw (1506.3a,875.1b)--(1498.0a,875.1b);
draw (214.2a,888.5b)--(222.5a,888.5b);
draw (1506.3a,888.5b)--(1498.0a,888.5b);
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
linetype 1;
linetype -2;
put_text( btex Non-bond (KS) etex, 1365.8a, 865.7b, 0, 3);
linetype 1;
draw (1383.5a,865.7b)--(1470.9a,865.7b);
draw (1383.5a,874.0b)--(1383.5a,857.4b);
draw (1470.9a,874.0b)--(1470.9a,857.4b);
draw (431.8a,825.1b)--(431.8a,850.1b);
draw (423.5a,825.1b)--(440.1a,825.1b);
draw (423.5a,850.1b)--(440.1a,850.1b);
draw (691.1a,688.8b)--(691.1a,764.0b);
draw (682.8a,688.8b)--(699.4a,688.8b);
draw (682.8a,764.0b)--(699.4a,764.0b);
draw (950.4a,565.2b)--(950.4a,650.0b);
draw (942.1a,565.2b)--(958.7a,565.2b);
draw (942.1a,650.0b)--(958.7a,650.0b);
draw (1080.0a,521.7b)--(1080.0a,642.1b);
draw (1071.7a,521.7b)--(1088.3a,521.7b);
draw (1071.7a,642.1b)--(1088.3a,642.1b);
gpdraw(1,431.8a,831.2b);
gpdraw(1,691.1a,714.1b);
gpdraw(1,950.4a,591.3b);
gpdraw(1,1080.0a,611.9b);
gpdraw(1,1427.2a,865.7b);
linetype 0;
linetype -2;
put_text( btex Forward FFT etex, 1365.8a, 829.1b, 0, 3);
linetype 0;
draw (1383.5a,829.1b)--(1470.9a,829.1b);
draw (1383.5a,837.4b)--(1383.5a,820.8b);
draw (1470.9a,837.4b)--(1470.9a,820.8b);
draw (431.8a,715.2b)--(431.8a,770.1b);
draw (423.5a,715.2b)--(440.1a,715.2b);
draw (423.5a,770.1b)--(440.1a,770.1b);
draw (691.1a,569.1b)--(691.1a,716.9b);
draw (682.8a,569.1b)--(699.4a,569.1b);
draw (682.8a,716.9b)--(699.4a,716.9b);
draw (950.4a,435.8b)--(950.4a,602.1b);
draw (942.1a,435.8b)--(958.7a,435.8b);
draw (942.1a,602.1b)--(958.7a,602.1b);
draw (1080.0a,389.6b)--(1080.0a,612.4b);
draw (1071.7a,389.6b)--(1088.3a,389.6b);
draw (1071.7a,612.4b)--(1088.3a,612.4b);
gpdraw(8,431.8a,730.0b);
gpdraw(8,691.1a,630.6b);
gpdraw(8,950.4a,500.8b);
gpdraw(8,1080.0a,571.3b);
gpdraw(8,1427.2a,829.1b);
linetype 2;
linetype -2;
put_text( btex Inverse FFT etex, 1365.8a, 792.5b, 0, 3);
linetype 2;
draw (1383.5a,792.5b)--(1470.9a,792.5b);
draw (1383.5a,800.8b)--(1383.5a,784.2b);
draw (1470.9a,800.8b)--(1470.9a,784.2b);
draw (431.8a,718.6b)--(431.8a,738.8b);
draw (423.5a,718.6b)--(440.1a,718.6b);
draw (423.5a,738.8b)--(440.1a,738.8b);
draw (691.1a,581.9b)--(691.1a,623.1b);
draw (682.8a,581.9b)--(699.4a,581.9b);
draw (682.8a,623.1b)--(699.4a,623.1b);
draw (950.4a,453.3b)--(950.4a,470.8b);
draw (942.1a,453.3b)--(958.7a,453.3b);
draw (942.1a,470.8b)--(958.7a,470.8b);
draw (1080.0a,386.0b)--(1080.0a,401.5b);
draw (1071.7a,386.0b)--(1088.3a,386.0b);
draw (1071.7a,401.5b)--(1088.3a,401.5b);
gpdraw(0,431.8a,728.3b);
gpdraw(0,691.1a,593.3b);
gpdraw(0,950.4a,459.6b);
gpdraw(0,1080.0a,392.7b);
gpdraw(0,1427.2a,792.5b);
linetype 6;
linetype -2;
put_text( btex P3ME Assign Charge etex, 1365.8a, 755.9b, 0, 3);
linetype 6;
draw (1383.5a,755.9b)--(1470.9a,755.9b);
draw (1383.5a,764.2b)--(1383.5a,747.6b);
draw (1470.9a,764.2b)--(1470.9a,747.6b);
draw (431.8a,452.8b)--(431.8a,468.1b);
draw (423.5a,452.8b)--(440.1a,452.8b);
draw (423.5a,468.1b)--(440.1a,468.1b);
draw (691.1a,380.2b)--(691.1a,402.2b);
draw (682.8a,380.2b)--(699.4a,380.2b);
draw (682.8a,402.2b)--(699.4a,402.2b);
draw (950.4a,348.9b)--(950.4a,369.6b);
draw (942.1a,348.9b)--(958.7a,348.9b);
draw (942.1a,369.6b)--(958.7a,369.6b);
draw (1080.0a,343.3b)--(1080.0a,364.4b);
draw (1071.7a,343.3b)--(1088.3a,343.3b);
draw (1071.7a,364.4b)--(1088.3a,364.4b);
gpdraw(6,431.8a,460.1b);
gpdraw(6,691.1a,389.0b);
gpdraw(6,950.4a,356.8b);
gpdraw(6,1080.0a,350.1b);
gpdraw(6,1427.2a,755.9b);
linetype 7;
linetype -2;
put_text( btex P3ME Convolve etex, 1365.8a, 719.3b, 0, 3);
linetype 7;
draw (1383.5a,719.3b)--(1470.9a,719.3b);
draw (1383.5a,727.6b)--(1383.5a,711.0b);
draw (1470.9a,727.6b)--(1470.9a,711.0b);
draw (431.8a,519.1b)--(431.8a,587.7b);
draw (423.5a,519.1b)--(440.1a,519.1b);
draw (423.5a,587.7b)--(440.1a,587.7b);
draw (691.1a,386.1b)--(691.1a,425.5b);
draw (682.8a,386.1b)--(699.4a,386.1b);
draw (682.8a,425.5b)--(699.4a,425.5b);
draw (950.4a,256.8b)--(950.4a,273.8b);
draw (942.1a,256.8b)--(958.7a,256.8b);
draw (942.1a,273.8b)--(958.7a,273.8b);
draw (1080.0a,177.1b)--(1080.0a,194.7b);
draw (1071.7a,177.1b)--(1088.3a,177.1b);
draw (1071.7a,194.7b)--(1088.3a,194.7b);
gpdraw(7,431.8a,558.2b);
gpdraw(7,691.1a,408.7b);
gpdraw(7,950.4a,262.3b);
gpdraw(7,1080.0a,185.5b);
gpdraw(7,1427.2a,719.3b);
linetype 1;
linetype -2;
put_text( btex P3ME Interpolate etex, 1365.8a, 682.7b, 0, 3);
linetype 1;
draw (1383.5a,682.7b)--(1470.9a,682.7b);
draw (1383.5a,691.0b)--(1383.5a,674.4b);
draw (1470.9a,691.0b)--(1470.9a,674.4b);
draw (431.8a,472.9b)--(431.8a,494.9b);
draw (423.5a,472.9b)--(440.1a,472.9b);
draw (423.5a,494.9b)--(440.1a,494.9b);
draw (691.1a,320.3b)--(691.1a,374.7b);
draw (682.8a,320.3b)--(699.4a,320.3b);
draw (682.8a,374.7b)--(699.4a,374.7b);
draw (950.4a,171.9b)--(950.4a,274.6b);
draw (942.1a,171.9b)--(958.7a,171.9b);
draw (942.1a,274.6b)--(958.7a,274.6b);
draw (1080.0a,116.5b)--(1080.0a,252.2b);
draw (1071.7a,116.5b)--(1088.3a,116.5b);
draw (1071.7a,252.2b)--(1088.3a,252.2b);
gpdraw(9,431.8a,483.5b);
gpdraw(9,691.1a,345.6b);
gpdraw(9,950.4a,221.6b);
gpdraw(9,1080.0a,176.2b);
gpdraw(9,1427.2a,682.7b);
draw (431.8a,831.2b)--(691.1a,714.1b)--(950.4a,591.3b)--(1080.0a,611.9b);
linetype 6;
draw (431.8a,460.1b)--(691.1a,389.0b)--(950.4a,356.8b)--(1080.0a,350.1b);
linetype 7;
draw (431.8a,558.2b)--(691.1a,408.7b)--(950.4a,262.3b)--(1080.0a,185.5b);
linetype 0;
draw (431.8a,730.0b)--(691.1a,630.6b)--(950.4a,500.8b)--(1080.0a,571.3b);
linetype 1;
draw (431.8a,483.5b)--(691.1a,345.6b)--(950.4a,221.6b)--(1080.0a,176.2b);
linetype 2;
draw (431.8a,728.3b)--(691.1a,593.3b)--(950.4a,459.6b)--(1080.0a,392.7b);
linetype -2;
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
