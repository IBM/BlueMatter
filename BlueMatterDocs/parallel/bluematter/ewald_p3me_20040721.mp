%GNUPLOT Metapost output: Wed Jul 21 23:19:11 2004


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
\usepackage{textcomp}
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

beginfig(15);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  1e+007 etex, 196.5a, 109.8b, 0, 3);
draw (214.2a,167.5b)--(222.5a,167.5b);
draw (1146.3a,167.5b)--(1138.0a,167.5b);
draw (214.2a,201.2b)--(222.5a,201.2b);
draw (1146.3a,201.2b)--(1138.0a,201.2b);
draw (214.2a,225.2b)--(222.5a,225.2b);
draw (1146.3a,225.2b)--(1138.0a,225.2b);
draw (214.2a,243.7b)--(222.5a,243.7b);
draw (1146.3a,243.7b)--(1138.0a,243.7b);
draw (214.2a,258.9b)--(222.5a,258.9b);
draw (1146.3a,258.9b)--(1138.0a,258.9b);
draw (214.2a,271.7b)--(222.5a,271.7b);
draw (1146.3a,271.7b)--(1138.0a,271.7b);
draw (214.2a,282.8b)--(222.5a,282.8b);
draw (1146.3a,282.8b)--(1138.0a,282.8b);
draw (214.2a,292.6b)--(222.5a,292.6b);
draw (1146.3a,292.6b)--(1138.0a,292.6b);
draw (214.2a,301.4b)--(230.8a,301.4b);
draw (1146.3a,301.4b)--(1129.7a,301.4b);
put_text( btex  1e+008 etex, 196.5a, 301.4b, 0, 3);
draw (214.2a,359.1b)--(222.5a,359.1b);
draw (1146.3a,359.1b)--(1138.0a,359.1b);
draw (214.2a,392.8b)--(222.5a,392.8b);
draw (1146.3a,392.8b)--(1138.0a,392.8b);
draw (214.2a,416.8b)--(222.5a,416.8b);
draw (1146.3a,416.8b)--(1138.0a,416.8b);
draw (214.2a,435.3b)--(222.5a,435.3b);
draw (1146.3a,435.3b)--(1138.0a,435.3b);
draw (214.2a,450.5b)--(222.5a,450.5b);
draw (1146.3a,450.5b)--(1138.0a,450.5b);
draw (214.2a,463.3b)--(222.5a,463.3b);
draw (1146.3a,463.3b)--(1138.0a,463.3b);
draw (214.2a,474.4b)--(222.5a,474.4b);
draw (1146.3a,474.4b)--(1138.0a,474.4b);
draw (214.2a,484.2b)--(222.5a,484.2b);
draw (1146.3a,484.2b)--(1138.0a,484.2b);
draw (214.2a,493.0b)--(230.8a,493.0b);
draw (1146.3a,493.0b)--(1129.7a,493.0b);
put_text( btex  1e+009 etex, 196.5a, 493.0b, 0, 3);
draw (214.2a,550.7b)--(222.5a,550.7b);
draw (1146.3a,550.7b)--(1138.0a,550.7b);
draw (214.2a,584.4b)--(222.5a,584.4b);
draw (1146.3a,584.4b)--(1138.0a,584.4b);
draw (214.2a,608.4b)--(222.5a,608.4b);
draw (1146.3a,608.4b)--(1138.0a,608.4b);
draw (214.2a,626.9b)--(222.5a,626.9b);
draw (1146.3a,626.9b)--(1138.0a,626.9b);
draw (214.2a,642.1b)--(222.5a,642.1b);
draw (1146.3a,642.1b)--(1138.0a,642.1b);
draw (214.2a,654.9b)--(222.5a,654.9b);
draw (1146.3a,654.9b)--(1138.0a,654.9b);
draw (214.2a,666.0b)--(222.5a,666.0b);
draw (1146.3a,666.0b)--(1138.0a,666.0b);
draw (214.2a,675.8b)--(222.5a,675.8b);
draw (1146.3a,675.8b)--(1138.0a,675.8b);
draw (214.2a,684.6b)--(230.8a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  1e+010 etex, 196.5a, 684.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,684.6b)--(214.2a,668.0b);
put_text( btex  10 etex, 214.2a, 73.2b, 0, 2);
draw (307.7a,109.8b)--(307.7a,118.1b);
draw (307.7a,684.6b)--(307.7a,676.3b);
draw (362.4a,109.8b)--(362.4a,118.1b);
draw (362.4a,684.6b)--(362.4a,676.3b);
draw (401.3a,109.8b)--(401.3a,118.1b);
draw (401.3a,684.6b)--(401.3a,676.3b);
draw (431.4a,109.8b)--(431.4a,118.1b);
draw (431.4a,684.6b)--(431.4a,676.3b);
draw (456.0a,109.8b)--(456.0a,118.1b);
draw (456.0a,684.6b)--(456.0a,676.3b);
draw (476.8a,109.8b)--(476.8a,118.1b);
draw (476.8a,684.6b)--(476.8a,676.3b);
draw (494.8a,109.8b)--(494.8a,118.1b);
draw (494.8a,684.6b)--(494.8a,676.3b);
draw (510.7a,109.8b)--(510.7a,118.1b);
draw (510.7a,684.6b)--(510.7a,676.3b);
draw (524.9a,109.8b)--(524.9a,126.4b);
draw (524.9a,684.6b)--(524.9a,668.0b);
put_text( btex  100 etex, 524.9a, 73.2b, 0, 2);
draw (618.4a,109.8b)--(618.4a,118.1b);
draw (618.4a,684.6b)--(618.4a,676.3b);
draw (673.1a,109.8b)--(673.1a,118.1b);
draw (673.1a,684.6b)--(673.1a,676.3b);
draw (712.0a,109.8b)--(712.0a,118.1b);
draw (712.0a,684.6b)--(712.0a,676.3b);
draw (742.1a,109.8b)--(742.1a,118.1b);
draw (742.1a,684.6b)--(742.1a,676.3b);
draw (766.7a,109.8b)--(766.7a,118.1b);
draw (766.7a,684.6b)--(766.7a,676.3b);
draw (787.5a,109.8b)--(787.5a,118.1b);
draw (787.5a,684.6b)--(787.5a,676.3b);
draw (805.5a,109.8b)--(805.5a,118.1b);
draw (805.5a,684.6b)--(805.5a,676.3b);
draw (821.4a,109.8b)--(821.4a,118.1b);
draw (821.4a,684.6b)--(821.4a,676.3b);
draw (835.6a,109.8b)--(835.6a,126.4b);
draw (835.6a,684.6b)--(835.6a,668.0b);
put_text( btex  1000 etex, 835.6a, 73.2b, 0, 2);
draw (929.1a,109.8b)--(929.1a,118.1b);
draw (929.1a,684.6b)--(929.1a,676.3b);
draw (983.8a,109.8b)--(983.8a,118.1b);
draw (983.8a,684.6b)--(983.8a,676.3b);
draw (1022.7a,109.8b)--(1022.7a,118.1b);
draw (1022.7a,684.6b)--(1022.7a,676.3b);
draw (1052.8a,109.8b)--(1052.8a,118.1b);
draw (1052.8a,684.6b)--(1052.8a,676.3b);
draw (1077.4a,109.8b)--(1077.4a,118.1b);
draw (1077.4a,684.6b)--(1077.4a,676.3b);
draw (1098.2a,109.8b)--(1098.2a,118.1b);
draw (1098.2a,684.6b)--(1098.2a,676.3b);
draw (1116.2a,109.8b)--(1116.2a,118.1b);
draw (1116.2a,684.6b)--(1116.2a,676.3b);
draw (1132.1a,109.8b)--(1132.1a,118.1b);
draw (1132.1a,684.6b)--(1132.1a,676.3b);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  10000 etex, 1146.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time (nanoseconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 680.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Ewald 43k etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
draw (1023.5a,649.7b)--(1110.9a,649.7b);
draw (1023.5a,658.0b)--(1023.5a,641.4b);
draw (1110.9a,658.0b)--(1110.9a,641.4b);
draw (371.2a,623.6b)--(371.2a,636.0b);
draw (362.9a,623.6b)--(379.5a,623.6b);
draw (362.9a,636.0b)--(379.5a,636.0b);
draw (558.2a,506.0b)--(558.2a,521.0b);
draw (549.9a,506.0b)--(566.5a,506.0b);
draw (549.9a,521.0b)--(566.5a,521.0b);
draw (745.3a,396.1b)--(745.3a,418.3b);
draw (737.0a,396.1b)--(753.6a,396.1b);
draw (737.0a,418.3b)--(753.6a,418.3b);
draw (838.8a,352.4b)--(838.8a,380.9b);
draw (830.5a,352.4b)--(847.1a,352.4b);
draw (830.5a,380.9b)--(847.1a,380.9b);
gpdraw(0,371.2a,626.0b);
gpdraw(0,558.2a,509.0b);
gpdraw(0,745.3a,401.0b);
gpdraw(0,838.8a,356.5b);
gpdraw(0,1067.2a,649.7b);
draw (371.2a,626.0b)--(558.2a,509.0b)--(745.3a,401.0b)--(838.8a,356.5b);
linetype 1;
linetype -2;
put_text( btex P3ME 43k etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
draw (1023.5a,613.1b)--(1110.9a,613.1b);
draw (1023.5a,621.4b)--(1023.5a,604.8b);
draw (1110.9a,621.4b)--(1110.9a,604.8b);
draw (371.2a,559.0b)--(371.2a,584.9b);
draw (362.9a,559.0b)--(379.5a,559.0b);
draw (362.9a,584.9b)--(379.5a,584.9b);
draw (558.2a,450.3b)--(558.2a,479.0b);
draw (549.9a,450.3b)--(566.5a,450.3b);
draw (549.9a,479.0b)--(566.5a,479.0b);
draw (745.3a,358.4b)--(745.3a,393.8b);
draw (737.0a,358.4b)--(753.6a,358.4b);
draw (737.0a,393.8b)--(753.6a,393.8b);
draw (838.8a,321.8b)--(838.8a,452.4b);
draw (830.5a,321.8b)--(847.1a,321.8b);
draw (830.5a,452.4b)--(847.1a,452.4b);
gpdraw(1,371.2a,564.6b);
gpdraw(1,558.2a,456.8b);
gpdraw(1,745.3a,367.0b);
gpdraw(1,838.8a,338.7b);
gpdraw(1,1067.2a,613.1b);
draw (371.2a,564.6b)--(558.2a,456.8b)--(745.3a,367.0b)--(838.8a,338.7b);
linetype 2;
linetype -2;
put_text( btex Ewald 5k etex, 1005.8a, 576.5b, 0, 3);
linetype 2;
draw (1023.5a,576.5b)--(1110.9a,576.5b);
draw (1023.5a,584.8b)--(1023.5a,568.2b);
draw (1110.9a,584.8b)--(1110.9a,568.2b);
draw (371.2a,356.0b)--(371.2a,380.3b);
draw (362.9a,356.0b)--(379.5a,356.0b);
draw (362.9a,380.3b)--(379.5a,380.3b);
draw (558.2a,250.5b)--(558.2a,277.3b);
draw (549.9a,250.5b)--(566.5a,250.5b);
draw (549.9a,277.3b)--(566.5a,277.3b);
draw (745.3a,169.6b)--(745.3a,202.9b);
draw (737.0a,169.6b)--(753.6a,169.6b);
draw (737.0a,202.9b)--(753.6a,202.9b);
draw (838.8a,159.9b)--(838.8a,192.6b);
draw (830.5a,159.9b)--(847.1a,159.9b);
draw (830.5a,192.6b)--(847.1a,192.6b);
gpdraw(2,371.2a,362.7b);
gpdraw(2,558.2a,257.7b);
gpdraw(2,745.3a,178.0b);
gpdraw(2,838.8a,168.2b);
gpdraw(2,1067.2a,576.5b);
draw (371.2a,362.7b)--(558.2a,257.7b)--(745.3a,178.0b)--(838.8a,168.2b);
linetype 3;
linetype -2;
put_text( btex P3ME 5k etex, 1005.8a, 539.9b, 0, 3);
linetype 3;
draw (1023.5a,539.9b)--(1110.9a,539.9b);
draw (1023.5a,548.2b)--(1023.5a,531.6b);
draw (1110.9a,548.2b)--(1110.9a,531.6b);
draw (371.2a,358.3b)--(371.2a,384.5b);
draw (362.9a,358.3b)--(379.5a,358.3b);
draw (362.9a,384.5b)--(379.5a,384.5b);
draw (558.2a,258.5b)--(558.2a,286.6b);
draw (549.9a,258.5b)--(566.5a,258.5b);
draw (549.9a,286.6b)--(566.5a,286.6b);
draw (745.3a,172.3b)--(745.3a,206.1b);
draw (737.0a,172.3b)--(753.6a,172.3b);
draw (737.0a,206.1b)--(753.6a,206.1b);
draw (838.8a,157.1b)--(838.8a,193.0b);
draw (830.5a,157.1b)--(847.1a,157.1b);
draw (830.5a,193.0b)--(847.1a,193.0b);
gpdraw(3,371.2a,365.4b);
gpdraw(3,558.2a,265.9b);
gpdraw(3,745.3a,180.3b);
gpdraw(3,838.8a,166.1b);
gpdraw(3,1067.2a,539.9b);
draw (371.2a,365.4b)--(558.2a,265.9b)--(745.3a,180.3b)--(838.8a,166.1b);
linetype -2;
draw (214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
