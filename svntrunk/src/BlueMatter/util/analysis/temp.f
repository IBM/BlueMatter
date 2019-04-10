      program test
      real*16 sum,sum2,rmsd,energy,ave
      real*16 totale,intrae,intere,ke,temp,boltzman
      integer step
      character*16 fname

      boltzman=0.00198718

      ave=0.0
      rmsd=0.0
      sum=0.0
      sum2=0.0
      read(5,*)timestep,nline,natom
      read(5,*)fname

      open(10,file=fname)

      read(10,*)
      do i=1,nline
         read(10,*)step,totale,intrae,intere,ke
         temp=ke/(1.5*natom*boltzman)
         write(6,'(i5,5x,f7.3)')step,temp
         sum=sum+temp
         sum2=sum2+temp**2
      enddo
      close(10)
c 20   format(5x,f12.6)

c      write(6,*)sum**2/nline,sum2
      ave=sum/nline
      rmsd=(sum2-sum*sum/nline)/nline
      rmsd=sqrt(rmsd)

      write(6,10)timestep,rmsd,ave
 10   format(3f16.7) 
      end
