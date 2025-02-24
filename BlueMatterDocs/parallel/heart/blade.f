      integer P
      nit = 1000         ! number of iterations
c problem size global
      Nx = 128
      Ny = 128
      Nz = 128
      Nx = 1024
      Ny = 1024
      Nz = 1024
      print*,' give n'
      read(*,*)n
c      n = 16      ! number of Blade nodes
! BW = Bandwidth between nodes in Bytes/s

c       ppn = 32    ! number of FPU per blade node for SRX-2
c       clock = 2.5e9 ! clock speed in GHz for SRX-2
c       fpc = 64      ! flops per cycle per node for SRX-2
c       BW = 19.2e9/8.0    ! for SRX-2

       ppn = 4     ! number of FPU per blade node for GS 
       clock = 2.0e9 ! clock speed in GHz for GS
       fpc = 8      ! flops per cycle per node for GS
       BW = 8.0e9/8.0    ! for GS

c       ppn = 4    ! number of FPU per blade node for GPUL-2
c       clock = 2.5e9 ! clock speed in GHz for GPUL-2
c       fpc = 8      ! flops per cycle per node for GPUL-2
c       BW = 6.4e9/8.0    ! for GPUL-2

c       ppn = 2    ! number of FPU per blade node for IA-32
c       clock = 4.0e9 ! clock speed in GHz for IA-32
c       fpc = 4      ! flops per cycle per node for IA-32
c       BW = 4.0e9/8.0    ! for IA32

      P = n*ppn   ! P = number of FPUs on machine

      fpcc = 200   ! flops between communicate per grid point
      S = clock*fpc ! performance of node in f/s/node
      e = 0.8     ! compute efficiency of node

      F = 1.*nit*(1.*Nx*Ny*Nz/n)*(fpcc) ! flops per node between communicate

      tcomp = F/(e*S)  ! compute time in seconds per node
! v = bytes communicated per grid point (on cube faces)
! 5 vectors, 3 dims, 6 faces, 4 bytes/word
      v = 5. * 3 * 6 * 4  
! C = number of grid points communicating per face per node
      C = (1.*Nx*Ny*Nz/n)**(2./3.)
      B = nit*v*C                ! bytes communicated per node
      w = 0.8                  ! communication efficiency 
      AL = 5.0e-6*nit          ! MPI latency 
      tcomm = AL + B/(BW*w)
      print*,F,S,B,BW
      print*,' number of blade nodes = ',n
      print*,' problem size = ',Nx,' x ',Ny,' x ',Nz
      print*,' number of iterations = ',nit
      ampn = 5.0*(3.0*4*Nx*Ny*Nz/n)*1.e-6
      print*,' memory reqd. per node = ',ampn,' MBytes'
      print*,' compute flops per node per iter = ',F/nit
      print*,' bytes of data communicated per node per iter = '
     .     ,B/nit
      print*,' compute time = ',tcomp,' seconds'
      print*,' communication time ',tcomm,' seconds '
      print*,' total time = ',tcomp + tcomm,' seconds'
      anperf = F*1.e-9/(tcomp+tcomm)
      print*,' node performance in Gflops/s = ',anperf
      eff = 100.*anperf/(S*1.e-9) ! machine efficiency
      print*,' machine efficiency = ',eff
     .     ,' percent'
      end
