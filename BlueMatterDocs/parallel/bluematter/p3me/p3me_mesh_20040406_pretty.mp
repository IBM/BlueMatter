%GNUPLOT Metapost output: Mon Apr 26 01:33:12 2004


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

beginfig(1);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  $10^4$ etex, 196.5a, 109.8b, 0, 3);
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
put_text( btex  $10^5$ etex, 196.5a, 289.5b, 0, 3);
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
put_text( btex  $10^6$ etex, 196.5a, 469.2b, 0, 3);
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
put_text( btex  $10^7$ etex, 196.5a, 648.9b, 0, 3);
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
put_text( btex  $10^8$ etex, 196.5a, 828.6b, 0, 3);
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
put_text( btex Elapsed Time (pclocks) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Node Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
put_text( btex Total Time Step etex, 656.0a, 680.5b, 0, 3);
draw (673.7a,680.5b)--(761.1a,680.5b);
draw (673.7a,688.8b)--(673.7a,672.2b);
draw (761.1a,688.8b)--(761.1a,672.2b);
draw (802.3a,790.0b)--(802.3a,796.1b);
draw (794.0a,790.0b)--(810.6a,790.0b);
draw (794.0a,796.1b)--(810.6a,796.1b);
draw (1155.1a,680.6b)--(1155.1a,692.3b);
draw (1146.8a,680.6b)--(1163.4a,680.6b);
draw (1146.8a,692.3b)--(1163.4a,692.3b);
draw (1272.7a,659.8b)--(1272.7a,673.8b);
draw (1264.4a,659.8b)--(1281.0a,659.8b);
draw (1264.4a,673.8b)--(1281.0a,673.8b);
gpdraw(0,802.3a,792.6b);
gpdraw(0,1155.1a,686.3b);
gpdraw(0,1272.7a,666.4b);
gpdraw(0,717.4a,680.5b);
linetype 2;
put_text( btex GlobalizePositions\_Cntl etex, 656.0a, 643.9b, 0, 3);
draw (673.7a,643.9b)--(761.1a,643.9b);
draw (673.7a,652.2b)--(673.7a,635.6b);
draw (761.1a,652.2b)--(761.1a,635.6b);
draw (802.3a,547.5b)--(802.3a,579.5b);
draw (794.0a,547.5b)--(810.6a,547.5b);
draw (794.0a,579.5b)--(810.6a,579.5b);
draw (1155.1a,543.8b)--(1155.1a,577.1b);
draw (1146.8a,543.8b)--(1163.4a,543.8b);
draw (1146.8a,577.1b)--(1163.4a,577.1b);
draw (1272.7a,545.8b)--(1272.7a,578.0b);
draw (1264.4a,545.8b)--(1281.0a,545.8b);
draw (1264.4a,578.0b)--(1281.0a,578.0b);
gpdraw(8,802.3a,563.7b);
gpdraw(8,1155.1a,562.1b);
gpdraw(8,1272.7a,563.4b);
gpdraw(8,717.4a,643.9b);
linetype 1;
put_text( btex P2\_KSpace\_Control etex, 656.0a, 607.3b, 0, 3);
draw (673.7a,607.3b)--(761.1a,607.3b);
draw (673.7a,615.6b)--(673.7a,599.0b);
draw (761.1a,615.6b)--(761.1a,599.0b);
draw (802.3a,713.2b)--(802.3a,717.0b);
draw (794.0a,713.2b)--(810.6a,713.2b);
draw (794.0a,717.0b)--(810.6a,717.0b);
draw (1155.1a,584.2b)--(1155.1a,595.3b);
draw (1146.8a,584.2b)--(1163.4a,584.2b);
draw (1146.8a,595.3b)--(1163.4a,595.3b);
draw (1272.7a,550.6b)--(1272.7a,562.9b);
draw (1264.4a,550.6b)--(1281.0a,550.6b);
draw (1264.4a,562.9b)--(1281.0a,562.9b);
gpdraw(1,802.3a,715.1b);
gpdraw(1,1155.1a,590.2b);
gpdraw(1,1272.7a,557.4b);
gpdraw(1,717.4a,607.3b);
linetype 2;
put_text( btex P2\_NSQ\_Control etex, 656.0a, 570.7b, 0, 3);
draw (673.7a,570.7b)--(761.1a,570.7b);
draw (673.7a,579.0b)--(673.7a,562.4b);
draw (761.1a,579.0b)--(761.1a,562.4b);
draw (802.3a,724.1b)--(802.3a,735.6b);
draw (794.0a,724.1b)--(810.6a,724.1b);
draw (794.0a,735.6b)--(810.6a,735.6b);
draw (1155.1a,562.8b)--(1155.1a,592.2b);
draw (1146.8a,562.8b)--(1163.4a,562.8b);
draw (1146.8a,592.2b)--(1163.4a,592.2b);
draw (1272.7a,511.1b)--(1272.7a,553.9b);
draw (1264.4a,511.1b)--(1281.0a,511.1b);
draw (1264.4a,553.9b)--(1281.0a,553.9b);
gpdraw(2,802.3a,729.0b);
gpdraw(2,1155.1a,576.4b);
gpdraw(2,1272.7a,531.5b);
gpdraw(2,717.4a,570.7b);
linetype 3;
put_text( btex P3ME\_AssignCharge\_Control etex, 656.0a, 534.1b, 0, 3);
draw (673.7a,534.1b)--(761.1a,534.1b);
draw (673.7a,542.4b)--(673.7a,525.8b);
draw (761.1a,542.4b)--(761.1a,525.8b);
draw (802.3a,447.0b)--(802.3a,457.7b);
draw (794.0a,447.0b)--(810.6a,447.0b);
draw (794.0a,457.7b)--(810.6a,457.7b);
draw (1155.1a,377.6b)--(1155.1a,394.6b);
draw (1146.8a,377.6b)--(1163.4a,377.6b);
draw (1146.8a,394.6b)--(1163.4a,394.6b);
draw (1272.7a,368.3b)--(1272.7a,387.0b);
draw (1264.4a,368.3b)--(1281.0a,368.3b);
draw (1264.4a,387.0b)--(1281.0a,387.0b);
gpdraw(3,802.3a,453.1b);
gpdraw(3,1155.1a,384.7b);
gpdraw(3,1272.7a,375.7b);
gpdraw(3,717.4a,534.1b);
linetype 4;
put_text( btex P3ME\_Convolve etex, 656.0a, 497.5b, 0, 3);
draw (673.7a,497.5b)--(761.1a,497.5b);
draw (673.7a,505.8b)--(673.7a,489.2b);
draw (761.1a,505.8b)--(761.1a,489.2b);
draw (802.3a,421.7b)--(802.3a,517.1b);
draw (794.0a,421.7b)--(810.6a,421.7b);
draw (794.0a,517.1b)--(810.6a,517.1b);
draw (1155.1a,248.1b)--(1155.1a,335.0b);
draw (1146.8a,248.1b)--(1163.4a,248.1b);
draw (1146.8a,335.0b)--(1163.4a,335.0b);
draw (1272.7a,204.4b)--(1272.7a,273.2b);
draw (1264.4a,204.4b)--(1281.0a,204.4b);
draw (1264.4a,273.2b)--(1281.0a,273.2b);
gpdraw(4,802.3a,482.5b);
gpdraw(4,1155.1a,299.9b);
gpdraw(4,1272.7a,245.8b);
gpdraw(4,717.4a,497.5b);
linetype 5;
put_text( btex P3ME\_ForwardFFT\_Control etex, 656.0a, 460.9b, 0, 3);
draw (673.7a,460.9b)--(761.1a,460.9b);
draw (673.7a,469.2b)--(673.7a,452.6b);
draw (761.1a,469.2b)--(761.1a,452.6b);
draw (802.3a,643.5b)--(802.3a,652.1b);
draw (794.0a,643.5b)--(810.6a,643.5b);
draw (794.0a,652.1b)--(810.6a,652.1b);
draw (1155.1a,512.1b)--(1155.1a,536.2b);
draw (1146.8a,512.1b)--(1163.4a,512.1b);
draw (1146.8a,536.2b)--(1163.4a,536.2b);
draw (1272.7a,474.7b)--(1272.7a,503.0b);
draw (1264.4a,474.7b)--(1281.0a,474.7b);
draw (1264.4a,503.0b)--(1281.0a,503.0b);
gpdraw(5,802.3a,647.8b);
gpdraw(5,1155.1a,525.2b);
gpdraw(5,1272.7a,491.6b);
gpdraw(5,717.4a,460.9b);
linetype 6;
put_text( btex P3ME\_Interpolate\_Control etex, 656.0a, 424.3b, 0, 3);
draw (673.7a,424.3b)--(761.1a,424.3b);
draw (673.7a,432.6b)--(673.7a,416.0b);
draw (761.1a,432.6b)--(761.1a,416.0b);
draw (802.3a,461.8b)--(802.3a,476.3b);
draw (794.0a,461.8b)--(810.6a,461.8b);
draw (794.0a,476.3b)--(810.6a,476.3b);
draw (1155.1a,310.5b)--(1155.1a,362.9b);
draw (1146.8a,310.5b)--(1163.4a,310.5b);
draw (1146.8a,362.9b)--(1163.4a,362.9b);
draw (1272.7a,266.2b)--(1272.7a,340.3b);
draw (1264.4a,266.2b)--(1281.0a,266.2b);
draw (1264.4a,340.3b)--(1281.0a,340.3b);
gpdraw(6,802.3a,470.0b);
gpdraw(6,1155.1a,334.0b);
gpdraw(6,1272.7a,299.0b);
gpdraw(6,717.4a,424.3b);
linetype 7;
put_text( btex P3ME\_Rev\_FFT etex, 656.0a, 387.7b, 0, 3);
draw (673.7a,387.7b)--(761.1a,387.7b);
draw (673.7a,396.0b)--(673.7a,379.4b);
draw (761.1a,396.0b)--(761.1a,379.4b);
draw (802.3a,643.4b)--(802.3a,659.0b);
draw (794.0a,643.4b)--(810.6a,643.4b);
draw (794.0a,659.0b)--(810.6a,659.0b);
draw (1155.1a,515.8b)--(1155.1a,532.7b);
draw (1146.8a,515.8b)--(1163.4a,515.8b);
draw (1146.8a,532.7b)--(1163.4a,532.7b);
draw (1272.7a,478.3b)--(1272.7a,493.6b);
draw (1264.4a,478.3b)--(1281.0a,478.3b);
draw (1264.4a,493.6b)--(1281.0a,493.6b);
gpdraw(7,802.3a,651.6b);
gpdraw(7,1155.1a,523.7b);
gpdraw(7,1272.7a,487.9b);
gpdraw(7,717.4a,387.7b);
linetype 1;
put_text( btex ReduceForces\_Cntl etex, 656.0a, 351.1b, 0, 3);
draw (673.7a,351.1b)--(761.1a,351.1b);
draw (673.7a,359.4b)--(673.7a,342.8b);
draw (761.1a,359.4b)--(761.1a,342.8b);
draw (802.3a,537.6b)--(802.3a,569.6b);
draw (794.0a,537.6b)--(810.6a,537.6b);
draw (794.0a,569.6b)--(810.6a,569.6b);
draw (1155.1a,543.0b)--(1155.1a,567.0b);
draw (1146.8a,543.0b)--(1163.4a,543.0b);
draw (1146.8a,567.0b)--(1163.4a,567.0b);
draw (1272.7a,543.7b)--(1272.7a,564.5b);
draw (1264.4a,543.7b)--(1281.0a,543.7b);
draw (1264.4a,564.5b)--(1281.0a,564.5b);
gpdraw(9,802.3a,555.4b);
gpdraw(9,1155.1a,557.3b);
gpdraw(9,1272.7a,556.0b);
gpdraw(9,717.4a,351.1b);
linetype 2;
put_text( btex UDF\_StdHarmonicBondForce etex, 656.0a, 314.5b, 0, 3);
draw (673.7a,314.5b)--(761.1a,314.5b);
draw (673.7a,322.8b)--(673.7a,306.2b);
draw (761.1a,322.8b)--(761.1a,306.2b);
draw (802.3a,325.6b)--(802.3a,378.3b);
draw (794.0a,325.6b)--(810.6a,325.6b);
draw (794.0a,378.3b)--(810.6a,378.3b);
draw (1155.1a,223.1b)--(1155.1a,293.8b);
draw (1146.8a,223.1b)--(1163.4a,223.1b);
draw (1146.8a,293.8b)--(1163.4a,293.8b);
draw (1272.7a,210.6b)--(1272.7a,289.6b);
draw (1264.4a,210.6b)--(1281.0a,210.6b);
draw (1264.4a,289.6b)--(1281.0a,289.6b);
gpdraw(0,802.3a,353.4b);
gpdraw(0,1155.1a,249.3b);
gpdraw(0,1272.7a,230.0b);
gpdraw(0,717.4a,314.5b);
linetype 0;
draw (802.3a,792.6b)--(1155.1a,686.3b)--(1272.7a,666.4b);
linetype 2;
draw (802.3a,563.7b)--(1155.1a,562.1b)--(1272.7a,563.4b);
linetype 1;
draw (802.3a,715.1b)--(1155.1a,590.2b)--(1272.7a,557.4b);
linetype 2;
draw (802.3a,729.0b)--(1155.1a,576.4b)--(1272.7a,531.5b);
linetype 3;
draw (802.3a,453.1b)--(1155.1a,384.7b)--(1272.7a,375.7b);
linetype 4;
draw (802.3a,482.5b)--(1155.1a,299.9b)--(1272.7a,245.8b);
linetype 5;
draw (802.3a,647.8b)--(1155.1a,525.2b)--(1272.7a,491.6b);
linetype 6;
draw (802.3a,470.0b)--(1155.1a,334.0b)--(1272.7a,299.0b);
linetype 7;
draw (802.3a,651.6b)--(1155.1a,523.7b)--(1272.7a,487.9b);
linetype 1;
draw (802.3a,555.4b)--(1155.1a,557.3b)--(1272.7a,556.0b);
linetype 2;
draw (802.3a,353.4b)--(1155.1a,249.3b)--(1272.7a,230.0b);
endfig;
end.
b);
endfig;
end.
