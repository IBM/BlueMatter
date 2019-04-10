/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 #include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <BlueMatter/BMT.hpp>

#define FNAMESIZE 1000
#define BUFFSIZE 1000000
#define NUMFILES_DEF 50
#define PRECISION_DEF -1
#define VELOCITIES_DEF -1
#define MAXATOMS 100000

int doCSV = 0;

using namespace std;

inline bool IsValidPrecision(int p) {
        return (p==16 || p==32 || p ==64 || p==0 || p==PRECISION_DEF);
}

inline void complainInvalid(const char *arg,int def_val) {
        cout << "Warning: Invalid argument : " << arg << " defaulted to " << def_val << endl;
}

int EstimateClosestFrame(int cstep, int ssstep, int s1, int s2) {
	assert(s2>=s1);
	assert(s1>0);
	assert(cstep>=ssstep);

	int fstep = cstep - ssstep;
	// necessary for ensuring termination and solution
	assert( (fstep%s1)==0);

	while( (fstep%s2)!=0 ) fstep += s1;	

	return (fstep + ssstep);
}

int FindRoughNeighbor(int cstep, int ssstep, int s1, int s2) {
	assert(s2>=s1);
	assert(s1>0);
	assert(cstep>=ssstep);

	int fstep = cstep - ssstep;

	if(fstep%s2 == 0)  return cstep;

	int q = 1 + fstep/s2;
	int target = q * s2;

	int prevstep = fstep;

	while(fstep<target ) {
		prevstep = fstep;
		fstep += s1;
	}
	assert(target>=prevstep);
	assert(target<=fstep);

	if( (target-prevstep) < (fstep-target) ) 
		return prevstep + ssstep;

	return fstep + ssstep;
}

int main(int argc, char **argv, char **envp) {

    if (argc < 4) {
        cout << "bmt2timeseries BMTFileName BMTNameTemplate Atomlist  <-skip_factor 1> <-coordformat %s> <-start 0> <-numfiles 250> <-verbose> <-CSV> " << endl;
        cout << " PS: All input BMTs are assumed to have the same OuterTimeStepInPicoSeconds, SnapshotPeriodInOTS, and SimulationStartStep." ;
        cout << "  Also, the skip_factor should be at least the skip_factor in each input BMT." << endl;
        return 1;
    }
    XYZ *coor;

    char ofile[FNAMESIZE];
    char ifile_template[FNAMESIZE];
    char ifile[FNAMESIZE];
    char alistfile[FNAMESIZE];

    strcpy(ofile,argv[1]);
    strcpy(ifile_template,argv[2]);
    strcpy(alistfile,argv[3]);

    FILE *fmain = fopen(ofile, "rb");
    if (fmain) {
        cerr << "File " << ofile << " already exists.  Exiting." << endl;
        fclose(fmain);
        exit(-1);
    }

    char bufS[256];
    int NA=0;
    int AlistIA[MAXATOMS];
    FILE *alistPF = fopen(alistfile,"r");
    assert(alistPF != NULL);

    fgets(bufS,256,alistPF);
    while(!feof(alistPF))
      {
	int val;
	int nbs = sscanf(bufS,"%d",&val);
	if(nbs == 1)
	  {
	    AlistIA[NA]= val-1;
	    NA++;
	  }
	fgets(bufS,256,alistPF);
      }
    int concat=0;
    int start = 0;
    int skip  = 1;
    int precision= PRECISION_DEF;
    int velocities = VELOCITIES_DEF;
    int doappend = 0;
    int  IndexOutput = 0;
    int NoWatersOutput = 0;
    int nooverlap=0;
    int warning=0;
    int noalign=0;
    unsigned int snapshotperiod=0; 
    double OTSinPico;
    int simulationstartstep;
    int numfiles=NUMFILES_DEF;
    int verbose=0;
    char coordformatS[20] = "%8.3f ";
    char formS[64];
    FILE *outPF = stdout;

    int a=argc - 1;
    while(a > 0)
    {
	if (!strcmp("-skip_factor",argv[a]))
	  {sscanf(argv[a+1],"%d",&skip);}
	if (!strcmp("-coordformat",argv[a]))
	  {sscanf(argv[a+1],"%s",&coordformatS);}
	if (!strcmp("-start",argv[a]))
	  {sscanf(argv[a+1],"%d",&start);}
	if (!strcmp("-verbose",argv[a]))
	  {verbose = 1;}
	if (!strcmp("-numfiles",argv[a]))
	  {sscanf(argv[a+1],"%d",&numfiles);}
	if (!strcmp("-CSV",argv[a]))
		{doCSV = 1;}
	a--;
    }
    sprintf(formS," %s %s %s ",coordformatS,coordformatS,coordformatS);

    int netskip = 1;
    int e_fstep; ;

    FILE *outfPF=NULL;
    bool bFirstBMT=true;
    int lastwrittenfstep=-1;
    int e_fsteptowritefromnextbmt=-1;
    bool bFirstFrameInBMT=true;

    for (int n=start ; n<numfiles; n++) 
      {
    	sprintf(ifile, ifile_template, n);
	
        FILE *f = fopen(ifile, "rb");
        if (!f) 
	  {
	    if(verbose) cerr << "\nWarning: BMT File " << ifile << " not found !!" << endl << endl;
	    continue;
	  }
	fclose(f);
	
	if(verbose) cerr << "Info: Last BMT input file started ... " << ifile << endl;
	
	BMTReader bmtr_p(ifile); // leading pointer
	BMTReader bmtr_q(ifile); // trailing pointer 

        int origskip  = 1;
	bmtr_p.getAttrib("BMTSKIPFACTOR",&origskip);

	if(bFirstBMT) 
	  {
            if (!doCSV)
		{
	    fprintf(outPF,"# ");
	    for(int i=0;i<argc;i++)
	      fprintf(outPF,"%s ",argv[i]);
	    fprintf(outPF,"\n");

	    fprintf(outPF,"#time NCols= %d ",NA);
	    for(int i=0;i<NA;i++)
	      fprintf(outPF,"%d ",AlistIA[i] + 1);
	    fprintf(outPF,"\n");
		}

	    // retrieve for sanity check in later loops
	    bmtr_p.getAttrib("SNAPSHOTPERIODINOTS",&snapshotperiod);
	    bmtr_p.getAttrib("OUTERTIMESTEPINPICOSECONDS",&OTSinPico);
	    bmtr_p.getAttrib("SIMULATIONSTARTSTEP",&simulationstartstep);
	    netskip = skip * snapshotperiod;
	    int initStep;
	    bmtr_p.getAttrib("INITIALSTEP",&initStep);

	    e_fstep = FindRoughNeighbor(initStep,simulationstartstep,origskip*snapshotperiod,netskip);
	    
	    if(precision==PRECISION_DEF) precision = bmtr_p.getPrecisionBits();
	    if(velocities==VELOCITIES_DEF) velocities = (bmtr_p.getIncludeVelocities()) ? 1:0  ;
	    
	    if(velocities==1 && !bmtr_p.getIncludeVelocities() ) {
	      cerr << "Warning: Velocities could not be included because there is no velocities in the input BMTs !! " << endl;
	      cerr << "	... continuing without velocities !" << endl;
	      velocities=0;
	    }
	    string sTraj ;
	    bmtr_p.getAttrib("TRAJECTORYFORMAT",&sTraj);
	    bool Recenter=false;
	    if(sTraj.find(TRAJRECENTER)!=string::npos) {
	      Recenter=true;
	    }
	    bFirstBMT = false;
	  }

	// sanity check
	unsigned int origsnp;
	double origots;
	int origsss;
	bmtr_p.getAttrib("SNAPSHOTPERIODINOTS",&origsnp);
	bmtr_p.getAttrib("OUTERTIMESTEPINPICOSECONDS",&origots);
	bmtr_p.getAttrib("SIMULATIONSTARTSTEP",&origsss);
	assert(origsnp==snapshotperiod);
	assert(origots==OTSinPico);
	assert(origsss==simulationstartstep);

	int netorigskip = snapshotperiod*origskip; // this is for the current BMT

	if(precision > bmtr_p.getPrecisionBits()) {
		cerr << "Warning: Desired precision (" << precision << ") is greater than whats supplied in the input BMT ("<< bmtr_p.getPrecisionBits() <<  ") !!" <<endl;
	}

	unsigned int fstep  =0;
	unsigned int prevfstep =0;

	BMTReader *ptr=NULL;
	bFirstFrameInBMT=true;

        while (!bmtr_p.ReadFrame()) 
	  {
	    NameTypeHandle::BMTATTRIBUTETYPE typ;
	    if(!bmtr_p.hasFrameAttrib("Step",&typ)) 
	      {
		cerr << "Error: No Time Step Information in the supplied BMT frame in file " << ifile << ". " ;
		assert(false);
		exit(0);
	      }
	    
	    
	    bmtr_p.getFrameAttrib("Step",&fstep);
	    
	    if(fstep<e_fstep) 
	      {
		if(verbose) cerr << "Info: Skipping frame with step " << fstep << " while expecting " << e_fstep << endl;
		bmtr_q.ReadFrame(); // now p and q are even
		prevfstep = fstep; 
		continue;
	      }
	    if(fstep==e_fstep) 
	      {
		if(verbose) cerr << "Info: Writing frame with step " << fstep << endl;
		ptr = &bmtr_p;
		lastwrittenfstep = fstep;
	      }
	    else { // (fstep>e_fstep) 
	      if(noalign ) 
		{
		  cerr << "Error: Frame in input BMT file " << ifile << " does not align; found frame step " << fstep << " while expecting frame step " << e_fstep << endl; 
		  assert(false);
		  exit(0);
		}
	      
	      assert(prevfstep<e_fstep);
	      if( (e_fstep-prevfstep) < (fstep-e_fstep) ) 
		{
		  if(verbose) cerr << "Warning: Frame aligned at time step : " << prevfstep << " (approx to " << e_fstep << ")" <<  endl;
		  ptr = &bmtr_q;
		  lastwrittenfstep = prevfstep;
		}
	      else 
		{
		  if(verbose) cerr << "Warning: Frame aligned at time step : " << fstep << " (approx to " << e_fstep << ")" <<  endl;
		  ptr = &bmtr_p;
		  lastwrittenfstep = fstep;
		}
	    }
	    
	    if(noalign && bFirstFrameInBMT && e_fsteptowritefromnextbmt>=0 && lastwrittenfstep>=0 ) 
	      {
		if( e_fsteptowritefromnextbmt != lastwrittenfstep) 
		  {	
		    cerr << "Error: Frame in input BMT file " << ifile << " does not align with that of previous BMT input; found frame step " << lastwrittenfstep << " while expecting frame step " << e_fsteptowritefromnextbmt << endl; 
		    assert(false);
		    exit(0);
		    
		  }
	      }
	    bFirstFrameInBMT=false;
	    
	    if(verbose) cerr << "Info: Writing contents ..." << endl;

	    coor = ptr->getPositions();
		if(doCSV)
		{
			for(int i=0;i<NA;i++)
			{
				int a = AlistIA[i];
				fprintf(outPF," %d , %d ,  %f , %f , %f \n",fstep,a+1,coor[a].mX,coor[a].mY,coor[a].mZ);
			} 
		}
		else
 		{
	    		fprintf(outPF,"%d ",fstep);
	    		for(int i=0;i<NA;i++)
	      		{
				fprintf(outPF,formS,coor[AlistIA[i]].mX,coor[AlistIA[i]].mY,coor[AlistIA[i]].mZ);
	      		}

	    		fprintf(outPF,"\n");
		}

	    bmtr_q.ReadFrame(); // now p and q are even
	    prevfstep = fstep; 
	    e_fstep = EstimateClosestFrame(fstep+netorigskip,simulationstartstep,netorigskip,netskip);
	  } // end while

	e_fsteptowritefromnextbmt = lastwrittenfstep + netskip;
 
	double starttime = simulationstartstep*OTSinPico;

	if(verbose) {
        	cerr << "Info: Last frame visited: " << fstep << endl;
        	cerr << "Info: Last frame written: " << lastwrittenfstep << endl;
		cerr << "Info: Final Simulation time: " << (lastwrittenfstep-simulationstartstep)*OTSinPico/1000.0 << " nanoseconds" << endl;
		cerr << "Info: Last BMT input file completed: " << ifile << endl;
		cerr << endl;
	}
    } // end for
    fclose(fmain);
}


