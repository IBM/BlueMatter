size(0,3cm);
real s=2.0;
int xm = 16;
int ym = 16;
int yskip = 2;
for (int i = 0; i < xm; ++i)
{
  for (int j = 0; j < ym; ++j)
  {
    if ((j + i) % yskip == 0)
    {
      filldraw(shift(i*s,j*s)*unitsquare, red);
    }
    else
    {
      draw(shift(i*s,j*s)*unitsquare);
    }
  }
}

