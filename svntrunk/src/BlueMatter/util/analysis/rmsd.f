      program test
      real*16 sum,sum2,rmsd,energy,ave
      character*16 fname

      ave=0.0
      rmsd=0.0
      sum=0.0
      sum2=0.0
      read(5,*)timestep,nline
      read(5,*)fname

      open(10,file=fname)

      read(10,*)
      do i=1,nline
         read(10,20)energy
         sum=sum+energy
         sum2=sum2+energy**2
      enddo
      close(10)
 20   format(5x,f12.6)

c      write(6,*)sum**2/nline,sum2
      ave=sum/nline
      rmsd=(sum2-sum*sum/nline)/nline
      rmsd=sqrt(rmsd)

      write(6,10)timestep,rmsd,ave
 10   format(3f16.7) 
      end
