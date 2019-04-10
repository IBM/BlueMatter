% metapost file to draw figure data dependencies for 3D FFT and inverse

input boxes;
defaultfont := "lsr";

vardef labelarrow
  (suffix BoxA,BoxB) expr Line =
  drawarrow Line
  cutbefore bpath BoxA
  cutafter bpath BoxB;
  point (.5*length Line) of Line
enddef;

vardef centerarrow
 (suffix BoxA,BoxB) expr Line =
  drawarrow Line
  cutbefore bpath BoxA
  cutafter bpath BoxB;
%  point (0.5*length Line) of Line
enddef;

color yellow, orange;
yellow:=red+green;
orange:=red+(green/2);

beginfig(0);

verbatimtex
  \documentclass{article}
%  \usepackage[LY1]{fontenc}	% specify text font encoding
  \usepackage{amsmath}
%  \usepackage[expert]{lucidabr}
  \usepackage[mtbold]{mathtime}
  \newcounter{nodecount}
  \newcounter{nodecountinc}
  \newcommand{\arrayslice}{
  \setcounter{nodecountinc}{\value{nodecount}}
  \stepcounter{nodecountinc}
  \begin{minipage}{2.1in}
  \(\begin{array}{rl}
    x_0\in & [c_0\cdot n_0, (c_0+1)\cdot n_0 -1]\\
    x_1\in & [c_1\cdot n_1, (c_1+1)\cdot n_1 -1]\\
    x_2\in & [\thenodecount\cdot n_2, \thenodecountinc\cdot n_2 -1]
    \end{array} \)
  \end{minipage}
  \stepcounter{nodecount}
  }
  \begin{document}
  etex;

%interim defaultdy := 10bp;
%interim defaultdx := 10bp;

  defaultfont := "lsr";
  defaultscale:=.75; circmargin:=4pt;


  boxjoin(a.c=b.c-(2.5in,0in));

  picture mb;
  mb:=thelabel(btex \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{x_0,x_1,x_2} \\
    \rho(x_0,x_1,x_2)
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.chargemesh(mb);

  picture pfftz;
  pfftz:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{x_0,x_1}\\
    \mathcal{F}_{x_2}^{1}[\rho(x_0,x_1,x_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.fftz(pfftz);

  drawboxed(chargemesh, fftz);
  centerarrow(chargemesh,fftz)  chargemesh.c{dir0}..fftz.w;

  boxjoin();

  picture pffty;
  pffty:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{x_0,k_2}\\
    \mathcal{F}_{x_1}^{1}[\rho(x_0,x_1,k_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.ffty(pffty);

  ffty.c=fftz.c-(2.5in, 1.25in);

  draw bpath ffty;
  draw pic(ffty);
  centerarrow(fftz,ffty)
  fftz.c..ffty.c;

  picture pfftx;
  pfftx:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{k_1,k_2}\\
    \mathcal{F}_{x_0}^{1}[\rho(x_0,k_1,k_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.fftx(pfftx);

  fftx.c=ffty.c+(2.5in, 0in);

  draw bpath fftx;
  draw pic(fftx);
  centerarrow(ffty,fftx)
  ffty.c..fftx.c;

  picture pgfun;
  pgfun:=thelabel(btex
    \large
    \begin{minipage}{2.0in}
    \begin{gather*}
    \overline{k_0,k_1,k_2}\\
    \phi(k_0,k_1,k_2) = \\
    G(k_0,k_1,k_2)\,\rho(k_0,k_1,k_2)
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.gfun(pgfun);

  gfun.c=fftx.c-(2.5in, 1.25in);

  draw bpath gfun;
  draw pic(gfun);
  centerarrow(fftx,gfun)
  fftx.c..gfun.c;

  picture pfftkx;
  pfftkx:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{k_1,k_2}\\
    \mathcal{F}_{k_0}^{-1}[\phi(k_0,k_1,k_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.fftkx(pfftkx);

  fftkx.c=gfun.c+(2.5in, 0.0in);

  draw bpath fftkx;
  draw pic(fftkx);
  centerarrow(gfun,fftkx)
  gfun.c..fftkx.c;

  picture pfftky;
  pfftky:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{x_0,k_2}\\
    \mathcal{F}_{k_1}^{-1}[\phi(x_0,k_1,k_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.fftky(pfftky);

  fftky.c=fftkx.c-(2.5in, 1.25in);

  draw bpath fftky;
  draw pic(fftky);
  centerarrow(fftkx,fftky)
  fftkx.c..fftky.c;

  picture pfftkz;
  pfftkz:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{x_0,x_1}\\
    \mathcal{F}_{k_2}^{-1}[\phi(x_0,x_1,k_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.fftkz(pfftkz);

  fftkz.c=fftky.c+(2.5in, 0.0in);

  draw bpath fftkz;
  draw pic(fftkz);
  centerarrow(fftky,fftkz)
  fftky.c..fftkz.c;

  picture ppotmesh;
  ppotmesh:=thelabel(btex
    \large
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{x_1,x_2,x_3}\\
    \phi(x_0,x_1,x_2)
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.potmesh(ppotmesh);

  potmesh.c=fftkz.c-(2.5in, 1.25in);

  draw bpath potmesh;
  draw pic(potmesh);
  centerarrow(fftkz,potmesh)
  fftkz.c..potmesh.c;
  
endfig;

beginfig(1);

  boxjoin(a.sw=b.nw;a.se=b.ne);
  picture pa;
  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [0:n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a0(pa);

  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [n_2:2\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a1(pa);

  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [2\cdot n_2:3\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a2(pa);

  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [3\cdot n_2:4\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a3(pa);


  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [4\cdot n_2:5\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a4(pa);

  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [5\cdot n_2:6\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a5(pa);

  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [6\cdot n_2:7\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a6(pa);

  pa:=thelabel(btex
    \begin{minipage}{2.0in}
    \(\begin{array}{rl}
    a & [c_0\cdot n_0 : (c_0+1)\cdot n_0 -1]\\
    & [c_1\cdot n_1 : (c_1+1)\cdot n_1 -1]\\
    & [7\cdot n_2:8\cdot n_2 -1]
    \end{array} \)
    \end{minipage}
    etex, (0,0));
  boxit.a7(pa);

  drawboxed(a0,a1,a2,a3,a4,a5,a6,a7);
endfig;

beginfig(2);
  verbatimtex
  etex;

  boxjoin(a.sw=b.nw;a.se=b.ne);
  picture pa;
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[0](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[1](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[2](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[3](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[4](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[5](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[6](pa);
  pa:=thelabel(btex \arrayslice etex, (0,0));
  boxit.a[7](pa);

  for i = 0 upto 7:
    drawboxed(a[i]);
  endfor;

%  for i = 1 upto 7:
%    centerarrow(a[0],a[i])
%    a[0].w{(-i,-1)}..{(i,-1)}a[i].w;
%  endfor;

  for i = 0 upto 7:
    for j = 0 upto 7:
      centerarrow(a[i],a[j])
      a[i].e{(i+j,i-j)}..{(-i-j,i-j)}a[j].e;
%      a[i].e{(i+j,i-j)}..{(-i-j,i-j)}a[j].e;
    endfor;
  endfor;

endfig;

end;
