%GNUPLOT Metapost output: Mon Apr 12 15:43:24 2004


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

%color currentcolor; currentcolor=black;
  color currentcolor; currentcolor=white;
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

color col[],cyan, magenta, yellow, white;
cyan=blue+green; magenta=red+blue;yellow=green+red;
white=blue+green+red;
%col[-2]:=col[-1]:=col0:=black;
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

beginfig(13);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1626.3a,109.8b)--(1609.7a,109.8b);
put_text( btex  $10^4$ etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,153.1b)--(222.5a,153.1b);
draw (1626.3a,153.1b)--(1618.0a,153.1b);
draw (214.2a,210.3b)--(222.5a,210.3b);
draw (1626.3a,210.3b)--(1618.0a,210.3b);
draw (214.2a,239.6b)--(222.5a,239.6b);
draw (1626.3a,239.6b)--(1618.0a,239.6b);
draw (214.2a,253.6b)--(230.8a,253.6b);
draw (1626.3a,253.6b)--(1609.7a,253.6b);
put_text( btex  $10^5$ etex, 196.5a, 253.6b, 0, 3);
draw (214.2a,296.8b)--(222.5a,296.8b);
draw (1626.3a,296.8b)--(1618.0a,296.8b);
draw (214.2a,354.0b)--(222.5a,354.0b);
draw (1626.3a,354.0b)--(1618.0a,354.0b);
draw (214.2a,383.4b)--(222.5a,383.4b);
draw (1626.3a,383.4b)--(1618.0a,383.4b);
draw (214.2a,397.3b)--(230.8a,397.3b);
draw (1626.3a,397.3b)--(1609.7a,397.3b);
put_text( btex  $10^6$ etex, 196.5a, 397.3b, 0, 3);
draw (214.2a,440.6b)--(222.5a,440.6b);
draw (1626.3a,440.6b)--(1618.0a,440.6b);
draw (214.2a,497.8b)--(222.5a,497.8b);
draw (1626.3a,497.8b)--(1618.0a,497.8b);
draw (214.2a,527.1b)--(222.5a,527.1b);
draw (1626.3a,527.1b)--(1618.0a,527.1b);
draw (214.2a,541.1b)--(230.8a,541.1b);
draw (1626.3a,541.1b)--(1609.7a,541.1b);
put_text( btex  $10^7$ etex, 196.5a, 541.1b, 0, 3);
draw (214.2a,584.4b)--(222.5a,584.4b);
draw (1626.3a,584.4b)--(1618.0a,584.4b);
draw (214.2a,641.6b)--(222.5a,641.6b);
draw (1626.3a,641.6b)--(1618.0a,641.6b);
draw (214.2a,670.9b)--(222.5a,670.9b);
draw (1626.3a,670.9b)--(1618.0a,670.9b);
draw (214.2a,684.8b)--(230.8a,684.8b);
draw (1626.3a,684.8b)--(1609.7a,684.8b);
put_text( btex  $10^8$ etex, 196.5a, 684.8b, 0, 3);
draw (214.2a,728.1b)--(222.5a,728.1b);
draw (1626.3a,728.1b)--(1618.0a,728.1b);
draw (214.2a,785.3b)--(222.5a,785.3b);
draw (1626.3a,785.3b)--(1618.0a,785.3b);
draw (214.2a,814.7b)--(222.5a,814.7b);
draw (1626.3a,814.7b)--(1618.0a,814.7b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1626.3a,828.6b)--(1609.7a,828.6b);
put_text( btex  $10^9$ etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  $1$ etex, 214.2a, 73.2b, 0, 2);
draw (355.9a,109.8b)--(355.9a,118.1b);
draw (355.9a,828.6b)--(355.9a,820.3b);
draw (438.8a,109.8b)--(438.8a,118.1b);
draw (438.8a,828.6b)--(438.8a,820.3b);
draw (497.6a,109.8b)--(497.6a,118.1b);
draw (497.6a,828.6b)--(497.6a,820.3b);
draw (543.2a,109.8b)--(543.2a,118.1b);
draw (543.2a,828.6b)--(543.2a,820.3b);
draw (580.5a,109.8b)--(580.5a,118.1b);
draw (580.5a,828.6b)--(580.5a,820.3b);
draw (612.0a,109.8b)--(612.0a,118.1b);
draw (612.0a,828.6b)--(612.0a,820.3b);
draw (639.3a,109.8b)--(639.3a,118.1b);
draw (639.3a,828.6b)--(639.3a,820.3b);
draw (663.4a,109.8b)--(663.4a,118.1b);
draw (663.4a,828.6b)--(663.4a,820.3b);
draw (684.9a,109.8b)--(684.9a,126.4b);
draw (684.9a,828.6b)--(684.9a,812.0b);
put_text( btex  $10$ etex, 684.9a, 73.2b, 0, 2);
draw (826.6a,109.8b)--(826.6a,118.1b);
draw (826.6a,828.6b)--(826.6a,820.3b);
draw (909.5a,109.8b)--(909.5a,118.1b);
draw (909.5a,828.6b)--(909.5a,820.3b);
draw (968.3a,109.8b)--(968.3a,118.1b);
draw (968.3a,828.6b)--(968.3a,820.3b);
draw (1013.9a,109.8b)--(1013.9a,118.1b);
draw (1013.9a,828.6b)--(1013.9a,820.3b);
draw (1051.2a,109.8b)--(1051.2a,118.1b);
draw (1051.2a,828.6b)--(1051.2a,820.3b);
draw (1082.7a,109.8b)--(1082.7a,118.1b);
draw (1082.7a,828.6b)--(1082.7a,820.3b);
draw (1110.0a,109.8b)--(1110.0a,118.1b);
draw (1110.0a,828.6b)--(1110.0a,820.3b);
draw (1134.1a,109.8b)--(1134.1a,118.1b);
draw (1134.1a,828.6b)--(1134.1a,820.3b);
draw (1155.6a,109.8b)--(1155.6a,126.4b);
draw (1155.6a,828.6b)--(1155.6a,812.0b);
put_text( btex  $100$ etex, 1155.6a, 73.2b, 0, 2);
draw (1297.3a,109.8b)--(1297.3a,118.1b);
draw (1297.3a,828.6b)--(1297.3a,820.3b);
draw (1380.2a,109.8b)--(1380.2a,118.1b);
draw (1380.2a,828.6b)--(1380.2a,820.3b);
draw (1439.0a,109.8b)--(1439.0a,118.1b);
draw (1439.0a,828.6b)--(1439.0a,820.3b);
draw (1484.6a,109.8b)--(1484.6a,118.1b);
draw (1484.6a,828.6b)--(1484.6a,820.3b);
draw (1521.9a,109.8b)--(1521.9a,118.1b);
draw (1521.9a,828.6b)--(1521.9a,820.3b);
draw (1553.4a,109.8b)--(1553.4a,118.1b);
draw (1553.4a,828.6b)--(1553.4a,820.3b);
draw (1580.7a,109.8b)--(1580.7a,118.1b);
draw (1580.7a,828.6b)--(1580.7a,820.3b);
draw (1604.8a,109.8b)--(1604.8a,118.1b);
draw (1604.8a,828.6b)--(1604.8a,820.3b);
draw (1626.3a,109.8b)--(1626.3a,126.4b);
draw (1626.3a,828.6b)--(1626.3a,812.0b);
put_text( btex  $1000$ etex, 1626.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1626.3a,109.8b)--(1626.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (pclocks) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Node Count etex, 920.2a, 18.3b, 0, 2);
linetype 0;
put_text( btex Ewald\_AllReduce etex, 750.1a, 806.3b, 0, 3);
draw (767.8a,806.3b)--(855.2a,806.3b);
draw (767.8a,814.6b)--(767.8a,798.0b);
draw (855.2a,814.6b)--(855.2a,798.0b);
draw (922.7a,305.0b)--(922.7a,524.4b);
draw (914.4a,305.0b)--(931.0a,305.0b);
draw (914.4a,524.4b)--(931.0a,524.4b);
draw (1206.1a,310.5b)--(1206.1a,506.6b);
draw (1197.8a,310.5b)--(1214.4a,310.5b);
draw (1197.8a,506.6b)--(1214.4a,506.6b);
draw (1347.8a,316.5b)--(1347.8a,496.3b);
draw (1339.5a,316.5b)--(1356.1a,316.5b);
draw (1339.5a,496.3b)--(1356.1a,496.3b);
draw (1489.5a,321.9b)--(1489.5a,477.7b);
draw (1481.2a,321.9b)--(1497.8a,321.9b);
draw (1481.2a,477.7b)--(1497.8a,477.7b);
gpdraw(0,922.7a,420.6b);
gpdraw(0,1206.1a,369.3b);
gpdraw(0,1347.8a,363.5b);
gpdraw(0,1489.5a,379.5b);
gpdraw(0,811.5a,806.3b);
linetype 1;
put_text( btex Ewald\_Eiks etex, 750.1a, 769.7b, 0, 3);
draw (767.8a,769.7b)--(855.2a,769.7b);
draw (767.8a,778.0b)--(767.8a,761.4b);
draw (855.2a,778.0b)--(855.2a,761.4b);
draw (922.7a,368.7b)--(922.7a,373.9b);
draw (914.4a,368.7b)--(931.0a,368.7b);
draw (914.4a,373.9b)--(931.0a,373.9b);
draw (1206.1a,287.8b)--(1206.1a,300.2b);
draw (1197.8a,287.8b)--(1214.4a,287.8b);
draw (1197.8a,300.2b)--(1214.4a,300.2b);
draw (1347.8a,251.7b)--(1347.8a,269.5b);
draw (1339.5a,251.7b)--(1356.1a,251.7b);
draw (1339.5a,269.5b)--(1356.1a,269.5b);
draw (1489.5a,219.6b)--(1489.5a,248.4b);
draw (1481.2a,219.6b)--(1497.8a,219.6b);
draw (1481.2a,248.4b)--(1497.8a,248.4b);
gpdraw(1,922.7a,371.3b);
gpdraw(1,1206.1a,293.9b);
gpdraw(1,1347.8a,261.8b);
gpdraw(1,1489.5a,235.6b);
gpdraw(1,811.5a,769.7b);
linetype 2;
put_text( btex Ewald\_KSpace etex, 750.1a, 733.1b, 0, 3);
draw (767.8a,733.1b)--(855.2a,733.1b);
draw (767.8a,741.4b)--(767.8a,724.8b);
draw (855.2a,741.4b)--(855.2a,724.8b);
draw (922.7a,770.4b)--(922.7a,770.8b);
draw (914.4a,770.4b)--(931.0a,770.4b);
draw (914.4a,770.8b)--(931.0a,770.8b);
draw (1206.1a,724.5b)--(1206.1a,724.9b);
draw (1197.8a,724.5b)--(1214.4a,724.5b);
draw (1197.8a,724.9b)--(1214.4a,724.9b);
draw (1347.8a,717.7b)--(1347.8a,718.1b);
draw (1339.5a,717.7b)--(1356.1a,717.7b);
draw (1339.5a,718.1b)--(1356.1a,718.1b);
draw (1489.5a,714.6b)--(1489.5a,715.2b);
draw (1481.2a,714.6b)--(1497.8a,714.6b);
draw (1481.2a,715.2b)--(1497.8a,715.2b);
gpdraw(2,922.7a,770.6b);
gpdraw(2,1206.1a,724.7b);
gpdraw(2,1347.8a,717.9b);
gpdraw(2,1489.5a,714.8b);
gpdraw(2,811.5a,733.1b);
linetype 3;
put_text( btex Ewald\_Loop etex, 750.1a, 696.5b, 0, 3);
draw (767.8a,696.5b)--(855.2a,696.5b);
draw (767.8a,704.8b)--(767.8a,688.2b);
draw (855.2a,704.8b)--(855.2a,688.2b);
draw (922.7a,770.3b)--(922.7a,770.7b);
draw (914.4a,770.3b)--(931.0a,770.3b);
draw (914.4a,770.7b)--(931.0a,770.7b);
draw (1206.1a,724.4b)--(1206.1a,724.8b);
draw (1197.8a,724.4b)--(1214.4a,724.4b);
draw (1197.8a,724.8b)--(1214.4a,724.8b);
draw (1347.8a,717.7b)--(1347.8a,718.0b);
draw (1339.5a,717.7b)--(1356.1a,717.7b);
draw (1339.5a,718.0b)--(1356.1a,718.0b);
draw (1489.5a,714.6b)--(1489.5a,715.1b);
draw (1481.2a,714.6b)--(1497.8a,714.6b);
draw (1481.2a,715.1b)--(1497.8a,715.1b);
gpdraw(3,922.7a,770.5b);
gpdraw(3,1206.1a,724.6b);
gpdraw(3,1347.8a,717.9b);
gpdraw(3,1489.5a,714.8b);
gpdraw(3,811.5a,696.5b);
linetype 4;
put_text( btex GlobalizePositions\_Cntl etex, 750.1a, 659.9b, 0, 3);
draw (767.8a,659.9b)--(855.2a,659.9b);
draw (767.8a,668.2b)--(767.8a,651.6b);
draw (855.2a,668.2b)--(855.2a,651.6b);
draw (922.7a,478.9b)--(922.7a,508.2b);
draw (914.4a,478.9b)--(931.0a,478.9b);
draw (914.4a,508.2b)--(931.0a,508.2b);
draw (1206.1a,480.0b)--(1206.1a,506.5b);
draw (1197.8a,480.0b)--(1214.4a,480.0b);
draw (1197.8a,506.5b)--(1214.4a,506.5b);
draw (1347.8a,479.5b)--(1347.8a,508.6b);
draw (1339.5a,479.5b)--(1356.1a,479.5b);
draw (1339.5a,508.6b)--(1356.1a,508.6b);
draw (1489.5a,480.6b)--(1489.5a,505.9b);
draw (1481.2a,480.6b)--(1497.8a,480.6b);
draw (1481.2a,505.9b)--(1497.8a,505.9b);
gpdraw(4,922.7a,494.0b);
gpdraw(4,1206.1a,494.1b);
gpdraw(4,1347.8a,493.9b);
gpdraw(4,1489.5a,494.8b);
gpdraw(4,811.5a,659.9b);
linetype 5;
put_text( btex P2\_KSpace\_Control etex, 750.1a, 623.3b, 0, 3);
draw (767.8a,623.3b)--(855.2a,623.3b);
draw (767.8a,631.6b)--(767.8a,615.0b);
draw (855.2a,631.6b)--(855.2a,615.0b);
draw (922.7a,770.4b)--(922.7a,770.8b);
draw (914.4a,770.4b)--(931.0a,770.4b);
draw (914.4a,770.8b)--(931.0a,770.8b);
draw (1206.1a,724.5b)--(1206.1a,724.9b);
draw (1197.8a,724.5b)--(1214.4a,724.5b);
draw (1197.8a,724.9b)--(1214.4a,724.9b);
draw (1347.8a,717.7b)--(1347.8a,718.1b);
draw (1339.5a,717.7b)--(1356.1a,717.7b);
draw (1339.5a,718.1b)--(1356.1a,718.1b);
draw (1489.5a,714.6b)--(1489.5a,715.2b);
draw (1481.2a,714.6b)--(1497.8a,714.6b);
draw (1481.2a,715.2b)--(1497.8a,715.2b);
gpdraw(5,922.7a,770.6b);
gpdraw(5,1206.1a,724.7b);
gpdraw(5,1347.8a,717.9b);
gpdraw(5,1489.5a,714.8b);
gpdraw(5,811.5a,623.3b);
linetype 6;
put_text( btex P2\_NSQ\_Control etex, 750.1a, 586.7b, 0, 3);
draw (767.8a,586.7b)--(855.2a,586.7b);
draw (767.8a,595.0b)--(767.8a,578.4b);
draw (855.2a,595.0b)--(855.2a,578.4b);
draw (922.7a,623.4b)--(922.7a,634.3b);
draw (914.4a,623.4b)--(931.0a,623.4b);
draw (914.4a,634.3b)--(931.0a,634.3b);
draw (1206.1a,536.9b)--(1206.1a,555.1b);
draw (1197.8a,536.9b)--(1214.4a,536.9b);
draw (1197.8a,555.1b)--(1214.4a,555.1b);
draw (1347.8a,493.9b)--(1347.8a,519.5b);
draw (1339.5a,493.9b)--(1356.1a,493.9b);
draw (1339.5a,519.5b)--(1356.1a,519.5b);
draw (1489.5a,452.5b)--(1489.5a,491.4b);
draw (1481.2a,452.5b)--(1497.8a,452.5b);
draw (1481.2a,491.4b)--(1497.8a,491.4b);
gpdraw(6,922.7a,627.6b);
gpdraw(6,1206.1a,544.1b);
gpdraw(6,1347.8a,505.0b);
gpdraw(6,1489.5a,469.1b);
gpdraw(6,811.5a,586.7b);
linetype 7;
put_text( btex ReduceForces\_Cntl etex, 750.1a, 550.1b, 0, 3);
draw (767.8a,550.1b)--(855.2a,550.1b);
draw (767.8a,558.4b)--(767.8a,541.8b);
draw (855.2a,558.4b)--(855.2a,541.8b);
draw (922.7a,473.7b)--(922.7a,502.0b);
draw (914.4a,473.7b)--(931.0a,473.7b);
draw (914.4a,502.0b)--(931.0a,502.0b);
draw (1206.1a,479.3b)--(1206.1a,499.9b);
draw (1197.8a,479.3b)--(1214.4a,479.3b);
draw (1197.8a,499.9b)--(1214.4a,499.9b);
draw (1347.8a,480.6b)--(1347.8a,500.0b);
draw (1339.5a,480.6b)--(1356.1a,480.6b);
draw (1339.5a,500.0b)--(1356.1a,500.0b);
draw (1489.5a,480.9b)--(1489.5a,500.9b);
draw (1481.2a,480.9b)--(1497.8a,480.9b);
draw (1481.2a,500.9b)--(1497.8a,500.9b);
gpdraw(7,922.7a,491.9b);
gpdraw(7,1206.1a,491.2b);
gpdraw(7,1347.8a,490.9b);
gpdraw(7,1489.5a,491.7b);
gpdraw(7,811.5a,550.1b);
linetype 2;
put_text( btex UDF\_KineticEnergy etex, 750.1a, 513.5b, 0, 3);
draw (767.8a,513.5b)--(855.2a,513.5b);
draw (767.8a,521.8b)--(767.8a,505.2b);
draw (855.2a,521.8b)--(855.2a,505.2b);
draw (922.7a,199.4b)--(922.7a,210.3b);
draw (914.4a,199.4b)--(931.0a,199.4b);
draw (914.4a,210.3b)--(931.0a,210.3b);
draw (1206.1a,169.5b)--(1206.1a,174.1b);
draw (1197.8a,169.5b)--(1214.4a,169.5b);
draw (1197.8a,174.1b)--(1214.4a,174.1b);
draw (1347.8a,161.8b)--(1347.8a,167.0b);
draw (1339.5a,161.8b)--(1356.1a,161.8b);
draw (1339.5a,167.0b)--(1356.1a,167.0b);
draw (1489.5a,159.0b)--(1489.5a,164.5b);
draw (1481.2a,159.0b)--(1497.8a,159.0b);
draw (1481.2a,164.5b)--(1497.8a,164.5b);
gpdraw(8,922.7a,203.9b);
gpdraw(8,1206.1a,171.2b);
gpdraw(8,1347.8a,165.6b);
gpdraw(8,1489.5a,162.4b);
gpdraw(8,811.5a,513.5b);
linetype 1;
put_text( btex UDF\_StdHarmonicBondForce etex, 750.1a, 476.9b, 0, 3);
draw (767.8a,476.9b)--(855.2a,476.9b);
draw (767.8a,485.2b)--(767.8a,468.6b);
draw (855.2a,485.2b)--(855.2a,468.6b);
draw (922.7a,304.7b)--(922.7a,349.1b);
draw (914.4a,304.7b)--(931.0a,304.7b);
draw (914.4a,349.1b)--(931.0a,349.1b);
draw (1206.1a,238.0b)--(1206.1a,301.1b);
draw (1197.8a,238.0b)--(1214.4a,238.0b);
draw (1197.8a,301.1b)--(1214.4a,301.1b);
draw (1347.8a,222.7b)--(1347.8a,298.1b);
draw (1339.5a,222.7b)--(1356.1a,222.7b);
draw (1339.5a,298.1b)--(1356.1a,298.1b);
draw (1489.5a,212.7b)--(1489.5a,275.0b);
draw (1481.2a,212.7b)--(1497.8a,212.7b);
draw (1481.2a,275.0b)--(1497.8a,275.0b);
gpdraw(9,922.7a,325.9b);
gpdraw(9,1206.1a,264.6b);
gpdraw(9,1347.8a,242.8b);
gpdraw(9,1489.5a,227.6b);
gpdraw(9,811.5a,476.9b);
linetype 2;
put_text( btex UDF\_UpdatePosition etex, 750.1a, 440.3b, 0, 3);
draw (767.8a,440.3b)--(855.2a,440.3b);
draw (767.8a,448.6b)--(767.8a,432.0b);
draw (855.2a,448.6b)--(855.2a,432.0b);
draw (922.7a,194.8b)--(922.7a,197.9b);
draw (914.4a,194.8b)--(931.0a,194.8b);
draw (914.4a,197.9b)--(931.0a,197.9b);
draw (1206.1a,167.0b)--(1206.1a,171.8b);
draw (1197.8a,167.0b)--(1214.4a,167.0b);
draw (1197.8a,171.8b)--(1214.4a,171.8b);
draw (1347.8a,161.8b)--(1347.8a,167.0b);
draw (1339.5a,161.8b)--(1356.1a,161.8b);
draw (1339.5a,167.0b)--(1356.1a,167.0b);
draw (1489.5a,159.0b)--(1489.5a,164.5b);
draw (1481.2a,159.0b)--(1497.8a,159.0b);
draw (1481.2a,164.5b)--(1497.8a,164.5b);
gpdraw(0,922.7a,196.0b);
gpdraw(0,1206.1a,169.6b);
gpdraw(0,1347.8a,163.8b);
gpdraw(0,1489.5a,160.8b);
gpdraw(0,811.5a,440.3b);
linetype 3;
put_text( btex UDF\_UpdateVelocity etex, 750.1a, 403.7b, 0, 3);
draw (767.8a,403.7b)--(855.2a,403.7b);
draw (767.8a,412.0b)--(767.8a,395.4b);
draw (855.2a,412.0b)--(855.2a,395.4b);
draw (922.7a,209.0b)--(922.7a,234.8b);
draw (914.4a,209.0b)--(931.0a,209.0b);
draw (914.4a,234.8b)--(931.0a,234.8b);
draw (1206.1a,169.5b)--(1206.1a,189.8b);
draw (1197.8a,169.5b)--(1214.4a,169.5b);
draw (1197.8a,189.8b)--(1214.4a,189.8b);
draw (1347.8a,161.8b)--(1347.8a,180.4b);
draw (1339.5a,161.8b)--(1356.1a,161.8b);
draw (1339.5a,180.4b)--(1356.1a,180.4b);
draw (1489.5a,156.1b)--(1489.5a,174.1b);
draw (1481.2a,156.1b)--(1497.8a,156.1b);
draw (1481.2a,174.1b)--(1497.8a,174.1b);
gpdraw(1,922.7a,224.7b);
gpdraw(1,1206.1a,180.2b);
gpdraw(1,1347.8a,170.6b);
gpdraw(1,1489.5a,165.1b);
gpdraw(1,811.5a,403.7b);
linetype 4;
put_text( btex UDF\_WaterSPCRattle etex, 750.1a, 367.1b, 0, 3);
draw (767.8a,367.1b)--(855.2a,367.1b);
draw (767.8a,375.4b)--(767.8a,358.8b);
draw (855.2a,375.4b)--(855.2a,358.8b);
draw (922.7a,480.3b)--(922.7a,487.7b);
draw (914.4a,480.3b)--(931.0a,480.3b);
draw (914.4a,487.7b)--(931.0a,487.7b);
draw (1206.1a,389.4b)--(1206.1a,404.1b);
draw (1197.8a,389.4b)--(1214.4a,389.4b);
draw (1197.8a,404.1b)--(1214.4a,404.1b);
draw (1347.8a,341.8b)--(1347.8a,363.1b);
draw (1339.5a,341.8b)--(1356.1a,341.8b);
draw (1339.5a,363.1b)--(1356.1a,363.1b);
draw (1489.5a,268.0b)--(1489.5a,331.4b);
draw (1481.2a,268.0b)--(1497.8a,268.0b);
draw (1481.2a,331.4b)--(1497.8a,331.4b);
gpdraw(2,922.7a,484.0b);
gpdraw(2,1206.1a,399.1b);
gpdraw(2,1347.8a,357.9b);
gpdraw(2,1489.5a,318.7b);
gpdraw(2,811.5a,367.1b);
linetype 5;
put_text( btex UDF\_WaterSPCShake etex, 750.1a, 330.5b, 0, 3);
draw (767.8a,330.5b)--(855.2a,330.5b);
draw (767.8a,338.8b)--(767.8a,322.2b);
draw (855.2a,338.8b)--(855.2a,322.2b);
draw (922.7a,459.0b)--(922.7a,466.6b);
draw (914.4a,459.0b)--(931.0a,459.0b);
draw (914.4a,466.6b)--(931.0a,466.6b);
draw (1206.1a,368.5b)--(1206.1a,384.2b);
draw (1197.8a,368.5b)--(1214.4a,368.5b);
draw (1197.8a,384.2b)--(1214.4a,384.2b);
draw (1347.8a,321.9b)--(1347.8a,344.2b);
draw (1339.5a,321.9b)--(1356.1a,321.9b);
draw (1339.5a,344.2b)--(1356.1a,344.2b);
draw (1489.5a,252.9b)--(1489.5a,314.2b);
draw (1481.2a,252.9b)--(1497.8a,252.9b);
draw (1481.2a,314.2b)--(1497.8a,314.2b);
gpdraw(3,922.7a,462.8b);
gpdraw(3,1206.1a,378.8b);
gpdraw(3,1347.8a,338.6b);
gpdraw(3,1489.5a,301.1b);
gpdraw(3,811.5a,330.5b);
linetype 6;
put_text( btex TS\_from\_GlobalizePositions\_Cntl etex, 750.1a, 293.9b, 0, 3);
draw (767.8a,293.9b)--(855.2a,293.9b);
draw (767.8a,302.2b)--(767.8a,285.6b);
draw (855.2a,302.2b)--(855.2a,285.6b);
draw (922.7a,778.6b)--(922.7a,787.5b);
draw (914.4a,778.6b)--(931.0a,778.6b);
draw (914.4a,787.5b)--(931.0a,787.5b);
draw (1206.1a,731.1b)--(1206.1a,748.6b);
draw (1197.8a,731.1b)--(1214.4a,731.1b);
draw (1197.8a,748.6b)--(1214.4a,748.6b);
draw (1347.8a,723.1b)--(1347.8a,743.6b);
draw (1339.5a,723.1b)--(1356.1a,723.1b);
draw (1339.5a,743.6b)--(1356.1a,743.6b);
draw (1489.5a,719.3b)--(1489.5a,742.9b);
draw (1481.2a,719.3b)--(1497.8a,719.3b);
draw (1481.2a,742.9b)--(1497.8a,742.9b);
gpdraw(4,922.7a,779.1b);
gpdraw(4,1206.1a,731.8b);
gpdraw(4,1347.8a,723.9b);
gpdraw(4,1489.5a,720.2b);
gpdraw(4,811.5a,293.9b);
linetype 0;
draw (922.7a,420.6b)--(1206.1a,369.3b)--(1347.8a,363.5b)--(1489.5a,379.5b);
linetype 1;
draw (922.7a,371.3b)--(1206.1a,293.9b)--(1347.8a,261.8b)--(1489.5a,235.6b);
linetype 2;
draw (922.7a,770.6b)--(1206.1a,724.7b)--(1347.8a,717.9b)--(1489.5a,714.8b);
linetype 3;
draw (922.7a,770.5b)--(1206.1a,724.6b)--(1347.8a,717.9b)--(1489.5a,714.8b);
linetype 4;
draw (922.7a,494.0b)--(1206.1a,494.1b)--(1347.8a,493.9b)--(1489.5a,494.8b);
linetype 5;
draw (922.7a,770.6b)--(1206.1a,724.7b)--(1347.8a,717.9b)--(1489.5a,714.8b);
linetype 6;
draw (922.7a,627.6b)--(1206.1a,544.1b)--(1347.8a,505.0b)--(1489.5a,469.1b);
linetype 7;
draw (922.7a,491.9b)--(1206.1a,491.2b)--(1347.8a,490.9b)--(1489.5a,491.7b);
linetype 2;
draw (922.7a,203.9b)--(1206.1a,171.2b)--(1347.8a,165.6b)--(1489.5a,162.4b);
linetype 1;
draw (922.7a,325.9b)--(1206.1a,264.6b)--(1347.8a,242.8b)--(1489.5a,227.6b);
linetype 2;
draw (922.7a,196.0b)--(1206.1a,169.6b)--(1347.8a,163.8b)--(1489.5a,160.8b);
linetype 3;
draw (922.7a,224.7b)--(1206.1a,180.2b)--(1347.8a,170.6b)--(1489.5a,165.1b);
linetype 4;
draw (922.7a,484.0b)--(1206.1a,399.1b)--(1347.8a,357.9b)--(1489.5a,318.7b);
linetype 5;
draw (922.7a,462.8b)--(1206.1a,378.8b)--(1347.8a,338.6b)--(1489.5a,301.1b);
linetype 7;
draw (922.7a,779.1b)--(1206.1a,731.8b)--(1347.8a,723.9b)--(1489.5a,720.2b);
endfig;
end.
b);
endfig;
end.
