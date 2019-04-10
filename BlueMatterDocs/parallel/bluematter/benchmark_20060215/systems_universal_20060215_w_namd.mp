%GNUPLOT Metapost output: Mon Feb 20 22:37:25 2006


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

beginfig(3);
w:=5.000in;h:=3.000in;
a:=w/1200.0;b:=h/720.0;
scalepen 1; ptsize 1.000;linetype -2;
draw (161.1a,109.8b)--(177.7a,109.8b);
draw (1146.3a,109.8b)--(1129.7a,109.8b);
put_text( btex  0 etex, 143.4a, 109.8b, 0, 3);
draw (161.1a,167.3b)--(177.7a,167.3b);
draw (1146.3a,167.3b)--(1129.7a,167.3b);
put_text( btex  50 etex, 143.4a, 167.3b, 0, 3);
draw (161.1a,224.8b)--(177.7a,224.8b);
draw (1146.3a,224.8b)--(1129.7a,224.8b);
put_text( btex  100 etex, 143.4a, 224.8b, 0, 3);
draw (161.1a,282.2b)--(177.7a,282.2b);
draw (1146.3a,282.2b)--(1129.7a,282.2b);
put_text( btex  150 etex, 143.4a, 282.2b, 0, 3);
draw (161.1a,339.7b)--(177.7a,339.7b);
draw (1146.3a,339.7b)--(1129.7a,339.7b);
put_text( btex  200 etex, 143.4a, 339.7b, 0, 3);
draw (161.1a,397.2b)--(177.7a,397.2b);
draw (1146.3a,397.2b)--(1129.7a,397.2b);
put_text( btex  250 etex, 143.4a, 397.2b, 0, 3);
draw (161.1a,454.7b)--(177.7a,454.7b);
draw (1146.3a,454.7b)--(1129.7a,454.7b);
put_text( btex  300 etex, 143.4a, 454.7b, 0, 3);
draw (161.1a,512.2b)--(177.7a,512.2b);
draw (1146.3a,512.2b)--(1129.7a,512.2b);
put_text( btex  350 etex, 143.4a, 512.2b, 0, 3);
draw (161.1a,569.6b)--(177.7a,569.6b);
draw (1146.3a,569.6b)--(1129.7a,569.6b);
put_text( btex  400 etex, 143.4a, 569.6b, 0, 3);
draw (161.1a,627.1b)--(177.7a,627.1b);
draw (1146.3a,627.1b)--(1129.7a,627.1b);
put_text( btex  450 etex, 143.4a, 627.1b, 0, 3);
draw (161.1a,684.6b)--(177.7a,684.6b);
draw (1146.3a,684.6b)--(1129.7a,684.6b);
put_text( btex  500 etex, 143.4a, 684.6b, 0, 3);
draw (1146.3a,109.8b)--(1146.3a,126.4b);
draw (1146.3a,684.6b)--(1146.3a,668.0b);
put_text( btex  0.1 etex, 1146.3a, 73.2b, 0, 2);
draw (1056.5a,109.8b)--(1056.5a,118.1b);
draw (1056.5a,684.6b)--(1056.5a,676.3b);
draw (1003.9a,109.8b)--(1003.9a,118.1b);
draw (1003.9a,684.6b)--(1003.9a,676.3b);
draw (966.6a,109.8b)--(966.6a,118.1b);
draw (966.6a,684.6b)--(966.6a,676.3b);
draw (937.7a,109.8b)--(937.7a,118.1b);
draw (937.7a,684.6b)--(937.7a,676.3b);
draw (914.1a,109.8b)--(914.1a,118.1b);
draw (914.1a,684.6b)--(914.1a,676.3b);
draw (894.1a,109.8b)--(894.1a,118.1b);
draw (894.1a,684.6b)--(894.1a,676.3b);
draw (876.8a,109.8b)--(876.8a,118.1b);
draw (876.8a,684.6b)--(876.8a,676.3b);
draw (861.5a,109.8b)--(861.5a,118.1b);
draw (861.5a,684.6b)--(861.5a,676.3b);
draw (847.8a,109.8b)--(847.8a,126.4b);
draw (847.8a,684.6b)--(847.8a,668.0b);
put_text( btex  1 etex, 847.8a, 73.2b, 0, 2);
draw (758.0a,109.8b)--(758.0a,118.1b);
draw (758.0a,684.6b)--(758.0a,676.3b);
draw (705.4a,109.8b)--(705.4a,118.1b);
draw (705.4a,684.6b)--(705.4a,676.3b);
draw (668.2a,109.8b)--(668.2a,118.1b);
draw (668.2a,684.6b)--(668.2a,676.3b);
draw (639.2a,109.8b)--(639.2a,118.1b);
draw (639.2a,684.6b)--(639.2a,676.3b);
draw (615.6a,109.8b)--(615.6a,118.1b);
draw (615.6a,684.6b)--(615.6a,676.3b);
draw (595.6a,109.8b)--(595.6a,118.1b);
draw (595.6a,684.6b)--(595.6a,676.3b);
draw (578.3a,109.8b)--(578.3a,118.1b);
draw (578.3a,684.6b)--(578.3a,676.3b);
draw (563.1a,109.8b)--(563.1a,118.1b);
draw (563.1a,684.6b)--(563.1a,676.3b);
draw (549.4a,109.8b)--(549.4a,126.4b);
draw (549.4a,684.6b)--(549.4a,668.0b);
put_text( btex  10 etex, 549.4a, 73.2b, 0, 2);
draw (459.6a,109.8b)--(459.6a,118.1b);
draw (459.6a,684.6b)--(459.6a,676.3b);
draw (407.0a,109.8b)--(407.0a,118.1b);
draw (407.0a,684.6b)--(407.0a,676.3b);
draw (369.7a,109.8b)--(369.7a,118.1b);
draw (369.7a,684.6b)--(369.7a,676.3b);
draw (340.8a,109.8b)--(340.8a,118.1b);
draw (340.8a,684.6b)--(340.8a,676.3b);
draw (317.2a,109.8b)--(317.2a,118.1b);
draw (317.2a,684.6b)--(317.2a,676.3b);
draw (297.2a,109.8b)--(297.2a,118.1b);
draw (297.2a,684.6b)--(297.2a,676.3b);
draw (279.9a,109.8b)--(279.9a,118.1b);
draw (279.9a,684.6b)--(279.9a,676.3b);
draw (264.6a,109.8b)--(264.6a,118.1b);
draw (264.6a,684.6b)--(264.6a,676.3b);
draw (250.9a,109.8b)--(250.9a,126.4b);
draw (250.9a,684.6b)--(250.9a,668.0b);
put_text( btex  100 etex, 250.9a, 73.2b, 0, 2);
draw (161.1a,109.8b)--(161.1a,118.1b);
draw (161.1a,684.6b)--(161.1a,676.3b);
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(161.1a,684.6b)--(161.1a,109.8b);
put_text( btex Computation Rate (time-steps/second) etex, 36.6a, 397.2b, 1, 2);
put_text( btex Atoms/Node etex, 653.7a, 18.3b, 0, 2);
linetype 0;
linetype -2;
put_text( btex SOPE SPI etex, 1026.4a, 500.7b, 0, 3);
linetype 0;
draw (1044.1a,500.7b)--(1131.5a,500.7b);
draw (421.3a,273.5b)--(511.1a,369.1b)--(601.0a,494.6b)--(601.0a,494.5b)--(690.8a,528.7b)
--(690.8a,637.2b)--(780.6a,642.2b)--(870.5a,607.8b);
gpdraw(0,421.3a,273.5b);
gpdraw(0,511.1a,369.1b);
gpdraw(0,601.0a,494.6b);
gpdraw(0,601.0a,494.5b);
gpdraw(0,690.8a,528.7b);
gpdraw(0,690.8a,637.2b);
gpdraw(0,780.6a,642.2b);
gpdraw(0,870.5a,607.8b);
gpdraw(0,1087.8a,500.7b);
linetype 1;
linetype -2;
put_text( btex Rhodopsin SPI etex, 1026.4a, 464.1b, 0, 3);
linetype 1;
draw (1044.1a,464.1b)--(1131.5a,464.1b);
draw (272.9a,177.5b)--(362.7a,231.8b)--(452.6a,312.0b)--(452.6a,312.2b)--(542.4a,418.1b)
--(542.4a,419.0b)--(632.3a,563.3b)--(722.1a,565.4b);
gpdraw(1,272.9a,177.5b);
gpdraw(1,362.7a,231.8b);
gpdraw(1,452.6a,312.0b);
gpdraw(1,452.6a,312.2b);
gpdraw(1,542.4a,418.1b);
gpdraw(1,542.4a,419.0b);
gpdraw(1,632.3a,563.3b);
gpdraw(1,722.1a,565.4b);
gpdraw(1,1087.8a,464.1b);
linetype 2;
linetype -2;
put_text( btex ApoA1 Respa SPI etex, 1026.4a, 427.5b, 0, 3);
linetype 2;
draw (1044.1a,427.5b)--(1131.5a,427.5b);
draw (174.7a,141.5b)--(264.5a,169.8b)--(354.4a,217.2b)--(354.4a,217.7b)--(444.2a,302.5b)
--(444.2a,291.8b)--(534.0a,423.0b)--(623.9a,568.2b);
gpdraw(2,174.7a,141.5b);
gpdraw(2,264.5a,169.8b);
gpdraw(2,354.4a,217.2b);
gpdraw(2,354.4a,217.7b);
gpdraw(2,444.2a,302.5b);
gpdraw(2,444.2a,291.8b);
gpdraw(2,534.0a,423.0b);
gpdraw(2,623.9a,568.2b);
gpdraw(2,1087.8a,427.5b);
linetype 3;
linetype -2;
put_text( btex ApoA1 SPI etex, 1026.4a, 390.9b, 0, 3);
linetype 3;
draw (1044.1a,390.9b)--(1131.5a,390.9b);
draw (174.7a,141.4b)--(264.5a,169.8b)--(354.4a,217.2b)--(354.4a,217.5b)--(444.2a,302.4b)
--(444.2a,291.5b)--(534.0a,421.9b)--(623.9a,556.5b);
gpdraw(3,174.7a,141.4b);
gpdraw(3,264.5a,169.8b);
gpdraw(3,354.4a,217.2b);
gpdraw(3,354.4a,217.5b);
gpdraw(3,444.2a,302.4b);
gpdraw(3,444.2a,291.5b);
gpdraw(3,534.0a,421.9b);
gpdraw(3,623.9a,556.5b);
gpdraw(3,1087.8a,390.9b);
linetype 4;
linetype -2;
put_text( btex SOPE MPI etex, 1026.4a, 354.3b, 0, 3);
linetype 4;
draw (1044.1a,354.3b)--(1131.5a,354.3b);
draw (421.3a,263.7b)--(511.1a,329.0b)--(601.0a,362.0b)--(601.0a,356.3b)--(690.8a,336.2b)
--(690.8a,314.6b)--(780.6a,267.0b)--(870.5a,201.6b);
gpdraw(4,421.3a,263.7b);
gpdraw(4,511.1a,329.0b);
gpdraw(4,601.0a,362.0b);
gpdraw(4,601.0a,356.3b);
gpdraw(4,690.8a,336.2b);
gpdraw(4,690.8a,314.6b);
gpdraw(4,780.6a,267.0b);
gpdraw(4,870.5a,201.6b);
gpdraw(4,1087.8a,354.3b);
linetype 5;
linetype -2;
put_text( btex Rhodopsin MPI etex, 1026.4a, 317.7b, 0, 3);
linetype 5;
draw (1044.1a,317.7b)--(1131.5a,317.7b);
draw (272.9a,178.3b)--(362.7a,231.8b)--(452.6a,289.3b)--(452.6a,287.8b)--(542.4a,316.4b)
--(542.4a,307.1b)--(632.3a,270.1b)--(722.1a,199.0b);
gpdraw(5,272.9a,178.3b);
gpdraw(5,362.7a,231.8b);
gpdraw(5,452.6a,289.3b);
gpdraw(5,452.6a,287.8b);
gpdraw(5,542.4a,316.4b);
gpdraw(5,542.4a,307.1b);
gpdraw(5,632.3a,270.1b);
gpdraw(5,722.1a,199.0b);
gpdraw(5,1087.8a,317.7b);
linetype 6;
linetype -2;
put_text( btex ApoA1 Respa MPI etex, 1026.4a, 281.1b, 0, 3);
linetype 6;
draw (1044.1a,281.1b)--(1131.5a,281.1b);
draw (174.7a,142.2b)--(264.5a,169.4b)--(354.4a,211.1b)--(354.4a,210.0b)--(444.2a,262.5b)
--(444.2a,243.9b)--(534.0a,266.7b)--(623.9a,207.0b);
gpdraw(6,174.7a,142.2b);
gpdraw(6,264.5a,169.4b);
gpdraw(6,354.4a,211.1b);
gpdraw(6,354.4a,210.0b);
gpdraw(6,444.2a,262.5b);
gpdraw(6,444.2a,243.9b);
gpdraw(6,534.0a,266.7b);
gpdraw(6,623.9a,207.0b);
gpdraw(6,1087.8a,281.1b);
linetype 7;
linetype -2;
put_text( btex ApoA1 MPI etex, 1026.4a, 244.5b, 0, 3);
linetype 7;
draw (1044.1a,244.5b)--(1131.5a,244.5b);
draw (174.7a,142.1b)--(264.5a,169.4b)--(354.4a,211.0b)--(354.4a,209.9b)--(444.2a,262.1b)
--(444.2a,243.9b)--(534.0a,266.5b)--(623.9a,201.2b);
gpdraw(7,174.7a,142.1b);
gpdraw(7,264.5a,169.4b);
gpdraw(7,354.4a,211.0b);
gpdraw(7,354.4a,209.9b);
gpdraw(7,444.2a,262.1b);
gpdraw(7,444.2a,243.9b);
gpdraw(7,534.0a,266.5b);
gpdraw(7,623.9a,201.2b);
gpdraw(7,1087.8a,244.5b);
linetype 0;
linetype -2;
put_text( btex ApoA1 NAMD MPI etex, 1026.4a, 207.9b, 0, 3);
linetype 0;
draw (1044.1a,207.9b)--(1131.5a,207.9b);
draw (174.7a,151.9b)--(264.5a,176.3b)--(354.4a,218.8b)--(444.2a,265.6b)--(534.0a,297.5b)
--(623.9a,294.8b);
gpdraw(8,174.7a,151.9b);
gpdraw(8,264.5a,176.3b);
gpdraw(8,354.4a,218.8b);
gpdraw(8,444.2a,265.6b);
gpdraw(8,534.0a,297.5b);
gpdraw(8,623.9a,294.8b);
gpdraw(8,1087.8a,207.9b);
linetype 1;
linetype -2;
put_text( btex ApoA1 NAMD Non-blocking Msging Layer etex, 1026.4a, 171.3b, 0, 3);
linetype 1;
draw (1044.1a,171.3b)--(1131.5a,171.3b);
draw (161.1a,155.5b)--(174.7a,158.3b)--(264.5a,193.1b)--(354.4a,243.5b)--(444.2a,295.2b)
--(534.0a,330.9b);
gpdraw(9,174.7a,158.3b);
gpdraw(9,264.5a,193.1b);
gpdraw(9,354.4a,243.5b);
gpdraw(9,444.2a,295.2b);
gpdraw(9,534.0a,330.9b);
gpdraw(9,1087.8a,171.3b);
linetype 2;
linetype -2;
put_text( btex ApoA1 NAMD on Lemieux (Elan/Quadrics) etex, 1026.4a, 134.7b, 0, 3);
linetype 2;
draw (1044.1a,134.7b)--(1131.5a,134.7b);
draw (161.1a,156.1b)--(174.2a,159.1b)--(264.4a,191.9b)--(317.1a,201.0b)--(366.5a,199.6b);
gpdraw(0,174.2a,159.1b);
gpdraw(0,264.4a,191.9b);
gpdraw(0,317.1a,201.0b);
gpdraw(0,366.5a,199.6b);
gpdraw(0,1087.8a,134.7b);
linetype -2;
draw (161.1a,109.8b)--(1146.3a,109.8b)--(1146.3a,684.6b)--(161.1a,684.6b)--(161.1a,109.8b);
endfig;
% BEGPOST
verbatimtex
 \end{document}
etex
% ENDPOST
end.
