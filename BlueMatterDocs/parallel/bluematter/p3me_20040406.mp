%GNUPLOT Metapost output: Wed Apr 21 23:13:24 2004


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

beginfig(7);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1626.3a,109.8b)--(1609.7a,109.8b);
put_text( btex  $10^4$ etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,163.9b)--(222.5a,163.9b);
draw (1626.3a,163.9b)--(1618.0a,163.9b);
draw (214.2a,195.5b)--(222.5a,195.5b);
draw (1626.3a,195.5b)--(1618.0a,195.5b);
draw (214.2a,218.0b)--(222.5a,218.0b);
draw (1626.3a,218.0b)--(1618.0a,218.0b);
draw (214.2a,235.4b)--(222.5a,235.4b);
draw (1626.3a,235.4b)--(1618.0a,235.4b);
draw (214.2a,249.6b)--(222.5a,249.6b);
draw (1626.3a,249.6b)--(1618.0a,249.6b);
draw (214.2a,261.7b)--(222.5a,261.7b);
draw (1626.3a,261.7b)--(1618.0a,261.7b);
draw (214.2a,272.1b)--(222.5a,272.1b);
draw (1626.3a,272.1b)--(1618.0a,272.1b);
draw (214.2a,281.3b)--(222.5a,281.3b);
draw (1626.3a,281.3b)--(1618.0a,281.3b);
draw (214.2a,289.5b)--(230.8a,289.5b);
draw (1626.3a,289.5b)--(1609.7a,289.5b);
put_text( btex  $10^5$ etex, 196.5a, 289.5b, 0, 3);
draw (214.2a,343.6b)--(222.5a,343.6b);
draw (1626.3a,343.6b)--(1618.0a,343.6b);
draw (214.2a,375.2b)--(222.5a,375.2b);
draw (1626.3a,375.2b)--(1618.0a,375.2b);
draw (214.2a,397.7b)--(222.5a,397.7b);
draw (1626.3a,397.7b)--(1618.0a,397.7b);
draw (214.2a,415.1b)--(222.5a,415.1b);
draw (1626.3a,415.1b)--(1618.0a,415.1b);
draw (214.2a,429.3b)--(222.5a,429.3b);
draw (1626.3a,429.3b)--(1618.0a,429.3b);
draw (214.2a,441.4b)--(222.5a,441.4b);
draw (1626.3a,441.4b)--(1618.0a,441.4b);
draw (214.2a,451.8b)--(222.5a,451.8b);
draw (1626.3a,451.8b)--(1618.0a,451.8b);
draw (214.2a,461.0b)--(222.5a,461.0b);
draw (1626.3a,461.0b)--(1618.0a,461.0b);
draw (214.2a,469.2b)--(230.8a,469.2b);
draw (1626.3a,469.2b)--(1609.7a,469.2b);
put_text( btex  $10^6$ etex, 196.5a, 469.2b, 0, 3);
draw (214.2a,523.3b)--(222.5a,523.3b);
draw (1626.3a,523.3b)--(1618.0a,523.3b);
draw (214.2a,554.9b)--(222.5a,554.9b);
draw (1626.3a,554.9b)--(1618.0a,554.9b);
draw (214.2a,577.4b)--(222.5a,577.4b);
draw (1626.3a,577.4b)--(1618.0a,577.4b);
draw (214.2a,594.8b)--(222.5a,594.8b);
draw (1626.3a,594.8b)--(1618.0a,594.8b);
draw (214.2a,609.0b)--(222.5a,609.0b);
draw (1626.3a,609.0b)--(1618.0a,609.0b);
draw (214.2a,621.1b)--(222.5a,621.1b);
draw (1626.3a,621.1b)--(1618.0a,621.1b);
draw (214.2a,631.5b)--(222.5a,631.5b);
draw (1626.3a,631.5b)--(1618.0a,631.5b);
draw (214.2a,640.7b)--(222.5a,640.7b);
draw (1626.3a,640.7b)--(1618.0a,640.7b);
draw (214.2a,648.9b)--(230.8a,648.9b);
draw (1626.3a,648.9b)--(1609.7a,648.9b);
put_text( btex  $10^7$ etex, 196.5a, 648.9b, 0, 3);
draw (214.2a,703.0b)--(222.5a,703.0b);
draw (1626.3a,703.0b)--(1618.0a,703.0b);
draw (214.2a,734.6b)--(222.5a,734.6b);
draw (1626.3a,734.6b)--(1618.0a,734.6b);
draw (214.2a,757.1b)--(222.5a,757.1b);
draw (1626.3a,757.1b)--(1618.0a,757.1b);
draw (214.2a,774.5b)--(222.5a,774.5b);
draw (1626.3a,774.5b)--(1618.0a,774.5b);
draw (214.2a,788.7b)--(222.5a,788.7b);
draw (1626.3a,788.7b)--(1618.0a,788.7b);
draw (214.2a,800.8b)--(222.5a,800.8b);
draw (1626.3a,800.8b)--(1618.0a,800.8b);
draw (214.2a,811.2b)--(222.5a,811.2b);
draw (1626.3a,811.2b)--(1618.0a,811.2b);
draw (214.2a,820.4b)--(222.5a,820.4b);
draw (1626.3a,820.4b)--(1618.0a,820.4b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1626.3a,828.6b)--(1609.7a,828.6b);
put_text( btex  $10^8$ etex, 196.5a, 828.6b, 0, 3);
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
put_text( btex GlobalizePositions\_Cntl etex, 750.1a, 811.2b, 0, 3);
draw (767.8a,811.2b)--(855.2a,811.2b);
draw (767.8a,819.5b)--(767.8a,802.9b);
draw (855.2a,819.5b)--(855.2a,802.9b);
draw (922.7a,575.4b)--(922.7a,607.4b);
draw (914.4a,575.4b)--(931.0a,575.4b);
draw (914.4a,607.4b)--(931.0a,607.4b);
draw (1347.8a,571.6b)--(1347.8a,604.9b);
draw (1339.5a,571.6b)--(1356.1a,571.6b);
draw (1339.5a,604.9b)--(1356.1a,604.9b);
draw (1489.5a,573.7b)--(1489.5a,605.9b);
draw (1481.2a,573.7b)--(1497.8a,573.7b);
draw (1481.2a,605.9b)--(1497.8a,605.9b);
gpdraw(0,922.7a,591.6b);
gpdraw(0,1347.8a,589.9b);
gpdraw(0,1489.5a,591.2b);
gpdraw(0,811.5a,811.2b);
linetype 1;
put_text( btex P2\_KSpace\_Control etex, 750.1a, 774.6b, 0, 3);
draw (767.8a,774.6b)--(855.2a,774.6b);
draw (767.8a,782.9b)--(767.8a,766.3b);
draw (855.2a,782.9b)--(855.2a,766.3b);
draw (922.7a,741.0b)--(922.7a,744.8b);
draw (914.4a,741.0b)--(931.0a,741.0b);
draw (914.4a,744.8b)--(931.0a,744.8b);
draw (1347.8a,612.0b)--(1347.8a,623.1b);
draw (1339.5a,612.0b)--(1356.1a,612.0b);
draw (1339.5a,623.1b)--(1356.1a,623.1b);
draw (1489.5a,578.5b)--(1489.5a,590.8b);
draw (1481.2a,578.5b)--(1497.8a,578.5b);
draw (1481.2a,590.8b)--(1497.8a,590.8b);
gpdraw(1,922.7a,743.0b);
gpdraw(1,1347.8a,618.1b);
gpdraw(1,1489.5a,585.3b);
gpdraw(1,811.5a,774.6b);
linetype 2;
put_text( btex P2\_NSQ\_Control etex, 750.1a, 738.0b, 0, 3);
draw (767.8a,738.0b)--(855.2a,738.0b);
draw (767.8a,746.3b)--(767.8a,729.7b);
draw (855.2a,746.3b)--(855.2a,729.7b);
draw (922.7a,751.9b)--(922.7a,763.5b);
draw (914.4a,751.9b)--(931.0a,751.9b);
draw (914.4a,763.5b)--(931.0a,763.5b);
draw (1347.8a,590.6b)--(1347.8a,620.1b);
draw (1339.5a,590.6b)--(1356.1a,590.6b);
draw (1339.5a,620.1b)--(1356.1a,620.1b);
draw (1489.5a,538.9b)--(1489.5a,581.8b);
draw (1481.2a,538.9b)--(1497.8a,538.9b);
draw (1481.2a,581.8b)--(1497.8a,581.8b);
gpdraw(2,922.7a,756.8b);
gpdraw(2,1347.8a,604.2b);
gpdraw(2,1489.5a,559.4b);
gpdraw(2,811.5a,738.0b);
linetype 3;
put_text( btex P3ME\_AssignCharge\_Control etex, 750.1a, 701.4b, 0, 3);
draw (767.8a,701.4b)--(855.2a,701.4b);
draw (767.8a,709.7b)--(767.8a,693.1b);
draw (855.2a,709.7b)--(855.2a,693.1b);
draw (922.7a,474.8b)--(922.7a,485.5b);
draw (914.4a,474.8b)--(931.0a,474.8b);
draw (914.4a,485.5b)--(931.0a,485.5b);
draw (1347.8a,405.5b)--(1347.8a,422.4b);
draw (1339.5a,405.5b)--(1356.1a,405.5b);
draw (1339.5a,422.4b)--(1356.1a,422.4b);
draw (1489.5a,396.1b)--(1489.5a,414.8b);
draw (1481.2a,396.1b)--(1497.8a,396.1b);
draw (1481.2a,414.8b)--(1497.8a,414.8b);
gpdraw(3,922.7a,480.9b);
gpdraw(3,1347.8a,412.5b);
gpdraw(3,1489.5a,403.6b);
gpdraw(3,811.5a,701.4b);
linetype 4;
put_text( btex P3ME\_Convolve etex, 750.1a, 664.8b, 0, 3);
draw (767.8a,664.8b)--(855.2a,664.8b);
draw (767.8a,673.1b)--(767.8a,656.5b);
draw (855.2a,673.1b)--(855.2a,656.5b);
draw (922.7a,449.5b)--(922.7a,545.0b);
draw (914.4a,449.5b)--(931.0a,449.5b);
draw (914.4a,545.0b)--(931.0a,545.0b);
draw (1347.8a,275.9b)--(1347.8a,362.9b);
draw (1339.5a,275.9b)--(1356.1a,275.9b);
draw (1339.5a,362.9b)--(1356.1a,362.9b);
draw (1489.5a,232.2b)--(1489.5a,301.1b);
draw (1481.2a,232.2b)--(1497.8a,232.2b);
draw (1481.2a,301.1b)--(1497.8a,301.1b);
gpdraw(4,922.7a,510.3b);
gpdraw(4,1347.8a,327.8b);
gpdraw(4,1489.5a,273.6b);
gpdraw(4,811.5a,664.8b);
linetype 5;
put_text( btex P3ME\_ForwardFFT\_Control etex, 750.1a, 628.2b, 0, 3);
draw (767.8a,628.2b)--(855.2a,628.2b);
draw (767.8a,636.5b)--(767.8a,619.9b);
draw (855.2a,636.5b)--(855.2a,619.9b);
draw (922.7a,671.3b)--(922.7a,679.9b);
draw (914.4a,671.3b)--(931.0a,671.3b);
draw (914.4a,679.9b)--(931.0a,679.9b);
draw (1347.8a,540.0b)--(1347.8a,564.0b);
draw (1339.5a,540.0b)--(1356.1a,540.0b);
draw (1339.5a,564.0b)--(1356.1a,564.0b);
draw (1489.5a,502.5b)--(1489.5a,530.9b);
draw (1481.2a,502.5b)--(1497.8a,502.5b);
draw (1481.2a,530.9b)--(1497.8a,530.9b);
gpdraw(5,922.7a,675.6b);
gpdraw(5,1347.8a,553.0b);
gpdraw(5,1489.5a,519.5b);
gpdraw(5,811.5a,628.2b);
linetype 6;
put_text( btex P3ME\_Interpolate\_Control etex, 750.1a, 591.6b, 0, 3);
draw (767.8a,591.6b)--(855.2a,591.6b);
draw (767.8a,599.9b)--(767.8a,583.3b);
draw (855.2a,599.9b)--(855.2a,583.3b);
draw (922.7a,489.6b)--(922.7a,504.1b);
draw (914.4a,489.6b)--(931.0a,489.6b);
draw (914.4a,504.1b)--(931.0a,504.1b);
draw (1347.8a,338.3b)--(1347.8a,390.8b);
draw (1339.5a,338.3b)--(1356.1a,338.3b);
draw (1339.5a,390.8b)--(1356.1a,390.8b);
draw (1489.5a,294.0b)--(1489.5a,368.2b);
draw (1481.2a,294.0b)--(1497.8a,294.0b);
draw (1481.2a,368.2b)--(1497.8a,368.2b);
gpdraw(6,922.7a,497.8b);
gpdraw(6,1347.8a,361.9b);
gpdraw(6,1489.5a,326.8b);
gpdraw(6,811.5a,591.6b);
linetype 7;
put_text( btex P3ME\_Rev\_FFT etex, 750.1a, 555.0b, 0, 3);
draw (767.8a,555.0b)--(855.2a,555.0b);
draw (767.8a,563.3b)--(767.8a,546.7b);
draw (855.2a,563.3b)--(855.2a,546.7b);
draw (922.7a,671.2b)--(922.7a,686.8b);
draw (914.4a,671.2b)--(931.0a,671.2b);
draw (914.4a,686.8b)--(931.0a,686.8b);
draw (1347.8a,543.7b)--(1347.8a,560.5b);
draw (1339.5a,543.7b)--(1356.1a,543.7b);
draw (1339.5a,560.5b)--(1356.1a,560.5b);
draw (1489.5a,506.2b)--(1489.5a,521.4b);
draw (1481.2a,506.2b)--(1497.8a,506.2b);
draw (1481.2a,521.4b)--(1497.8a,521.4b);
gpdraw(7,922.7a,679.4b);
gpdraw(7,1347.8a,551.6b);
gpdraw(7,1489.5a,515.8b);
gpdraw(7,811.5a,555.0b);
linetype 1;
put_text( btex ReduceForces\_Cntl etex, 750.1a, 518.4b, 0, 3);
draw (767.8a,518.4b)--(855.2a,518.4b);
draw (767.8a,526.7b)--(767.8a,510.1b);
draw (855.2a,526.7b)--(855.2a,510.1b);
draw (922.7a,565.5b)--(922.7a,597.4b);
draw (914.4a,565.5b)--(931.0a,565.5b);
draw (914.4a,597.4b)--(931.0a,597.4b);
draw (1347.8a,570.9b)--(1347.8a,594.8b);
draw (1339.5a,570.9b)--(1356.1a,570.9b);
draw (1339.5a,594.8b)--(1356.1a,594.8b);
draw (1489.5a,571.5b)--(1489.5a,592.3b);
draw (1481.2a,571.5b)--(1497.8a,571.5b);
draw (1481.2a,592.3b)--(1497.8a,592.3b);
gpdraw(7,922.7a,583.3b);
gpdraw(7,1347.8a,585.2b);
gpdraw(7,1489.5a,583.8b);
gpdraw(7,811.5a,518.4b);
linetype 2;
put_text( btex StdHarmonicBondForce etex, 750.1a, 481.8b, 0, 3);
draw (767.8a,481.8b)--(855.2a,481.8b);
draw (767.8a,490.1b)--(767.8a,473.5b);
draw (855.2a,490.1b)--(855.2a,473.5b);
draw (922.7a,353.5b)--(922.7a,406.2b);
draw (914.4a,353.5b)--(931.0a,353.5b);
draw (914.4a,406.2b)--(931.0a,406.2b);
draw (1347.8a,250.9b)--(1347.8a,321.7b);
draw (1339.5a,250.9b)--(1356.1a,250.9b);
draw (1339.5a,321.7b)--(1356.1a,321.7b);
draw (1489.5a,238.5b)--(1489.5a,317.4b);
draw (1481.2a,238.5b)--(1497.8a,238.5b);
draw (1481.2a,317.4b)--(1497.8a,317.4b);
gpdraw(0,922.7a,381.3b);
gpdraw(0,1347.8a,277.1b);
gpdraw(0,1489.5a,257.9b);
gpdraw(0,811.5a,481.8b);
linetype 3;
put_text( btex UDF\_UpdatePosition etex, 750.1a, 445.2b, 0, 3);
draw (767.8a,445.2b)--(855.2a,445.2b);
draw (767.8a,453.5b)--(767.8a,436.9b);
draw (855.2a,453.5b)--(855.2a,436.9b);
draw (922.7a,216.0b)--(922.7a,219.9b);
draw (914.4a,216.0b)--(931.0a,216.0b);
draw (914.4a,219.9b)--(931.0a,219.9b);
draw (1347.8a,174.8b)--(1347.8a,181.3b);
draw (1339.5a,174.8b)--(1356.1a,174.8b);
draw (1339.5a,181.3b)--(1356.1a,181.3b);
draw (1489.5a,171.3b)--(1489.5a,178.1b);
draw (1481.2a,171.3b)--(1497.8a,171.3b);
draw (1481.2a,178.1b)--(1497.8a,178.1b);
gpdraw(1,922.7a,218.3b);
gpdraw(1,1347.8a,177.1b);
gpdraw(1,1489.5a,173.1b);
gpdraw(1,811.5a,445.2b);
linetype 4;
put_text( btex UDF\_UpdateVelocity etex, 750.1a, 408.6b, 0, 3);
draw (767.8a,408.6b)--(855.2a,408.6b);
draw (767.8a,416.9b)--(767.8a,400.3b);
draw (855.2a,416.9b)--(855.2a,400.3b);
draw (922.7a,232.2b)--(922.7a,280.4b);
draw (914.4a,232.2b)--(931.0a,232.2b);
draw (914.4a,280.4b)--(931.0a,280.4b);
draw (1347.8a,163.9b)--(1347.8a,192.9b);
draw (1339.5a,163.9b)--(1356.1a,163.9b);
draw (1339.5a,192.9b)--(1356.1a,192.9b);
draw (1489.5a,159.9b)--(1489.5a,184.4b);
draw (1481.2a,159.9b)--(1497.8a,159.9b);
draw (1481.2a,184.4b)--(1497.8a,184.4b);
gpdraw(2,922.7a,260.4b);
gpdraw(2,1347.8a,179.3b);
gpdraw(2,1489.5a,171.9b);
gpdraw(2,811.5a,408.6b);
linetype 5;
put_text( btex UDF\_WaterSPCRattle etex, 750.1a, 372.0b, 0, 3);
draw (767.8a,372.0b)--(855.2a,372.0b);
draw (767.8a,380.3b)--(767.8a,363.7b);
draw (855.2a,380.3b)--(855.2a,363.7b);
draw (922.7a,571.3b)--(922.7a,582.5b);
draw (914.4a,571.3b)--(931.0a,571.3b);
draw (914.4a,582.5b)--(931.0a,582.5b);
draw (1347.8a,399.6b)--(1347.8a,426.6b);
draw (1339.5a,399.6b)--(1356.1a,399.6b);
draw (1339.5a,426.6b)--(1356.1a,426.6b);
draw (1489.5a,311.8b)--(1489.5a,387.3b);
draw (1481.2a,311.8b)--(1497.8a,311.8b);
draw (1481.2a,387.3b)--(1497.8a,387.3b);
gpdraw(3,922.7a,577.8b);
gpdraw(3,1347.8a,420.2b);
gpdraw(3,1489.5a,371.6b);
gpdraw(3,811.5a,372.0b);
linetype 6;
put_text( btex UDF\_WaterSPCShake etex, 750.1a, 335.4b, 0, 3);
draw (767.8a,335.4b)--(855.2a,335.4b);
draw (767.8a,343.7b)--(767.8a,327.1b);
draw (855.2a,343.7b)--(855.2a,327.1b);
draw (922.7a,544.4b)--(922.7a,556.0b);
draw (914.4a,544.4b)--(931.0a,544.4b);
draw (914.4a,556.0b)--(931.0a,556.0b);
draw (1347.8a,374.7b)--(1347.8a,403.0b);
draw (1339.5a,374.7b)--(1356.1a,374.7b);
draw (1339.5a,403.0b)--(1356.1a,403.0b);
draw (1489.5a,287.1b)--(1489.5a,364.7b);
draw (1481.2a,287.1b)--(1497.8a,287.1b);
draw (1481.2a,364.7b)--(1497.8a,364.7b);
gpdraw(4,922.7a,551.1b);
gpdraw(4,1347.8a,395.8b);
gpdraw(4,1489.5a,348.6b);
gpdraw(4,811.5a,335.4b);
linetype 7;
put_text( btex Total Time Step etex, 750.1a, 298.8b, 0, 3);
draw (767.8a,298.8b)--(855.2a,298.8b);
draw (767.8a,307.1b)--(767.8a,290.5b);
draw (855.2a,307.1b)--(855.2a,290.5b);
draw (922.7a,817.9b)--(922.7a,823.9b);
draw (914.4a,817.9b)--(931.0a,817.9b);
draw (914.4a,823.9b)--(931.0a,823.9b);
draw (1347.8a,708.5b)--(1347.8a,720.2b);
draw (1339.5a,708.5b)--(1356.1a,708.5b);
draw (1339.5a,720.2b)--(1356.1a,720.2b);
draw (1489.5a,687.6b)--(1489.5a,701.7b);
draw (1481.2a,687.6b)--(1497.8a,687.6b);
draw (1481.2a,701.7b)--(1497.8a,701.7b);
gpdraw(5,922.7a,820.5b);
gpdraw(5,1347.8a,714.1b);
gpdraw(5,1489.5a,694.2b);
gpdraw(5,811.5a,298.8b);
linetype 0;
draw (922.7a,591.6b)--(1347.8a,589.9b)--(1489.5a,591.2b);
linetype 1;
draw (922.7a,743.0b)--(1347.8a,618.1b)--(1489.5a,585.3b);
linetype 2;
draw (922.7a,756.8b)--(1347.8a,604.2b)--(1489.5a,559.4b);
linetype 3;
draw (922.7a,480.9b)--(1347.8a,412.5b)--(1489.5a,403.6b);
linetype 4;
draw (922.7a,510.3b)--(1347.8a,327.8b)--(1489.5a,273.6b);
linetype 5;
draw (922.7a,675.6b)--(1347.8a,553.0b)--(1489.5a,519.5b);
linetype 6;
draw (922.7a,497.8b)--(1347.8a,361.9b)--(1489.5a,326.8b);
linetype 7;
draw (922.7a,679.4b)--(1347.8a,551.6b)--(1489.5a,515.8b);
linetype 1;
draw (922.7a,583.3b)--(1347.8a,585.2b)--(1489.5a,583.8b);
linetype 2;
draw (922.7a,381.3b)--(1347.8a,277.1b)--(1489.5a,257.9b);
linetype 3;
draw (922.7a,218.3b)--(1347.8a,177.1b)--(1489.5a,173.1b);
linetype 4;
draw (922.7a,260.4b)--(1347.8a,179.3b)--(1489.5a,171.9b);
linetype 5;
draw (922.7a,577.8b)--(1347.8a,420.2b)--(1489.5a,371.6b);
linetype 6;
draw (922.7a,551.1b)--(1347.8a,395.8b)--(1489.5a,348.6b);
linetype 7;
draw (922.7a,820.5b)--(1347.8a,714.1b)--(1489.5a,694.2b);
endfig;
end.
b);
endfig;
end.
