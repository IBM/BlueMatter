# module containing routines to make crude performance "estimates" on 3D FFTs
# given a few parameters

import math

class FFTestimate:
    # n*ln(n) coefficient for 1D FFT
    fftFLOPs = 4*1991/(32*math.log(32))
    # fftFLOPs = 24 # Dong's estimate
    bytesPerComplex=16
    bitsPerComplex=bytesPerComplex*8
    # args:
    # clock speed in Hz
    # compRate in FLOPS/cycle/node
    # compEff
    # netBW in bits/cycle/node
    # latency (packet latency) in cycles
    def __init__(self, clock, compRate, compEff, netBW, latency):
        self.__tCycle=1.0/float(clock)
        self.__compRate=compRate
        self.__compEff=compEff
        self.__netBW=netBW
        self.__latency=latency
    def commTime(self, p, n):
        # p is node count, n is mesh size
        return(3.0*self.__tCycle*(math.pow(n,3)*FFTestimate.bitsPerComplex/(p*self.__netBW) + math.pow(p,.6666667)*self.__latency))
    def compTime(self, p, n):
        # p is node count, n is mesh size
        flopCount=FFTestimate.fftFLOPs*n*math.log(n)
        return(3.0*self.__tCycle*flopCount*n*n/(self.__compEff*float(p)*self.__compRate))
    def packetSize(self, p, n):
        return(math.pow(n,3)*math.pow(p,-1.666667)*FFTestimate.bytesPerComplex)
    






