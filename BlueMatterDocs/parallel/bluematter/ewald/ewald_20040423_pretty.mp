%GNUPLOT Metapost output: Mon Apr 26 00:45:39 2004


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

beginfig(32);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  $10^4$ etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,150.0b)--(222.5a,150.0b);
draw (1386.3a,150.0b)--(1378.0a,150.0b);
draw (214.2a,203.1b)--(222.5a,203.1b);
draw (1386.3a,203.1b)--(1378.0a,203.1b);
draw (214.2a,230.4b)--(222.5a,230.4b);
draw (1386.3a,230.4b)--(1378.0a,230.4b);
draw (214.2a,243.3b)--(230.8a,243.3b);
draw (1386.3a,243.3b)--(1369.7a,243.3b);
put_text( btex  $10^5$ etex, 196.5a, 243.3b, 0, 3);
draw (214.2a,283.5b)--(222.5a,283.5b);
draw (1386.3a,283.5b)--(1378.0a,283.5b);
draw (214.2a,336.7b)--(222.5a,336.7b);
draw (1386.3a,336.7b)--(1378.0a,336.7b);
draw (214.2a,363.9b)--(222.5a,363.9b);
draw (1386.3a,363.9b)--(1378.0a,363.9b);
draw (214.2a,376.9b)--(230.8a,376.9b);
draw (1386.3a,376.9b)--(1369.7a,376.9b);
put_text( btex  $10^6$ etex, 196.5a, 376.9b, 0, 3);
draw (214.2a,417.1b)--(222.5a,417.1b);
draw (1386.3a,417.1b)--(1378.0a,417.1b);
draw (214.2a,470.2b)--(222.5a,470.2b);
draw (1386.3a,470.2b)--(1378.0a,470.2b);
draw (214.2a,497.5b)--(222.5a,497.5b);
draw (1386.3a,497.5b)--(1378.0a,497.5b);
draw (214.2a,510.4b)--(230.8a,510.4b);
draw (1386.3a,510.4b)--(1369.7a,510.4b);
put_text( btex  $10^7$ etex, 196.5a, 510.4b, 0, 3);
draw (214.2a,550.6b)--(222.5a,550.6b);
draw (1386.3a,550.6b)--(1378.0a,550.6b);
draw (214.2a,603.8b)--(222.5a,603.8b);
draw (1386.3a,603.8b)--(1378.0a,603.8b);
draw (214.2a,631.0b)--(222.5a,631.0b);
draw (1386.3a,631.0b)--(1378.0a,631.0b);
draw (214.2a,644.0b)--(230.8a,644.0b);
draw (1386.3a,644.0b)--(1369.7a,644.0b);
put_text( btex  $10^8$ etex, 196.5a, 644.0b, 0, 3);
draw (214.2a,684.2b)--(222.5a,684.2b);
draw (1386.3a,684.2b)--(1378.0a,684.2b);
draw (214.2a,737.3b)--(222.5a,737.3b);
draw (1386.3a,737.3b)--(1378.0a,737.3b);
draw (214.2a,764.6b)--(222.5a,764.6b);
draw (1386.3a,764.6b)--(1378.0a,764.6b);
draw (214.2a,777.5b)--(230.8a,777.5b);
draw (1386.3a,777.5b)--(1369.7a,777.5b);
put_text( btex  $10^9$ etex, 196.5a, 777.5b, 0, 3);
draw (214.2a,817.7b)--(222.5a,817.7b);
draw (1386.3a,817.7b)--(1378.0a,817.7b);
draw (214.2a,870.9b)--(222.5a,870.9b);
draw (1386.3a,870.9b)--(1378.0a,870.9b);
draw (214.2a,898.1b)--(222.5a,898.1b);
draw (1386.3a,898.1b)--(1378.0a,898.1b);
draw (214.2a,911.1b)--(230.8a,911.1b);
draw (1386.3a,911.1b)--(1369.7a,911.1b);
put_text( btex  $10^{10}$ etex, 196.5a, 911.1b, 0, 3);
draw (214.2a,951.3b)--(222.5a,951.3b);
draw (1386.3a,951.3b)--(1378.0a,951.3b);
draw (214.2a,1004.4b)--(222.5a,1004.4b);
draw (1386.3a,1004.4b)--(1378.0a,1004.4b);
draw (214.2a,1031.7b)--(222.5a,1031.7b);
draw (1386.3a,1031.7b)--(1378.0a,1031.7b);
draw (214.2a,1044.6b)--(230.8a,1044.6b);
draw (1386.3a,1044.6b)--(1369.7a,1044.6b);
put_text( btex  $10^{11}$ etex, 196.5a, 1044.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,1044.6b)--(214.2a,1028.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
draw (331.8a,109.8b)--(331.8a,118.1b);
draw (331.8a,1044.6b)--(331.8a,1036.3b);
draw (400.6a,109.8b)--(400.6a,118.1b);
draw (400.6a,1044.6b)--(400.6a,1036.3b);
draw (449.4a,109.8b)--(449.4a,118.1b);
draw (449.4a,1044.6b)--(449.4a,1036.3b);
draw (487.3a,109.8b)--(487.3a,118.1b);
draw (487.3a,1044.6b)--(487.3a,1036.3b);
draw (518.2a,109.8b)--(518.2a,118.1b);
draw (518.2a,1044.6b)--(518.2a,1036.3b);
draw (544.4a,109.8b)--(544.4a,118.1b);
draw (544.4a,1044.6b)--(544.4a,1036.3b);
draw (567.0a,109.8b)--(567.0a,118.1b);
draw (567.0a,1044.6b)--(567.0a,1036.3b);
draw (587.0a,109.8b)--(587.0a,118.1b);
draw (587.0a,1044.6b)--(587.0a,1036.3b);
draw (604.9a,109.8b)--(604.9a,126.4b);
draw (604.9a,1044.6b)--(604.9a,1028.0b);
put_text( btex  10 etex, 604.9a, 73.2b, 0, 2);
draw (722.5a,109.8b)--(722.5a,118.1b);
draw (722.5a,1044.6b)--(722.5a,1036.3b);
draw (791.3a,109.8b)--(791.3a,118.1b);
draw (791.3a,1044.6b)--(791.3a,1036.3b);
draw (840.1a,109.8b)--(840.1a,118.1b);
draw (840.1a,1044.6b)--(840.1a,1036.3b);
draw (878.0a,109.8b)--(878.0a,118.1b);
draw (878.0a,1044.6b)--(878.0a,1036.3b);
draw (908.9a,109.8b)--(908.9a,118.1b);
draw (908.9a,1044.6b)--(908.9a,1036.3b);
draw (935.1a,109.8b)--(935.1a,118.1b);
draw (935.1a,1044.6b)--(935.1a,1036.3b);
draw (957.7a,109.8b)--(957.7a,118.1b);
draw (957.7a,1044.6b)--(957.7a,1036.3b);
draw (977.7a,109.8b)--(977.7a,118.1b);
draw (977.7a,1044.6b)--(977.7a,1036.3b);
draw (995.6a,109.8b)--(995.6a,126.4b);
draw (995.6a,1044.6b)--(995.6a,1028.0b);
put_text( btex  100 etex, 995.6a, 73.2b, 0, 2);
draw (1113.2a,109.8b)--(1113.2a,118.1b);
draw (1113.2a,1044.6b)--(1113.2a,1036.3b);
draw (1182.0a,109.8b)--(1182.0a,118.1b);
draw (1182.0a,1044.6b)--(1182.0a,1036.3b);
draw (1230.8a,109.8b)--(1230.8a,118.1b);
draw (1230.8a,1044.6b)--(1230.8a,1036.3b);
draw (1268.7a,109.8b)--(1268.7a,118.1b);
draw (1268.7a,1044.6b)--(1268.7a,1036.3b);
draw (1299.6a,109.8b)--(1299.6a,118.1b);
draw (1299.6a,1044.6b)--(1299.6a,1036.3b);
draw (1325.8a,109.8b)--(1325.8a,118.1b);
draw (1325.8a,1044.6b)--(1325.8a,1036.3b);
draw (1348.4a,109.8b)--(1348.4a,118.1b);
draw (1348.4a,1044.6b)--(1348.4a,1036.3b);
draw (1368.4a,109.8b)--(1368.4a,118.1b);
draw (1368.4a,1044.6b)--(1368.4a,1036.3b);
draw (1386.3a,109.8b)--(1386.3a,126.4b);
draw (1386.3a,1044.6b)--(1386.3a,1028.0b);
put_text( btex  1000 etex, 1386.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,1044.6b)--(214.2a,1044.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (pclocks) etex, 36.6a, 577.2b, 1, 2);
put_text( btex Node Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
put_text( btex Total Time Step etex, 1245.8a, 1009.7b, 0, 3);
draw (1263.5a,1009.7b)--(1350.9a,1009.7b);
draw (1263.5a,1018.0b)--(1263.5a,1001.4b);
draw (1350.9a,1018.0b)--(1350.9a,1001.4b);
draw (214.2a,921.6b)--(214.2a,922.0b);
draw (205.9a,921.6b)--(222.5a,921.6b);
draw (205.9a,922.0b)--(222.5a,922.0b);
draw (567.0a,801.7b)--(567.0a,802.2b);
draw (558.7a,801.7b)--(575.3a,801.7b);
draw (558.7a,802.2b)--(575.3a,802.2b);
draw (802.3a,672.4b)--(802.3a,673.9b);
draw (794.0a,672.4b)--(810.6a,672.4b);
draw (794.0a,673.9b)--(810.6a,673.9b);
draw (1037.5a,574.2b)--(1037.5a,577.7b);
draw (1029.2a,574.2b)--(1045.8a,574.2b);
draw (1029.2a,577.7b)--(1045.8a,577.7b);
draw (1155.1a,548.5b)--(1155.1a,553.3b);
draw (1146.8a,548.5b)--(1163.4a,548.5b);
draw (1146.8a,553.3b)--(1163.4a,553.3b);
draw (1272.7a,531.9b)--(1272.7a,537.6b);
draw (1264.4a,531.9b)--(1281.0a,531.9b);
draw (1264.4a,537.6b)--(1281.0a,537.6b);
gpdraw(0,214.2a,921.8b);
gpdraw(0,567.0a,801.9b);
gpdraw(0,802.3a,673.0b);
gpdraw(0,1037.5a,575.7b);
gpdraw(0,1155.1a,550.6b);
gpdraw(0,1272.7a,534.3b);
gpdraw(0,1307.2a,1009.7b);
linetype 1;
put_text( btex Ewald\_Eiks etex, 1245.8a, 973.1b, 0, 3);
draw (1263.5a,973.1b)--(1350.9a,973.1b);
draw (1263.5a,981.4b)--(1263.5a,964.8b);
draw (1350.9a,981.4b)--(1350.9a,964.8b);
draw (214.2a,528.3b)--(214.2a,528.6b);
draw (205.9a,528.3b)--(222.5a,528.3b);
draw (205.9a,528.6b)--(222.5a,528.6b);
draw (567.0a,408.7b)--(567.0a,410.8b);
draw (558.7a,408.7b)--(575.3a,408.7b);
draw (558.7a,410.8b)--(575.3a,410.8b);
draw (802.3a,330.0b)--(802.3a,335.6b);
draw (794.0a,330.0b)--(810.6a,330.0b);
draw (794.0a,335.6b)--(810.6a,335.6b);
draw (1037.5a,252.1b)--(1037.5a,266.8b);
draw (1029.2a,252.1b)--(1045.8a,252.1b);
draw (1029.2a,266.8b)--(1045.8a,266.8b);
draw (1155.1a,219.7b)--(1155.1a,237.0b);
draw (1146.8a,219.7b)--(1163.4a,219.7b);
draw (1146.8a,237.0b)--(1163.4a,237.0b);
draw (1272.7a,190.1b)--(1272.7a,218.4b);
draw (1264.4a,190.1b)--(1281.0a,190.1b);
draw (1264.4a,218.4b)--(1281.0a,218.4b);
gpdraw(1,214.2a,528.5b);
gpdraw(1,567.0a,409.6b);
gpdraw(1,802.3a,332.8b);
gpdraw(1,1037.5a,260.7b);
gpdraw(1,1155.1a,230.8b);
gpdraw(1,1272.7a,206.2b);
gpdraw(1,1307.2a,973.1b);
put_text( btex Ewald\_AllReduce etex, 1245.8a, 936.5b, 0, 3);
draw (1263.5a,936.5b)--(1350.9a,936.5b);
draw (1263.5a,944.8b)--(1263.5a,928.2b);
draw (1350.9a,944.8b)--(1350.9a,928.2b);
draw (214.2a,223.2b)--(214.2a,225.5b);
draw (205.9a,223.2b)--(222.5a,223.2b);
draw (205.9a,225.5b)--(222.5a,225.5b);
draw (567.0a,373.5b)--(567.0a,567.1b);
draw (558.7a,373.5b)--(575.3a,373.5b);
draw (558.7a,567.1b)--(575.3a,567.1b);
draw (802.3a,373.5b)--(802.3a,520.0b);
draw (794.0a,373.5b)--(810.6a,373.5b);
draw (794.0a,520.0b)--(810.6a,520.0b);
draw (1037.5a,380.6b)--(1037.5a,451.3b);
draw (1029.2a,380.6b)--(1045.8a,380.6b);
draw (1029.2a,451.3b)--(1045.8a,451.3b);
draw (1155.1a,384.5b)--(1155.1a,443.1b);
draw (1146.8a,384.5b)--(1163.4a,384.5b);
draw (1146.8a,443.1b)--(1163.4a,443.1b);
draw (1272.7a,397.1b)--(1272.7a,457.2b);
draw (1264.4a,397.1b)--(1281.0a,397.1b);
draw (1264.4a,457.2b)--(1281.0a,457.2b);
gpdraw(7,214.2a,224.9b);
gpdraw(7,567.0a,528.0b);
gpdraw(7,802.3a,494.5b);
gpdraw(7,1037.5a,411.8b);
gpdraw(7,1155.1a,408.2b);
gpdraw(7,1272.7a,420.2b);
gpdraw(7,1307.2a,936.5b);
linetype 2;
put_text( btex Ewald\_Forces etex, 1245.8a, 899.9b, 0, 3);
draw (1263.5a,899.9b)--(1350.9a,899.9b);
draw (1263.5a,908.2b)--(1263.5a,891.6b);
draw (1350.9a,908.2b)--(1350.9a,891.6b);
draw (205.9a,816.5b)--(222.5a,816.5b);
draw (205.9a,816.5b)--(222.5a,816.5b);
draw (567.0a,693.1b)--(567.0a,696.0b);
draw (558.7a,693.1b)--(575.3a,693.1b);
draw (558.7a,696.0b)--(575.3a,696.0b);
draw (802.3a,561.4b)--(802.3a,566.5b);
draw (794.0a,561.4b)--(810.6a,561.4b);
draw (794.0a,566.5b)--(810.6a,566.5b);
draw (1037.5a,448.1b)--(1037.5a,458.3b);
draw (1029.2a,448.1b)--(1045.8a,448.1b);
draw (1029.2a,458.3b)--(1045.8a,458.3b);
draw (1155.1a,402.8b)--(1155.1a,420.3b);
draw (1146.8a,402.8b)--(1163.4a,402.8b);
draw (1146.8a,420.3b)--(1163.4a,420.3b);
draw (1272.7a,339.5b)--(1272.7a,384.4b);
draw (1264.4a,339.5b)--(1281.0a,339.5b);
draw (1264.4a,384.4b)--(1281.0a,384.4b);
gpdraw(2,214.2a,816.5b);
gpdraw(2,567.0a,694.4b);
gpdraw(2,802.3a,563.3b);
gpdraw(2,1037.5a,455.4b);
gpdraw(2,1155.1a,415.8b);
gpdraw(2,1272.7a,374.8b);
gpdraw(2,1307.2a,899.9b);
linetype 3;
put_text( btex Ewald\_KSpace etex, 1245.8a, 863.3b, 0, 3);
draw (1263.5a,863.3b)--(1350.9a,863.3b);
draw (1263.5a,871.6b)--(1263.5a,855.0b);
draw (1350.9a,871.6b)--(1350.9a,855.0b);
draw (205.9a,917.3b)--(222.5a,917.3b);
draw (205.9a,917.3b)--(222.5a,917.3b);
draw (567.0a,797.0b)--(567.0a,797.3b);
draw (558.7a,797.0b)--(575.3a,797.0b);
draw (558.7a,797.3b)--(575.3a,797.3b);
draw (802.3a,658.5b)--(802.3a,659.3b);
draw (794.0a,658.5b)--(810.6a,658.5b);
draw (794.0a,659.3b)--(810.6a,659.3b);
draw (1037.5a,539.0b)--(1037.5a,541.5b);
draw (1029.2a,539.0b)--(1045.8a,539.0b);
draw (1029.2a,541.5b)--(1045.8a,541.5b);
draw (1155.1a,504.0b)--(1155.1a,508.1b);
draw (1146.8a,504.0b)--(1163.4a,504.0b);
draw (1146.8a,508.1b)--(1163.4a,508.1b);
draw (1272.7a,477.1b)--(1272.7a,488.1b);
draw (1264.4a,477.1b)--(1281.0a,477.1b);
draw (1264.4a,488.1b)--(1281.0a,488.1b);
gpdraw(3,214.2a,917.3b);
gpdraw(3,567.0a,797.1b);
gpdraw(3,802.3a,658.8b);
gpdraw(3,1037.5a,539.9b);
gpdraw(3,1155.1a,505.7b);
gpdraw(3,1272.7a,480.2b);
gpdraw(3,1307.2a,863.3b);
linetype 4;
put_text( btex Ewald\_LocalReduce etex, 1245.8a, 826.7b, 0, 3);
draw (1263.5a,826.7b)--(1350.9a,826.7b);
draw (1263.5a,835.0b)--(1263.5a,818.4b);
draw (1350.9a,835.0b)--(1350.9a,818.4b);
draw (205.9a,816.5b)--(222.5a,816.5b);
draw (205.9a,816.5b)--(222.5a,816.5b);
draw (567.0a,693.1b)--(567.0a,696.0b);
draw (558.7a,693.1b)--(575.3a,693.1b);
draw (558.7a,696.0b)--(575.3a,696.0b);
draw (802.3a,561.4b)--(802.3a,566.5b);
draw (794.0a,561.4b)--(810.6a,561.4b);
draw (794.0a,566.5b)--(810.6a,566.5b);
draw (1037.5a,448.1b)--(1037.5a,458.3b);
draw (1029.2a,448.1b)--(1045.8a,448.1b);
draw (1029.2a,458.3b)--(1045.8a,458.3b);
draw (1155.1a,402.8b)--(1155.1a,420.3b);
draw (1146.8a,402.8b)--(1163.4a,402.8b);
draw (1146.8a,420.3b)--(1163.4a,420.3b);
draw (1272.7a,339.5b)--(1272.7a,384.4b);
draw (1264.4a,339.5b)--(1281.0a,339.5b);
draw (1264.4a,384.4b)--(1281.0a,384.4b);
gpdraw(4,214.2a,816.5b);
gpdraw(4,567.0a,694.4b);
gpdraw(4,802.3a,563.3b);
gpdraw(4,1037.5a,455.4b);
gpdraw(4,1155.1a,415.8b);
gpdraw(4,1272.7a,374.8b);
gpdraw(4,1307.2a,826.7b);
linetype 6;
put_text( btex GlobalizePositions\_Cntl etex, 1245.8a, 790.1b, 0, 3);
draw (1263.5a,790.1b)--(1350.9a,790.1b);
draw (1263.5a,798.4b)--(1263.5a,781.8b);
draw (1350.9a,798.4b)--(1350.9a,781.8b);
draw (214.2a,306.8b)--(214.2a,307.0b);
draw (205.9a,306.8b)--(222.5a,306.8b);
draw (205.9a,307.0b)--(222.5a,307.0b);
draw (567.0a,424.4b)--(567.0a,445.0b);
draw (558.7a,424.4b)--(575.3a,424.4b);
draw (558.7a,445.0b)--(575.3a,445.0b);
draw (802.3a,432.8b)--(802.3a,454.7b);
draw (794.0a,432.8b)--(810.6a,432.8b);
draw (794.0a,454.7b)--(810.6a,454.7b);
draw (1037.5a,430.7b)--(1037.5a,455.1b);
draw (1029.2a,430.7b)--(1045.8a,430.7b);
draw (1029.2a,455.1b)--(1045.8a,455.1b);
draw (1155.1a,432.5b)--(1155.1a,454.4b);
draw (1146.8a,432.5b)--(1163.4a,432.5b);
draw (1146.8a,454.4b)--(1163.4a,454.4b);
draw (1272.7a,432.8b)--(1272.7a,455.6b);
draw (1264.4a,432.8b)--(1281.0a,432.8b);
draw (1264.4a,455.6b)--(1281.0a,455.6b);
gpdraw(6,214.2a,306.9b);
gpdraw(6,567.0a,436.6b);
gpdraw(6,802.3a,442.8b);
gpdraw(6,1037.5a,444.4b);
gpdraw(6,1155.1a,444.9b);
gpdraw(6,1272.7a,446.0b);
gpdraw(6,1307.2a,790.1b);
linetype 5;
put_text( btex P2\_NSQ\_Control etex, 1245.8a, 753.5b, 0, 3);
draw (1263.5a,753.5b)--(1350.9a,753.5b);
draw (1263.5a,761.8b)--(1263.5a,745.2b);
draw (1350.9a,761.8b)--(1350.9a,745.2b);
draw (214.2a,767.3b)--(214.2a,773.7b);
draw (205.9a,767.3b)--(222.5a,767.3b);
draw (205.9a,773.7b)--(222.5a,773.7b);
draw (567.0a,646.8b)--(567.0a,655.0b);
draw (558.7a,646.8b)--(575.3a,646.8b);
draw (558.7a,655.0b)--(575.3a,655.0b);
draw (802.3a,566.3b)--(802.3a,576.0b);
draw (794.0a,566.3b)--(810.6a,566.3b);
draw (794.0a,576.0b)--(810.6a,576.0b);
draw (1037.5a,486.3b)--(1037.5a,502.5b);
draw (1029.2a,486.3b)--(1045.8a,486.3b);
draw (1029.2a,502.5b)--(1045.8a,502.5b);
draw (1155.1a,446.4b)--(1155.1a,470.0b);
draw (1146.8a,446.4b)--(1163.4a,446.4b);
draw (1146.8a,470.0b)--(1163.4a,470.0b);
draw (1272.7a,408.1b)--(1272.7a,443.7b);
draw (1264.4a,408.1b)--(1281.0a,408.1b);
draw (1264.4a,443.7b)--(1281.0a,443.7b);
gpdraw(3,214.2a,770.1b);
gpdraw(3,567.0a,649.9b);
gpdraw(3,802.3a,570.3b);
gpdraw(3,1037.5a,492.5b);
gpdraw(3,1155.1a,456.1b);
gpdraw(3,1272.7a,422.8b);
gpdraw(3,1307.2a,753.5b);
linetype 1;
put_text( btex ReduceForces\_Cntl etex, 1245.8a, 716.9b, 0, 3);
draw (1263.5a,716.9b)--(1350.9a,716.9b);
draw (1263.5a,725.2b)--(1263.5a,708.6b);
draw (1350.9a,725.2b)--(1350.9a,708.6b);
draw (214.2a,301.3b)--(214.2a,301.6b);
draw (205.9a,301.3b)--(222.5a,301.3b);
draw (205.9a,301.6b)--(222.5a,301.6b);
draw (567.0a,429.4b)--(567.0a,477.1b);
draw (558.7a,429.4b)--(575.3a,429.4b);
draw (558.7a,477.1b)--(575.3a,477.1b);
draw (802.3a,437.3b)--(802.3a,461.7b);
draw (794.0a,437.3b)--(810.6a,437.3b);
draw (794.0a,461.7b)--(810.6a,461.7b);
draw (1037.5a,430.8b)--(1037.5a,448.3b);
draw (1029.2a,430.8b)--(1045.8a,430.8b);
draw (1029.2a,448.3b)--(1045.8a,448.3b);
draw (1155.1a,431.5b)--(1155.1a,448.7b);
draw (1146.8a,431.5b)--(1163.4a,431.5b);
draw (1146.8a,448.7b)--(1163.4a,448.7b);
draw (1272.7a,432.2b)--(1272.7a,449.1b);
draw (1264.4a,432.2b)--(1281.0a,432.2b);
draw (1264.4a,449.1b)--(1281.0a,449.1b);
gpdraw(9,214.2a,301.5b);
gpdraw(9,567.0a,458.2b);
gpdraw(9,802.3a,454.1b);
gpdraw(9,1037.5a,441.6b);
gpdraw(9,1155.1a,441.6b);
gpdraw(9,1272.7a,441.7b);
gpdraw(9,1307.2a,716.9b);
linetype 3;
put_text( btex UDF\_StdHarmonicBondForce etex, 1245.8a, 680.3b, 0, 3);
draw (1263.5a,680.3b)--(1350.9a,680.3b);
draw (1263.5a,688.6b)--(1263.5a,672.0b);
draw (1350.9a,688.6b)--(1350.9a,672.0b);
draw (214.2a,475.7b)--(214.2a,475.8b);
draw (205.9a,475.7b)--(222.5a,475.7b);
draw (205.9a,475.8b)--(222.5a,475.8b);
draw (567.0a,348.9b)--(567.0a,376.7b);
draw (558.7a,348.9b)--(575.3a,348.9b);
draw (558.7a,376.7b)--(575.3a,376.7b);
draw (802.3a,260.8b)--(802.3a,308.5b);
draw (794.0a,260.8b)--(810.6a,260.8b);
draw (794.0a,308.5b)--(810.6a,308.5b);
draw (1037.5a,208.2b)--(1037.5a,271.0b);
draw (1029.2a,208.2b)--(1045.8a,208.2b);
draw (1029.2a,271.0b)--(1045.8a,271.0b);
draw (1155.1a,194.0b)--(1155.1a,262.9b);
draw (1146.8a,194.0b)--(1163.4a,194.0b);
draw (1146.8a,262.9b)--(1163.4a,262.9b);
draw (1272.7a,183.6b)--(1272.7a,242.6b);
draw (1264.4a,183.6b)--(1281.0a,183.6b);
draw (1264.4a,242.6b)--(1281.0a,242.6b);
gpdraw(1,214.2a,475.8b);
gpdraw(1,567.0a,360.1b);
gpdraw(1,802.3a,290.4b);
gpdraw(1,1037.5a,232.9b);
gpdraw(1,1155.1a,212.9b);
gpdraw(1,1272.7a,198.0b);
gpdraw(1,1307.2a,680.3b);
linetype 0;
draw (214.2a,921.8b)--(567.0a,801.9b)--(802.3a,673.0b)--(1037.5a,575.7b)--(1155.1a,550.6b)
--(1272.7a,534.3b);
linetype 1;
draw (214.2a,528.5b)--(567.0a,409.6b)--(802.3a,332.8b)--(1037.5a,260.7b)--(1155.1a,230.8b)
--(1272.7a,206.2b);
draw (214.2a,224.9b)--(567.0a,528.0b)--(802.3a,494.5b)--(1037.5a,411.8b)--(1155.1a,408.2b)
--(1272.7a,420.2b);
linetype 2;
draw (214.2a,816.5b)--(567.0a,694.4b)--(802.3a,563.3b)--(1037.5a,455.4b)--(1155.1a,415.8b)
--(1272.7a,374.8b);
linetype 3;
draw (214.2a,917.3b)--(567.0a,797.1b)--(802.3a,658.8b)--(1037.5a,539.9b)--(1155.1a,505.7b)
--(1272.7a,480.2b);
linetype 4;
draw (214.2a,816.5b)--(567.0a,694.4b)--(802.3a,563.3b)--(1037.5a,455.4b)--(1155.1a,415.8b)
--(1272.7a,374.8b);
linetype 6;
draw (214.2a,306.9b)--(567.0a,436.6b)--(802.3a,442.8b)--(1037.5a,444.4b)--(1155.1a,444.9b)
--(1272.7a,446.0b);
linetype 5;
draw (214.2a,770.1b)--(567.0a,649.9b)--(802.3a,570.3b)--(1037.5a,492.5b)--(1155.1a,456.1b)
--(1272.7a,422.8b);
linetype 1;
draw (214.2a,301.5b)--(567.0a,458.2b)--(802.3a,454.1b)--(1037.5a,441.6b)--(1155.1a,441.6b)
--(1272.7a,441.7b);
linetype 3;
draw (214.2a,475.8b)--(567.0a,360.1b)--(802.3a,290.4b)--(1037.5a,232.9b)--(1155.1a,212.9b)
--(1272.7a,198.0b);
endfig;
end.
b);
endfig;
end.
