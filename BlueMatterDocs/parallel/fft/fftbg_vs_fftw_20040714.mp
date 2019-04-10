%GNUPLOT Metapost output: Thu Jul 15 23:11:37 2004


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage{times}
\usepackage{mathptmx}
\usepackage{helvet}
\usepackage{courier}
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
draw (196.5a,153.1b)--(204.8a,153.1b);
draw (1146.3a,153.1b)--(1138.0a,153.1b);
draw (196.5a,178.4b)--(204.8a,178.4b);
draw (1146.3a,178.4b)--(1138.0a,178.4b);
draw (196.5a,196.3b)--(204.8a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (196.5a,210.2b)--(204.8a,210.2b);
draw (1146.3a,210.2b)--(1138.0a,210.2b);
draw (196.5a,221.6b)--(204.8a,221.6b);
draw (1146.3a,221.6b)--(1138.0a,221.6b);
draw (196.5a,231.2b)--(204.8a,231.2b);
draw (1146.3a,231.2b)--(1138.0a,231.2b);
draw (196.5a,239.6b)--(204.8a,239.6b);
draw (1146.3a,239.6b)--(1138.0a,239.6b);
draw (196.5a,246.9b)--(204.8a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (196.5a,253.5b)--(213.1a,253.5b);
draw (1146.3a,253.5b)--(1129.7a,253.5b);
put_text( btex  0.01 etex, 178.8a, 253.5b, 0, 3);
draw (196.5a,296.8b)--(204.8a,296.8b);
draw (1146.3a,296.8b)--(1138.0a,296.8b);
draw (196.5a,322.1b)--(204.8a,322.1b);
draw (1146.3a,322.1b)--(1138.0a,322.1b);
draw (196.5a,340.0b)--(204.8a,340.0b);
draw (1146.3a,340.0b)--(1138.0a,340.0b);
draw (196.5a,353.9b)--(204.8a,353.9b);
draw (1146.3a,353.9b)--(1138.0a,353.9b);
draw (196.5a,365.3b)--(204.8a,365.3b);
draw (1146.3a,365.3b)--(1138.0a,365.3b);
draw (196.5a,374.9b)--(204.8a,374.9b);
draw (1146.3a,374.9b)--(1138.0a,374.9b);
draw (196.5a,383.3b)--(204.8a,383.3b);
draw (1146.3a,383.3b)--(1138.0a,383.3b);
draw (196.5a,390.6b)--(204.8a,390.6b);
draw (1146.3a,390.6b)--(1138.0a,390.6b);
draw (196.5a,397.2b)--(213.1a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  0.1 etex, 178.8a, 397.2b, 0, 3);
draw (196.5a,440.5b)--(204.8a,440.5b);
draw (1146.3a,440.5b)--(1138.0a,440.5b);
draw (196.5a,465.8b)--(204.8a,465.8b);
draw (1146.3a,465.8b)--(1138.0a,465.8b);
draw (196.5a,483.7b)--(204.8a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (196.5a,497.6b)--(204.8a,497.6b);
draw (1146.3a,497.6b)--(1138.0a,497.6b);
draw (196.5a,509.0b)--(204.8a,509.0b);
draw (1146.3a,509.0b)--(1138.0a,509.0b);
draw (196.5a,518.6b)--(204.8a,518.6b);
draw (1146.3a,518.6b)--(1138.0a,518.6b);
draw (196.5a,527.0b)--(204.8a,527.0b);
draw (1146.3a,527.0b)--(1138.0a,527.0b);
draw (196.5a,534.3b)--(204.8a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (196.5a,540.9b)--(213.1a,540.9b);
draw (1146.3a,540.9b)--(1129.7a,540.9b);
put_text( btex  1 etex, 178.8a, 540.9b, 0, 3);
draw (196.5a,584.2b)--(204.8a,584.2b);
draw (1146.3a,584.2b)--(1138.0a,584.2b);
draw (196.5a,609.5b)--(204.8a,609.5b);
draw (1146.3a,609.5b)--(1138.0a,609.5b);
draw (196.5a,627.4b)--(204.8a,627.4b);
draw (1146.3a,627.4b)--(1138.0a,627.4b);
draw (196.5a,641.3b)--(204.8a,641.3b);
draw (1146.3a,641.3b)--(1138.0a,641.3b);
draw (196.5a,652.7b)--(204.8a,652.7b);
draw (1146.3a,652.7b)--(1138.0a,652.7b);
draw (196.5a,662.3b)--(204.8a,662.3b);
draw (1146.3a,662.3b)--(1138.0a,662.3b);
draw (196.5a,670.7b)--(204.8a,670.7b);
draw (1146.3a,670.7b)--(1138.0a,670.7b);
draw (196.5a,678.0b)--(204.8a,678.0b);
draw (1146.3a,678.0b)--(1138.0a,678.0b);
draw (196.5a,684.6b)--(213.1a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  10 etex, 178.8a, 684.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,684.6b)--(196.5a,668.0b);
put_text( btex  1 etex, 196.5a, 73.2b, 0, 2);
draw (268.0a,109.8b)--(268.0a,118.1b);
draw (268.0a,684.6b)--(268.0a,676.3b);
draw (309.8a,109.8b)--(309.8a,118.1b);
draw (309.8a,684.6b)--(309.8a,676.3b);
draw (339.5a,109.8b)--(339.5a,118.1b);
draw (339.5a,684.6b)--(339.5a,676.3b);
draw (362.5a,109.8b)--(362.5a,118.1b);
draw (362.5a,684.6b)--(362.5a,676.3b);
draw (381.3a,109.8b)--(381.3a,118.1b);
draw (381.3a,684.6b)--(381.3a,676.3b);
draw (397.2a,109.8b)--(397.2a,118.1b);
draw (397.2a,684.6b)--(397.2a,676.3b);
draw (410.9a,109.8b)--(410.9a,118.1b);
draw (410.9a,684.6b)--(410.9a,676.3b);
draw (423.1a,109.8b)--(423.1a,118.1b);
draw (423.1a,684.6b)--(423.1a,676.3b);
draw (434.0a,109.8b)--(434.0a,126.4b);
draw (434.0a,684.6b)--(434.0a,668.0b);
put_text( btex  10 etex, 434.0a, 73.2b, 0, 2);
draw (505.4a,109.8b)--(505.4a,118.1b);
draw (505.4a,684.6b)--(505.4a,676.3b);
draw (547.2a,109.8b)--(547.2a,118.1b);
draw (547.2a,684.6b)--(547.2a,676.3b);
draw (576.9a,109.8b)--(576.9a,118.1b);
draw (576.9a,684.6b)--(576.9a,676.3b);
draw (599.9a,109.8b)--(599.9a,118.1b);
draw (599.9a,684.6b)--(599.9a,676.3b);
draw (618.7a,109.8b)--(618.7a,118.1b);
draw (618.7a,684.6b)--(618.7a,676.3b);
draw (634.6a,109.8b)--(634.6a,118.1b);
draw (634.6a,684.6b)--(634.6a,676.3b);
draw (648.4a,109.8b)--(648.4a,118.1b);
draw (648.4a,684.6b)--(648.4a,676.3b);
draw (660.5a,109.8b)--(660.5a,118.1b);
draw (660.5a,684.6b)--(660.5a,676.3b);
draw (671.4a,109.8b)--(671.4a,126.4b);
draw (671.4a,684.6b)--(671.4a,668.0b);
put_text( btex  100 etex, 671.4a, 73.2b, 0, 2);
draw (742.9a,109.8b)--(742.9a,118.1b);
draw (742.9a,684.6b)--(742.9a,676.3b);
draw (784.7a,109.8b)--(784.7a,118.1b);
draw (784.7a,684.6b)--(784.7a,676.3b);
draw (814.4a,109.8b)--(814.4a,118.1b);
draw (814.4a,684.6b)--(814.4a,676.3b);
draw (837.4a,109.8b)--(837.4a,118.1b);
draw (837.4a,684.6b)--(837.4a,676.3b);
draw (856.2a,109.8b)--(856.2a,118.1b);
draw (856.2a,684.6b)--(856.2a,676.3b);
draw (872.1a,109.8b)--(872.1a,118.1b);
draw (872.1a,684.6b)--(872.1a,676.3b);
draw (885.8a,109.8b)--(885.8a,118.1b);
draw (885.8a,684.6b)--(885.8a,676.3b);
draw (898.0a,109.8b)--(898.0a,118.1b);
draw (898.0a,684.6b)--(898.0a,676.3b);
draw (908.9a,109.8b)--(908.9a,126.4b);
draw (908.9a,684.6b)--(908.9a,668.0b);
put_text( btex  1000 etex, 908.9a, 73.2b, 0, 2);
draw (980.3a,109.8b)--(980.3a,118.1b);
draw (980.3a,684.6b)--(980.3a,676.3b);
draw (1022.1a,109.8b)--(1022.1a,118.1b);
draw (1022.1a,684.6b)--(1022.1a,676.3b);
draw (1051.8a,109.8b)--(1051.8a,118.1b);
draw (1051.8a,684.6b)--(1051.8a,676.3b);
draw (1074.8a,109.8b)--(1074.8a,118.1b);
draw (1074.8a,684.6b)--(1074.8a,676.3b);
draw (1093.6a,109.8b)--(1093.6a,118.1b);
draw (1093.6a,684.6b)--(1093.6a,676.3b);
draw (1109.5a,109.8b)--(1109.5a,118.1b);
draw (1109.5a,684.6b)--(1109.5a,676.3b);
draw (1123.3a,109.8b)--(1123.3a,118.1b);
draw (1123.3a,684.6b)--(1123.3a,676.3b);
draw (1135.4a,109.8b)--(1135.4a,118.1b);
draw (1135.4a,684.6b)--(1135.4a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  10000 etex, 1146.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
put_text( btex 3D-FFT Execution Time (seconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 671.4a, 18.3b, 0, 2);
put_text( btex 128 x 128 x 128 FFT on BG/L etex, 500.9a, 150.5b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Volumetric FFT on MPI etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
gpdraw(0,196.5a,548.2b);
gpdraw(0,268.0a,510.3b);
gpdraw(0,339.5a,474.7b);
gpdraw(0,410.9a,436.6b);
gpdraw(0,482.4a,387.6b);
gpdraw(0,553.9a,345.8b);
gpdraw(0,625.4a,305.8b);
gpdraw(0,696.9a,268.5b);
gpdraw(0,768.3a,227.0b);
gpdraw(0,839.8a,203.0b);
gpdraw(0,911.3a,194.0b);
gpdraw(0,982.8a,203.3b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
linetype -2;
put_text( btex Volumetric FFT on Active Packet/Blade etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
gpdraw(1,410.9a,420.0b);
gpdraw(1,553.9a,329.1b);
gpdraw(1,696.9a,261.7b);
gpdraw(1,839.8a,168.0b);
gpdraw(1,911.3a,154.0b);
gpdraw(1,1067.2a,613.1b);
linetype 2;
linetype -2;
put_text( btex FFTW Port etex, 1005.8a, 576.5b, 0, 3);
linetype 2;
gpdraw(2,196.5a,519.6b);
gpdraw(2,268.0a,502.4b);
gpdraw(2,339.5a,478.1b);
gpdraw(2,410.9a,445.8b);
gpdraw(2,482.4a,405.0b);
gpdraw(2,553.9a,362.7b);
gpdraw(2,625.4a,332.1b);
gpdraw(2,696.9a,288.8b);
gpdraw(2,768.3a,290.7b);
gpdraw(2,839.8a,288.3b);
gpdraw(2,911.3a,311.5b);
gpdraw(2,982.8a,310.5b);
gpdraw(2,1067.2a,576.5b);
linetype -2;
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
