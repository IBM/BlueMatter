%GNUPLOT Metapost output: Sun Apr 25 01:35:16 2004


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

beginfig(21);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,329.4b)--(213.1a,329.4b);
draw (1146.3a,329.4b)--(1129.7a,329.4b);
put_text( btex  0.001 etex, 178.8a, 329.4b, 0, 3);
draw (196.5a,367.0b)--(204.8a,367.0b);
draw (1146.3a,367.0b)--(1138.0a,367.0b);
draw (196.5a,388.9b)--(204.8a,388.9b);
draw (1146.3a,388.9b)--(1138.0a,388.9b);
draw (196.5a,404.5b)--(204.8a,404.5b);
draw (1146.3a,404.5b)--(1138.0a,404.5b);
draw (196.5a,416.6b)--(204.8a,416.6b);
draw (1146.3a,416.6b)--(1138.0a,416.6b);
draw (196.5a,426.5b)--(204.8a,426.5b);
draw (1146.3a,426.5b)--(1138.0a,426.5b);
draw (196.5a,434.9b)--(204.8a,434.9b);
draw (1146.3a,434.9b)--(1138.0a,434.9b);
draw (196.5a,442.1b)--(204.8a,442.1b);
draw (1146.3a,442.1b)--(1138.0a,442.1b);
draw (196.5a,448.5b)--(204.8a,448.5b);
draw (1146.3a,448.5b)--(1138.0a,448.5b);
draw (196.5a,454.2b)--(213.1a,454.2b);
draw (1146.3a,454.2b)--(1129.7a,454.2b);
put_text( btex  0.01 etex, 178.8a, 454.2b, 0, 3);
draw (196.5a,491.8b)--(204.8a,491.8b);
draw (1146.3a,491.8b)--(1138.0a,491.8b);
draw (196.5a,513.7b)--(204.8a,513.7b);
draw (1146.3a,513.7b)--(1138.0a,513.7b);
draw (196.5a,529.3b)--(204.8a,529.3b);
draw (1146.3a,529.3b)--(1138.0a,529.3b);
draw (196.5a,541.4b)--(204.8a,541.4b);
draw (1146.3a,541.4b)--(1138.0a,541.4b);
draw (196.5a,551.3b)--(204.8a,551.3b);
draw (1146.3a,551.3b)--(1138.0a,551.3b);
draw (196.5a,559.7b)--(204.8a,559.7b);
draw (1146.3a,559.7b)--(1138.0a,559.7b);
draw (196.5a,566.9b)--(204.8a,566.9b);
draw (1146.3a,566.9b)--(1138.0a,566.9b);
draw (196.5a,573.3b)--(204.8a,573.3b);
draw (1146.3a,573.3b)--(1138.0a,573.3b);
draw (196.5a,579.0b)--(213.1a,579.0b);
draw (1146.3a,579.0b)--(1129.7a,579.0b);
put_text( btex  0.1 etex, 178.8a, 579.0b, 0, 3);
draw (196.5a,616.6b)--(204.8a,616.6b);
draw (1146.3a,616.6b)--(1138.0a,616.6b);
draw (196.5a,638.5b)--(204.8a,638.5b);
draw (1146.3a,638.5b)--(1138.0a,638.5b);
draw (196.5a,654.1b)--(204.8a,654.1b);
draw (1146.3a,654.1b)--(1138.0a,654.1b);
draw (196.5a,666.2b)--(204.8a,666.2b);
draw (1146.3a,666.2b)--(1138.0a,666.2b);
draw (196.5a,676.1b)--(204.8a,676.1b);
draw (1146.3a,676.1b)--(1138.0a,676.1b);
draw (196.5a,684.5b)--(204.8a,684.5b);
draw (1146.3a,684.5b)--(1138.0a,684.5b);
draw (196.5a,691.7b)--(204.8a,691.7b);
draw (1146.3a,691.7b)--(1138.0a,691.7b);
draw (196.5a,698.1b)--(204.8a,698.1b);
draw (1146.3a,698.1b)--(1138.0a,698.1b);
draw (196.5a,703.8b)--(213.1a,703.8b);
draw (1146.3a,703.8b)--(1129.7a,703.8b);
put_text( btex  1 etex, 178.8a, 703.8b, 0, 3);
draw (196.5a,741.4b)--(204.8a,741.4b);
draw (1146.3a,741.4b)--(1138.0a,741.4b);
draw (196.5a,763.3b)--(204.8a,763.3b);
draw (1146.3a,763.3b)--(1138.0a,763.3b);
draw (196.5a,778.9b)--(204.8a,778.9b);
draw (1146.3a,778.9b)--(1138.0a,778.9b);
draw (196.5a,791.0b)--(204.8a,791.0b);
draw (1146.3a,791.0b)--(1138.0a,791.0b);
draw (196.5a,800.9b)--(204.8a,800.9b);
draw (1146.3a,800.9b)--(1138.0a,800.9b);
draw (196.5a,809.3b)--(204.8a,809.3b);
draw (1146.3a,809.3b)--(1138.0a,809.3b);
draw (196.5a,816.5b)--(204.8a,816.5b);
draw (1146.3a,816.5b)--(1138.0a,816.5b);
draw (196.5a,822.9b)--(204.8a,822.9b);
draw (1146.3a,822.9b)--(1138.0a,822.9b);
draw (196.5a,828.6b)--(213.1a,828.6b);
draw (1146.3a,828.6b)--(1129.7a,828.6b);
put_text( btex  10 etex, 178.8a, 828.6b, 0, 3);
draw (196.5a,329.4b)--(196.5a,346.0b);
draw (196.5a,828.6b)--(196.5a,812.0b);
put_text( btex  1 etex, 196.5a, 292.8b, 0, 2);
draw (291.8a,329.4b)--(291.8a,337.7b);
draw (291.8a,828.6b)--(291.8a,820.3b);
draw (347.6a,329.4b)--(347.6a,337.7b);
draw (347.6a,828.6b)--(347.6a,820.3b);
draw (387.1a,329.4b)--(387.1a,337.7b);
draw (387.1a,828.6b)--(387.1a,820.3b);
draw (417.8a,329.4b)--(417.8a,337.7b);
draw (417.8a,828.6b)--(417.8a,820.3b);
draw (442.9a,329.4b)--(442.9a,337.7b);
draw (442.9a,828.6b)--(442.9a,820.3b);
draw (464.1a,329.4b)--(464.1a,337.7b);
draw (464.1a,828.6b)--(464.1a,820.3b);
draw (482.4a,329.4b)--(482.4a,337.7b);
draw (482.4a,828.6b)--(482.4a,820.3b);
draw (498.6a,329.4b)--(498.6a,337.7b);
draw (498.6a,828.6b)--(498.6a,820.3b);
draw (513.1a,329.4b)--(513.1a,346.0b);
draw (513.1a,828.6b)--(513.1a,812.0b);
put_text( btex  10 etex, 513.1a, 292.8b, 0, 2);
draw (608.4a,329.4b)--(608.4a,337.7b);
draw (608.4a,828.6b)--(608.4a,820.3b);
draw (664.2a,329.4b)--(664.2a,337.7b);
draw (664.2a,828.6b)--(664.2a,820.3b);
draw (703.7a,329.4b)--(703.7a,337.7b);
draw (703.7a,828.6b)--(703.7a,820.3b);
draw (734.4a,329.4b)--(734.4a,337.7b);
draw (734.4a,828.6b)--(734.4a,820.3b);
draw (759.5a,329.4b)--(759.5a,337.7b);
draw (759.5a,828.6b)--(759.5a,820.3b);
draw (780.7a,329.4b)--(780.7a,337.7b);
draw (780.7a,828.6b)--(780.7a,820.3b);
draw (799.0a,329.4b)--(799.0a,337.7b);
draw (799.0a,828.6b)--(799.0a,820.3b);
draw (815.2a,329.4b)--(815.2a,337.7b);
draw (815.2a,828.6b)--(815.2a,820.3b);
draw (829.7a,329.4b)--(829.7a,346.0b);
draw (829.7a,828.6b)--(829.7a,812.0b);
put_text( btex  100 etex, 829.7a, 292.8b, 0, 2);
draw (925.0a,329.4b)--(925.0a,337.7b);
draw (925.0a,828.6b)--(925.0a,820.3b);
draw (980.8a,329.4b)--(980.8a,337.7b);
draw (980.8a,828.6b)--(980.8a,820.3b);
draw (1020.3a,329.4b)--(1020.3a,337.7b);
draw (1020.3a,828.6b)--(1020.3a,820.3b);
draw (1051.0a,329.4b)--(1051.0a,337.7b);
draw (1051.0a,828.6b)--(1051.0a,820.3b);
draw (1076.1a,329.4b)--(1076.1a,337.7b);
draw (1076.1a,828.6b)--(1076.1a,820.3b);
draw (1097.3a,329.4b)--(1097.3a,337.7b);
draw (1097.3a,828.6b)--(1097.3a,820.3b);
draw (1115.6a,329.4b)--(1115.6a,337.7b);
draw (1115.6a,828.6b)--(1115.6a,820.3b);
draw (1131.8a,329.4b)--(1131.8a,337.7b);
draw (1131.8a,828.6b)--(1131.8a,820.3b);
draw (1146.3a,329.4b)--(1146.3a,346.0b);
draw (1146.3a,828.6b)--(1146.3a,812.0b);
put_text( btex  1000 etex, 1146.3a, 292.8b, 0, 2);
draw (196.5a,329.4b)--(1146.3a,329.4b)--(1146.3a,828.6b)--(196.5a,828.6b)--(196.5a,329.4b);
put_text( btex (Task Count)$^{2/3} \times$ Time (sec.) etex, 36.6a, 579.0b, 1, 2);
put_text( btex Task Count etex, 671.4a, 237.9b, 0, 2);
linetype 0;
put_text( btex $128^3$ MPI Collective etex, 466.4a, 164.7b, 0, 3);
draw (484.1a,164.7b)--(571.5a,164.7b);
draw (196.5a,750.2b)--(482.4a,720.7b)--(673.0a,672.8b)--(863.6a,651.3b)--(958.9a,640.1b)
--(1054.3a,642.2b);
gpdraw(0,196.5a,750.2b);
gpdraw(0,482.4a,720.7b);
gpdraw(0,673.0a,672.8b);
gpdraw(0,863.6a,651.3b);
gpdraw(0,958.9a,640.1b);
gpdraw(0,1054.3a,642.2b);
gpdraw(0,527.8a,164.7b);
linetype 1;
put_text( btex $128^3$ Active Packet etex, 466.4a, 128.1b, 0, 3);
draw (484.1a,128.1b)--(571.5a,128.1b);
draw (482.4a,673.9b)--(673.0a,645.1b)--(863.6a,636.6b)--(1054.3a,636.9b);
gpdraw(1,482.4a,673.9b);
gpdraw(1,673.0a,645.1b);
gpdraw(1,863.6a,636.6b);
gpdraw(1,1054.3a,636.9b);
gpdraw(1,527.8a,128.1b);
linetype 2;
put_text( btex $128^3$ Model etex, 466.4a, 91.5b, 0, 3);
draw (484.1a,91.5b)--(571.5a,91.5b);
draw (196.5a,591.5b)--(482.4a,627.1b)--(673.0a,632.0b)--(768.3a,619.5b)--(863.6a,631.4b)
--(958.9a,627.9b)--(1054.3a,615.3b)--(1146.3a,628.3b);
linetype 3;
put_text( btex $64^3$ MPI Collective etex, 466.4a, 54.9b, 0, 3);
draw (484.1a,54.9b)--(571.5a,54.9b);
draw (196.5a,615.4b)--(482.4a,568.5b)--(673.0a,537.3b)--(863.6a,542.3b)--(958.9a,540.0b)
--(1054.3a,561.9b);
gpdraw(2,196.5a,615.4b);
gpdraw(2,482.4a,568.5b);
gpdraw(2,673.0a,537.3b);
gpdraw(2,863.6a,542.3b);
gpdraw(2,958.9a,540.0b);
gpdraw(2,1054.3a,561.9b);
gpdraw(2,527.8a,54.9b);
linetype 4;
put_text( btex $64^3$ Active Packet etex, 466.4a, 18.3b, 0, 3);
draw (484.1a,18.3b)--(571.5a,18.3b);
draw (196.5a,598.8b)--(482.4a,551.3b)--(673.0a,534.3b)--(863.6a,528.1b)--(1054.3a,526.6b);
gpdraw(3,196.5a,598.8b);
gpdraw(3,482.4a,551.3b);
gpdraw(3,673.0a,534.3b);
gpdraw(3,863.6a,528.1b);
gpdraw(3,1054.3a,526.6b);
gpdraw(3,527.8a,18.3b);
linetype 5;
put_text( btex $64^3$ Model etex, 1031.0a, 164.7b, 0, 3);
draw (1048.7a,164.7b)--(1136.1a,164.7b);
draw (196.5a,470.4b)--(482.4a,512.3b)--(673.0a,518.2b)--(768.3a,505.7b)--(863.6a,517.9b)
--(958.9a,514.5b)--(1054.3a,502.0b)--(1146.3a,515.2b);
linetype 6;
put_text( btex $32^3$ MPI Collective etex, 1031.0a, 128.1b, 0, 3);
draw (1048.7a,128.1b)--(1136.1a,128.1b);
draw (196.5a,446.8b)--(482.4a,443.5b)--(673.0a,425.8b)--(863.6a,457.0b)--(958.9a,484.5b);
gpdraw(4,196.5a,446.8b);
gpdraw(4,482.4a,443.5b);
gpdraw(4,673.0a,425.8b);
gpdraw(4,863.6a,457.0b);
gpdraw(4,958.9a,484.5b);
gpdraw(4,1092.4a,128.1b);
linetype 7;
put_text( btex $32^3$ Active Packet etex, 1031.0a, 91.5b, 0, 3);
draw (1048.7a,91.5b)--(1136.1a,91.5b);
draw (196.5a,475.4b)--(482.4a,437.9b)--(673.0a,425.6b)--(863.6a,420.1b)--(1054.3a,446.7b);
gpdraw(5,196.5a,475.4b);
gpdraw(5,482.4a,437.9b);
gpdraw(5,673.0a,425.6b);
gpdraw(5,863.6a,420.1b);
gpdraw(5,1054.3a,446.7b);
gpdraw(5,1092.4a,91.5b);
linetype 0;
put_text( btex $32^3$ Model etex, 1031.0a, 54.9b, 0, 3);
draw (1048.7a,54.9b)--(1136.1a,54.9b);
draw (196.5a,347.8b)--(482.4a,397.5b)--(673.0a,404.3b)--(768.3a,391.8b)--(863.6a,404.5b)
--(958.9a,401.2b)--(1054.3a,388.7b)--(1146.3a,402.1b);
endfig;
end.
b);
endfig;
end.
