#include "colors.inc"

#declare NodeFactor = 0.5;
#declare VoxelFactor = 0.8;
#declare Nx = 32;
#declare Ny = 32;
#declare Nz = 32;
#declare Px = 8;
#declare Py = 8;
#declare Pz = 8;

#declare NPx = Nx/Px;
#declare NPy = Ny/Py;
#declare NPz = Nz/Pz;
// X, Y, Z coordinates of current voxel (integer)
// Nx, Ny, Nz mesh dimensions (for FFT)
// Px, Py, Pz processor mesh dimensions
// Ox, Oy, Oz location of origin of volume decomposition model
// Dx, Dy, Dz location of origin of FFTZ decomposition model
// Delta interpolation variable (to take value from clock variable)
#macro Vol2FFTZ(X, Y, Z, Ox, Oy, Oz, Dx, Dy, Dz, Delta)
  #local Psx = floor(X/NPx);
  #local Psy = floor(Y/NPy);
  #local Psz = floor(Z/NPz);

  #local dx = mod(X, NPx);
  #local dy = mod(Y, NPy);
  #local dz = mod(Z, NPz);
  #local Pdx = floor(X/NPx);
  #local Pdy = floor(Y/NPy);
  #local Pdz = floor(((dy + NPy*dx)*Pz)/(NPx*NPy ));

  #local sX = Psx*NPx + NodeFactor*dx + Ox;
  #local sY = Psy*NPy + NodeFactor*dy + Oy;
  #local sZ = Psz*NPz + NodeFactor*dz + Oz;

  #local dX = Pdx*NPx + NodeFactor*dx + Dx;
  #local dY = Pdy*NPy + NodeFactor*dy + Dy;
  #local dZ = Pdz*Nz + NodeFactor*Z + Dz;

  #local iX = (1-Delta)*sX + Delta*dX;
  #local iY = (1-Delta)*sY + Delta*dY;
  #local iZ = (1-Delta)*sZ + Delta*dZ;
  box { 
        <0,0,0>,
        NodeFactor*VoxelFactor*<1, 1, 1>
        texture {
          pigment { color rgbft <X/Nx, Y/Ny, Z/Nz, 0, 0> }
          finish { ambient 0.9 specular 0.5 roughness 0.92 }
        }
        translate <iX, iY, iZ> 
  }
#end

// X, Y, Z coordinates of current voxel (integer)
// Nx, Ny, Nz mesh dimensions (for FFT)
// Px, Py, Pz processor mesh dimensions
// Ox, Oy, Oz location of origin of volume decomposition model
// Dx, Dy, Dz location of origin of FFTZ decomposition model
// Delta interpolation variable (to take value from clock variable)
#macro FFTZ2FFTY(X, Y, Z, Ox, Oy, Oz, Dx, Dy, Dz, Delta)
  #local dx = mod(X, NPx);
  #local dy = mod(Y, NPy);
  #local dz = mod(Z, NPz);

  #local Psx = floor(X/NPx);
  #local Psy = floor(Y/NPy);
  #local Psz = floor(((dy + NPy*dx)*Pz)/(NPx*NPy ));

  #local Pdx = floor(X/NPx);
  #local Pdy = floor(((dx + NPx*dz)*Py)/(NPx*NPz));
  #local Pdz = floor(Z/NPz);

  #local sX = Psx*NPx + NodeFactor*dx + Ox;
  #local sY = Psy*NPy + NodeFactor*dy + Oy;
  #local sZ = Psz*Nz + NodeFactor*Z + Oz;

  #local dX = Pdx*NPx + NodeFactor*dx + Dx;
  #local dY = Pdy*Ny + NodeFactor*Y + Dy;
  #local dZ = Pdz*NPz + NodeFactor*dz + Dz;

  #local iX = (1-Delta)*sX + Delta*dX;
  #local iY = (1-Delta)*sY + Delta*dY;
  #local iZ = (1-Delta)*sZ + Delta*dZ;
  box { 
        <0,0,0>,
        NodeFactor*VoxelFactor*<1, 1, 1>
        texture {
          pigment { color rgbft <X/Nx, Y/Ny, Z/Nz, 0, 0> }
          finish { ambient 0.9 specular 0.5 roughness 0.92 }
        }
        translate <iX, iY, iZ> 
  }
#end

// X, Y, Z coordinates of current voxel (integer)
// Nx, Ny, Nz mesh dimensions (for FFT)
// Px, Py, Pz processor mesh dimensions
// Ox, Oy, Oz location of origin of volume decomposition model
// Dx, Dy, Dz location of origin of FFTZ decomposition model
// Delta interpolation variable (to take value from clock variable)
#macro FFTY2FFTX(X, Y, Z, Ox, Oy, Oz, Dx, Dy, Dz, Delta)
  #local dx = mod(X, NPx);
  #local dy = mod(Y, NPy);
  #local dz = mod(Z, NPz);

  #local Psx = floor(X/NPx);
  #local Psy = floor(((dx + NPx*dz)*Py)/(NPx*NPz));
  #local Psz = floor(Z/NPz);


  #local Pdx = floor(((dy +NPy*dz)*Px)/(NPy*NPz));
  #local Pdy = floor(Y/NPy);
  #local Pdz = floor(Z/NPz);

  #local sX = Psx*NPx + NodeFactor*dx + Ox;
  #local sY = Psy*Ny + NodeFactor*Y + Oy;
  #local sZ = Psz*NPz + NodeFactor*dz + Oz;

  #local dX = Pdx*Nx + NodeFactor*X + Dx;
  #local dY = Pdy*NPy + NodeFactor*dy + Dy;
  #local dZ = Pdz*NPz + NodeFactor*dz + Dz;

  #local iX = (1-Delta)*sX + Delta*dX;
  #local iY = (1-Delta)*sY + Delta*dY;
  #local iZ = (1-Delta)*sZ + Delta*dZ;
  box { 
        <0,0,0>,
        NodeFactor*VoxelFactor*<1, 1, 1>
        texture {
          pigment { color rgbft <X/Nx, Y/Ny, Z/Nz, 0, 0> }
          finish { ambient 0.9 specular 0.5 roughness 0.92 }
        }
        translate <iX, iY, iZ> 
  }
#end

background { color Black }
#declare Px = 4;
#declare Py = 4;
#declare Pz = 4;
#declare Nx = 8;
#declare Ny = 8;
#declare Nz = 8;

//camera {
//  location <1.2*Px*Nx, 1.2*Py*Ny, 1.2*Pz*Nz>
//  look_at <Px*Nx/2, Py*Ny/2, Pz*Nz/2>
//}

#switch(3*clock)
  #range(0, 1)
    #declare LocClock = 3*clock;
    camera {
             location (1 - LocClock)*<1.5*Nx, 1.5*Ny, 1.5*Nz> + LocClock*<0.75*Px*Nx, 0.75*Py*Ny, Pz*Nz/2>
             look_at (1 - LocClock)*<.5*Nx, .5*Ny, .5*Nz> + LocClock*<.5*Nx, .5*Ny, Pz*Nz/2>
    }
  #break
  #range(1, 2)
    #declare LocClock = 3*clock - 1;
    camera {
             location (1 - LocClock)*<0.75*Px*Nx, 0.75*Py*Ny, Pz*Nz/2> + LocClock*<0.75*Px*Nx, Py*Ny/2, 0.75*Pz*Nz>
             look_at (1 - LocClock)*<.5*Nx, .5*Ny, .5*Pz*Nz> + LocClock*<.5*Nx, .5*Py*Ny, Nz/2>
    }
  #break
  #range(2, 3)
    #declare LocClock = 3*clock - 2;
    camera {
             location (1 - LocClock)*<0.75*Px*Nx, 0.5*Py*Ny, 0.75*Pz*Nz> + LocClock*<.5*Px*Nx, 0.75*Py*Ny, 0.75*Pz*Nz>
             look_at (1 - LocClock)*<.5*Nx, .5*Py*Ny, .5*Nz> + LocClock*<.5*Px*Nx, .5*Ny, Nz/2>
    }
  #break
#end
 
#declare IndexX = 0;
#while(IndexX < Nx)
  #declare IndexY = 0;
  #while(IndexY < Ny)
    #declare IndexZ = 0;
    #while(IndexZ < Nz)
      #switch (3*clock)
        #range(0, 1) 
          #declare LocClock = 3*clock;
          Vol2FFTZ(IndexX, IndexY, IndexZ, 0, 0, 0, 0, 0, 0, LocClock)
        #break
        #range(1, 2)
          #declare LocClock = (3*clock - 1);
          FFTZ2FFTY(IndexX, IndexY, IndexZ, 0, 0, 0, 0, 0, 0, LocClock)
        #break
        #range(2, 3)
          #declare LocClock = 3*clock - 2;
          FFTY2FFTX(IndexX, IndexY, IndexZ, 0, 0, 0, 0, 0, 0, LocClock)
        #break
      #end
      #declare IndexZ = IndexZ + 1;
    #end
    #declare IndexY = IndexY + 1;
  #end
  #declare IndexX = IndexX + 1;
#end


// light_source { <-4*Nx, -4*Ny, -4*Nz> color White }
//light_source { <4*Nx, 4*Ny, 4*Nz> color White }
// light_source { <4*Nx, 4*Ny, -4*Nz> color White }
