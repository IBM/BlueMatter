%GNUPLOT Metapost output: Fri Jul 16 00:32:03 2004


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
\usepackage{times}
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

beginfig(6);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  1e-005 etex, 196.5a, 109.8b, 0, 3);
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
draw (320.7a,109.8b)--(320.7a,118.1b);
draw (320.7a,828.6b)--(320.7a,820.3b);
draw (383.1a,109.8b)--(383.1a,118.1b);
draw (383.1a,828.6b)--(383.1a,820.3b);
draw (427.3a,109.8b)--(427.3a,118.1b);
draw (427.3a,828.6b)--(427.3a,820.3b);
draw (461.6a,109.8b)--(461.6a,118.1b);
draw (461.6a,828.6b)--(461.6a,820.3b);
draw (489.6a,109.8b)--(489.6a,118.1b);
draw (489.6a,828.6b)--(489.6a,820.3b);
draw (513.3a,109.8b)--(513.3a,118.1b);
draw (513.3a,828.6b)--(513.3a,820.3b);
draw (533.8a,109.8b)--(533.8a,118.1b);
draw (533.8a,828.6b)--(533.8a,820.3b);
draw (551.9a,109.8b)--(551.9a,118.1b);
draw (551.9a,828.6b)--(551.9a,820.3b);
draw (568.1a,109.8b)--(568.1a,126.4b);
draw (568.1a,828.6b)--(568.1a,812.0b);
put_text( btex  10 etex, 568.1a, 73.2b, 0, 2);
draw (674.7a,109.8b)--(674.7a,118.1b);
draw (674.7a,828.6b)--(674.7a,820.3b);
draw (737.0a,109.8b)--(737.0a,118.1b);
draw (737.0a,828.6b)--(737.0a,820.3b);
draw (781.2a,109.8b)--(781.2a,118.1b);
draw (781.2a,828.6b)--(781.2a,820.3b);
draw (815.5a,109.8b)--(815.5a,118.1b);
draw (815.5a,828.6b)--(815.5a,820.3b);
draw (843.5a,109.8b)--(843.5a,118.1b);
draw (843.5a,828.6b)--(843.5a,820.3b);
draw (867.2a,109.8b)--(867.2a,118.1b);
draw (867.2a,828.6b)--(867.2a,820.3b);
draw (887.7a,109.8b)--(887.7a,118.1b);
draw (887.7a,828.6b)--(887.7a,820.3b);
draw (905.8a,109.8b)--(905.8a,118.1b);
draw (905.8a,828.6b)--(905.8a,820.3b);
draw (922.0a,109.8b)--(922.0a,126.4b);
draw (922.0a,828.6b)--(922.0a,812.0b);
put_text( btex  100 etex, 922.0a, 73.2b, 0, 2);
draw (1028.6a,109.8b)--(1028.6a,118.1b);
draw (1028.6a,828.6b)--(1028.6a,820.3b);
draw (1090.9a,109.8b)--(1090.9a,118.1b);
draw (1090.9a,828.6b)--(1090.9a,820.3b);
draw (1135.1a,109.8b)--(1135.1a,118.1b);
draw (1135.1a,828.6b)--(1135.1a,820.3b);
draw (1169.4a,109.8b)--(1169.4a,118.1b);
draw (1169.4a,828.6b)--(1169.4a,820.3b);
draw (1197.4a,109.8b)--(1197.4a,118.1b);
draw (1197.4a,828.6b)--(1197.4a,820.3b);
draw (1221.1a,109.8b)--(1221.1a,118.1b);
draw (1221.1a,828.6b)--(1221.1a,820.3b);
draw (1241.7a,109.8b)--(1241.7a,118.1b);
draw (1241.7a,828.6b)--(1241.7a,820.3b);
draw (1259.8a,109.8b)--(1259.8a,118.1b);
draw (1259.8a,828.6b)--(1259.8a,820.3b);
draw (1276.0a,109.8b)--(1276.0a,126.4b);
draw (1276.0a,828.6b)--(1276.0a,812.0b);
put_text( btex  1000 etex, 1276.0a, 73.2b, 0, 2);
draw (1382.5a,109.8b)--(1382.5a,118.1b);
draw (1382.5a,828.6b)--(1382.5a,820.3b);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Time (sec.) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Task Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $128^3$ MPI Collective etex, 1245.8a, 793.7b, 0, 3);
linetype 0;
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (214.2a,714.9b)--(320.7a,683.3b)--(427.3a,653.6b)--(533.8a,621.9b)--(640.4a,581.0b)
--(746.9a,546.1b)--(853.4a,512.8b)--(960.0a,481.7b)--(1066.5a,447.1b)--(1173.1a,427.1b)
--(1279.6a,419.6b)--(1386.1a,427.3b);
gpdraw(0,214.2a,714.9b);
gpdraw(0,320.7a,683.3b);
gpdraw(0,427.3a,653.6b);
gpdraw(0,533.8a,621.9b);
gpdraw(0,640.4a,581.0b);
gpdraw(0,746.9a,546.1b);
gpdraw(0,853.4a,512.8b);
gpdraw(0,960.0a,481.7b);
gpdraw(0,1066.5a,447.1b);
gpdraw(0,1173.1a,427.1b);
gpdraw(0,1279.6a,419.6b);
gpdraw(0,1386.1a,427.3b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
linetype -2;
put_text( btex $128^3$ Active Packet etex, 1245.8a, 757.1b, 0, 3);
linetype 1;
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (533.8a,608.0b)--(746.9a,532.3b)--(960.0a,476.0b)--(1173.1a,397.9b)--(1279.6a,386.2b);
gpdraw(1,533.8a,608.0b);
gpdraw(1,746.9a,532.3b);
gpdraw(1,960.0a,476.0b);
gpdraw(1,1173.1a,397.9b);
gpdraw(1,1279.6a,386.2b);
gpdraw(1,1307.2a,757.1b);
linetype 2;
linetype -2;
put_text( btex $128^3$ Model etex, 1245.8a, 720.5b, 0, 3);
linetype 2;
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (214.2a,601.0b)--(533.8a,563.0b)--(746.9a,519.7b)--(853.4a,483.6b)--(960.0a,471.0b)
--(1066.5a,443.6b)--(1173.1a,375.5b)--(1279.6a,362.8b)--(1386.1a,335.4b)--(1386.3a,335.3b);
linetype 3;
linetype -2;
put_text( btex $64^3$ MPI Collective etex, 1245.8a, 683.9b, 0, 3);
linetype 3;
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (214.2a,591.2b)--(320.7a,562.0b)--(427.3a,532.5b)--(533.8a,514.0b)--(640.4a,475.3b)
--(746.9a,441.9b)--(853.4a,410.2b)--(960.0a,382.6b)--(1066.5a,359.5b)--(1173.1a,355.0b)
--(1279.6a,365.2b);
gpdraw(2,214.2a,591.2b);
gpdraw(2,320.7a,562.0b);
gpdraw(2,427.3a,532.5b);
gpdraw(2,533.8a,514.0b);
gpdraw(2,640.4a,475.3b);
gpdraw(2,746.9a,441.9b);
gpdraw(2,853.4a,410.2b);
gpdraw(2,960.0a,382.6b);
gpdraw(2,1066.5a,359.5b);
gpdraw(2,1173.1a,355.0b);
gpdraw(2,1279.6a,365.2b);
gpdraw(2,1307.2a,683.9b);
linetype 4;
linetype -2;
put_text( btex $64^3$ Active Packet etex, 1245.8a, 647.3b, 0, 3);
linetype 4;
draw (1263.5a,647.3b)--(1350.9a,647.3b);
draw (214.2a,608.0b)--(533.8a,490.3b)--(746.9a,425.9b)--(960.0a,371.9b)--(1173.1a,295.8b)
--(1279.6a,301.7b);
gpdraw(3,214.2a,608.0b);
gpdraw(3,533.8a,490.3b);
gpdraw(3,746.9a,425.9b);
gpdraw(3,960.0a,371.9b);
gpdraw(3,1173.1a,295.8b);
gpdraw(3,1279.6a,301.7b);
gpdraw(3,1307.2a,647.3b);
linetype 5;
linetype -2;
put_text( btex $64^3$ Model etex, 1245.8a, 610.7b, 0, 3);
linetype 5;
draw (1263.5a,610.7b)--(1350.9a,610.7b);
draw (214.2a,484.7b)--(533.8a,452.8b)--(746.9a,410.4b)--(853.4a,374.3b)--(960.0a,362.1b)
--(1066.5a,334.8b)--(1173.1a,266.2b)--(1279.6a,253.9b)--(1386.1a,226.6b)--(1386.3a,226.5b);
linetype 6;
linetype -2;
put_text( btex $32^3$ MPI Collective etex, 1245.8a, 574.1b, 0, 3);
linetype 6;
draw (1263.5a,574.1b)--(1350.9a,574.1b);
draw (214.2a,467.4b)--(320.7a,438.6b)--(427.3a,415.2b)--(533.8a,400.6b)--(640.4a,367.6b)
--(746.9a,336.9b)--(853.4a,321.4b)--(960.0a,301.1b)--(1066.5a,299.3b)--(1173.1a,296.8b);
gpdraw(4,214.2a,467.4b);
gpdraw(4,320.7a,438.6b);
gpdraw(4,427.3a,415.2b);
gpdraw(4,533.8a,400.6b);
gpdraw(4,640.4a,367.6b);
gpdraw(4,746.9a,336.9b);
gpdraw(4,853.4a,321.4b);
gpdraw(4,960.0a,301.1b);
gpdraw(4,1066.5a,299.3b);
gpdraw(4,1173.1a,296.8b);
gpdraw(4,1307.2a,574.1b);
linetype 7;
linetype -2;
put_text( btex $32^3$ Active Packet etex, 1245.8a, 537.5b, 0, 3);
linetype 7;
draw (1263.5a,537.5b)--(1350.9a,537.5b);
draw (214.2a,489.6b)--(533.8a,381.4b)--(746.9a,321.5b)--(960.0a,268.2b)--(1173.1a,222.1b);
gpdraw(5,214.2a,489.6b);
gpdraw(5,533.8a,381.4b);
gpdraw(5,746.9a,321.5b);
gpdraw(5,960.0a,268.2b);
gpdraw(5,1173.1a,222.1b);
gpdraw(5,1307.2a,537.5b);
linetype 0;
linetype -2;
put_text( btex $32^3$ Model etex, 1245.8a, 500.9b, 0, 3);
linetype 0;
draw (1263.5a,500.9b)--(1350.9a,500.9b);
draw (214.2a,367.1b)--(533.8a,342.6b)--(746.9a,301.1b)--(853.4a,265.0b)--(960.0a,253.2b)
--(1066.5a,226.0b)--(1173.1a,156.8b)--(1279.6a,145.0b);
linetype -2;
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
