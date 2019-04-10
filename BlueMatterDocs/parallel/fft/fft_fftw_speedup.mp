%GNUPLOT Metapost output: Fri May 13 14:11:13 2005


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

beginfig(5);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (196.5a,109.8b)--(213.1a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  1 etex, 178.8a, 109.8b, 0, 3);
draw (196.5a,153.1b)--(204.8a,153.1b);
draw (1146.3a,153.1b)--(1138.0a,153.1b);
draw (196.5a,178.4b)--(204.8a,178.4b);
draw (1146.3a,178.4b)--(1138.0a,178.4b);
draw (196.5a,196.3b)--(204.8a,196.3b);
draw (1146.3a,196.3b)--(1138.0a,196.3b);
draw (196.5a,210.2b)--(204.8a,210.2b);
draw (1146.3a,210.2b)--(1138.0a,210.2b);
draw (196.5a,221.6b)--(204.8a,221.6b);
draw (1146.3a,221.6b)--(1138.0a,221.6b);
draw (196.5a,231.2b)--(204.8a,231.2b);
draw (1146.3a,231.2b)--(1138.0a,231.2b);
draw (196.5a,239.6b)--(204.8a,239.6b);
draw (1146.3a,239.6b)--(1138.0a,239.6b);
draw (196.5a,246.9b)--(204.8a,246.9b);
draw (1146.3a,246.9b)--(1138.0a,246.9b);
draw (196.5a,253.5b)--(213.1a,253.5b);
draw (1146.3a,253.5b)--(1129.7a,253.5b);
put_text( btex  10 etex, 178.8a, 253.5b, 0, 3);
draw (196.5a,296.8b)--(204.8a,296.8b);
draw (1146.3a,296.8b)--(1138.0a,296.8b);
draw (196.5a,322.1b)--(204.8a,322.1b);
draw (1146.3a,322.1b)--(1138.0a,322.1b);
draw (196.5a,340.0b)--(204.8a,340.0b);
draw (1146.3a,340.0b)--(1138.0a,340.0b);
draw (196.5a,353.9b)--(204.8a,353.9b);
draw (1146.3a,353.9b)--(1138.0a,353.9b);
draw (196.5a,365.3b)--(204.8a,365.3b);
draw (1146.3a,365.3b)--(1138.0a,365.3b);
draw (196.5a,374.9b)--(204.8a,374.9b);
draw (1146.3a,374.9b)--(1138.0a,374.9b);
draw (196.5a,383.3b)--(204.8a,383.3b);
draw (1146.3a,383.3b)--(1138.0a,383.3b);
draw (196.5a,390.6b)--(204.8a,390.6b);
draw (1146.3a,390.6b)--(1138.0a,390.6b);
draw (196.5a,397.2b)--(213.1a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  100 etex, 178.8a, 397.2b, 0, 3);
draw (196.5a,440.5b)--(204.8a,440.5b);
draw (1146.3a,440.5b)--(1138.0a,440.5b);
draw (196.5a,465.8b)--(204.8a,465.8b);
draw (1146.3a,465.8b)--(1138.0a,465.8b);
draw (196.5a,483.7b)--(204.8a,483.7b);
draw (1146.3a,483.7b)--(1138.0a,483.7b);
draw (196.5a,497.6b)--(204.8a,497.6b);
draw (1146.3a,497.6b)--(1138.0a,497.6b);
draw (196.5a,509.0b)--(204.8a,509.0b);
draw (1146.3a,509.0b)--(1138.0a,509.0b);
draw (196.5a,518.6b)--(204.8a,518.6b);
draw (1146.3a,518.6b)--(1138.0a,518.6b);
draw (196.5a,527.0b)--(204.8a,527.0b);
draw (1146.3a,527.0b)--(1138.0a,527.0b);
draw (196.5a,534.3b)--(204.8a,534.3b);
draw (1146.3a,534.3b)--(1138.0a,534.3b);
draw (196.5a,540.9b)--(213.1a,540.9b);
draw (1146.3a,540.9b)--(1129.7a,540.9b);
put_text( btex  1000 etex, 178.8a, 540.9b, 0, 3);
draw (196.5a,584.2b)--(204.8a,584.2b);
draw (1146.3a,584.2b)--(1138.0a,584.2b);
draw (196.5a,609.5b)--(204.8a,609.5b);
draw (1146.3a,609.5b)--(1138.0a,609.5b);
draw (196.5a,627.4b)--(204.8a,627.4b);
draw (1146.3a,627.4b)--(1138.0a,627.4b);
draw (196.5a,641.3b)--(204.8a,641.3b);
draw (1146.3a,641.3b)--(1138.0a,641.3b);
draw (196.5a,652.7b)--(204.8a,652.7b);
draw (1146.3a,652.7b)--(1138.0a,652.7b);
draw (196.5a,662.3b)--(204.8a,662.3b);
draw (1146.3a,662.3b)--(1138.0a,662.3b);
draw (196.5a,670.7b)--(204.8a,670.7b);
draw (1146.3a,670.7b)--(1138.0a,670.7b);
draw (196.5a,678.0b)--(204.8a,678.0b);
draw (1146.3a,678.0b)--(1138.0a,678.0b);
draw (196.5a,684.6b)--(213.1a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  10000 etex, 178.8a, 684.6b, 0, 3);
draw (196.5a,109.8b)--(196.5a,126.4b);
draw (196.5a,684.6b)--(196.5a,668.0b);
put_text( btex  1 etex, 196.5a, 73.2b, 0, 2);
draw (268.0a,109.8b)--(268.0a,118.1b);
draw (268.0a,684.6b)--(268.0a,676.3b);
draw (309.8a,109.8b)--(309.8a,118.1b);
draw (309.8a,684.6b)--(309.8a,676.3b);
draw (339.5a,109.8b)--(339.5a,118.1b);
draw (339.5a,684.6b)--(339.5a,676.3b);
draw (362.5a,109.8b)--(362.5a,118.1b);
draw (362.5a,684.6b)--(362.5a,676.3b);
draw (381.3a,109.8b)--(381.3a,118.1b);
draw (381.3a,684.6b)--(381.3a,676.3b);
draw (397.2a,109.8b)--(397.2a,118.1b);
draw (397.2a,684.6b)--(397.2a,676.3b);
draw (410.9a,109.8b)--(410.9a,118.1b);
draw (410.9a,684.6b)--(410.9a,676.3b);
draw (423.1a,109.8b)--(423.1a,118.1b);
draw (423.1a,684.6b)--(423.1a,676.3b);
draw (434.0a,109.8b)--(434.0a,126.4b);
draw (434.0a,684.6b)--(434.0a,668.0b);
put_text( btex  10 etex, 434.0a, 73.2b, 0, 2);
draw (505.4a,109.8b)--(505.4a,118.1b);
draw (505.4a,684.6b)--(505.4a,676.3b);
draw (547.2a,109.8b)--(547.2a,118.1b);
draw (547.2a,684.6b)--(547.2a,676.3b);
draw (576.9a,109.8b)--(576.9a,118.1b);
draw (576.9a,684.6b)--(576.9a,676.3b);
draw (599.9a,109.8b)--(599.9a,118.1b);
draw (599.9a,684.6b)--(599.9a,676.3b);
draw (618.7a,109.8b)--(618.7a,118.1b);
draw (618.7a,684.6b)--(618.7a,676.3b);
draw (634.6a,109.8b)--(634.6a,118.1b);
draw (634.6a,684.6b)--(634.6a,676.3b);
draw (648.4a,109.8b)--(648.4a,118.1b);
draw (648.4a,684.6b)--(648.4a,676.3b);
draw (660.5a,109.8b)--(660.5a,118.1b);
draw (660.5a,684.6b)--(660.5a,676.3b);
draw (671.4a,109.8b)--(671.4a,126.4b);
draw (671.4a,684.6b)--(671.4a,668.0b);
put_text( btex  100 etex, 671.4a, 73.2b, 0, 2);
draw (742.9a,109.8b)--(742.9a,118.1b);
draw (742.9a,684.6b)--(742.9a,676.3b);
draw (784.7a,109.8b)--(784.7a,118.1b);
draw (784.7a,684.6b)--(784.7a,676.3b);
draw (814.4a,109.8b)--(814.4a,118.1b);
draw (814.4a,684.6b)--(814.4a,676.3b);
draw (837.4a,109.8b)--(837.4a,118.1b);
draw (837.4a,684.6b)--(837.4a,676.3b);
draw (856.2a,109.8b)--(856.2a,118.1b);
draw (856.2a,684.6b)--(856.2a,676.3b);
draw (872.1a,109.8b)--(872.1a,118.1b);
draw (872.1a,684.6b)--(872.1a,676.3b);
draw (885.8a,109.8b)--(885.8a,118.1b);
draw (885.8a,684.6b)--(885.8a,676.3b);
draw (898.0a,109.8b)--(898.0a,118.1b);
draw (898.0a,684.6b)--(898.0a,676.3b);
draw (908.9a,109.8b)--(908.9a,126.4b);
draw (908.9a,684.6b)--(908.9a,668.0b);
put_text( btex  1000 etex, 908.9a, 73.2b, 0, 2);
draw (980.3a,109.8b)--(980.3a,118.1b);
draw (980.3a,684.6b)--(980.3a,676.3b);
draw (1022.1a,109.8b)--(1022.1a,118.1b);
draw (1022.1a,684.6b)--(1022.1a,676.3b);
draw (1051.8a,109.8b)--(1051.8a,118.1b);
draw (1051.8a,684.6b)--(1051.8a,676.3b);
draw (1074.8a,109.8b)--(1074.8a,118.1b);
draw (1074.8a,684.6b)--(1074.8a,676.3b);
draw (1093.6a,109.8b)--(1093.6a,118.1b);
draw (1093.6a,684.6b)--(1093.6a,676.3b);
draw (1109.5a,109.8b)--(1109.5a,118.1b);
draw (1109.5a,684.6b)--(1109.5a,676.3b);
draw (1123.3a,109.8b)--(1123.3a,118.1b);
draw (1123.3a,684.6b)--(1123.3a,676.3b);
draw (1135.4a,109.8b)--(1135.4a,118.1b);
draw (1135.4a,684.6b)--(1135.4a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  10000 etex, 1146.3a, 73.2b, 0, 2);
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
put_text( btex Speedup etex, 36.6a, 397.2b, 1, 2);
put_text( btex Number of processors etex, 671.4a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Ideal Speedup etex, 1005.8a, 217.9b, 0, 3);
linetype 0;
draw (1023.5a,217.9b)--(1110.9a,217.9b);
draw (196.5a,109.8b)--(205.9a,115.5b)--(215.3a,121.2b)--(224.7a,126.8b)--(234.0a,132.5b)
--(243.4a,138.2b)--(252.8a,143.9b)--(262.2a,149.6b)--(271.6a,155.2b)--(281.0a,160.9b)
--(290.4a,166.6b)--(299.7a,172.3b)--(309.1a,178.0b)--(318.5a,183.6b)--(327.9a,189.3b)
--(337.3a,195.0b)--(346.7a,200.7b)--(356.1a,206.4b)--(365.5a,212.0b)--(374.8a,217.7b)
--(384.2a,223.4b)--(393.6a,229.1b)--(403.0a,234.8b)--(412.4a,240.4b)--(421.8a,246.1b)
--(431.2a,251.8b)--(440.5a,257.5b)--(449.9a,263.2b)--(459.3a,268.8b)--(468.7a,274.5b)
--(478.1a,280.2b)--(487.5a,285.9b)--(496.9a,291.6b)--(506.2a,297.3b)--(515.6a,302.9b)
--(525.0a,308.6b)--(534.4a,314.3b)--(543.8a,320.0b)--(553.2a,325.7b)--(562.6a,331.3b)
--(571.9a,337.0b)--(581.3a,342.7b)--(590.7a,348.4b)--(600.1a,354.1b)--(609.5a,359.7b)
--(618.9a,365.4b)--(628.3a,371.1b)--(637.7a,376.8b)--(647.0a,382.5b)--(656.4a,388.1b)
--(665.8a,393.8b)--(675.2a,399.5b)--(684.6a,405.2b)--(694.0a,410.9b)--(703.4a,416.5b)
--(712.7a,422.2b)--(722.1a,427.9b)--(731.5a,433.6b)--(740.9a,439.3b)--(750.3a,444.9b)
--(759.7a,450.6b)--(769.1a,456.3b)--(778.4a,462.0b)--(787.8a,467.7b)--(797.2a,473.3b)
--(806.6a,479.0b)--(816.0a,484.7b)--(825.4a,490.4b)--(834.8a,496.1b)--(844.1a,501.7b)
--(853.5a,507.4b)--(862.9a,513.1b)--(872.3a,518.8b)--(881.7a,524.5b)--(891.1a,530.1b)
--(900.5a,535.8b)--(909.9a,541.5b)--(919.2a,547.2b)--(928.6a,552.9b)--(938.0a,558.5b)
--(947.4a,564.2b)--(956.8a,569.9b)--(966.2a,575.6b)--(975.6a,581.3b)--(984.9a,586.9b)
--(994.3a,592.6b)--(1003.7a,598.3b)--(1013.1a,604.0b)--(1022.5a,609.7b)--(1031.9a,615.4b)
--(1041.3a,621.0b)--(1050.6a,626.7b)--(1060.0a,632.4b)--(1069.4a,638.1b)--(1078.8a,643.8b)
--(1088.2a,649.4b)--(1097.6a,655.1b)--(1107.0a,660.8b)--(1116.3a,666.5b)--(1125.7a,672.2b);
linetype 1;
linetype -2;
put_text( btex FFTBG on MPI etex, 1005.8a, 181.3b, 0, 3);
linetype 1;
draw (1023.5a,181.3b)--(1110.9a,181.3b);
draw (196.5a,109.8b)--(410.9a,220.3b)--(482.4a,262.5b)--(553.9a,314.9b)--(625.4a,355.6b)
--(696.9a,379.5b)--(768.3a,439.5b)--(839.8a,476.0b)--(911.3a,502.7b)--(982.8a,518.7b)
--(1054.3a,528.8b)--(1125.7a,538.6b);
gpdraw(0,196.5a,109.8b);
gpdraw(0,410.9a,220.3b);
gpdraw(0,482.4a,262.5b);
gpdraw(0,553.9a,314.9b);
gpdraw(0,625.4a,355.6b);
gpdraw(0,696.9a,379.5b);
gpdraw(0,768.3a,439.5b);
gpdraw(0,839.8a,476.0b);
gpdraw(0,911.3a,502.7b);
gpdraw(0,982.8a,518.7b);
gpdraw(0,1054.3a,528.8b);
gpdraw(0,1125.7a,538.6b);
gpdraw(0,1067.2a,181.3b);
linetype 2;
linetype -2;
put_text( btex FFTW Port etex, 1005.8a, 144.7b, 0, 3);
linetype 2;
draw (1023.5a,144.7b)--(1110.9a,144.7b);
draw (196.5a,109.8b)--(268.0a,138.3b)--(410.9a,205.7b)--(482.4a,247.9b)--(553.9a,295.7b)
--(625.4a,323.9b)--(696.9a,367.8b)--(768.3a,368.3b)--(839.8a,368.1b)--(982.8a,357.9b);
gpdraw(1,196.5a,109.8b);
gpdraw(1,268.0a,138.3b);
gpdraw(1,410.9a,205.7b);
gpdraw(1,482.4a,247.9b);
gpdraw(1,553.9a,295.7b);
gpdraw(1,625.4a,323.9b);
gpdraw(1,696.9a,367.8b);
gpdraw(1,768.3a,368.3b);
gpdraw(1,839.8a,368.1b);
gpdraw(1,982.8a,357.9b);
gpdraw(1,1067.2a,144.7b);
linetype -2;
draw (196.5a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(196.5a,684.6b)--(196.5a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
