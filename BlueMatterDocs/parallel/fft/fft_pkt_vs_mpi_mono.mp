%GNUPLOT Metapost output: Sun Jul 17 20:30:43 2005


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
colorlines:=false;
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
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1506.3a,109.8b)--(1489.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,157.4b)--(222.5a,157.4b);
draw (1506.3a,157.4b)--(1498.0a,157.4b);
draw (214.2a,220.3b)--(222.5a,220.3b);
draw (1506.3a,220.3b)--(1498.0a,220.3b);
draw (214.2a,252.6b)--(222.5a,252.6b);
draw (1506.3a,252.6b)--(1498.0a,252.6b);
draw (214.2a,268.0b)--(230.8a,268.0b);
draw (1506.3a,268.0b)--(1489.7a,268.0b);
put_text( btex  0.001 etex, 196.5a, 268.0b, 0, 3);
draw (214.2a,315.6b)--(222.5a,315.6b);
draw (1506.3a,315.6b)--(1498.0a,315.6b);
draw (214.2a,378.5b)--(222.5a,378.5b);
draw (1506.3a,378.5b)--(1498.0a,378.5b);
draw (214.2a,410.8b)--(222.5a,410.8b);
draw (1506.3a,410.8b)--(1498.0a,410.8b);
draw (214.2a,426.1b)--(230.8a,426.1b);
draw (1506.3a,426.1b)--(1489.7a,426.1b);
put_text( btex  0.01 etex, 196.5a, 426.1b, 0, 3);
draw (214.2a,473.7b)--(222.5a,473.7b);
draw (1506.3a,473.7b)--(1498.0a,473.7b);
draw (214.2a,536.7b)--(222.5a,536.7b);
draw (1506.3a,536.7b)--(1498.0a,536.7b);
draw (214.2a,569.0b)--(222.5a,569.0b);
draw (1506.3a,569.0b)--(1498.0a,569.0b);
draw (214.2a,584.3b)--(230.8a,584.3b);
draw (1506.3a,584.3b)--(1489.7a,584.3b);
put_text( btex  0.1 etex, 196.5a, 584.3b, 0, 3);
draw (214.2a,631.9b)--(222.5a,631.9b);
draw (1506.3a,631.9b)--(1498.0a,631.9b);
draw (214.2a,694.8b)--(222.5a,694.8b);
draw (1506.3a,694.8b)--(1498.0a,694.8b);
draw (214.2a,727.1b)--(222.5a,727.1b);
draw (1506.3a,727.1b)--(1498.0a,727.1b);
draw (214.2a,742.4b)--(230.8a,742.4b);
draw (1506.3a,742.4b)--(1489.7a,742.4b);
put_text( btex  1 etex, 196.5a, 742.4b, 0, 3);
draw (214.2a,790.1b)--(222.5a,790.1b);
draw (1506.3a,790.1b)--(1498.0a,790.1b);
draw (214.2a,853.0b)--(222.5a,853.0b);
draw (1506.3a,853.0b)--(1498.0a,853.0b);
draw (214.2a,885.3b)--(222.5a,885.3b);
draw (1506.3a,885.3b)--(1498.0a,885.3b);
draw (214.2a,900.6b)--(230.8a,900.6b);
draw (1506.3a,900.6b)--(1489.7a,900.6b);
put_text( btex  10 etex, 196.5a, 900.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,900.6b)--(214.2a,884.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
draw (292.0a,109.8b)--(292.0a,118.1b);
draw (292.0a,900.6b)--(292.0a,892.3b);
draw (394.8a,109.8b)--(394.8a,118.1b);
draw (394.8a,900.6b)--(394.8a,892.3b);
draw (447.6a,109.8b)--(447.6a,118.1b);
draw (447.6a,900.6b)--(447.6a,892.3b);
draw (472.6a,109.8b)--(472.6a,126.4b);
draw (472.6a,900.6b)--(472.6a,884.0b);
put_text( btex  10 etex, 472.6a, 73.2b, 0, 2);
draw (550.4a,109.8b)--(550.4a,118.1b);
draw (550.4a,900.6b)--(550.4a,892.3b);
draw (653.2a,109.8b)--(653.2a,118.1b);
draw (653.2a,900.6b)--(653.2a,892.3b);
draw (706.0a,109.8b)--(706.0a,118.1b);
draw (706.0a,900.6b)--(706.0a,892.3b);
draw (731.0a,109.8b)--(731.0a,126.4b);
draw (731.0a,900.6b)--(731.0a,884.0b);
put_text( btex  100 etex, 731.0a, 73.2b, 0, 2);
draw (808.8a,109.8b)--(808.8a,118.1b);
draw (808.8a,900.6b)--(808.8a,892.3b);
draw (911.7a,109.8b)--(911.7a,118.1b);
draw (911.7a,900.6b)--(911.7a,892.3b);
draw (964.4a,109.8b)--(964.4a,118.1b);
draw (964.4a,900.6b)--(964.4a,892.3b);
draw (989.5a,109.8b)--(989.5a,126.4b);
draw (989.5a,900.6b)--(989.5a,884.0b);
put_text( btex  1000 etex, 989.5a, 73.2b, 0, 2);
draw (1067.3a,109.8b)--(1067.3a,118.1b);
draw (1067.3a,900.6b)--(1067.3a,892.3b);
draw (1170.1a,109.8b)--(1170.1a,118.1b);
draw (1170.1a,900.6b)--(1170.1a,892.3b);
draw (1222.8a,109.8b)--(1222.8a,118.1b);
draw (1222.8a,900.6b)--(1222.8a,892.3b);
draw (1247.9a,109.8b)--(1247.9a,126.4b);
draw (1247.9a,900.6b)--(1247.9a,884.0b);
put_text( btex  10000 etex, 1247.9a, 73.2b, 0, 2);
draw (1325.7a,109.8b)--(1325.7a,118.1b);
draw (1325.7a,900.6b)--(1325.7a,892.3b);
draw (1428.5a,109.8b)--(1428.5a,118.1b);
draw (1428.5a,900.6b)--(1428.5a,892.3b);
draw (1481.3a,109.8b)--(1481.3a,118.1b);
draw (1481.3a,900.6b)--(1481.3a,892.3b);
draw (1506.3a,109.8b)--(1506.3a,126.4b);
draw (1506.3a,900.6b)--(1506.3a,884.0b);
put_text( btex  100000 etex, 1506.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (seconds) etex, 36.6a, 505.2b, 1, 2);
put_text( btex Node Count etex, 860.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $128^3$ MPI etex, 1365.8a, 865.7b, 0, 3);
linetype 0;
draw (1383.5a,865.7b)--(1470.9a,865.7b);
draw (214.2a,746.4b)--(447.6a,624.8b)--(525.4a,578.3b)--(603.2a,520.7b)--(681.0a,475.9b)
--(758.7a,449.5b)--(836.5a,383.6b)--(914.3a,343.4b)--(992.1a,313.9b)--(1069.9a,296.4b)
--(1147.7a,285.2b)--(1225.5a,274.4b)--(1303.3a,268.2b);
draw (214.2a,581.4b)--(447.6a,480.0b)--(681.0a,337.7b)--(758.7a,314.2b)--(914.3a,231.7b)
--(992.1a,220.3b)--(1069.9a,218.1b)--(1147.7a,218.8b);
gpdraw(0,214.2a,746.4b);
gpdraw(0,447.6a,624.8b);
gpdraw(0,525.4a,578.3b);
gpdraw(0,603.2a,520.7b);
gpdraw(0,681.0a,475.9b);
gpdraw(0,758.7a,449.5b);
gpdraw(0,836.5a,383.6b);
gpdraw(0,914.3a,343.4b);
gpdraw(0,992.1a,313.9b);
gpdraw(0,1069.9a,296.4b);
gpdraw(0,1147.7a,285.2b);
gpdraw(0,1225.5a,274.4b);
gpdraw(0,1303.3a,268.2b);
gpdraw(0,214.2a,581.4b);
gpdraw(0,447.6a,480.0b);
gpdraw(0,681.0a,337.7b);
gpdraw(0,758.7a,314.2b);
gpdraw(0,914.3a,231.7b);
gpdraw(0,992.1a,220.3b);
gpdraw(0,1069.9a,218.1b);
gpdraw(0,1147.7a,218.8b);
gpdraw(0,1427.2a,865.7b);
linetype 1;
linetype -2;
put_text( btex $128^3$ BG/L ADE Single Core etex, 1365.8a, 829.1b, 0, 3);
linetype 1;
draw (1383.5a,829.1b)--(1470.9a,829.1b);
draw (914.3a,342.5b)--(992.1a,317.3b)--(1069.9a,278.7b)--(1147.7a,276.4b)--(1225.5a,251.3b)
--(1303.3a,231.4b);
gpdraw(1,914.3a,342.5b);
gpdraw(1,992.1a,317.3b);
gpdraw(1,1069.9a,278.7b);
gpdraw(1,1147.7a,276.4b);
gpdraw(1,1225.5a,251.3b);
gpdraw(1,1303.3a,231.4b);
gpdraw(1,1427.2a,829.1b);
linetype 2;
linetype -2;
put_text( btex $64^3$ MPI etex, 1365.8a, 792.5b, 0, 3);
linetype 2;
draw (1383.5a,792.5b)--(1470.9a,792.5b);
draw (214.2a,417.0b)--(447.6a,331.5b)--(525.4a,284.5b)--(603.2a,243.5b)--(681.0a,213.6b)
--(758.7a,192.2b)--(836.5a,183.2b)--(914.3a,172.2b)--(992.1a,173.0b);
gpdraw(2,214.2a,417.0b);
gpdraw(2,447.6a,331.5b);
gpdraw(2,525.4a,284.5b);
gpdraw(2,603.2a,243.5b);
gpdraw(2,681.0a,213.6b);
gpdraw(2,758.7a,192.2b);
gpdraw(2,836.5a,183.2b);
gpdraw(2,914.3a,172.2b);
gpdraw(2,992.1a,173.0b);
gpdraw(2,1427.2a,792.5b);
linetype 3;
linetype -2;
put_text( btex $64^3$ BG/L ADE Single Core etex, 1365.8a, 755.9b, 0, 3);
linetype 3;
draw (1383.5a,755.9b)--(1470.9a,755.9b);
draw (914.3a,203.3b)--(992.1a,191.0b)--(1069.9a,165.8b)--(1147.7a,178.3b);
gpdraw(3,914.3a,203.3b);
gpdraw(3,992.1a,191.0b);
gpdraw(3,1069.9a,165.8b);
gpdraw(3,1147.7a,178.3b);
gpdraw(3,1427.2a,755.9b);
linetype -2;
draw (214.2a,109.8b)--(1506.3a,109.8b)--(1506.3a,900.6b)--(214.2a,900.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
