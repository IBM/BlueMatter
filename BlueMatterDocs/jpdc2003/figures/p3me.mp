% metapost file to draw figure data dependencies for molecular
% dynamics with ewald treatment of periodic boundary conditions

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
  \def\imag{i}
  %\documentclass[20pt,dvips,landscape]{foils}
  \documentclass{article}
  \usepackage[LY1]{fontenc}	% specify text font encoding
  \usepackage{amsmath}
  \usepackage[usenames,dvips]{color}
  \usepackage[expert]{lucidabr}
  %\Black{\color{yellow}} % use yellow for default type color
  \color{yellow}
  %\globalColor{\pagecolor{black}} % make the pagecolor black
  \pagecolor{black}
  \begin{document}
  etex;

%interim defaultdy := 10bp;
%interim defaultdx := 10bp;

  defaultfont := "lsr";
  defaultscale:=.75; circmargin:=4pt;


  boxjoin(a.e=b.w-(1.5in,1.0in));

  picture pb;
  pb:=thelabel(btex \LARGE
    \begin{minipage}{1.25in}
    \begin{gather*}
    \text{particle ID:\ } \overline{l} \\
    \mathbf{r}_l
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.initpos(pb);

  picture prf;
  prf:=thelabel(btex
    \LARGE
    \begin{minipage}{1.25in}
    \begin{gather*}
    \overline{l, m}\\
    \mathbf{F}_{lm}^{pair}(\mathbf{r}_l,\mathbf{r}_m)
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.realforce(prf);

  drawboxed(initpos, realforce);
  centerarrow(initpos,realforce)  initpos.c{dir45}..realforce.w;

  boxjoin();

  picture pk;
  pk:=thelabel(btex
    \LARGE
    \begin{minipage}{1.25in}
    \begin{gather*}
    \overline{l,\alpha}\\
    \mathbf{r}_l,\mathbf{k}_\alpha\\
    \exp(\imag\,\mathbf{k}_\alpha\cdot\mathbf{r}_l)
    \end{gather*}
    \end{minipage}
    etex,(0,0));

  boxit.expk(pk);
  initpos.e=expk.w-(-.5in, -1.5in);

  draw bpath expk;
  draw pic(expk);
  centerarrow(initpos,expk)
  initpos.c{dir-45}..{dir-45}expk.c;
  
  picture prho;
  prho:=thelabel(btex
    \LARGE
    \begin{minipage}{3.0in}
    \begin{gather*}
    \overline{\alpha}\\
    \rho(\mathbf{k}_\alpha) \propto
    \sum_{l=1}^{N}q_l\,\exp\left(\imag\,\mathbf{k}_\alpha\cdot\mathbf{r}_l\right)
    \end{gather*}
    \end{minipage}
  etex,(0,0));
  boxit.rhok(prho);
  expk.e=rhok.w-(-1.0in, -1.5in);

  draw bpath rhok;
  draw pic(rhok);

  centerarrow(expk,rhok) expk.c{dir-45}..{dir-45}rhok.c;
  
  picture pfk;
  pfk:=thelabel(btex
    \LARGE
    \begin{minipage}{5.0in}
    \begin{gather*}
    \overline{l,\alpha}\\
    \mathbf{F}_{l\alpha} \propto \frac{\mathbf{k}_\alpha\,\exp\left(
      \frac{-\left(\mathbf{k_\alpha\cdot
	k_\alpha}\right)}{4\,{\kappa}^2}\right)}{\left(
      \mathbf{k_\alpha\cdot k_\alpha}\right) }\,
    \exp\left(\imag\,\mathbf{k_\alpha\cdot
      r_l}\right)\,\rho\left(-\mathbf{k}_\alpha\right)
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.fk(pfk);
  rhok.e=fk.w-(1.0in, 0);
  draw bpath fk;
  draw pic(fk);

  centerarrow(expk,fk) expk.e{dir0}..{dir-45}fk.c;
  centerarrow(rhok,fk) rhok.c{dir0}..fk.c;

  picture pftot;
  pftot:=thelabel(btex
    \LARGE
    \begin{minipage}{2.5in}
    \begin{gather*}
    \text{particle ID:\ }\overline{l}\\
    \mathbf{F}_l^{total} = \sum_m \mathbf{F}_{lm}^{pair} + \sum_\alpha \mathbf{F}_{l\alpha}
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.ftot(pftot);
  fk.n=ftot.s-(1.25in, 2.0in);
  draw bpath ftot;
  draw pic(ftot);

  centerarrow(fk, ftot) fk.c{dir90}..{dir90}ftot.c;
  centerarrow(realforce, ftot) realforce.e..ftot.c;

endfig;

beginfig(1);

%interim defaultdy := 10bp;
%interim defaultdx := 10bp;

  defaultfont := "lsr";
  defaultscale:=.75; circmargin:=4pt;


  boxjoin(a.e=b.w-(1.5in,1.0in));

  picture pb;
  pb:=thelabel(btex \LARGE
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{l}\\
    R\equiv\{\mathbf{r}_l\}\\
    \parallel R\parallel\approx 30,000
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.initpos(pb);

  picture prf;
  prf:=thelabel(btex
    \LARGE
    \begin{minipage}{3.25in}
    \begin{gather*}
    \overline{l, m}\\
    \parallel R\times R\parallel\approx {30,000}^2\\
    \text{in a system with cutoff} = 12\AA\\
    \approx 22,000,000
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.realforce(prf);

  drawboxed(initpos, realforce);
  centerarrow(initpos,realforce)  initpos.c{dir45}..realforce.w;

  boxjoin();

  picture pk;
  pk:=thelabel(btex
    \LARGE
    \begin{minipage}{2.5in}
    \begin{gather*}
    \overline{l,\alpha}\\
    K\equiv\{\mathbf{k}_\alpha\}\\
    \parallel R\times K \parallel\approx 6,000,000
    \end{gather*}
    \end{minipage}
    etex,(0,0));

  boxit.expk(pk);
  initpos.e=expk.w-(-.5in, -1.5in);

  draw bpath expk;
  draw pic(expk);
  centerarrow(initpos,expk)
  initpos.c{dir-45}..{dir-45}expk.c;
  
  picture prho;
  prho:=thelabel(btex
    \LARGE
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{\alpha}\\
    \parallel K \parallel\approx 200
    \end{gather*}
    \end{minipage}
  etex,(0,0));
  boxit.rhok(prho);
  expk.e=rhok.w-(-1.0in, -1.5in);

  draw bpath rhok;
  draw pic(rhok);

  centerarrow(expk,rhok) expk.c{dir-45}..{dir-45}rhok.c;
  
  picture pfk;
  pfk:=thelabel(btex
    \LARGE
    \begin{minipage}{2.5in}
    \begin{gather*}
    \overline{l,\alpha}\\
    \parallel R\times K \parallel\approx 6,000,000
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.fk(pfk);
  rhok.e=fk.w-(1.0in, 0);
  draw bpath fk;
  draw pic(fk);

  centerarrow(expk,fk) expk.e{dir0}..{dir-45}fk.c;
  centerarrow(rhok,fk) rhok.c{dir0}..fk.c;

  picture pftot;
  pftot:=thelabel(btex
    \LARGE
    \begin{minipage}{1.5in}
    \begin{gather*}
    \overline{l}\\
    \parallel R\parallel\approx 30,000
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.ftot(pftot);
  fk.n=ftot.s-(1.75in, 2.0in);
  draw bpath ftot;
  draw pic(ftot);

  centerarrow(fk, ftot) fk.c{dir90}..{dir90}ftot.c;
  centerarrow(realforce, ftot) realforce.e..ftot.c;

endfig;

beginfig(2);

%interim defaultdy := 10bp;
%interim defaultdx := 10bp;

  defaultfont := "lsr";
  defaultscale:=.75; circmargin:=4pt;


  boxjoin(a.e=b.w-(1.5in,1.0in));

  picture pb;
  pb:=thelabel(btex \large
    \begin{minipage}{1.0in}
    \begin{gather*}
    \text{particle ID:\ } \overline{l} \\
    \mathbf{r}_l
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.initpos(pb);

  picture prf;
  prf:=thelabel(btex
    \large
    \begin{minipage}{1.0in}
    \begin{gather*}
    \overline{l, m}\\
    \mathbf{F}_{lm}^{pair}(\mathbf{r}_l,\mathbf{r}_m)
    \end{gather*}
    \end{minipage}
    etex, (0,0));
  boxit.realforce(prf);

  drawboxed(initpos, realforce);
  centerarrow(initpos,realforce)  initpos.c{dir45}..realforce.w;

  boxjoin();

  picture pmesh;
  pmesh:=thelabel(btex
    \large
    \begin{minipage}{1.25in}
    \begin{gather*}
    \overline{x_0,x_1,x_2}\\
    \rho_{mesh}(x_0,x_1,x_2)
    \end{gather*}
    \end{minipage}
    etex,(0,0));

  boxit.mesh(pmesh);
  initpos.e=mesh.w-(-.5in, -1.5in);

  draw bpath mesh;
  draw pic(mesh);
  centerarrow(initpos,mesh)
  initpos.c{dir-45}..{dir-45}mesh.c;
  
  picture pfft;
  pfft:=thelabel(btex
    \large
    \begin{minipage}{2.0in}
    \begin{gather*}
    \overline{x_0,x_1,x_2}\\
    \rho(k_0,k_1,k_2)=\\
    \mathcal{F}_{x_0,x_1,x_2}^{3}[\rho(x_0,x_1,x_2)]
    \end{gather*}
    \end{minipage}
  etex,(0,0));
  boxit.fft(pfft);
  mesh.e=fft.w-(-1.0in, -1.5in);

  draw bpath fft;
  draw pic(fft);

  centerarrow(mesh,fft) mesh.c{dir-45}..{dir-45}fft.c;
  
  picture pgfun;
  pgfun:=thelabel(btex
    \large
    \begin{minipage}{2.0in}
    \begin{gather*}
    \overline{k_0,k_1,k_2}\\
    \phi(k_0,k_1,k_2) =\\
    G(k_0,k_1,k_2)\,\rho(k_0,k_1,k_2)
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.gfun(pgfun);
  fft.e=gfun.w-(0.5in, 1.5in);
  draw bpath gfun;
  draw pic(gfun);

  centerarrow(fft,gfun) fft.e{dir0}..{dir-45}gfun.c;
  
  picture pifft;
  pifft:=thelabel(btex
    \large
    \begin{minipage}{2.0in}
    \begin{gather*}
    \overline{k_0,k_1,k_2}\\
    \phi(x_0,x_1,x_2) =\\
    \mathcal{F}_{k_0,k_1,k_2}^{-3}[\phi(k_0,k_1,k_2)]
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.ifft(pifft);
  gfun.e=ifft.w-(0.5in, -1.5in);
  draw bpath ifft;
  draw pic(ifft);

  centerarrow(gfun,ifft) gfun.e{dir0}..{dir-45}ifft.c;

  picture pftot;
  pftot:=thelabel(btex
    \large
    \begin{minipage}{2.0in}
    \begin{gather*}
    \text{particle ID:\ }\overline{l}\\
    \mathbf{F}_l^{total} = \sum_m \mathbf{F}_{lm}^{pair} + \mathbf{F}_{l}[\phi]
    \end{gather*}
    \end{minipage}
    etex, (0,0));

  boxit.ftot(pftot);
  ifft.n=ftot.s-(0.5in, 2.0in);
  draw bpath ftot;
  draw pic(ftot);

  centerarrow(ifft, ftot) ifft.c{dir90}..{dir90}ftot.c;
  centerarrow(realforce, ftot) realforce.e..ftot.c;

endfig;

end;
