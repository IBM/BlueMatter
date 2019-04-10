%GNUPLOT Metapost output: Sun Apr 25 01:35:33 2004


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

% latex prologue to enable use of times-roman font and other facilities
verbatimtex
  \documentclass{article}
%  \usepackage[LY1]{fontenc}	% specify text font encoding
  %\usepackage{amsmath}
  %\usepackage[usenames,dvips]{color}
%  \usepackage[expert]{lucidabr}
  \usepackage{times}
  \begin{document}
  etex;

%font changes
verbatimtex
\def\setfont#1#2{%.
  \font\gpfont=#1 at #2pt
\gpfont}
\setfont{cmr10}{10.00}
etex

color currentcolor; currentcolor=black;
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
col[-2]:=col[-1]:=col0:=white;
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

beginfig(22);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  1e-05 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,145.9b)--(222.5a,145.9b);
draw (1386.3a,145.9b)--(1378.0a,145.9b);
draw (214.2a,193.5b)--(222.5a,193.5b);
draw (1386.3a,193.5b)--(1378.0a,193.5b);
draw (214.2a,218.0b)--(222.5a,218.0b);
draw (1386.3a,218.0b)--(1378.0a,218.0b);
draw (214.2a,229.6b)--(230.8a,229.6b);
draw (1386.3a,229.6b)--(1369.7a,229.6b);
put_text( btex  0.0001 etex, 196.5a, 229.6b, 0, 3);
draw (214.2a,265.7b)--(222.5a,265.7b);
draw (1386.3a,265.7b)--(1378.0a,265.7b);
draw (214.2a,313.3b)--(222.5a,313.3b);
draw (1386.3a,313.3b)--(1378.0a,313.3b);
draw (214.2a,337.8b)--(222.5a,337.8b);
draw (1386.3a,337.8b)--(1378.0a,337.8b);
draw (214.2a,349.4b)--(230.8a,349.4b);
draw (1386.3a,349.4b)--(1369.7a,349.4b);
put_text( btex  0.001 etex, 196.5a, 349.4b, 0, 3);
draw (214.2a,385.5b)--(222.5a,385.5b);
draw (1386.3a,385.5b)--(1378.0a,385.5b);
draw (214.2a,433.1b)--(222.5a,433.1b);
draw (1386.3a,433.1b)--(1378.0a,433.1b);
draw (214.2a,457.6b)--(222.5a,457.6b);
draw (1386.3a,457.6b)--(1378.0a,457.6b);
draw (214.2a,469.2b)--(230.8a,469.2b);
draw (1386.3a,469.2b)--(1369.7a,469.2b);
put_text( btex  0.01 etex, 196.5a, 469.2b, 0, 3);
draw (214.2a,505.3b)--(222.5a,505.3b);
draw (1386.3a,505.3b)--(1378.0a,505.3b);
draw (214.2a,552.9b)--(222.5a,552.9b);
draw (1386.3a,552.9b)--(1378.0a,552.9b);
draw (214.2a,577.4b)--(222.5a,577.4b);
draw (1386.3a,577.4b)--(1378.0a,577.4b);
draw (214.2a,589.0b)--(230.8a,589.0b);
draw (1386.3a,589.0b)--(1369.7a,589.0b);
put_text( btex  0.1 etex, 196.5a, 589.0b, 0, 3);
draw (214.2a,625.1b)--(222.5a,625.1b);
draw (1386.3a,625.1b)--(1378.0a,625.1b);
draw (214.2a,672.7b)--(222.5a,672.7b);
draw (1386.3a,672.7b)--(1378.0a,672.7b);
draw (214.2a,697.2b)--(222.5a,697.2b);
draw (1386.3a,697.2b)--(1378.0a,697.2b);
draw (214.2a,708.8b)--(230.8a,708.8b);
draw (1386.3a,708.8b)--(1369.7a,708.8b);
put_text( btex  1 etex, 196.5a, 708.8b, 0, 3);
draw (214.2a,744.9b)--(222.5a,744.9b);
draw (1386.3a,744.9b)--(1378.0a,744.9b);
draw (214.2a,792.5b)--(222.5a,792.5b);
draw (1386.3a,792.5b)--(1378.0a,792.5b);
draw (214.2a,817.0b)--(222.5a,817.0b);
draw (1386.3a,817.0b)--(1378.0a,817.0b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1386.3a,828.6b)--(1369.7a,828.6b);
put_text( btex  10 etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
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
put_text( btex  10 etex, 604.9a, 73.2b, 0, 2);
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
put_text( btex  100 etex, 995.6a, 73.2b, 0, 2);
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
put_text( btex  1000 etex, 1386.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Time (sec.) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Task Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
put_text( btex $128^3$ MPI Collective etex, 1245.8a, 793.7b, 0, 3);
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (214.2a,753.4b)--(567.0a,652.9b)--(802.3a,558.9b)--(1037.5a,490.1b)--(1155.1a,455.3b)
--(1272.7a,433.3b);
gpdraw(0,214.2a,753.4b);
gpdraw(0,567.0a,652.9b);
gpdraw(0,802.3a,558.9b);
gpdraw(0,1037.5a,490.1b);
gpdraw(0,1155.1a,455.3b);
gpdraw(0,1272.7a,433.3b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
put_text( btex $128^3$ Active Packet etex, 1245.8a, 757.1b, 0, 3);
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (567.0a,608.0b)--(802.3a,532.3b)--(1037.5a,476.0b)--(1272.7a,428.2b);
gpdraw(1,567.0a,608.0b);
gpdraw(1,802.3a,532.3b);
gpdraw(1,1037.5a,476.0b);
gpdraw(1,1272.7a,428.2b);
gpdraw(1,1307.2a,757.1b);
linetype 2;
put_text( btex $128^3$ Model etex, 1245.8a, 720.5b, 0, 3);
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (214.2a,601.0b)--(567.0a,563.0b)--(802.3a,519.7b)--(919.9a,483.6b)--(1037.5a,471.0b)
--(1155.1a,443.6b)--(1272.7a,407.5b)--(1386.3a,396.7b);
linetype 3;
put_text( btex $64^3$ MPI Collective etex, 1245.8a, 683.9b, 0, 3);
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (214.2a,623.9b)--(567.0a,506.8b)--(802.3a,428.8b)--(1037.5a,385.5b)--(1155.1a,359.2b)
--(1272.7a,356.2b);
gpdraw(2,214.2a,623.9b);
gpdraw(2,567.0a,506.8b);
gpdraw(2,802.3a,428.8b);
gpdraw(2,1037.5a,385.5b);
gpdraw(2,1155.1a,359.2b);
gpdraw(2,1272.7a,356.2b);
gpdraw(2,1307.2a,683.9b);
linetype 4;
put_text( btex $64^3$ Active Packet etex, 1245.8a, 647.3b, 0, 3);
draw (1263.5a,647.3b)--(1350.9a,647.3b);
draw (214.2a,608.0b)--(567.0a,490.3b)--(802.3a,425.9b)--(1037.5a,371.9b)--(1272.7a,322.3b);
gpdraw(3,214.2a,608.0b);
gpdraw(3,567.0a,490.3b);
gpdraw(3,802.3a,425.9b);
gpdraw(3,1037.5a,371.9b);
gpdraw(3,1272.7a,322.3b);
gpdraw(3,1307.2a,647.3b);
linetype 5;
put_text( btex $64^3$ Model etex, 1245.8a, 610.7b, 0, 3);
draw (1263.5a,610.7b)--(1350.9a,610.7b);
draw (214.2a,484.7b)--(567.0a,452.8b)--(802.3a,410.4b)--(919.9a,374.3b)--(1037.5a,362.1b)
--(1155.1a,334.8b)--(1272.7a,298.7b)--(1386.3a,288.1b);
linetype 6;
put_text( btex $32^3$ MPI Collective etex, 1245.8a, 574.1b, 0, 3);
draw (1263.5a,574.1b)--(1350.9a,574.1b);
draw (214.2a,462.1b)--(567.0a,386.8b)--(802.3a,321.7b)--(1037.5a,303.6b)--(1155.1a,305.9b);
gpdraw(4,214.2a,462.1b);
gpdraw(4,567.0a,386.8b);
gpdraw(4,802.3a,321.7b);
gpdraw(4,1037.5a,303.6b);
gpdraw(4,1155.1a,305.9b);
gpdraw(4,1307.2a,574.1b);
linetype 7;
put_text( btex $32^3$ Active Packet etex, 1245.8a, 537.5b, 0, 3);
draw (1263.5a,537.5b)--(1350.9a,537.5b);
draw (214.2a,489.6b)--(567.0a,381.4b)--(802.3a,321.5b)--(1037.5a,268.2b)--(1272.7a,245.6b);
gpdraw(5,214.2a,489.6b);
gpdraw(5,567.0a,381.4b);
gpdraw(5,802.3a,321.5b);
gpdraw(5,1037.5a,268.2b);
gpdraw(5,1272.7a,245.6b);
gpdraw(5,1307.2a,537.5b);
linetype 0;
put_text( btex $32^3$ Model etex, 1245.8a, 500.9b, 0, 3);
draw (1263.5a,500.9b)--(1350.9a,500.9b);
draw (214.2a,367.1b)--(567.0a,342.6b)--(802.3a,301.1b)--(919.9a,265.0b)--(1037.5a,253.2b)
--(1155.1a,226.0b)--(1272.7a,189.9b)--(1386.3a,179.5b);
endfig;
end.
b);
endfig;
end.
