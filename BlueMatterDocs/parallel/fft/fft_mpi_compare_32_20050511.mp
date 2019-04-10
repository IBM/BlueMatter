%GNUPLOT Metapost output: Fri May 13 12:28:14 2005


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

beginfig(2);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  1e-005 etex, 196.5a, 109.8b, 0, 3);
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
put_text( btex  0.0001 etex, 196.5a, 289.5b, 0, 3);
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
put_text( btex  0.001 etex, 196.5a, 469.2b, 0, 3);
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
put_text( btex  0.01 etex, 196.5a, 648.9b, 0, 3);
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
put_text( btex  0.1 etex, 196.5a, 828.6b, 0, 3);
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
put_text( btex $32^3$ MPI Collective (MPICH2) etex, 1245.8a, 793.7b, 0, 3);
linetype 0;
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (214.2a,646.1b)--(320.7a,603.1b)--(427.3a,567.8b)--(533.8a,546.0b)--(640.4a,496.6b)
--(746.9a,450.5b)--(853.4a,427.2b)--(960.0a,396.7b)--(1066.5a,394.1b)--(1173.1a,390.3b);
gpdraw(0,214.2a,646.1b);
gpdraw(0,320.7a,603.1b);
gpdraw(0,427.3a,567.8b);
gpdraw(0,533.8a,546.0b);
gpdraw(0,640.4a,496.6b);
gpdraw(0,746.9a,450.5b);
gpdraw(0,853.4a,427.2b);
gpdraw(0,960.0a,396.7b);
gpdraw(0,1066.5a,394.1b);
gpdraw(0,1173.1a,390.3b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
linetype -2;
put_text( btex $32^3$ MPI Collective (optimized) etex, 1245.8a, 757.1b, 0, 3);
linetype 1;
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (214.2a,638.6b)--(533.8a,541.4b)--(640.4a,488.0b)--(746.9a,441.4b)--(853.4a,407.4b)
--(960.0a,383.1b)--(1066.5a,372.9b)--(1173.1a,360.4b)--(1279.6a,361.3b);
gpdraw(1,214.2a,638.6b);
gpdraw(1,533.8a,541.4b);
gpdraw(1,640.4a,488.0b);
gpdraw(1,746.9a,441.4b);
gpdraw(1,853.4a,407.4b);
gpdraw(1,960.0a,383.1b);
gpdraw(1,1066.5a,372.9b);
gpdraw(1,1173.1a,360.4b);
gpdraw(1,1279.6a,361.3b);
gpdraw(1,1307.2a,757.1b);
linetype 2;
linetype -2;
put_text( btex $32^3$ BG/L ADE etex, 1245.8a, 720.5b, 0, 3);
linetype 2;
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (214.2a,679.5b)--(533.8a,517.2b)--(746.9a,427.4b)--(960.0a,347.4b)--(1173.1a,278.3b);
gpdraw(2,214.2a,679.5b);
gpdraw(2,533.8a,517.2b);
gpdraw(2,746.9a,427.4b);
gpdraw(2,960.0a,347.4b);
gpdraw(2,1173.1a,278.3b);
gpdraw(2,1307.2a,720.5b);
linetype 3;
linetype -2;
put_text( btex $32^3$ Model etex, 1245.8a, 683.9b, 0, 3);
linetype 3;
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (214.2a,495.7b)--(533.8a,459.0b)--(746.9a,396.7b)--(853.4a,342.6b)--(960.0a,324.8b)
--(1066.5a,284.1b)--(1173.1a,180.3b)--(1279.6a,162.5b);
linetype -2;
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
