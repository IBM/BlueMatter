%GNUPLOT Metapost output: Thu Feb 23 02:11:59 2006


%% Add \documentclass and \begin{dcoument} for latex
%% NB you should set the environment variable TEX to the name of your
%% latex executable (normally latex) inorder for metapost to work
%% or run
%% mpost --tex=latex ...

% BEGPRE
verbatimtex
\documentclass{article}
\usepackage[mtbold]{mathtime}
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

beginfig(5);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (214.2a,109.8b)--(230.8a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0.0001 etex, 196.5a, 109.8b, 0, 3);
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
put_text( btex  0.001 etex, 196.5a, 301.4b, 0, 3);
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
put_text( btex  0.01 etex, 196.5a, 493.0b, 0, 3);
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
put_text( btex  0.1 etex, 196.5a, 684.6b, 0, 3);
draw (214.2a,109.8b)--(214.2a,126.4b);
draw (214.2a,684.6b)--(214.2a,668.0b);
put_text( btex  100 etex, 214.2a, 73.2b, 0, 2);
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
put_text( btex  1000 etex, 524.9a, 73.2b, 0, 2);
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
put_text( btex  10000 etex, 835.6a, 73.2b, 0, 2);
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
put_text( btex  100000 etex, 1146.3a, 73.2b, 0, 2);
draw (214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b)--(214.2a,109.8b);
put_text( btex Elapsed Time/Time-step (seconds) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Node Count etex, 680.2a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex Total Time etex, 1005.8a, 649.7b, 0, 3);
linetype 0;
draw (1023.5a,649.7b)--(1110.9a,649.7b);
draw (434.6a,536.3b)--(528.1a,488.7b)--(621.6a,444.4b)--(621.6a,444.9b)--(715.2a,406.8b)
--(715.2a,405.0b)--(808.7a,378.0b)--(902.2a,369.9b);
gpdraw(0,434.6a,536.3b);
gpdraw(0,528.1a,488.7b);
gpdraw(0,621.6a,444.4b);
gpdraw(0,621.6a,444.9b);
gpdraw(0,715.2a,406.8b);
gpdraw(0,715.2a,405.0b);
gpdraw(0,808.7a,378.0b);
gpdraw(0,902.2a,369.9b);
gpdraw(0,1067.2a,649.7b);
linetype 1;
linetype -2;
put_text( btex K-space etex, 1005.8a, 613.1b, 0, 3);
linetype 1;
draw (1023.5a,613.1b)--(1110.9a,613.1b);
draw (434.6a,452.2b)--(528.1a,408.0b)--(621.6a,365.4b)--(621.6a,366.6b)--(715.2a,353.5b)
--(715.2a,325.8b)--(808.7a,319.9b)--(902.2a,317.6b);
gpdraw(1,434.6a,452.2b);
gpdraw(1,528.1a,408.0b);
gpdraw(1,621.6a,365.4b);
gpdraw(1,621.6a,366.6b);
gpdraw(1,715.2a,353.5b);
gpdraw(1,715.2a,325.8b);
gpdraw(1,808.7a,319.9b);
gpdraw(1,902.2a,317.6b);
gpdraw(1,1067.2a,613.1b);
linetype 2;
linetype -2;
put_text( btex Real Space etex, 1005.8a, 576.5b, 0, 3);
linetype 2;
draw (1023.5a,576.5b)--(1110.9a,576.5b);
draw (434.6a,466.6b)--(528.1a,407.8b)--(621.6a,348.9b)--(715.2a,290.5b)--(808.7a,233.6b)
--(902.2a,178.6b);
gpdraw(2,434.6a,466.6b);
gpdraw(2,434.6a,466.6b);
gpdraw(2,528.1a,407.8b);
gpdraw(2,621.6a,348.9b);
gpdraw(2,715.2a,290.5b);
gpdraw(2,808.7a,233.6b);
gpdraw(2,902.2a,178.6b);
gpdraw(2,1067.2a,576.5b);
linetype 3;
linetype -2;
put_text( btex Neighborhood Broadcast etex, 1005.8a, 539.9b, 0, 3);
linetype 3;
draw (1023.5a,539.9b)--(1110.9a,539.9b);
draw (434.6a,239.4b)--(528.1a,222.1b)--(621.6a,213.3b)--(621.6a,213.1b)--(715.2a,196.4b)
--(715.2a,197.3b)--(808.7a,183.7b)--(902.2a,186.4b);
gpdraw(3,434.6a,239.4b);
gpdraw(3,528.1a,222.1b);
gpdraw(3,621.6a,213.3b);
gpdraw(3,621.6a,213.1b);
gpdraw(3,715.2a,196.4b);
gpdraw(3,715.2a,197.3b);
gpdraw(3,808.7a,183.7b);
gpdraw(3,902.2a,186.4b);
gpdraw(3,1067.2a,539.9b);
linetype 4;
linetype -2;
put_text( btex Neighborhood Reduce etex, 1005.8a, 503.3b, 0, 3);
linetype 4;
draw (1023.5a,503.3b)--(1110.9a,503.3b);
draw (434.6a,245.0b)--(528.1a,222.3b)--(621.6a,209.9b)--(621.6a,211.6b)--(715.2a,188.3b)
--(715.2a,193.4b)--(808.7a,177.6b)--(902.2a,182.9b);
gpdraw(4,434.6a,245.0b);
gpdraw(4,528.1a,222.3b);
gpdraw(4,621.6a,209.9b);
gpdraw(4,621.6a,211.6b);
gpdraw(4,715.2a,188.3b);
gpdraw(4,715.2a,193.4b);
gpdraw(4,808.7a,177.6b);
gpdraw(4,902.2a,182.9b);
gpdraw(4,1067.2a,503.3b);
linetype -2;
draw (214.2a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(214.2a,684.6b)--(214.2a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
