# module containing routines to make crude performance "estimates"
# for BG/L hardware

import math

#molecules per cubic meter
waterDensity=3.34e28
#atoms per cubic angstrom
waterAtomDensity=3*waterDensity*1e-30

#sizeof(double)
#bytesPerDouble=8.0
bytesPerDouble=4.0 #for single precision fft

class VerletIntegrator:
    # processor cycles to propagate updated velocity and position
    # current estimate is memory limited
    tVerlet=24
    def __init__(self):
	"Initialize verlet integrator instance"
    def time(self):
	return VerletIntegrator.tVerlet

class Tree:
    # bandwidth in bits/cycle
    treeBW=4
    def __init__(self):
	"Initialize tree instance"
    def bandwidth(self):
	return Tree.treeBW
    def globalizeTime(self, dataInBytes):
	# modified from original overly pessimistic since bandwidth
	# down is separate from bandwidth up (removed factor of two
	# from result)
	return 8*float(dataInBytes)/Tree.treeBW
    def scaledReduce(self, doubleCount):
	# assumes two reductions required
	# first establishes scale based on exponents (16 bit/double)
	# second carries out reduce with scaled values (64 bits/double)
	return (16+64)*float(doubleCount)/Tree.treeBW
class Memory:
    # L3 hit latency in cycles
    L3hit=25
    # L3 miss latency in cycles
    L3miss=75
    def __init__(self, procMesh, chargeMesh):
        self.__memSize =\
                       float(chargeMesh[0])*float(chargeMesh[1])*float(chargeMesh[2])*2.0*bytesPerDouble/(float(procMesh[0])*float(procMesh[1])*float(procMesh[2]))
    def memTime(self):
        if self.__memSize < 4e6:
            return self.__memSize*Memory.L3hit
        return self.__memSize*Memory.L3miss
class Torus:
    # bandwidth in bits/cycle/link
    torusBW=2
    # link utilization fraction
    linkUtilization=0.8
    # procmesh is a list or tuple with three elements (px,py,pz)
    # giving the processor mesh dimensions
    def __init__(self, procMesh):
	"Initialize torus instance"
	self.__procMesh = (procMesh[0], procMesh[1], procMesh[2])
    def bandwidth(self):
	return Torus.torusBW
    # args are data volume received by each node, dim array specifying
    # which dimensions of proc array are partipating: len(dim) <=3,
    # 0 <= dim[i] < 3
    def all2allTime(self, dataInBytes, dim):
	rows=[]
        procs=1
	for d in dim:
	    rows.append(self.__procMesh[d])
            procs=procs*self.__procMesh[d]
        msgSize=dataInBytes/float(procs)
        if msgSize < 256:
            msgSize=max((msgSize, 256*8.0/float(min(rows))))
	timeInCycles = 8*procs*float(msgSize)*max(rows)/(8*Torus.torusBW*Torus.linkUtilization)
	return timeInCycles
    def all2allTimeForMesh(self, dataInBytes, dim, chargeMesh):
        # returns tuple of (timeInCycles, msgSize, msgCount)
	rows=[]
        procs=1
        dataCountPerNode =\
                         float(chargeMesh[0])*\
                         float(chargeMesh[1])*\
                         float(chargeMesh[2])/\
                         (float(self.__procMesh[0])*
                          float(self.__procMesh[1])*\
                          float(self.__procMesh[2]))
	for d in dim:
	    rows.append(self.__procMesh[d])
            procs=procs*self.__procMesh[d]
        msgCount=min((dataCountPerNode, procs))
        msgSize=dataInBytes/float(msgCount)
        oldMsgSize=msgSize
        if msgSize < 256:
            msgSize=max((msgSize, 256*8.0/float(min(rows))))
	timeInCycles = 8*procs*float(msgSize)*max(rows)/(8*Torus.torusBW*Torus.linkUtilization)
	return (timeInCycles, oldMsgSize, msgCount)
class VolumeDecomp:
    def __init__(self, procMesh, atomCount):
	self.__cellRadius=math.pow(atomCount/waterAtomDensity,0.33333333)*math.sqrt(3)*0.5/float(min(procMesh))
        self.__procMesh=procMesh
    def cellRadius(self):
        return(self.__cellRadius)
    def siteCount(self, cutoff):
	return(waterAtomDensity*4*math.pi*math.pow(self.__cellRadius+cutoff,3)/3)

class RealFFT3D:
    # nlog(n) coefficient for 1D fft in cycles
    # fftCycleCountBase = 1991/(32*math.log(32))
    # fftCycleCountBase = 7346/(2*128*math.log(128))
    fftCycleCountBase = 8/(math.log(2)*4) # ideal behavior for fused mult/add wit 4 issues/cycle
    # fftCycleCountBase = 5/(math.log(2)*4*.3)
    # args are lists containing dimensions of processor mesh and
    # charge mesh
    def __init__(self, procMesh, chargeMesh):
	self.__torus=Torus(procMesh)
	self.__procMesh=procMesh
	self.__chargeMesh=chargeMesh
        self.__memory = Memory(procMesh, chargeMesh)
    def memTime(self):
        return self.__memory.memTime()
    def commStats(self):
        # returns message size and counts for each communication phase
        # as (msgSize01,msgCount01,msgSize02,msgCount02,msgSize12,msgCount12)
        if self.__procMesh[0]*self.__procMesh[1]*self.__procMesh[2] == 1:
            return(0,0,0,0,0,0)
	dataVolume = \
		   2*bytesPerDouble*\
		   float((self.__chargeMesh[0]/self.__procMesh[0]))*\
		   float((self.__chargeMesh[1]/self.__procMesh[1]))*\
		   float((self.__chargeMesh[2]/self.__procMesh[2]))
        vals =\
             (self.__torus.all2allTimeForMesh(dataVolume, [0], self.__chargeMesh)[1:3],\
              self.__torus.all2allTimeForMesh(dataVolume, (0,2), self.__chargeMesh)[1:3],\
              self.__torus.all2allTimeForMesh(dataVolume, (1,2), self.__chargeMesh)[1:3])
        return vals
    def commTime(self):
	# three all-to-all communications (one in each plane) are
	# required
        if self.__procMesh[0]*self.__procMesh[1]*self.__procMesh[2] == 1:
            return(0)
	dataVolume = \
		   2*bytesPerDouble*\
		   float((self.__chargeMesh[0]/self.__procMesh[0]))*\
		   float((self.__chargeMesh[1]/self.__procMesh[1]))*\
		   float((self.__chargeMesh[2]/self.__procMesh[2]))
	total = self.__torus.all2allTimeForMesh(dataVolume, [0], self.__chargeMesh)[0]+\
		self.__torus.all2allTimeForMesh(dataVolume, (0,2), self.__chargeMesh)[0]+\
		self.__torus.all2allTimeForMesh(dataVolume, (1,2), self.__chargeMesh)[0]
	return (total)
    def compTime(self):
	procCount=self.__procMesh[0]*self.__procMesh[1]*self.__procMesh[2]
	total =\
	      float(self.__chargeMesh[0])*float(self.__chargeMesh[1])*float(self.__chargeMesh[2])*math.log(self.__chargeMesh[2])+\
	      float(self.__chargeMesh[0])*float(self.__chargeMesh[2])*float(self.__chargeMesh[1])*math.log(self.__chargeMesh[1])+\
	      float(self.__chargeMesh[1])*float(self.__chargeMesh[2])*float(self.__chargeMesh[0])*math.log(self.__chargeMesh[0])
        minItems = min((self.__chargeMesh[0]*self.__chargeMesh[1],\
                       self.__chargeMesh[0]*self.__chargeMesh[2],\
                       self.__chargeMesh[2]*self.__chargeMesh[1]))
        if minItems < procCount:
            total = total/minItems
        else:
            total = total/procCount
	return total*RealFFT3D.fftCycleCountBase

class RealSpaceNB:
    # cycle count/non-bond pair interaction
    nonBondCycleCount=340
    def __init__(self, atomCount, cutoff):
	self.__cutoff=cutoff
	self.__atomCount=atomCount
    def compTime(self):
	total=self.__atomCount*RealSpaceNB.nonBondCycleCount*\
	       1.33333333*math.pi*self.__cutoff*self.__cutoff*self.__cutoff*\
	       waterAtomDensity
	return total
    def dataVolume(self):
	return 3*bytesPerDouble*self.__atomCount

# assumes 1 forward FFT, 1 inverse FFT, globalization of positions
# via tree, double computing of forces, no overlap
def tsEstimate_gp(procMesh, chargeMesh, atomCount, cutoff):
    fft=RealFFT3D(procMesh, chargeMesh)
    nb=RealSpaceNB(atomCount, cutoff)
    tree=Tree()
    verlet=VerletIntegrator()
    fftComm=2*fft.commTime()
    fftComp=2*fft.compTime()
    rsnb=nb.compTime()/float(procMesh[0]*procMesh[1]*procMesh[2])
    globalization=tree.globalizeTime(nb.dataVolume())
    tVerlet=atomCount*verlet.time()/float(procMesh[0]*procMesh[1]*procMesh[2])
    total=fftComm+fftComp+rsnb+globalization+tVerlet
    return (fftComm, fftComp, rsnb, globalization, tVerlet, total, fftComm+fftComp)

def tsEstimate_singleNode_gp(chargeMesh, atomCount, cutoff):
    fft=RealFFT3D((1,1,1), chargeMesh)
    nb=RealSpaceNB(atomCount, cutoff)
    verlet=VerletIntegrator()
    fftComp=2*fft.compTime()
    rsnb=nb.compTime()
    tVerlet=atomCount*verlet.time()
    total=fftComp+rsnb+tVerlet
    return (0, fftComp, rsnb, 0, tVerlet, total, fftComp)

# assumes 1 forward FFT, 1 inverse FFT, global force reduction via
# tree, single computing of forces, no overlap, every node propagates
# dynamics for particles within a cutoff distance of the volume that it owns.
def tsEstimate_gfr(procMesh, chargeMesh, atomCount, cutoff):
    fft=RealFFT3D(procMesh, chargeMesh)
    nb=RealSpaceNB(atomCount, cutoff)
    tree=Tree()
    verlet=VerletIntegrator()
    fftComm=2*fft.commTime()
    fftComp=2*fft.compTime()
    rsnb=nb.compTime()/float(2*procMesh[0]*procMesh[1]*procMesh[2])
    forceReduction=tree.scaledReduce(3*atomCount)
    tVerletBase=atomCount*verlet.time()
    v=VolumeDecomp(procMesh, atomCount)
    tVerlet=v.siteCount(cutoff)*verlet.time()
    total=fftComm+fftComp+rsnb+forceReduction+tVerlet
    return(fftComm, fftComp, rsnb, forceReduction, tVerlet, tVerletBase, total, fftComm+fftComp)

def tsEstimate_singleNode_gfr(chargeMesh, atomCount, cutoff):
    fft=RealFFT3D((1,1,1), chargeMesh)
    nb=RealSpaceNB(atomCount, cutoff)
    tree=Tree()
    verlet=VerletIntegrator()
    fftComm=2*fft.commTime()
    fftComp=2*fft.compTime()
    rsnb=nb.compTime()/2.0
    tVerlet=atomCount*verlet.time()
    tVerletBase=tVerlet
    total=fftComp+rsnb+tVerlet
    return(0, fftComp, rsnb, 0, tVerlet, tVerletBase, total, fftComp)
