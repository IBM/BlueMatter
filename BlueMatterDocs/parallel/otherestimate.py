# module containing routines to make crude performance "estimates"
# for "other" hardware

import fft_tree
import math

#molecules per cubic meter
waterDensity=3.34e28
#atoms per cubic angstrom
waterAtomDensity=3*waterDensity*1e-30

#flops/cycle conversion
bgl_flopsPerCycle=4

#sizeof(double)
bytesPerDouble=8

class OtherPlatform:
    # struct to hold data characterizing platform
    def __init__(self, clock, compRate, compEff, netBW, latency):
        self.tCycle=1.0/float(clock)
        self.compRate=compRate
        self.compEff=compEff
        self.netBW=netBW
        self.latency=latency
        
class VerletIntegrator:
    # processor cycles to propagate updated velocity and position
    # current estimate is memory limited
    verletFLOPS=24*bgl_flopsPerCycle
    def __init__(self, platform):
	"Initialize verlet integrator instance"
        self.__tVerlet=platform.tCycle*float(VerletIntegrator.verletFLOPS)/(float(platform.compRate*platform.compEff))
    def time(self):
	return self.__tVerlet

class VolumeDecomp:
    def __init__(self, procMesh, atomCount):
	self.__cellRadius=math.pow(atomCount/waterAtomDensity,0.33333333)*math.sqrt(3)*0.5/float(min(procMesh))
        self.__procMesh=procMesh
    def cellRadius(self):
        return(self.__cellRadius)
    def siteCount(self, cutoff):
	return(waterAtomDensity*4*math.pi*math.pow(self.__cellRadius+cutoff,3)/3)

class RealSpaceNB:
    # cycle count/non-bond pair interaction
    nonBondFLOPCount=340*bgl_flopsPerCycle
    def __init__(self, atomCount, cutoff, platform):
	self.__cutoff=cutoff
	self.__atomCount=atomCount
        self.__platform=platform
    def compTime(self):
	total=self.__platform.tCycle*\
               self.__atomCount*RealSpaceNB.nonBondFLOPCount*\
	       1.33333333*math.pi*self.__cutoff*self.__cutoff*self.__cutoff*\
	       waterAtomDensity/\
               (self.__platform.compRate*\
                self.__platform.compEff)
	return total
    def dataVolume(self):
	return 3*bytesPerDouble*self.__atomCount
    def globalizeTime(self, procs):
        return self.__platform.tCycle*(self.dataVolume()*8/float(self.__platform.netBW) + float(procs)*self.__platform.latency)

# assumes 1 forward FFT, 4 inverse FFTs, globalization of positions
# double computing of forces, no overlap
def tsEstimate_gp(p, platform, chargeMesh, atomCount, cutoff):
    fft=fft_tree.FFTestimate(1.0/platform.tCycle,\
                             platform.compRate,\
                             platform.compEff,\
                             platform.netBW,
                             platform.latency)
    nb=RealSpaceNB(atomCount, cutoff, platform)
    verlet=VerletIntegrator(platform)
    fftComm=5*fft.commTime(p, chargeMesh)
    fftComp=5*fft.compTime(p, chargeMesh)
    rsnb=nb.compTime()/float(p)
    globalization=nb.globalizeTime(p)
    tVerlet=atomCount*verlet.time()/float(p)
    total=fftComm+fftComp+rsnb+globalization+tVerlet
    return (fftComm, fftComp, rsnb, globalization, tVerlet, total, fftComm+fftComp)

def tsEstimate_power5blade(p, meshSize, atomCount, cutoff):
    # platform=OtherPlatform(2.0e9,8,1,8,1e4)
    platform=OtherPlatform(2.0e9,8,1,4,1e4)
    return tsEstimate_gp(p, platform, meshSize, atomCount, cutoff)

def tsEstimate_gpul2blade(p, meshSize, atomCount, cutoff):
    # platform=OtherPlatform(2.5e9,8,1,6.4,1.25e4)
    platform=OtherPlatform(2.5e9,8,1,3.2,1.25e4)
    return tsEstimate_gp(p, platform, meshSize, atomCount, cutoff)

def tsEstimate_ia32blade(p, meshSize, atomCount, cutoff):
    # platform=OtherPlatform(4.0e9,4,1,4,2e4)
    platform=OtherPlatform(4.0e9,4,1,2,2e4)
    return tsEstimate_gp(p, platform, meshSize, atomCount, cutoff)
