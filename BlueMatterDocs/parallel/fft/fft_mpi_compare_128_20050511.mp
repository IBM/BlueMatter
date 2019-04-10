%GNUPLOT Metapost output: Fri May 13 12:06:43 2005


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

beginfig(0);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1386.3a,109.8b)--(1369.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,153.1b)--(222.5a,153.1b);
draw (1386.3a,153.1b)--(1378.0a,153.1b);
draw (214.2a,210.3b)--(222.5a,210.3b);
draw (1386.3a,210.3b)--(1378.0a,210.3b);
draw (214.2a,239.6b)--(222.5a,239.6b);
draw (1386.3a,239.6b)--(1378.0a,239.6b);
draw (214.2a,253.6b)--(230.8a,253.6b);
draw (1386.3a,253.6b)--(1369.7a,253.6b);
put_text( btex  0.001 etex, 196.5a, 253.6b, 0, 3);
draw (214.2a,296.8b)--(222.5a,296.8b);
draw (1386.3a,296.8b)--(1378.0a,296.8b);
draw (214.2a,354.0b)--(222.5a,354.0b);
draw (1386.3a,354.0b)--(1378.0a,354.0b);
draw (214.2a,383.4b)--(222.5a,383.4b);
draw (1386.3a,383.4b)--(1378.0a,383.4b);
draw (214.2a,397.3b)--(230.8a,397.3b);
draw (1386.3a,397.3b)--(1369.7a,397.3b);
put_text( btex  0.01 etex, 196.5a, 397.3b, 0, 3);
draw (214.2a,440.6b)--(222.5a,440.6b);
draw (1386.3a,440.6b)--(1378.0a,440.6b);
draw (214.2a,497.8b)--(222.5a,497.8b);
draw (1386.3a,497.8b)--(1378.0a,497.8b);
draw (214.2a,527.1b)--(222.5a,527.1b);
draw (1386.3a,527.1b)--(1378.0a,527.1b);
draw (214.2a,541.1b)--(230.8a,541.1b);
draw (1386.3a,541.1b)--(1369.7a,541.1b);
put_text( btex  0.1 etex, 196.5a, 541.1b, 0, 3);
draw (214.2a,584.4b)--(222.5a,584.4b);
draw (1386.3a,584.4b)--(1378.0a,584.4b);
draw (214.2a,641.6b)--(222.5a,641.6b);
draw (1386.3a,641.6b)--(1378.0a,641.6b);
draw (214.2a,670.9b)--(222.5a,670.9b);
draw (1386.3a,670.9b)--(1378.0a,670.9b);
draw (214.2a,684.8b)--(230.8a,684.8b);
draw (1386.3a,684.8b)--(1369.7a,684.8b);
put_text( btex  1 etex, 196.5a, 684.8b, 0, 3);
draw (214.2a,728.1b)--(222.5a,728.1b);
draw (1386.3a,728.1b)--(1378.0a,728.1b);
draw (214.2a,785.3b)--(222.5a,785.3b);
draw (1386.3a,785.3b)--(1378.0a,785.3b);
draw (214.2a,814.7b)--(222.5a,814.7b);
draw (1386.3a,814.7b)--(1378.0a,814.7b);
draw (214.2a,828.6b)--(230.8a,828.6b);
draw (1386.3a,828.6b)--(1369.7a,828.6b);
put_text( btex  10 etex, 196.5a, 828.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,828.6b)--(214.2a,812.0b);
put_text( btex  1 etex, 214.2a, 73.2b, 0, 2);
draw (297.6a,109.8b)--(297.6a,118.1b);
draw (297.6a,828.6b)--(297.6a,820.3b);
draw (407.9a,109.8b)--(407.9a,118.1b);
draw (407.9a,828.6b)--(407.9a,820.3b);
draw (464.4a,109.8b)--(464.4a,118.1b);
draw (464.4a,828.6b)--(464.4a,820.3b);
draw (491.3a,109.8b)--(491.3a,126.4b);
draw (491.3a,828.6b)--(491.3a,812.0b);
put_text( btex  10 etex, 491.3a, 73.2b, 0, 2);
draw (574.7a,109.8b)--(574.7a,118.1b);
draw (574.7a,828.6b)--(574.7a,820.3b);
draw (684.9a,109.8b)--(684.9a,118.1b);
draw (684.9a,828.6b)--(684.9a,820.3b);
draw (741.5a,109.8b)--(741.5a,118.1b);
draw (741.5a,828.6b)--(741.5a,820.3b);
draw (768.3a,109.8b)--(768.3a,126.4b);
draw (768.3a,828.6b)--(768.3a,812.0b);
put_text( btex  100 etex, 768.3a, 73.2b, 0, 2);
draw (851.7a,109.8b)--(851.7a,118.1b);
draw (851.7a,828.6b)--(851.7a,820.3b);
draw (962.0a,109.8b)--(962.0a,118.1b);
draw (962.0a,828.6b)--(962.0a,820.3b);
draw (1018.5a,109.8b)--(1018.5a,118.1b);
draw (1018.5a,828.6b)--(1018.5a,820.3b);
draw (1045.4a,109.8b)--(1045.4a,126.4b);
draw (1045.4a,828.6b)--(1045.4a,812.0b);
put_text( btex  1000 etex, 1045.4a, 73.2b, 0, 2);
draw (1128.8a,109.8b)--(1128.8a,118.1b);
draw (1128.8a,828.6b)--(1128.8a,820.3b);
draw (1239.0a,109.8b)--(1239.0a,118.1b);
draw (1239.0a,828.6b)--(1239.0a,820.3b);
draw (1295.6a,109.8b)--(1295.6a,118.1b);
draw (1295.6a,828.6b)--(1295.6a,820.3b);
draw (1322.5a,109.8b)--(1322.5a,126.4b);
draw (1322.5a,828.6b)--(1322.5a,812.0b);
put_text( btex  10000 etex, 1322.5a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
put_text( btex Time (sec.) etex, 36.6a, 469.2b, 1, 2);
put_text( btex Task Count etex, 800.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex $128^3$ MPI Collective (MPICH2) etex, 1245.8a, 793.7b, 0, 3);
linetype 0;
draw (1263.5a,793.7b)--(1350.9a,793.7b);
draw (214.2a,692.1b)--(297.6a,654.3b)--(381.0a,618.6b)--(464.4a,580.5b)--(547.8a,531.5b)
--(631.2a,489.6b)--(714.6a,449.7b)--(798.0a,412.3b)--(881.4a,370.8b)--(964.8a,346.8b)
--(1048.2a,337.8b)--(1131.6a,347.1b);
gpdraw(0,214.2a,692.1b);
gpdraw(0,297.6a,654.3b);
gpdraw(0,381.0a,618.6b);
gpdraw(0,464.4a,580.5b);
gpdraw(0,547.8a,531.5b);
gpdraw(0,631.2a,489.6b);
gpdraw(0,714.6a,449.7b);
gpdraw(0,798.0a,412.3b);
gpdraw(0,881.4a,370.8b);
gpdraw(0,964.8a,346.8b);
gpdraw(0,1048.2a,337.8b);
gpdraw(0,1131.6a,347.1b);
gpdraw(0,1307.2a,793.7b);
linetype 1;
linetype -2;
put_text( btex $128^3$ MPI Collective (optimized) etex, 1245.8a, 757.1b, 0, 3);
linetype 1;
draw (1263.5a,757.1b)--(1350.9a,757.1b);
draw (214.2a,688.4b)--(464.4a,577.9b)--(547.8a,535.6b)--(631.2a,483.2b)--(714.6a,442.5b)
--(798.0a,418.6b)--(881.4a,358.6b)--(964.8a,322.1b)--(1048.2a,295.4b)--(1131.6a,279.4b)
--(1215.1a,269.2b)--(1298.5a,259.5b);
gpdraw(1,214.2a,688.4b);
gpdraw(1,464.4a,577.9b);
gpdraw(1,547.8a,535.6b);
gpdraw(1,631.2a,483.2b);
gpdraw(1,714.6a,442.5b);
gpdraw(1,798.0a,418.6b);
gpdraw(1,881.4a,358.6b);
gpdraw(1,964.8a,322.1b);
gpdraw(1,1048.2a,295.4b);
gpdraw(1,1131.6a,279.4b);
gpdraw(1,1215.1a,269.2b);
gpdraw(1,1298.5a,259.5b);
gpdraw(1,1307.2a,757.1b);
linetype 2;
linetype -2;
put_text( btex $128^3$ BG/L ADE Single Core etex, 1245.8a, 720.5b, 0, 3);
linetype 2;
draw (1263.5a,720.5b)--(1350.9a,720.5b);
draw (964.8a,321.3b)--(1048.2a,298.4b)--(1131.6a,263.4b)--(1215.1a,261.2b)--(1298.5a,238.4b)
--(1381.9a,220.3b);
gpdraw(2,964.8a,321.3b);
gpdraw(2,1048.2a,298.4b);
gpdraw(2,1131.6a,263.4b);
gpdraw(2,1215.1a,261.2b);
gpdraw(2,1298.5a,238.4b);
gpdraw(2,1381.9a,220.3b);
gpdraw(2,1307.2a,720.5b);
linetype 3;
linetype -2;
put_text( btex $128^3$ Model etex, 1245.8a, 683.9b, 0, 3);
linetype 3;
draw (1263.5a,683.9b)--(1350.9a,683.9b);
draw (214.2a,555.4b)--(464.4a,509.9b)--(631.2a,457.9b)--(714.6a,414.7b)--(798.0a,399.4b)
--(881.4a,366.5b)--(964.8a,284.8b)--(1048.2a,269.6b)--(1131.6a,236.7b)--(1215.1a,193.5b)
--(1298.5a,180.0b)--(1381.9a,147.6b);
linetype -2;
draw (214.2a,109.8b)--(1386.3a,109.8b)--(1386.3a,828.6b)--(214.2a,828.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
