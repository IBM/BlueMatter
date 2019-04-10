%GNUPLOT Metapost output: Tue Oct 29 14:43:44 2002


defaultmpt:=mpt:=4;
th:=.6;

def scalepen expr n = pickup pencircle scaled (n*th) enddef;
def ptsize expr n = mpt:=n*defaultmpt enddef;

textmag:= 1.000;
vardef makepic(expr str) =
  if picture str : str scaled textmag
  % otherwise a string
  else: str infont defaultfont scaled (defaultscale*textmag)
  fi
enddef;

def	infontsize(expr str, size) =
  infont str scaled (size /	fontsize str)
enddef;


  verbatimtex
  \def\imag{i}
  %\documentclass[20pt,dvips,landscape]{foils}
  \documentclass{article}
  %\usepackage[LY1]{fontenc}	% specify text font encoding
  %\usepackage{amsmath}
  %\usepackage[usenames,dvips]{color}
  \usepackage{times}
  \begin{document}
  etex;

color currentcolor; currentcolor=black;
boolean colorlines,dashedlines;
colorlines:=false;
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
w:=5in;h:=3in;
a:=w/1500;b:=h/900;
scalepen 1; ptsize 1;linetype -2;
draw (176a,180b)--(176a,200b);
draw (176a,1125b)--(176a,1105b);
put_text( btex 0 etex, 176a, 135b, 0, 2);
draw (321a,180b)--(321a,200b);
draw (321a,1125b)--(321a,1105b);
put_text( btex 2000 etex, 321a, 135b, 0, 2);
draw (466a,180b)--(466a,200b);
draw (466a,1125b)--(466a,1105b);
put_text( btex 4000 etex, 466a, 135b, 0, 2);
draw (610a,180b)--(610a,200b);
draw (610a,1125b)--(610a,1105b);
put_text( btex 6000 etex, 610a, 135b, 0, 2);
draw (755a,180b)--(755a,200b);
draw (755a,1125b)--(755a,1105b);
put_text( btex 8000 etex, 755a, 135b, 0, 2);
draw (900a,180b)--(900a,200b);
draw (900a,1125b)--(900a,1105b);
put_text( btex 10000 etex, 900a, 135b, 0, 2);
draw (1045a,180b)--(1045a,200b);
draw (1045a,1125b)--(1045a,1105b);
put_text( btex 12000 etex, 1045a, 135b, 0, 2);
draw (1190a,180b)--(1190a,200b);
draw (1190a,1125b)--(1190a,1105b);
put_text( btex 14000 etex, 1190a, 135b, 0, 2);
draw (1334a,180b)--(1334a,200b);
draw (1334a,1125b)--(1334a,1105b);
put_text( btex 16000 etex, 1334a, 135b, 0, 2);
draw (1479a,180b)--(1479a,200b);
draw (1479a,1125b)--(1479a,1105b);
put_text( btex 18000 etex, 1479a, 135b, 0, 2);
draw (1624a,180b)--(1624a,200b);
draw (1624a,1125b)--(1624a,1105b);
put_text( btex 20000 etex, 1624a, 135b, 0, 2);
draw (1624a,180b)--(1604a,180b);
put_text( btex 200 etex, 1646a, 180b, 0, 1);
draw (1624a,266b)--(1604a,266b);
put_text( btex 400 etex, 1646a, 266b, 0, 1);
draw (1624a,352b)--(1604a,352b);
put_text( btex 600 etex, 1646a, 352b, 0, 1);
draw (1624a,438b)--(1604a,438b);
put_text( btex 800 etex, 1646a, 438b, 0, 1);
draw (1624a,524b)--(1604a,524b);
put_text( btex 1000 etex, 1646a, 524b, 0, 1);
draw (1624a,610b)--(1604a,610b);
put_text( btex 1200 etex, 1646a, 610b, 0, 1);
draw (1624a,695b)--(1604a,695b);
put_text( btex 1400 etex, 1646a, 695b, 0, 1);
draw (1624a,781b)--(1604a,781b);
put_text( btex 1600 etex, 1646a, 781b, 0, 1);
draw (1624a,867b)--(1604a,867b);
put_text( btex 1800 etex, 1646a, 867b, 0, 1);
draw (1624a,953b)--(1604a,953b);
put_text( btex 2000 etex, 1646a, 953b, 0, 1);
draw (1624a,1039b)--(1604a,1039b);
put_text( btex 2200 etex, 1646a, 1039b, 0, 1);
draw (1624a,1125b)--(1604a,1125b);
put_text( btex 2400 etex, 1646a, 1125b, 0, 1);
draw (176a,180b)--(1624a,180b)--(1624a,1125b)--(176a,1125b)--(176a,180b);
put_text( btex Speed-up etex, 1777a, 652b, 1, 2);
put_text( btex Node Count etex, 900a, 68b, 0, 2);
linetype 0;
put_text( btex 5000 atoms (speed-up) etex, 1385a, 658b, 0, 3);
draw (1407a,658b)--(1515a,658b);
draw (213a,222b)--(250a,290b)--(324a,362b)--(324a,422b)--(473a,539b)
--(473a,652b)--(769a,757b)--(769a,812b)--(1362a,1073b)--(1565a,1125b);
linetype 1;
put_text( btex 10,000 atoms (speed-up) etex, 1385a, 613b, 0, 3);
draw (1407a,613b)--(1515a,613b);
draw (213a,229b)--(250a,303b)--(324a,382b)--(324a,432b)--(473a,545b)
--(473a,626b)--(769a,721b)--(769a,757b)--(1362a,930b)--(1624a,966b);
linetype 2;
put_text( btex 20,000 atoms (speed-up) etex, 1385a, 568b, 0, 3);
draw (1407a,568b)--(1515a,568b);
draw (213a,238b)--(250a,318b)--(324a,406b)--(324a,442b)--(473a,551b)
--(473a,603b)--(769a,689b)--(769a,710b)--(1362a,824b)--(1624a,845b);
linetype 3;
put_text( btex 30,000 atoms (speed-up) etex, 1385a, 523b, 0, 3);
draw (1407a,523b)--(1515a,523b);
draw (213a,242b)--(250a,326b)--(324a,418b)--(324a,447b)--(473a,553b)
--(473a,592b)--(769a,674b)--(769a,689b)--(1362a,781b)--(1624a,797b);
linetype -2;
draw (176a,180b)--(196a,180b);
put_text( btex 0 etex, 154a, 180b, 0, 3);
draw (176a,315b)--(196a,315b);
put_text( btex 0.1 etex, 154a, 315b, 0, 3);
draw (176a,450b)--(196a,450b);
put_text( btex 0.2 etex, 154a, 450b, 0, 3);
draw (176a,585b)--(196a,585b);
put_text( btex 0.3 etex, 154a, 585b, 0, 3);
draw (176a,720b)--(196a,720b);
put_text( btex 0.4 etex, 154a, 720b, 0, 3);
draw (176a,855b)--(196a,855b);
put_text( btex 0.5 etex, 154a, 855b, 0, 3);
draw (176a,990b)--(196a,990b);
put_text( btex 0.6 etex, 154a, 990b, 0, 3);
draw (176a,1125b)--(196a,1125b);
put_text( btex 0.7 etex, 154a, 1125b, 0, 3);
draw (176a,180b)--(176a,200b);
draw (176a,1125b)--(176a,1105b);
put_text( btex 0 etex, 176a, 135b, 0, 2);
draw (321a,180b)--(321a,200b);
draw (321a,1125b)--(321a,1105b);
put_text( btex 2000 etex, 321a, 135b, 0, 2);
draw (466a,180b)--(466a,200b);
draw (466a,1125b)--(466a,1105b);
put_text( btex 4000 etex, 466a, 135b, 0, 2);
draw (610a,180b)--(610a,200b);
draw (610a,1125b)--(610a,1105b);
put_text( btex 6000 etex, 610a, 135b, 0, 2);
draw (755a,180b)--(755a,200b);
draw (755a,1125b)--(755a,1105b);
put_text( btex 8000 etex, 755a, 135b, 0, 2);
draw (900a,180b)--(900a,200b);
draw (900a,1125b)--(900a,1105b);
put_text( btex 10000 etex, 900a, 135b, 0, 2);
draw (1045a,180b)--(1045a,200b);
draw (1045a,1125b)--(1045a,1105b);
put_text( btex 12000 etex, 1045a, 135b, 0, 2);
draw (1190a,180b)--(1190a,200b);
draw (1190a,1125b)--(1190a,1105b);
put_text( btex 14000 etex, 1190a, 135b, 0, 2);
draw (1334a,180b)--(1334a,200b);
draw (1334a,1125b)--(1334a,1105b);
put_text( btex 16000 etex, 1334a, 135b, 0, 2);
draw (1479a,180b)--(1479a,200b);
draw (1479a,1125b)--(1479a,1105b);
put_text( btex 18000 etex, 1479a, 135b, 0, 2);
draw (1624a,180b)--(1624a,200b);
draw (1624a,1125b)--(1624a,1105b);
put_text( btex 20000 etex, 1624a, 135b, 0, 2);
draw (176a,180b)--(1624a,180b)--(1624a,1125b)--(176a,1125b)--(176a,180b);
put_text( btex Efficiency etex, 43a, 652b, 1, 2);
put_text( btex Node Count etex, 900a, 68b, 0, 2);
linetype 0;
put_text( btex 5000 atoms (efficiency) etex, 733a, 1057b, 0, 3);
draw (755a,1057b)--(863a,1057b);
draw (213a,964b)--(250a,782b)--(324a,591b)--(324a,684b)--(473a,521b)
--(473a,608b)--(769a,434b)--(769a,456b)--(1362a,368b)--(1624a,353b);
gpdraw(0,213a,964b);
gpdraw(0,250a,782b);
gpdraw(0,324a,591b);
gpdraw(0,324a,684b);
gpdraw(0,473a,521b);
gpdraw(0,473a,608b);
gpdraw(0,769a,434b);
gpdraw(0,769a,456b);
gpdraw(0,1362a,368b);
gpdraw(0,809a,1057b);
linetype 1;
put_text( btex 10,000 atoms (efficiency) etex, 733a, 1012b, 0, 3);
draw (755a,1012b)--(863a,1012b);
draw (213a,1010b)--(250a,823b)--(324a,622b)--(324a,699b)--(473a,526b)
--(473a,588b)--(769a,421b)--(769a,434b)--(1362a,340b)--(1624a,326b);
gpdraw(1,213a,1010b);
gpdraw(1,250a,823b);
gpdraw(1,324a,622b);
gpdraw(1,324a,699b);
gpdraw(1,473a,526b);
gpdraw(1,473a,588b);
gpdraw(1,769a,421b);
gpdraw(1,769a,434b);
gpdraw(1,1362a,340b);
gpdraw(1,809a,1012b);
linetype 2;
put_text( btex 20,000 atoms (efficiency) etex, 733a, 967b, 0, 3);
draw (755a,967b)--(863a,967b);
draw (213a,1062b)--(250a,868b)--(324a,658b)--(324a,714b)--(473a,530b)
--(473a,570b)--(769a,408b)--(769a,416b)--(1362a,320b)--(1624a,307b);
gpdraw(2,213a,1062b);
gpdraw(2,250a,868b);
gpdraw(2,324a,658b);
gpdraw(2,324a,714b);
gpdraw(2,473a,530b);
gpdraw(2,473a,570b);
gpdraw(2,769a,408b);
gpdraw(2,769a,416b);
gpdraw(2,1362a,320b);
gpdraw(2,809a,967b);
linetype 3;
put_text( btex 30,000 atoms (efficiency) etex, 733a, 922b, 0, 3);
draw (755a,922b)--(863a,922b);
draw (213a,1090b)--(250a,893b)--(324a,678b)--(324a,722b)--(473a,532b)
--(473a,562b)--(769a,403b)--(769a,408b)--(1362a,312b)--(1624a,299b);
gpdraw(3,213a,1090b);
gpdraw(3,250a,893b);
gpdraw(3,324a,678b);
gpdraw(3,324a,722b);
gpdraw(3,473a,532b);
gpdraw(3,473a,562b);
gpdraw(3,769a,403b);
gpdraw(3,769a,408b);
gpdraw(3,1362a,312b);
gpdraw(3,809a,922b);
endfig;
end.
