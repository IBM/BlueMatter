%GNUPLOT Metapost output: Fri May 13 14:15:43 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
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

beginfig(6);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,366.0b)--(213.1a,366.0b);
draw (1146.3a,366.0b)--(1129.7a,366.0b);
put_text( btex  0.001 etex, 178.8a, 366.0b, 0, 3);
draw (196.5a,400.8b)--(204.8a,400.8b);
draw (1146.3a,400.8b)--(1138.0a,400.8b);
draw (196.5a,421.2b)--(204.8a,421.2b);
draw (1146.3a,421.2b)--(1138.0a,421.2b);
draw (196.5a,435.6b)--(204.8a,435.6b);
draw (1146.3a,435.6b)--(1138.0a,435.6b);
draw (196.5a,446.8b)--(204.8a,446.8b);
draw (1146.3a,446.8b)--(1138.0a,446.8b);
draw (196.5a,456.0b)--(204.8a,456.0b);
draw (1146.3a,456.0b)--(1138.0a,456.0b);
draw (196.5a,463.7b)--(204.8a,463.7b);
draw (1146.3a,463.7b)--(1138.0a,463.7b);
draw (196.5a,470.4b)--(204.8a,470.4b);
draw (1146.3a,470.4b)--(1138.0a,470.4b);
draw (196.5a,476.4b)--(204.8a,476.4b);
draw (1146.3a,476.4b)--(1138.0a,476.4b);
draw (196.5a,481.7b)--(213.1a,481.7b);
draw (1146.3a,481.7b)--(1129.7a,481.7b);
put_text( btex  0.01 etex, 178.8a, 481.7b, 0, 3);
draw (196.5a,516.5b)--(204.8a,516.5b);
draw (1146.3a,516.5b)--(1138.0a,516.5b);
draw (196.5a,536.8b)--(204.8a,536.8b);
draw (1146.3a,536.8b)--(1138.0a,536.8b);
draw (196.5a,551.3b)--(204.8a,551.3b);
draw (1146.3a,551.3b)--(1138.0a,551.3b);
draw (196.5a,562.5b)--(204.8a,562.5b);
draw (1146.3a,562.5b)--(1138.0a,562.5b);
draw (196.5a,571.6b)--(204.8a,571.6b);
draw (1146.3a,571.6b)--(1138.0a,571.6b);
draw (196.5a,579.4b)--(204.8a,579.4b);
draw (1146.3a,579.4b)--(1138.0a,579.4b);
draw (196.5a,586.1b)--(204.8a,586.1b);
draw (1146.3a,586.1b)--(1138.0a,586.1b);
draw (196.5a,592.0b)--(204.8a,592.0b);
draw (1146.3a,592.0b)--(1138.0a,592.0b);
draw (196.5a,597.3b)--(213.1a,597.3b);
draw (1146.3a,597.3b)--(1129.7a,597.3b);
put_text( btex  0.1 etex, 178.8a, 597.3b, 0, 3);
draw (196.5a,632.1b)--(204.8a,632.1b);
draw (1146.3a,632.1b)--(1138.0a,632.1b);
draw (196.5a,652.5b)--(204.8a,652.5b);
draw (1146.3a,652.5b)--(1138.0a,652.5b);
draw (196.5a,666.9b)--(204.8a,666.9b);
draw (1146.3a,666.9b)--(1138.0a,666.9b);
draw (196.5a,678.1b)--(204.8a,678.1b);
draw (1146.3a,678.1b)--(1138.0a,678.1b);
draw (196.5a,687.3b)--(204.8a,687.3b);
draw (1146.3a,687.3b)--(1138.0a,687.3b);
draw (196.5a,695.0b)--(204.8a,695.0b);
draw (1146.3a,695.0b)--(1138.0a,695.0b);
draw (196.5a,701.7b)--(204.8a,701.7b);
draw (1146.3a,701.7b)--(1138.0a,701.7b);
draw (196.5a,707.7b)--(204.8a,707.7b);
draw (1146.3a,707.7b)--(1138.0a,707.7b);
draw (196.5a,713.0b)--(213.1a,713.0b);
draw (1146.3a,713.0b)--(1129.7a,713.0b);
put_text( btex  1 etex, 178.8a, 713.0b, 0, 3);
draw (196.5a,747.8b)--(204.8a,747.8b);
draw (1146.3a,747.8b)--(1138.0a,747.8b);
draw (196.5a,768.1b)--(204.8a,768.1b);
draw (1146.3a,768.1b)--(1138.0a,768.1b);
draw (196.5a,782.6b)--(204.8a,782.6b);
draw (1146.3a,782.6b)--(1138.0a,782.6b);
draw (196.5a,793.8b)--(204.8a,793.8b);
draw (1146.3a,793.8b)--(1138.0a,793.8b);
draw (196.5a,802.9b)--(204.8a,802.9b);
draw (1146.3a,802.9b)--(1138.0a,802.9b);
draw (196.5a,810.7b)--(204.8a,810.7b);
draw (1146.3a,810.7b)--(1138.0a,810.7b);
draw (196.5a,817.4b)--(204.8a,817.4b);
draw (1146.3a,817.4b)--(1138.0a,817.4b);
draw (196.5a,823.3b)--(204.8a,823.3b);
draw (1146.3a,823.3b)--(1138.0a,823.3b);
draw (196.5a,828.6b)--(213.1a,828.6b);
draw (1146.3a,828.6b)--(1129.7a,828.6b);
put_text( btex  10 etex, 178.8a, 828.6b, 0, 3);
draw (196.5a,366.0b)--(196.5a,382.6b);
draw (196.5a,828.6b)--(196.5a,812.0b);
put_text( btex  1 etex, 196.5a, 329.4b, 0, 2);
draw (264.1a,366.0b)--(264.1a,374.3b);
draw (264.1a,828.6b)--(264.1a,820.3b);
draw (353.4a,366.0b)--(353.4a,374.3b);
draw (353.4a,828.6b)--(353.4a,820.3b);
draw (399.3a,366.0b)--(399.3a,374.3b);
draw (399.3a,828.6b)--(399.3a,820.3b);
draw (421.0a,366.0b)--(421.0a,382.6b);
draw (421.0a,828.6b)--(421.0a,812.0b);
put_text( btex  10 etex, 421.0a, 329.4b, 0, 2);
draw (488.6a,366.0b)--(488.6a,374.3b);
draw (488.6a,828.6b)--(488.6a,820.3b);
draw (577.9a,366.0b)--(577.9a,374.3b);
draw (577.9a,828.6b)--(577.9a,820.3b);
draw (623.8a,366.0b)--(623.8a,374.3b);
draw (623.8a,828.6b)--(623.8a,820.3b);
draw (645.5a,366.0b)--(645.5a,382.6b);
draw (645.5a,828.6b)--(645.5a,812.0b);
put_text( btex  100 etex, 645.5a, 329.4b, 0, 2);
draw (713.1a,366.0b)--(713.1a,374.3b);
draw (713.1a,828.6b)--(713.1a,820.3b);
draw (802.5a,366.0b)--(802.5a,374.3b);
draw (802.5a,828.6b)--(802.5a,820.3b);
draw (848.3a,366.0b)--(848.3a,374.3b);
draw (848.3a,828.6b)--(848.3a,820.3b);
draw (870.0a,366.0b)--(870.0a,382.6b);
draw (870.0a,828.6b)--(870.0a,812.0b);
put_text( btex  1000 etex, 870.0a, 329.4b, 0, 2);
draw (937.6a,366.0b)--(937.6a,374.3b);
draw (937.6a,828.6b)--(937.6a,820.3b);
draw (1027.0a,366.0b)--(1027.0a,374.3b);
draw (1027.0a,828.6b)--(1027.0a,820.3b);
draw (1072.8a,366.0b)--(1072.8a,374.3b);
draw (1072.8a,828.6b)--(1072.8a,820.3b);
draw (1094.6a,366.0b)--(1094.6a,382.6b);
draw (1094.6a,828.6b)--(1094.6a,812.0b);
put_text( btex  10000 etex, 1094.6a, 329.4b, 0, 2);
draw (196.5a,366.0b)--(1146.3a,366.0b)--(1146.3a,828.6b)--(196.5a,828.6b)--(196.5a,366.0b);
put_text( btex (Task Count)$^{2/3} \times$ Time (sec.) etex, 36.6a, 597.3b, 1, 2);
put_text( btex Task Count etex, 671.4a, 274.5b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $128^3$ MPI etex, 866.9a, 201.3b, 0, 3);
linetype 0;
draw (884.6a,201.3b)--(972.0a,201.3b);
draw (196.5a,715.8b)--(399.3a,696.5b)--(466.8a,685.8b)--(534.4a,666.8b)--(602.0a,657.3b)
--(669.6a,661.2b)--(737.2a,636.2b)--(804.8a,630.0b)--(872.4a,631.7b)--(939.9a,642.1b)
--(1007.5a,657.1b)--(1075.1a,672.5b);
gpdraw(0,196.5a,715.8b);
gpdraw(0,399.3a,696.5b);
gpdraw(0,466.8a,685.8b);
gpdraw(0,534.4a,666.8b);
gpdraw(0,602.0a,657.3b);
gpdraw(0,669.6a,661.2b);
gpdraw(0,737.2a,636.2b);
gpdraw(0,804.8a,630.0b);
gpdraw(0,872.4a,631.7b);
gpdraw(0,939.9a,642.1b);
gpdraw(0,1007.5a,657.1b);
gpdraw(0,1075.1a,672.5b);
gpdraw(0,928.3a,201.3b);
linetype 1;
linetype -2;
put_text( btex $128^3$ BG/L ADE Single Core etex, 866.9a, 164.7b, 0, 3);
linetype 1;
draw (884.6a,164.7b)--(972.0a,164.7b);
draw (804.8a,629.4b)--(872.4a,634.1b)--(939.9a,629.2b)--(1007.5a,650.7b)--(1075.1a,655.6b)
--(1142.7a,664.2b);
gpdraw(1,804.8a,629.4b);
gpdraw(1,872.4a,634.1b);
gpdraw(1,939.9a,629.2b);
gpdraw(1,1007.5a,650.7b);
gpdraw(1,1075.1a,655.6b);
gpdraw(1,1142.7a,664.2b);
gpdraw(1,928.3a,164.7b);
linetype 2;
linetype -2;
put_text( btex $64^3$ MPI etex, 866.9a, 128.1b, 0, 3);
linetype 2;
draw (884.6a,128.1b)--(972.0a,128.1b);
draw (196.5a,595.2b)--(399.3a,590.7b)--(602.0a,556.2b)--(669.6a,562.3b)--(804.8a,548.4b)
--(872.4a,563.3b)--(939.9a,584.9b)--(1007.5a,608.6b);
gpdraw(2,196.5a,595.2b);
gpdraw(2,399.3a,590.7b);
gpdraw(2,602.0a,556.2b);
gpdraw(2,669.6a,562.3b);
gpdraw(2,804.8a,548.4b);
gpdraw(2,872.4a,563.3b);
gpdraw(2,939.9a,584.9b);
gpdraw(2,1007.5a,608.6b);
gpdraw(2,928.3a,128.1b);
linetype 3;
linetype -2;
put_text( btex $64^3$ BG/L ADE Single Core etex, 866.9a, 91.5b, 0, 3);
linetype 3;
draw (884.6a,91.5b)--(972.0a,91.5b);
draw (804.8a,527.6b)--(872.4a,541.8b)--(939.9a,546.6b)--(1007.5a,578.9b);
gpdraw(3,804.8a,527.6b);
gpdraw(3,872.4a,541.8b);
gpdraw(3,939.9a,546.6b);
gpdraw(3,1007.5a,578.9b);
gpdraw(3,928.3a,91.5b);
linetype 4;
linetype -2;
put_text( btex $32^3$ MPI etex, 866.9a, 54.9b, 0, 3);
linetype 4;
draw (884.6a,54.9b)--(972.0a,54.9b);
draw (196.5a,475.0b)--(399.3a,482.1b)--(466.8a,470.9b)--(534.4a,464.1b)--(602.0a,465.5b)
--(669.6a,473.1b)--(737.2a,489.7b)--(804.8a,504.9b)--(872.4a,528.7b);
gpdraw(4,196.5a,475.0b);
gpdraw(4,399.3a,482.1b);
gpdraw(4,466.8a,470.9b);
gpdraw(4,534.4a,464.1b);
gpdraw(4,602.0a,465.5b);
gpdraw(4,669.6a,473.1b);
gpdraw(4,737.2a,489.7b);
gpdraw(4,804.8a,504.9b);
gpdraw(4,872.4a,528.7b);
gpdraw(4,928.3a,54.9b);
linetype 5;
linetype -2;
put_text( btex $32^3$ Active Packet etex, 866.9a, 18.3b, 0, 3);
linetype 5;
draw (884.6a,18.3b)--(972.0a,18.3b);
draw (196.5a,501.3b)--(399.3a,466.5b)--(534.4a,455.1b)--(669.6a,450.1b)--(804.8a,452.0b);
gpdraw(5,196.5a,501.3b);
gpdraw(5,399.3a,466.5b);
gpdraw(5,534.4a,455.1b);
gpdraw(5,669.6a,450.1b);
gpdraw(5,804.8a,452.0b);
gpdraw(5,928.3a,18.3b);
linetype -2;
draw (196.5a,366.0b)--(1146.3a,366.0b)--(1146.3a,828.6b)--(196.5a,828.6b)--(196.5a,366.0b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
