%GNUPLOT Metapost output: Wed Apr 21 19:12:03 2004


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

beginfig(6);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1866.3a,109.8b)--(1849.7a,109.8b);
put_text( btex  $10^4$ etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,145.9b)--(222.5a,145.9b);
draw (1866.3a,145.9b)--(1858.0a,145.9b);
draw (214.2a,193.5b)--(222.5a,193.5b);
draw (1866.3a,193.5b)--(1858.0a,193.5b);
draw (214.2a,218.0b)--(222.5a,218.0b);
draw (1866.3a,218.0b)--(1858.0a,218.0b);
draw (214.2a,229.6b)--(230.8a,229.6b);
draw (1866.3a,229.6b)--(1849.7a,229.6b);
put_text( btex  $10^5$ etex, 196.5a, 229.6b, 0, 3);
draw (214.2a,265.7b)--(222.5a,265.7b);
draw (1866.3a,265.7b)--(1858.0a,265.7b);
draw (214.2a,313.3b)--(222.5a,313.3b);
draw (1866.3a,313.3b)--(1858.0a,313.3b);
draw (214.2a,337.8b)--(222.5a,337.8b);
draw (1866.3a,337.8b)--(1858.0a,337.8b);
draw (214.2a,349.4b)--(230.8a,349.4b);
draw (1866.3a,349.4b)--(1849.7a,349.4b);
put_text( btex  $10^6$ etex, 196.5a, 349.4b, 0, 3);
draw (214.2a,385.5b)--(222.5a,385.5b);
draw (1866.3a,385.5b)--(1858.0a,385.5b);
draw (214.2a,433.1b)--(222.5a,433.1b);
draw (1866.3a,433.1b)--(1858.0a,433.1b);
draw (214.2a,457.6b)--(222.5a,457.6b);
draw (1866.3a,457.6b)--(1858.0a,457.6b);
draw (214.2a,469.2b)--(230.8a,469.2b);
draw (1866.3a,469.2b)--(1849.7a,469.2b);
put_text( btex  $10^7$ etex, 196.5a, 469.2b, 0, 3);
draw (214.2a,505.3b)--(222.5a,505.3b);
draw (1866.3a,505.3b)--(1858.0a,505.3b);
draw (214.2a,552.9b)--(222.5a,552.9b);
draw (1866.3a,552.9b)--(1858.0a,552.9b);
draw (214.2a,577.4b)--(222.5a,577.4b);
draw (1866.3a,577.4b)--(1858.0a,577.4b);
draw (214.2a,589.0b)--(230.8a,589.0b);
draw (1866.3a,589.0b)--(1849.7a,589.0b);
put_text( btex  $10^8$ etex, 196.5a, 589.0b, 0, 3);
draw (214.2a,625.1b)--(222.5a,625.1b);
draw (1866.3a,625.1b)--(1858.0a,625.1b);
draw (214.2a,672.7b)--(222.5a,672.7b);
draw (1866.3a,672.7b)--(1858.0a,672.7b);
draw (214.2a,697.2b)--(222.5a,697.2b);
draw (1866.3a,697.2b)--(1858.0a,697.2b);
draw (214.2a,708.8b)--(230.8a,708.8b);
draw (1866.3a,708.8b)--(1849.7a,708.8b);
put_text( btex  $10^9$ etex, 196.5a, 708.8b, 0, 3);
draw (214.2a,744.9b)--(222.5a,744.9b);
draw (1866.3a,744.9b)--(1858.0a,744.9b);
draw (214.2a,792.5b)--(222.5a,792.5b);
draw (1866.3a,792.5b)--(1858.0a,792.5b);
draw (214.2a,817.0b)--(222.5a,817.0b);
draw (1866.3a,817.0b)--(1858.0a,817.0b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1866.3a,828.6b)--(1849.7a,828.6b);
put_text( btex  $10^{10}$ etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  $1$ etex, 214.2a, 73.2b, 0, 2);
draw (380.0a,109.8b)--(380.0a,118.1b);
draw (380.0a,828.6b)--(380.0a,820.3b);
draw (477.0a,109.8b)--(477.0a,118.1b);
draw (477.0a,828.6b)--(477.0a,820.3b);
draw (545.8a,109.8b)--(545.8a,118.1b);
draw (545.8a,828.6b)--(545.8a,820.3b);
draw (599.1a,109.8b)--(599.1a,118.1b);
draw (599.1a,828.6b)--(599.1a,820.3b);
draw (642.7a,109.8b)--(642.7a,118.1b);
draw (642.7a,828.6b)--(642.7a,820.3b);
draw (679.6a,109.8b)--(679.6a,118.1b);
draw (679.6a,828.6b)--(679.6a,820.3b);
draw (711.5a,109.8b)--(711.5a,118.1b);
draw (711.5a,828.6b)--(711.5a,820.3b);
draw (739.7a,109.8b)--(739.7a,118.1b);
draw (739.7a,828.6b)--(739.7a,820.3b);
draw (764.9a,109.8b)--(764.9a,126.4b);
draw (764.9a,828.6b)--(764.9a,812.0b);
put_text( btex  $10$ etex, 764.9a, 73.2b, 0, 2);
draw (930.7a,109.8b)--(930.7a,118.1b);
draw (930.7a,828.6b)--(930.7a,820.3b);
draw (1027.7a,109.8b)--(1027.7a,118.1b);
draw (1027.7a,828.6b)--(1027.7a,820.3b);
draw (1096.5a,109.8b)--(1096.5a,118.1b);
draw (1096.5a,828.6b)--(1096.5a,820.3b);
draw (1149.8a,109.8b)--(1149.8a,118.1b);
draw (1149.8a,828.6b)--(1149.8a,820.3b);
draw (1193.4a,109.8b)--(1193.4a,118.1b);
draw (1193.4a,828.6b)--(1193.4a,820.3b);
draw (1230.3a,109.8b)--(1230.3a,118.1b);
draw (1230.3a,828.6b)--(1230.3a,820.3b);
draw (1262.2a,109.8b)--(1262.2a,118.1b);
draw (1262.2a,828.6b)--(1262.2a,820.3b);
draw (1290.4a,109.8b)--(1290.4a,118.1b);
draw (1290.4a,828.6b)--(1290.4a,820.3b);
draw (1315.6a,109.8b)--(1315.6a,126.4b);
draw (1315.6a,828.6b)--(1315.6a,812.0b);
put_text( btex  $100$ etex, 1315.6a, 73.2b, 0, 2);
draw (1481.4a,109.8b)--(1481.4a,118.1b);
draw (1481.4a,828.6b)--(1481.4a,820.3b);
draw (1578.4a,109.8b)--(1578.4a,118.1b);
draw (1578.4a,828.6b)--(1578.4a,820.3b);
draw (1647.2a,109.8b)--(1647.2a,118.1b);
draw (1647.2a,828.6b)--(1647.2a,820.3b);
draw (1700.5a,109.8b)--(1700.5a,118.1b);
draw (1700.5a,828.6b)--(1700.5a,820.3b);
draw (1744.1a,109.8b)--(1744.1a,118.1b);
draw (1744.1a,828.6b)--(1744.1a,820.3b);
draw (1781.0a,109.8b)--(1781.0a,118.1b);
draw (1781.0a,828.6b)--(1781.0a,820.3b);
draw (1812.9a,109.8b)--(1812.9a,118.1b);
draw (1812.9a,828.6b)--(1812.9a,820.3b);
draw (1841.1a,109.8b)--(1841.1a,118.1b);
draw (1841.1a,828.6b)--(1841.1a,820.3b);
draw (1866.3a,109.8b)--(1866.3a,126.4b);
draw (1866.3a,828.6b)--(1866.3a,812.0b);
put_text( btex  $1000$ etex, 1866.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1866.3a,109.8b)--(1866.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (pclocks) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Node Count etex, 1040.2a, 18.3b, 0, 2);
linetype 0;
put_text( btex Ewald\_AllReduce etex, 581.4a, 708.8b, 0, 3);
draw (599.1a,708.8b)--(686.5a,708.8b);
draw (599.1a,717.1b)--(599.1a,700.5b);
draw (686.5a,717.1b)--(686.5a,700.5b);
draw (711.5a,271.1b)--(711.5a,485.0b);
draw (703.2a,271.1b)--(719.8a,271.1b);
draw (703.2a,485.0b)--(719.8a,485.0b);
draw (1043.1a,273.4b)--(1043.1a,457.0b);
draw (1034.8a,273.4b)--(1051.4a,273.4b);
draw (1034.8a,457.0b)--(1051.4a,457.0b);
draw (1374.6a,277.5b)--(1374.6a,439.5b);
draw (1366.3a,277.5b)--(1382.9a,277.5b);
draw (1366.3a,439.5b)--(1382.9a,439.5b);
draw (1540.4a,282.6b)--(1540.4a,433.4b);
draw (1532.1a,282.6b)--(1548.7a,282.6b);
draw (1532.1a,433.4b)--(1548.7a,433.4b);
draw (1706.2a,286.8b)--(1706.2a,427.7b);
draw (1697.9a,286.8b)--(1714.5a,286.8b);
draw (1697.9a,427.7b)--(1714.5a,427.7b);
gpdraw(0,711.5a,411.3b);
gpdraw(0,1043.1a,383.9b);
gpdraw(0,1374.6a,328.9b);
gpdraw(0,1540.4a,322.5b);
gpdraw(0,1706.2a,334.0b);
gpdraw(0,642.8a,708.8b);
linetype 1;
put_text( btex Ewald\_Eiks etex, 581.4a, 672.2b, 0, 3);
draw (599.1a,672.2b)--(686.5a,672.2b);
draw (599.1a,680.5b)--(599.1a,663.9b);
draw (686.5a,680.5b)--(686.5a,663.9b);
draw (711.5a,396.1b)--(711.5a,397.5b);
draw (703.2a,396.1b)--(719.8a,396.1b);
draw (703.2a,397.5b)--(719.8a,397.5b);
draw (1043.1a,325.4b)--(1043.1a,393.8b);
draw (1034.8a,325.4b)--(1051.4a,325.4b);
draw (1034.8a,393.8b)--(1051.4a,393.8b);
draw (1374.6a,255.3b)--(1374.6a,268.4b);
draw (1366.3a,255.3b)--(1382.9a,255.3b);
draw (1366.3a,268.4b)--(1382.9a,268.4b);
draw (1540.4a,227.5b)--(1540.4a,242.8b);
draw (1532.1a,227.5b)--(1548.7a,227.5b);
draw (1532.1a,242.8b)--(1548.7a,242.8b);
draw (1706.2a,200.4b)--(1706.2a,226.4b);
draw (1697.9a,200.4b)--(1714.5a,200.4b);
draw (1697.9a,226.4b)--(1714.5a,226.4b);
gpdraw(1,711.5a,396.9b);
gpdraw(1,1043.1a,328.2b);
gpdraw(1,1374.6a,263.6b);
gpdraw(1,1540.4a,236.7b);
gpdraw(1,1706.2a,214.7b);
gpdraw(1,642.8a,672.2b);
linetype 2;
put_text( btex Ewald\_Forces etex, 581.4a, 635.6b, 0, 3);
draw (599.1a,635.6b)--(686.5a,635.6b);
draw (599.1a,643.9b)--(599.1a,627.3b);
draw (686.5a,643.9b)--(686.5a,627.3b);
draw (711.5a,612.1b)--(711.5a,614.8b);
draw (703.2a,612.1b)--(719.8a,612.1b);
draw (703.2a,614.8b)--(719.8a,614.8b);
draw (1043.1a,543.4b)--(1043.1a,550.2b);
draw (1034.8a,543.4b)--(1051.4a,543.4b);
draw (1034.8a,550.2b)--(1051.4a,550.2b);
draw (1374.6a,522.6b)--(1374.6a,524.0b);
draw (1366.3a,522.6b)--(1382.9a,522.6b);
draw (1366.3a,524.0b)--(1382.9a,524.0b);
draw (1540.4a,519.2b)--(1540.4a,520.5b);
draw (1532.1a,519.2b)--(1548.7a,519.2b);
draw (1532.1a,520.5b)--(1548.7a,520.5b);
draw (1706.2a,517.2b)--(1706.2a,518.8b);
draw (1697.9a,517.2b)--(1714.5a,517.2b);
draw (1697.9a,518.8b)--(1714.5a,518.8b);
gpdraw(2,711.5a,612.7b);
gpdraw(2,1043.1a,544.5b);
gpdraw(2,1374.6a,523.6b);
gpdraw(2,1540.4a,520.0b);
gpdraw(2,1706.2a,518.1b);
gpdraw(2,642.8a,635.6b);
linetype 3;
put_text( btex Ewald\_KSpace etex, 581.4a, 599.0b, 0, 3);
draw (599.1a,599.0b)--(686.5a,599.0b);
draw (599.1a,607.3b)--(599.1a,590.7b);
draw (686.5a,607.3b)--(686.5a,590.7b);
draw (711.5a,756.2b)--(711.5a,756.3b);
draw (703.2a,756.2b)--(719.8a,756.2b);
draw (703.2a,756.3b)--(719.8a,756.3b);
draw (1043.1a,660.5b)--(1043.1a,660.8b);
draw (1034.8a,660.5b)--(1051.4a,660.5b);
draw (1034.8a,660.8b)--(1051.4a,660.8b);
draw (1374.6a,622.0b)--(1374.6a,622.5b);
draw (1366.3a,622.0b)--(1382.9a,622.0b);
draw (1366.3a,622.5b)--(1382.9a,622.5b);
draw (1540.4a,616.4b)--(1540.4a,616.8b);
draw (1532.1a,616.4b)--(1548.7a,616.4b);
draw (1532.1a,616.8b)--(1548.7a,616.8b);
draw (1706.2a,613.8b)--(1706.2a,614.4b);
draw (1697.9a,613.8b)--(1714.5a,613.8b);
draw (1697.9a,614.4b)--(1714.5a,614.4b);
gpdraw(3,711.5a,756.2b);
gpdraw(3,1043.1a,660.6b);
gpdraw(3,1374.6a,622.2b);
gpdraw(3,1540.4a,616.6b);
gpdraw(3,1706.2a,614.0b);
gpdraw(3,642.8a,599.0b);
linetype 4;
put_text( btex Ewald\_LocalReduce etex, 581.4a, 562.4b, 0, 3);
draw (599.1a,562.4b)--(686.5a,562.4b);
draw (599.1a,570.7b)--(599.1a,554.1b);
draw (686.5a,570.7b)--(686.5a,554.1b);
draw (711.5a,558.4b)--(711.5a,561.1b);
draw (703.2a,558.4b)--(719.8a,558.4b);
draw (703.2a,561.1b)--(719.8a,561.1b);
draw (1043.1a,439.2b)--(1043.1a,443.0b);
draw (1034.8a,439.2b)--(1051.4a,439.2b);
draw (1034.8a,443.0b)--(1051.4a,443.0b);
draw (1374.6a,336.8b)--(1374.6a,347.4b);
draw (1366.3a,336.8b)--(1382.9a,336.8b);
draw (1366.3a,347.4b)--(1382.9a,347.4b);
draw (1540.4a,298.8b)--(1540.4a,312.2b);
draw (1532.1a,298.8b)--(1548.7a,298.8b);
draw (1532.1a,312.2b)--(1548.7a,312.2b);
draw (1706.2a,241.2b)--(1706.2a,285.9b);
draw (1697.9a,241.2b)--(1714.5a,241.2b);
draw (1697.9a,285.9b)--(1714.5a,285.9b);
gpdraw(4,711.5a,559.3b);
gpdraw(4,1043.1a,441.2b);
gpdraw(4,1374.6a,344.4b);
gpdraw(4,1540.4a,308.2b);
gpdraw(4,1706.2a,273.1b);
gpdraw(4,642.8a,562.4b);
linetype 5;
put_text( btex Ewald\_Loop etex, 581.4a, 525.8b, 0, 3);
draw (599.1a,525.8b)--(686.5a,525.8b);
draw (599.1a,534.1b)--(599.1a,517.5b);
draw (686.5a,534.1b)--(686.5a,517.5b);
draw (711.5a,756.1b)--(711.5a,756.2b);
draw (703.2a,756.1b)--(719.8a,756.1b);
draw (703.2a,756.2b)--(719.8a,756.2b);
draw (1043.1a,660.2b)--(1043.1a,660.7b);
draw (1034.8a,660.2b)--(1051.4a,660.2b);
draw (1034.8a,660.7b)--(1051.4a,660.7b);
draw (1374.6a,622.0b)--(1374.6a,622.4b);
draw (1366.3a,622.0b)--(1382.9a,622.0b);
draw (1366.3a,622.4b)--(1382.9a,622.4b);
draw (1540.4a,616.4b)--(1540.4a,616.8b);
draw (1532.1a,616.4b)--(1548.7a,616.4b);
draw (1532.1a,616.8b)--(1548.7a,616.8b);
draw (1706.2a,613.8b)--(1706.2a,614.3b);
draw (1697.9a,613.8b)--(1714.5a,613.8b);
draw (1697.9a,614.3b)--(1714.5a,614.3b);
gpdraw(5,711.5a,756.2b);
gpdraw(5,1043.1a,660.5b);
gpdraw(5,1374.6a,622.2b);
gpdraw(5,1540.4a,616.5b);
gpdraw(5,1706.2a,614.0b);
gpdraw(5,642.8a,525.8b);
linetype 6;
put_text( btex GlobalizePositions\_Cntl etex, 581.4a, 489.2b, 0, 3);
draw (599.1a,489.2b)--(686.5a,489.2b);
draw (599.1a,497.5b)--(599.1a,480.9b);
draw (686.5a,497.5b)--(686.5a,480.9b);
draw (711.5a,411.1b)--(711.5a,436.8b);
draw (703.2a,411.1b)--(719.8a,411.1b);
draw (703.2a,436.8b)--(719.8a,436.8b);
draw (1043.1a,416.4b)--(1043.1a,441.2b);
draw (1034.8a,416.4b)--(1051.4a,416.4b);
draw (1034.8a,441.2b)--(1051.4a,441.2b);
draw (1374.6a,419.5b)--(1374.6a,439.5b);
draw (1366.3a,419.5b)--(1382.9a,419.5b);
draw (1366.3a,439.5b)--(1382.9a,439.5b);
draw (1540.4a,417.9b)--(1540.4a,440.2b);
draw (1532.1a,417.9b)--(1548.7a,417.9b);
draw (1532.1a,440.2b)--(1548.7a,440.2b);
draw (1706.2a,419.1b)--(1706.2a,440.2b);
draw (1697.9a,419.1b)--(1714.5a,419.1b);
draw (1697.9a,440.2b)--(1714.5a,440.2b);
gpdraw(6,711.5a,424.0b);
gpdraw(6,1043.1a,428.3b);
gpdraw(6,1374.6a,430.2b);
gpdraw(6,1540.4a,429.9b);
gpdraw(6,1706.2a,430.3b);
gpdraw(6,642.8a,489.2b);
linetype 2;
put_text( btex P2\_KSpace\_Control etex, 581.4a, 452.6b, 0, 3);
draw (599.1a,452.6b)--(686.5a,452.6b);
draw (599.1a,460.9b)--(599.1a,444.3b);
draw (686.5a,460.9b)--(686.5a,444.3b);
draw (711.5a,756.2b)--(711.5a,756.3b);
draw (703.2a,756.2b)--(719.8a,756.2b);
draw (703.2a,756.3b)--(719.8a,756.3b);
draw (1043.1a,660.5b)--(1043.1a,660.8b);
draw (1034.8a,660.5b)--(1051.4a,660.5b);
draw (1034.8a,660.8b)--(1051.4a,660.8b);
draw (1374.6a,622.0b)--(1374.6a,622.5b);
draw (1366.3a,622.0b)--(1382.9a,622.0b);
draw (1366.3a,622.5b)--(1382.9a,622.5b);
draw (1540.4a,616.4b)--(1540.4a,616.8b);
draw (1532.1a,616.4b)--(1548.7a,616.4b);
draw (1532.1a,616.8b)--(1548.7a,616.8b);
draw (1706.2a,613.8b)--(1706.2a,614.4b);
draw (1697.9a,613.8b)--(1714.5a,613.8b);
draw (1697.9a,614.4b)--(1714.5a,614.4b);
gpdraw(8,711.5a,756.2b);
gpdraw(8,1043.1a,660.6b);
gpdraw(8,1374.6a,622.2b);
gpdraw(8,1540.4a,616.6b);
gpdraw(8,1706.2a,614.0b);
gpdraw(8,642.8a,452.6b);
linetype 3;
put_text( btex P2\_NSQ\_Control etex, 581.4a, 416.0b, 0, 3);
draw (599.1a,416.0b)--(686.5a,416.0b);
draw (599.1a,424.3b)--(599.1a,407.7b);
draw (686.5a,424.3b)--(686.5a,407.7b);
draw (711.5a,610.2b)--(711.5a,617.2b);
draw (703.2a,610.2b)--(719.8a,610.2b);
draw (703.2a,617.2b)--(719.8a,617.2b);
draw (1043.1a,537.8b)--(1043.1a,546.6b);
draw (1034.8a,537.8b)--(1051.4a,537.8b);
draw (1034.8a,546.6b)--(1051.4a,546.6b);
draw (1374.6a,466.1b)--(1374.6a,480.9b);
draw (1366.3a,466.1b)--(1382.9a,466.1b);
draw (1366.3a,480.9b)--(1382.9a,480.9b);
draw (1540.4a,430.1b)--(1540.4a,451.4b);
draw (1532.1a,430.1b)--(1548.7a,430.1b);
draw (1532.1a,451.4b)--(1548.7a,451.4b);
draw (1706.2a,395.9b)--(1706.2a,427.9b);
draw (1697.9a,395.9b)--(1714.5a,395.9b);
draw (1697.9a,427.9b)--(1714.5a,427.9b);
gpdraw(9,711.5a,613.0b);
gpdraw(9,1043.1a,541.1b);
gpdraw(9,1374.6a,471.7b);
gpdraw(9,1540.4a,439.0b);
gpdraw(9,1706.2a,409.2b);
gpdraw(9,642.8a,416.0b);
linetype 1;
put_text( btex ReduceForces\_Cntl etex, 581.4a, 379.4b, 0, 3);
draw (599.1a,379.4b)--(686.5a,379.4b);
draw (599.1a,387.7b)--(599.1a,371.1b);
draw (686.5a,387.7b)--(686.5a,371.1b);
draw (711.5a,409.8b)--(711.5a,439.6b);
draw (703.2a,409.8b)--(719.8a,409.8b);
draw (703.2a,439.6b)--(719.8a,439.6b);
draw (1043.1a,414.7b)--(1043.1a,438.8b);
draw (1034.8a,414.7b)--(1051.4a,414.7b);
draw (1034.8a,438.8b)--(1051.4a,438.8b);
draw (1374.6a,417.8b)--(1374.6a,435.2b);
draw (1366.3a,417.8b)--(1382.9a,417.8b);
draw (1366.3a,435.2b)--(1382.9a,435.2b);
draw (1540.4a,417.8b)--(1540.4a,434.1b);
draw (1532.1a,417.8b)--(1548.7a,417.8b);
draw (1532.1a,434.1b)--(1548.7a,434.1b);
draw (1706.2a,419.3b)--(1706.2a,436.1b);
draw (1697.9a,419.3b)--(1714.5a,419.3b);
draw (1697.9a,436.1b)--(1714.5a,436.1b);
gpdraw(9,711.5a,428.0b);
gpdraw(9,1043.1a,428.9b);
gpdraw(9,1374.6a,427.2b);
gpdraw(9,1540.4a,427.0b);
gpdraw(9,1706.2a,428.8b);
gpdraw(9,642.8a,379.4b);
linetype 3;
put_text( btex StdHarmonicBondForce etex, 581.4a, 342.8b, 0, 3);
draw (599.1a,342.8b)--(686.5a,342.8b);
draw (599.1a,351.1b)--(599.1a,334.5b);
draw (686.5a,351.1b)--(686.5a,334.5b);
draw (711.5a,348.9b)--(711.5a,358.0b);
draw (703.2a,348.9b)--(719.8a,348.9b);
draw (703.2a,358.0b)--(719.8a,358.0b);
draw (1043.1a,264.6b)--(1043.1a,309.7b);
draw (1034.8a,264.6b)--(1051.4a,264.6b);
draw (1034.8a,309.7b)--(1051.4a,309.7b);
draw (1374.6a,216.0b)--(1374.6a,272.3b);
draw (1366.3a,216.0b)--(1382.9a,216.0b);
draw (1366.3a,272.3b)--(1382.9a,272.3b);
draw (1540.4a,203.9b)--(1540.4a,263.0b);
draw (1532.1a,203.9b)--(1548.7a,203.9b);
draw (1532.1a,263.0b)--(1548.7a,263.0b);
draw (1706.2a,194.6b)--(1706.2a,247.5b);
draw (1697.9a,194.6b)--(1714.5a,194.6b);
draw (1697.9a,247.5b)--(1714.5a,247.5b);
gpdraw(1,711.5a,352.9b);
gpdraw(1,1043.1a,289.8b);
gpdraw(1,1374.6a,238.8b);
gpdraw(1,1540.4a,220.6b);
gpdraw(1,1706.2a,207.8b);
gpdraw(1,642.8a,342.8b);
linetype 2;
put_text( btex WaterSPCRattle etex, 581.4a, 306.2b, 0, 3);
draw (599.1a,306.2b)--(686.5a,306.2b);
draw (599.1a,314.5b)--(599.1a,297.9b);
draw (686.5a,314.5b)--(686.5a,297.9b);
draw (711.5a,492.6b)--(711.5a,493.9b);
draw (703.2a,492.6b)--(719.8a,492.6b);
draw (703.2a,493.9b)--(719.8a,493.9b);
draw (1043.1a,417.4b)--(1043.1a,423.9b);
draw (1034.8a,417.4b)--(1051.4a,417.4b);
draw (1034.8a,423.9b)--(1051.4a,423.9b);
draw (1374.6a,341.2b)--(1374.6a,355.0b);
draw (1366.3a,341.2b)--(1382.9a,341.2b);
draw (1366.3a,355.0b)--(1382.9a,355.0b);
draw (1540.4a,303.1b)--(1540.4a,320.8b);
draw (1532.1a,303.1b)--(1548.7a,303.1b);
draw (1532.1a,320.8b)--(1548.7a,320.8b);
draw (1706.2a,241.2b)--(1706.2a,294.8b);
draw (1697.9a,241.2b)--(1714.5a,241.2b);
draw (1697.9a,294.8b)--(1714.5a,294.8b);
gpdraw(0,711.5a,493.5b);
gpdraw(0,1043.1a,421.6b);
gpdraw(0,1374.6a,350.8b);
gpdraw(0,1540.4a,316.5b);
gpdraw(0,1706.2a,283.8b);
gpdraw(0,642.8a,306.2b);
linetype 7;
put_text( btex WaterSPCShake etex, 581.4a, 269.6b, 0, 3);
draw (599.1a,269.6b)--(686.5a,269.6b);
draw (599.1a,277.9b)--(599.1a,261.3b);
draw (686.5a,277.9b)--(686.5a,261.3b);
draw (711.5a,474.4b)--(711.5a,475.9b);
draw (703.2a,474.4b)--(719.8a,474.4b);
draw (703.2a,475.9b)--(719.8a,475.9b);
draw (1043.1a,399.5b)--(1043.1a,406.3b);
draw (1034.8a,399.5b)--(1051.4a,399.5b);
draw (1034.8a,406.3b)--(1051.4a,406.3b);
draw (1374.6a,324.3b)--(1374.6a,338.4b);
draw (1366.3a,324.3b)--(1382.9a,324.3b);
draw (1366.3a,338.4b)--(1382.9a,338.4b);
draw (1540.4a,287.6b)--(1540.4a,305.0b);
draw (1532.1a,287.6b)--(1548.7a,287.6b);
draw (1532.1a,305.0b)--(1548.7a,305.0b);
draw (1706.2a,228.5b)--(1706.2a,279.9b);
draw (1697.9a,228.5b)--(1714.5a,228.5b);
draw (1697.9a,279.9b)--(1714.5a,279.9b);
gpdraw(5,711.5a,475.4b);
gpdraw(5,1043.1a,403.9b);
gpdraw(5,1374.6a,333.8b);
gpdraw(5,1540.4a,300.4b);
gpdraw(5,1706.2a,269.1b);
gpdraw(5,642.8a,269.6b);
linetype 1;
put_text( btex TS\_from\_Ewald\_Eiks etex, 581.4a, 233.0b, 0, 3);
draw (599.1a,233.0b)--(686.5a,233.0b);
draw (599.1a,241.3b)--(599.1a,224.7b);
draw (686.5a,241.3b)--(686.5a,224.7b);
draw (711.5a,760.0b)--(711.5a,760.4b);
draw (703.2a,760.0b)--(719.8a,760.0b);
draw (703.2a,760.4b)--(719.8a,760.4b);
draw (1043.1a,667.3b)--(1043.1a,668.0b);
draw (1034.8a,667.3b)--(1051.4a,667.3b);
draw (1034.8a,668.0b)--(1051.4a,668.0b);
draw (1374.6a,627.7b)--(1374.6a,628.4b);
draw (1366.3a,627.7b)--(1382.9a,627.7b);
draw (1366.3a,628.4b)--(1382.9a,628.4b);
draw (1540.4a,621.1b)--(1540.4a,621.8b);
draw (1532.1a,621.1b)--(1548.7a,621.1b);
draw (1532.1a,621.8b)--(1548.7a,621.8b);
draw (1706.2a,617.9b)--(1706.2a,618.6b);
draw (1697.9a,617.9b)--(1714.5a,617.9b);
draw (1697.9a,618.6b)--(1714.5a,618.6b);
gpdraw(7,711.5a,760.2b);
gpdraw(7,1043.1a,667.6b);
gpdraw(7,1374.6a,628.0b);
gpdraw(7,1540.4a,621.4b);
gpdraw(7,1706.2a,618.2b);
gpdraw(7,642.8a,233.0b);
linetype 0;
draw (711.5a,411.3b)--(1043.1a,383.9b)--(1374.6a,328.9b)--(1540.4a,322.5b)--(1706.2a,334.0b);
linetype 1;
draw (711.5a,396.9b)--(1043.1a,328.2b)--(1374.6a,263.6b)--(1540.4a,236.7b)--(1706.2a,214.7b);
linetype 2;
draw (711.5a,612.7b)--(1043.1a,544.5b)--(1374.6a,523.6b)--(1540.4a,520.0b)--(1706.2a,518.1b);
linetype 3;
draw (711.5a,756.2b)--(1043.1a,660.6b)--(1374.6a,622.2b)--(1540.4a,616.6b)--(1706.2a,614.0b);
linetype 4;
draw (711.5a,559.3b)--(1043.1a,441.2b)--(1374.6a,344.4b)--(1540.4a,308.2b)--(1706.2a,273.1b);
linetype 5;
draw (711.5a,756.2b)--(1043.1a,660.5b)--(1374.6a,622.2b)--(1540.4a,616.5b)--(1706.2a,614.0b);
linetype 6;
draw (711.5a,424.0b)--(1043.1a,428.3b)--(1374.6a,430.2b)--(1540.4a,429.9b)--(1706.2a,430.3b);
linetype 2;
draw (711.5a,756.2b)--(1043.1a,660.6b)--(1374.6a,622.2b)--(1540.4a,616.6b)--(1706.2a,614.0b);
linetype 3;
draw (711.5a,613.0b)--(1043.1a,541.1b)--(1374.6a,471.7b)--(1540.4a,439.0b)--(1706.2a,409.2b);
linetype 1;
draw (711.5a,428.0b)--(1043.1a,428.9b)--(1374.6a,427.2b)--(1540.4a,427.0b)--(1706.2a,428.8b);
linetype 3;
draw (711.5a,352.9b)--(1043.1a,289.8b)--(1374.6a,238.8b)--(1540.4a,220.6b)--(1706.2a,207.8b);
linetype 2;
draw (711.5a,493.5b)--(1043.1a,421.6b)--(1374.6a,350.8b)--(1540.4a,316.5b)--(1706.2a,283.8b);
linetype 7;
draw (711.5a,475.4b)--(1043.1a,403.9b)--(1374.6a,333.8b)--(1540.4a,300.4b)--(1706.2a,269.1b);
linetype 1;
draw (711.5a,760.2b)--(1043.1a,667.6b)--(1374.6a,628.0b)--(1540.4a,621.4b)--(1706.2a,618.2b);
endfig;
end.
a,618.2b);
endfig;
end.
