%GNUPLOT Metapost output: Mon Apr 25 00:32:20 2005


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage[mtbold]{mathtime}
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

beginfig(8);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,163.9b)--(222.5a,163.9b);
draw (1386.3a,163.9b)--(1378.0a,163.9b);
draw (214.2a,195.5b)--(222.5a,195.5b);
draw (1386.3a,195.5b)--(1378.0a,195.5b);
draw (214.2a,218.0b)--(222.5a,218.0b);
draw (1386.3a,218.0b)--(1378.0a,218.0b);
draw (214.2a,235.4b)--(222.5a,235.4b);
draw (1386.3a,235.4b)--(1378.0a,235.4b);
draw (214.2a,249.6b)--(222.5a,249.6b);
draw (1386.3a,249.6b)--(1378.0a,249.6b);
draw (214.2a,261.7b)--(222.5a,261.7b);
draw (1386.3a,261.7b)--(1378.0a,261.7b);
draw (214.2a,272.1b)--(222.5a,272.1b);
draw (1386.3a,272.1b)--(1378.0a,272.1b);
draw (214.2a,281.3b)--(222.5a,281.3b);
draw (1386.3a,281.3b)--(1378.0a,281.3b);
draw (214.2a,289.5b)--(230.8a,289.5b);
draw (1386.3a,289.5b)--(1369.7a,289.5b);
put_text( btex  0.001 etex, 196.5a, 289.5b, 0, 3);
draw (214.2a,343.6b)--(222.5a,343.6b);
draw (1386.3a,343.6b)--(1378.0a,343.6b);
draw (214.2a,375.2b)--(222.5a,375.2b);
draw (1386.3a,375.2b)--(1378.0a,375.2b);
draw (214.2a,397.7b)--(222.5a,397.7b);
draw (1386.3a,397.7b)--(1378.0a,397.7b);
draw (214.2a,415.1b)--(222.5a,415.1b);
draw (1386.3a,415.1b)--(1378.0a,415.1b);
draw (214.2a,429.3b)--(222.5a,429.3b);
draw (1386.3a,429.3b)--(1378.0a,429.3b);
draw (214.2a,441.4b)--(222.5a,441.4b);
draw (1386.3a,441.4b)--(1378.0a,441.4b);
draw (214.2a,451.8b)--(222.5a,451.8b);
draw (1386.3a,451.8b)--(1378.0a,451.8b);
draw (214.2a,461.0b)--(222.5a,461.0b);
draw (1386.3a,461.0b)--(1378.0a,461.0b);
draw (214.2a,469.2b)--(230.8a,469.2b);
draw (1386.3a,469.2b)--(1369.7a,469.2b);
put_text( btex  0.01 etex, 196.5a, 469.2b, 0, 3);
draw (214.2a,523.3b)--(222.5a,523.3b);
draw (1386.3a,523.3b)--(1378.0a,523.3b);
draw (214.2a,554.9b)--(222.5a,554.9b);
draw (1386.3a,554.9b)--(1378.0a,554.9b);
draw (214.2a,577.4b)--(222.5a,577.4b);
draw (1386.3a,577.4b)--(1378.0a,577.4b);
draw (214.2a,594.8b)--(222.5a,594.8b);
draw (1386.3a,594.8b)--(1378.0a,594.8b);
draw (214.2a,609.0b)--(222.5a,609.0b);
draw (1386.3a,609.0b)--(1378.0a,609.0b);
draw (214.2a,621.1b)--(222.5a,621.1b);
draw (1386.3a,621.1b)--(1378.0a,621.1b);
draw (214.2a,631.5b)--(222.5a,631.5b);
draw (1386.3a,631.5b)--(1378.0a,631.5b);
draw (214.2a,640.7b)--(222.5a,640.7b);
draw (1386.3a,640.7b)--(1378.0a,640.7b);
draw (214.2a,648.9b)--(230.8a,648.9b);
draw (1386.3a,648.9b)--(1369.7a,648.9b);
put_text( btex  0.1 etex, 196.5a, 648.9b, 0, 3);
draw (214.2a,703.0b)--(222.5a,703.0b);
draw (1386.3a,703.0b)--(1378.0a,703.0b);
draw (214.2a,734.6b)--(222.5a,734.6b);
draw (1386.3a,734.6b)--(1378.0a,734.6b);
draw (214.2a,757.1b)--(222.5a,757.1b);
draw (1386.3a,757.1b)--(1378.0a,757.1b);
draw (214.2a,774.5b)--(222.5a,774.5b);
draw (1386.3a,774.5b)--(1378.0a,774.5b);
draw (214.2a,788.7b)--(222.5a,788.7b);
draw (1386.3a,788.7b)--(1378.0a,788.7b);
draw (214.2a,800.8b)--(222.5a,800.8b);
draw (1386.3a,800.8b)--(1378.0a,800.8b);
draw (214.2a,811.2b)--(222.5a,811.2b);
draw (1386.3a,811.2b)--(1378.0a,811.2b);
draw (214.2a,820.4b)--(222.5a,820.4b);
draw (1386.3a,820.4b)--(1378.0a,820.4b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1386.3a,828.6b)--(1369.7a,828.6b);
put_text( btex  1 etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  10 etex, 214.2a, 73.2b, 0, 2);
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
put_text( btex  100 etex, 604.9a, 73.2b, 0, 2);
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
put_text( btex  1000 etex, 995.6a, 73.2b, 0, 2);
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
put_text( btex  10000 etex, 1386.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (seconds) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Node Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Total Time Step etex, 1245.8a, 793.7b, 0, 3);
linetype 0;
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (411.6a,749.9b)--(646.8a,641.6b)--(882.0a,541.5b)--(999.6a,517.0b)--(1117.2a,470.9b)
--(1234.8a,460.6b);
gpdraw(0,411.6a,749.9b);
gpdraw(0,646.8a,641.6b);
gpdraw(0,882.0a,541.5b);
gpdraw(0,999.6a,517.0b);
gpdraw(0,1117.2a,470.9b);
gpdraw(0,1234.8a,460.6b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
linetype -2;
put_text( btex Reciprocal Space etex, 1245.8a, 757.1b, 0, 3);
linetype 1;
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (411.6a,666.2b)--(646.8a,581.0b)--(882.0a,466.3b)--(999.6a,437.7b)--(1117.2a,404.4b)
--(1234.8a,397.5b);
gpdraw(1,411.6a,666.2b);
gpdraw(1,646.8a,581.0b);
gpdraw(1,882.0a,466.3b);
gpdraw(1,999.6a,437.7b);
gpdraw(1,1117.2a,404.4b);
gpdraw(1,1234.8a,397.5b);
gpdraw(1,1307.2a,757.1b);
linetype 5;
linetype -2;
put_text( btex Real Space (from single core run) etex, 1245.8a, 720.5b, 0, 3);
linetype 5;
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (411.6a,727.8b)--(646.8a,619.6b)--(882.0a,505.2b)--(999.6a,453.4b)--(1234.8a,351.0b);
gpdraw(5,411.6a,727.8b);
gpdraw(5,646.8a,619.6b);
gpdraw(5,882.0a,505.2b);
gpdraw(5,999.6a,453.4b);
gpdraw(5,1234.8a,351.0b);
gpdraw(5,1307.2a,720.5b);
linetype 2;
linetype -2;
put_text( btex Local Force Reduction etex, 1245.8a, 683.9b, 0, 3);
linetype 2;
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (411.6a,583.8b)--(646.8a,489.8b)--(882.0a,429.2b)--(999.6a,420.7b)--(1117.2a,346.8b)
--(1234.8a,339.2b);
gpdraw(2,411.6a,583.8b);
gpdraw(2,646.8a,489.8b);
gpdraw(2,882.0a,429.2b);
gpdraw(2,999.6a,420.7b);
gpdraw(2,1117.2a,346.8b);
gpdraw(2,1234.8a,339.2b);
gpdraw(2,1307.2a,683.9b);
linetype 3;
linetype -2;
put_text( btex Local Position Broadcast etex, 1245.8a, 647.3b, 0, 3);
linetype 3;
draw (1263.5a,647.3b)--(1350.9a,647.3b);
draw (411.6a,447.1b)--(646.8a,412.7b)--(882.0a,301.1b)--(999.6a,342.6b)--(1117.2a,334.5b)
--(1234.8a,335.5b);
gpdraw(3,411.6a,447.1b);
gpdraw(3,646.8a,412.7b);
gpdraw(3,882.0a,301.1b);
gpdraw(3,999.6a,342.6b);
gpdraw(3,1117.2a,334.5b);
gpdraw(3,1234.8a,335.5b);
gpdraw(3,1307.2a,647.3b);
linetype 4;
linetype -2;
put_text( btex Bonded Forces etex, 1245.8a, 610.7b, 0, 3);
linetype 4;
draw (1263.5a,610.7b)--(1350.9a,610.7b);
draw (411.6a,501.9b)--(646.8a,394.2b)--(882.0a,286.3b)--(999.6a,234.6b)--(1117.2a,162.1b)
--(1234.8a,139.1b);
gpdraw(4,411.6a,501.9b);
gpdraw(4,646.8a,394.2b);
gpdraw(4,882.0a,286.3b);
gpdraw(4,999.6a,234.6b);
gpdraw(4,1117.2a,162.1b);
gpdraw(4,1234.8a,139.1b);
gpdraw(4,1307.2a,610.7b);
linetype -2;
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
