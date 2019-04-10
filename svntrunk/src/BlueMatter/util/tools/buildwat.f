      program buildwat
      parameter(mx=200)
      real x(mx,mx,mx),y(mx,mx,mx),z(mx,mx,mx)
      real lx,ly,lz,grd,vol,density
      real px,py,pz
      integer i,j,k,ii,jj,kk
      integer nx,ny,nz
      integer natom,nwat

      grd=3.10
      density=1/29.9143
      r0 = 0.9572
      ang = 104.52
      write(6,*)'input Lx,Ly,Lz'
      read(5,*)lx,ly,lz
      vol=lx*ly*lz
      nwat=abs(vol*density)

      nx=abs(lx/grd)
      ny=abs(ly/grd)
      nz=abs(lz/grd)
      px=nx*1.0
      py=ny*1.0
      pz=nz*1.0
      
c      write(6,*)nx,ny,nz

      write(6,*)'REMARK: water box with dimension',lx,ly,lz

      natom=0
      do i=1,nx+1
         do j=1,ny+1
            do k=1,nz+1
               natom=natom+1
               x(i,j,k)=i*grd-(px+1)/2*grd
               y(i,j,k)=j*grd-(py+1)/2*grd
               z(i,j,k)=k*grd-(pz+1)/2*grd
               if(natom.le.nwat)then
                  write(6,20)(3*natom-2),natom,
     $                 x(i,j,k),y(i,j,k),z(i,j,k)
                  write(6,30)(3*natom-1),natom,
     $                 x(i,j,k)+r0,y(i,j,k),z(i,j,k)
                  write(6,40)(3*natom),natom,x(i,j,k)+r0*cosd(104.52),
     $                 y(i,j,k)+r0*sind(104.52),z(i,j,k)
               endif
            enddo
         enddo
      enddo

      write(6,*)'TER'

 20   format('ATOM ',i6,'  OH2 TIP3',i5,4x,3f8.3)
 30   format('ATOM ',i6,'  H1  TIP3',i5,4x,3f8.3)
 40   format('ATOM ',i6,'  H2  TIP3',i5,4x,3f8.3)

      end

      



