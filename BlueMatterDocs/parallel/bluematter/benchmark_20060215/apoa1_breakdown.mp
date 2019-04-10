%GNUPLOT Metapost output: Thu Feb 23 11:33:40 2006


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

beginfig(2);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,181.9b)--(222.5a,181.9b);
draw (1386.3a,181.9b)--(1378.0a,181.9b);
draw (214.2a,224.1b)--(222.5a,224.1b);
draw (1386.3a,224.1b)--(1378.0a,224.1b);
draw (214.2a,254.1b)--(222.5a,254.1b);
draw (1386.3a,254.1b)--(1378.0a,254.1b);
draw (214.2a,277.3b)--(222.5a,277.3b);
draw (1386.3a,277.3b)--(1378.0a,277.3b);
draw (214.2a,296.2b)--(222.5a,296.2b);
draw (1386.3a,296.2b)--(1378.0a,296.2b);
draw (214.2a,312.3b)--(222.5a,312.3b);
draw (1386.3a,312.3b)--(1378.0a,312.3b);
draw (214.2a,326.2b)--(222.5a,326.2b);
draw (1386.3a,326.2b)--(1378.0a,326.2b);
draw (214.2a,338.4b)--(222.5a,338.4b);
draw (1386.3a,338.4b)--(1378.0a,338.4b);
draw (214.2a,349.4b)--(230.8a,349.4b);
draw (1386.3a,349.4b)--(1369.7a,349.4b);
put_text( btex  0.001 etex, 196.5a, 349.4b, 0, 3);
draw (214.2a,421.5b)--(222.5a,421.5b);
draw (1386.3a,421.5b)--(1378.0a,421.5b);
draw (214.2a,463.7b)--(222.5a,463.7b);
draw (1386.3a,463.7b)--(1378.0a,463.7b);
draw (214.2a,493.7b)--(222.5a,493.7b);
draw (1386.3a,493.7b)--(1378.0a,493.7b);
draw (214.2a,516.9b)--(222.5a,516.9b);
draw (1386.3a,516.9b)--(1378.0a,516.9b);
draw (214.2a,535.8b)--(222.5a,535.8b);
draw (1386.3a,535.8b)--(1378.0a,535.8b);
draw (214.2a,551.9b)--(222.5a,551.9b);
draw (1386.3a,551.9b)--(1378.0a,551.9b);
draw (214.2a,565.8b)--(222.5a,565.8b);
draw (1386.3a,565.8b)--(1378.0a,565.8b);
draw (214.2a,578.0b)--(222.5a,578.0b);
draw (1386.3a,578.0b)--(1378.0a,578.0b);
draw (214.2a,589.0b)--(230.8a,589.0b);
draw (1386.3a,589.0b)--(1369.7a,589.0b);
put_text( btex  0.01 etex, 196.5a, 589.0b, 0, 3);
draw (214.2a,661.1b)--(222.5a,661.1b);
draw (1386.3a,661.1b)--(1378.0a,661.1b);
draw (214.2a,703.3b)--(222.5a,703.3b);
draw (1386.3a,703.3b)--(1378.0a,703.3b);
draw (214.2a,733.3b)--(222.5a,733.3b);
draw (1386.3a,733.3b)--(1378.0a,733.3b);
draw (214.2a,756.5b)--(222.5a,756.5b);
draw (1386.3a,756.5b)--(1378.0a,756.5b);
draw (214.2a,775.4b)--(222.5a,775.4b);
draw (1386.3a,775.4b)--(1378.0a,775.4b);
draw (214.2a,791.5b)--(222.5a,791.5b);
draw (1386.3a,791.5b)--(1378.0a,791.5b);
draw (214.2a,805.4b)--(222.5a,805.4b);
draw (1386.3a,805.4b)--(1378.0a,805.4b);
draw (214.2a,817.6b)--(222.5a,817.6b);
draw (1386.3a,817.6b)--(1378.0a,817.6b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1386.3a,828.6b)--(1369.7a,828.6b);
put_text( btex  0.1 etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  100 etex, 214.2a, 73.2b, 0, 2);
draw (331.8a,109.8b)--(331.8a,118.1b);
draw (331.8a,828.6b)--(331.8a,820.3b);
draw (400.6a,109.8b)--(400.6a,118.1b);
draw (400.6a,828.6b)--(400.6a,820.3b);
draw (449.4a,109.8b)--(449.4a,118.1b);
draw (449.4a,828.6b)--(449.4a,820.3b);
draw (487.3a,109.8b)--(487.3a,118.1b);
draw (487.3a,828.6b)--(487.3a,820.3b);
draw (518.2a,109.8b)--(518.2a,118.1b);
draw (518.2a,828.6b)--(518.2a,820.3b);
draw (544.4a,109.8b)--(544.4a,118.1b);
draw (544.4a,828.6b)--(544.4a,820.3b);
draw (567.0a,109.8b)--(567.0a,118.1b);
draw (567.0a,828.6b)--(567.0a,820.3b);
draw (587.0a,109.8b)--(587.0a,118.1b);
draw (587.0a,828.6b)--(587.0a,820.3b);
draw (604.9a,109.8b)--(604.9a,126.4b);
draw (604.9a,828.6b)--(604.9a,812.0b);
put_text( btex  1000 etex, 604.9a, 73.2b, 0, 2);
draw (722.5a,109.8b)--(722.5a,118.1b);
draw (722.5a,828.6b)--(722.5a,820.3b);
draw (791.3a,109.8b)--(791.3a,118.1b);
draw (791.3a,828.6b)--(791.3a,820.3b);
draw (840.1a,109.8b)--(840.1a,118.1b);
draw (840.1a,828.6b)--(840.1a,820.3b);
draw (878.0a,109.8b)--(878.0a,118.1b);
draw (878.0a,828.6b)--(878.0a,820.3b);
draw (908.9a,109.8b)--(908.9a,118.1b);
draw (908.9a,828.6b)--(908.9a,820.3b);
draw (935.1a,109.8b)--(935.1a,118.1b);
draw (935.1a,828.6b)--(935.1a,820.3b);
draw (957.7a,109.8b)--(957.7a,118.1b);
draw (957.7a,828.6b)--(957.7a,820.3b);
draw (977.7a,109.8b)--(977.7a,118.1b);
draw (977.7a,828.6b)--(977.7a,820.3b);
draw (995.6a,109.8b)--(995.6a,126.4b);
draw (995.6a,828.6b)--(995.6a,812.0b);
put_text( btex  10000 etex, 995.6a, 73.2b, 0, 2);
draw (1113.2a,109.8b)--(1113.2a,118.1b);
draw (1113.2a,828.6b)--(1113.2a,820.3b);
draw (1182.0a,109.8b)--(1182.0a,118.1b);
draw (1182.0a,828.6b)--(1182.0a,820.3b);
draw (1230.8a,109.8b)--(1230.8a,118.1b);
draw (1230.8a,828.6b)--(1230.8a,820.3b);
draw (1268.7a,109.8b)--(1268.7a,118.1b);
draw (1268.7a,828.6b)--(1268.7a,820.3b);
draw (1299.6a,109.8b)--(1299.6a,118.1b);
draw (1299.6a,828.6b)--(1299.6a,820.3b);
draw (1325.8a,109.8b)--(1325.8a,118.1b);
draw (1325.8a,828.6b)--(1325.8a,820.3b);
draw (1348.4a,109.8b)--(1348.4a,118.1b);
draw (1348.4a,828.6b)--(1348.4a,820.3b);
draw (1368.4a,109.8b)--(1368.4a,118.1b);
draw (1368.4a,828.6b)--(1368.4a,820.3b);
draw (1386.3a,109.8b)--(1386.3a,126.4b);
draw (1386.3a,828.6b)--(1386.3a,812.0b);
put_text( btex  100000 etex, 1386.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time/Time-step (seconds) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Node Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Total Time P3ME 1:1 etex, 1245.8a, 793.7b, 0, 3);
linetype 0;
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (491.3a,723.3b)--(608.9a,656.7b)--(726.5a,596.1b)--(726.5a,595.8b)--(844.1a,535.3b)
--(844.1a,541.3b)--(961.8a,485.1b)--(1079.4a,447.8b);
gpdraw(0,491.3a,723.3b);
gpdraw(0,608.9a,656.7b);
gpdraw(0,726.5a,596.1b);
gpdraw(0,726.5a,595.8b);
gpdraw(0,844.1a,535.3b);
gpdraw(0,844.1a,541.3b);
gpdraw(0,961.8a,485.1b);
gpdraw(0,1079.4a,447.8b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
linetype -2;
put_text( btex Total Time P3ME 1:4 etex, 1245.8a, 757.1b, 0, 3);
linetype 1;
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (491.3a,723.2b)--(608.9a,656.7b)--(726.5a,596.1b)--(726.5a,595.6b)--(844.1a,535.3b)
--(844.1a,541.2b)--(961.8a,484.7b)--(1079.4a,445.1b);
gpdraw(1,491.3a,723.2b);
gpdraw(1,608.9a,656.7b);
gpdraw(1,726.5a,596.1b);
gpdraw(1,726.5a,595.6b);
gpdraw(1,844.1a,535.3b);
gpdraw(1,844.1a,541.2b);
gpdraw(1,961.8a,484.7b);
gpdraw(1,1079.4a,445.1b);
gpdraw(1,1307.2a,757.1b);
linetype 2;
linetype -2;
put_text( btex K-space P3ME 1:1 etex, 1245.8a, 720.5b, 0, 3);
linetype 2;
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (491.3a,558.2b)--(608.9a,502.7b)--(726.5a,447.2b)--(726.5a,448.9b)--(844.1a,397.1b)
--(844.1a,428.3b)--(961.8a,384.7b)--(1079.4a,376.1b);
gpdraw(2,491.3a,558.2b);
gpdraw(2,608.9a,502.7b);
gpdraw(2,726.5a,447.2b);
gpdraw(2,726.5a,448.9b);
gpdraw(2,844.1a,397.1b);
gpdraw(2,844.1a,428.3b);
gpdraw(2,961.8a,384.7b);
gpdraw(2,1079.4a,376.1b);
gpdraw(2,1307.2a,720.5b);
linetype 3;
linetype -2;
put_text( btex K-space P3ME 1:4 etex, 1245.8a, 683.9b, 0, 3);
linetype 3;
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (491.3a,418.4b)--(608.9a,367.0b)--(726.5a,315.2b)--(726.5a,321.3b)--(844.1a,274.0b)
--(844.1a,301.5b)--(961.8a,259.9b)--(1079.4a,251.3b);
gpdraw(3,491.3a,418.4b);
gpdraw(3,608.9a,367.0b);
gpdraw(3,726.5a,315.2b);
gpdraw(3,726.5a,321.3b);
gpdraw(3,844.1a,274.0b);
gpdraw(3,844.1a,301.5b);
gpdraw(3,961.8a,259.9b);
gpdraw(3,1079.4a,251.3b);
gpdraw(3,1307.2a,683.9b);
linetype 4;
linetype -2;
put_text( btex Real Space (MPI SC) etex, 1245.8a, 647.3b, 0, 3);
linetype 4;
draw (1263.5a,647.3b)--(1350.9a,647.3b);
draw (491.3a,692.4b)--(608.9a,620.0b)--(726.5a,547.3b)--(844.1a,474.2b)--(961.8a,401.2b)
--(1079.4a,329.4b);
gpdraw(4,491.3a,692.4b);
gpdraw(4,608.9a,620.0b);
gpdraw(4,726.5a,547.3b);
gpdraw(4,844.1a,474.2b);
gpdraw(4,961.8a,401.2b);
gpdraw(4,1079.4a,329.4b);
gpdraw(4,1307.2a,647.3b);
linetype 5;
linetype -2;
put_text( btex Neighborhood Bcast etex, 1245.8a, 610.7b, 0, 3);
linetype 5;
draw (1263.5a,610.7b)--(1350.9a,610.7b);
draw (491.3a,357.3b)--(608.9a,318.1b)--(726.5a,295.8b)--(844.1a,277.8b)--(844.1a,279.2b)
--(961.8a,262.0b)--(1079.4a,253.3b);
gpdraw(5,491.3a,357.3b);
gpdraw(5,608.9a,318.1b);
gpdraw(5,726.5a,295.8b);
gpdraw(5,726.5a,295.8b);
gpdraw(5,844.1a,277.8b);
gpdraw(5,844.1a,279.2b);
gpdraw(5,961.8a,262.0b);
gpdraw(5,1079.4a,253.3b);
gpdraw(5,1307.2a,610.7b);
linetype 6;
linetype -2;
put_text( btex Neighborhood Reduce etex, 1245.8a, 574.1b, 0, 3);
linetype 6;
draw (1263.5a,574.1b)--(1350.9a,574.1b);
draw (491.3a,346.5b)--(608.9a,320.7b)--(726.5a,288.9b)--(726.5a,287.6b)--(844.1a,273.0b)
--(844.1a,269.9b)--(961.8a,249.9b)--(1079.4a,238.4b);
gpdraw(6,491.3a,346.5b);
gpdraw(6,608.9a,320.7b);
gpdraw(6,726.5a,288.9b);
gpdraw(6,726.5a,287.6b);
gpdraw(6,844.1a,273.0b);
gpdraw(6,844.1a,269.9b);
gpdraw(6,961.8a,249.9b);
gpdraw(6,1079.4a,238.4b);
gpdraw(6,1307.2a,574.1b);
linetype -2;
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
