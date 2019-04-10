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
 #ifndef DATARECEIVERDVSPARSER_HPP
#define DATARECEIVERDVSPARSER_HPP

#include <fstream>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/db2/bondlist.hpp>
#include <db2/dbname.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <db2/stmthandle.hpp>
//AK : new include
#include <BlueMatter/Topology.hpp> //for MoleculeRun
#include <BlueMatter/RandomNumberGenerator.hpp>
#include <BlueMatter/BMT.hpp>

RandomNumberGenerator RanNumGenIF;


class DataReceiverDVSParser : public DataReceiver
{
private:

    double dvsrand()
    {
        const double half = 1 << 15;
        const double factor = 0.5/(half*half-1);
        return random()*factor;
    }

    unsigned numberofoutertimesteps;
    int numberofoutertimestepsknown;
    unsigned currentstep;
    int currentstepknown;
    int rtpknown;
    ED_ControlPressure pressure;
    ED_DynamicVariablesRandomState randomstate;
    ED_DynamicVariablesOneChainThermostat onechainthermo;
    ED_ControlReplicaExchangePeriodInOTS repexperiod;
    int onechainthermoknown;
    ArrayGrowable<ED_DynamicVariablesTwoChainFragmentThermostat> twochainthermolist;
    int pressureknown;
    unsigned velocityresamplerandomseed;
    unsigned velocityresamplerandomseedknown;
    unsigned velocityresampleperiodinots;
    unsigned velocityresampleperiodinotsknown;
    double   velocityresampletargettemperature;
    unsigned velocityresampletargettemperatureknown;
    double   pressurecontroltarget;
    unsigned pressurecontroltargetknown;
    unsigned snapshotperiodinots;
    unsigned snapshotperiodinotsknown;
    unsigned randomstateknown;
    unsigned blockrandomstate;
    unsigned repexperiodinots;
    unsigned repexperiodinotsknown;
    char bmtname[512];
    int dvsfrombmt;
    int bmttimestep;
    int bmttimestepknown;
    double bmtsimulationtime;
    int bmtsimulationtimeknown;
    int bmtstart;
    int bmtend;
    unsigned shiftv;
    unsigned shiftp;
    XYZ vshift;
    XYZ pshift;
    XYZ boxsize;
    double xmin, ymin, zmin, xmax, ymax, zmax;
    int boxknown;
    double resampleenergyloss;
    int resampleenergylossknown;
    ArrayGrowable<ED_DynamicVariablesSite> sites;
    vector<MoleculeRun> moleculeruns;
    char moleculerunstring[2048];
    int randomscatter;
    int invertv;
    int invertpv;
    // AK include definitions back from older version
    int inittemp;
    long inittempRandSeed;
    double setRandTemp;
    int dorotate;
    iXYZ rotation;
    // AK end declaration
    
    int zeromomentum;
    int zeroproteinmomentum;
    int zerowatermomentum;
    int zerononwatermomentum;
    int zeroprotmemwatmomentum;
    int sourceid;
    int systemid;
    double dither;
    double dithermolecule;
    int needdbase;
    int havesysinfo;
    char dbasename[512];
    int mapintobox;
    double *masses;

public:
    ED_InformationRTP rtp;
    int info;
    DataReceiverDVSParser()
    {
  dvsfrombmt = 0;
  bmtend = 1;
  bmtstart = 0;
  bmttimestepknown = 0;
  bmtsimulationtimeknown = 0;
        numberofoutertimesteps = 0;
        numberofoutertimestepsknown = 0;
        currentstep = 0;
        currentstepknown = 0;
        rtpknown = 0;
        pressureknown = 0;
        velocityresamplerandomseedknown = 0;
        velocityresampleperiodinots = 0;
        velocityresampleperiodinotsknown = 0;
        velocityresampletargettemperature = 0;
        velocityresampletargettemperatureknown = 0;
  randomstateknown = 0;
  onechainthermoknown = 0;
  blockrandomstate = 0;
  pressurecontroltarget = 0;
  pressurecontroltargetknown = 0;
        snapshotperiodinots = 0;
        snapshotperiodinotsknown = 0;
        resampleenergyloss = 0;
        resampleenergylossknown = 0;
  repexperiodinots = 0;
  repexperiodinotsknown = 0;
        boxknown = 0;
        // waterstart = 0;
        randomscatter = 0;
        invertv = 0;
        invertpv = 0;
        // AK - initialize (variables included)
  dorotate=0;
  inittemp=0;
  inittempRandSeed=0;
  setRandTemp=0.0;
        // AK end 
        zeromomentum = 0;
  zeroproteinmomentum = 0;
  zerowatermomentum = 0;
  zerononwatermomentum = 0;
  zeroprotmemwatmomentum = 0;
        sourceid = 0;
        systemid = 0;
        dither = 0;
        dithermolecule = 0;
        needdbase = 0;
  havesysinfo = 0;
        strcpy(dbasename, "mdsetup");
        mapintobox = 0;
  masses = NULL;

        pressure.mFullOuterTimeStep = 0;
        pressure.mVirial = XYZ::ZERO_VALUE();
        pressure.mVolumeVelocity = XYZ::ZERO_VALUE();
        pressure.mVolumeAcceleration = XYZ::ZERO_VALUE();
        pressure.mInternalPressure = XYZ::ZERO_VALUE();

        vshift = XYZ::ZERO_VALUE();
        pshift = XYZ::ZERO_VALUE();

        rtp.mCoulombMethod = 0;
        rtp.mInnerTimeStepInPicoSeconds = 0;
        rtp.mNumberOfInnerTimeSteps = 0;
        rtp.mOuterTimeStepInPicoSeconds = 0;
        rtp.mNumberOfOuterTimeSteps = 0;
        rtp.mNumberOfRESPALevels = 0;
        rtp.mPressureControlPistonMass = 0;
        rtp.mPressureControlTarget = 0;
        rtp.mVelocityResampleTargetTemperature = 0;
        rtp.mVelocityResamplePeriodInOTS = 0;
        rtp.mEnsemble = 0;
        rtp.mSnapshotPeriodInOTS = 0;
        rtp.mVelocityResampleRandomSeed = 0;
        rtp.mSourceID = 0;
        rtp.mExecutableID = 0;
        rtp.mNumberOfTwoChainThermostats = 0;
        rtp.mdummy3 = 0;
        rtp.mDynamicBoundingBoxMin = XYZ::ZERO_VALUE();
        rtp.mDynamicBoundingBoxMax = XYZ::ZERO_VALUE();
  info = 0;
    }


    //  All these setXXX guys assume the command line settings have happened first, and any duplicate setting
    //  was caused by values read from the snapshot file

    //  1)  Set all terms to 0 in rtp and in local variables
    //  2)  Set all known states to unknown
    //  3)  Parse command options and assign local variables accordingly
    //  4)  Load dvs and assign sites, rtp, energyloss, etc.
    //  5)  In final() call, overwrite rtp values by values set on command line

    void setInformationRTP(ED_InformationRTP &artp)
    {
        if (!rtpknown) {
            // this stomps on existing rtp settings
            // but stores final settings in local variables
            // in final(), the final rtp values are re-inserted
            rtp = artp;
            if (rtp.mDynamicBoundingBoxMin != XYZ::ZERO_VALUE() || rtp.mDynamicBoundingBoxMax != XYZ::ZERO_VALUE())
                setBoundingBox(rtp.mDynamicBoundingBoxMin.mX, rtp.mDynamicBoundingBoxMin.mY, rtp.mDynamicBoundingBoxMin.mZ,
                               rtp.mDynamicBoundingBoxMax.mX, rtp.mDynamicBoundingBoxMax.mY, rtp.mDynamicBoundingBoxMax.mZ);
            if (rtp.mVelocityResampleRandomSeed)
                setVelocityResampleRandomSeed(rtp.mVelocityResampleRandomSeed);
            if (rtp.mVelocityResamplePeriodInOTS)
                setVelocityResamplePeriod(rtp.mVelocityResamplePeriodInOTS);
            if (rtp.mVelocityResampleTargetTemperature)
                setTemp(rtp.mVelocityResampleTargetTemperature);
            if (rtp.mSnapshotPeriodInOTS)
                setSnapshotPeriod(rtp.mSnapshotPeriodInOTS);
            if (rtp.mNumberOfOuterTimeSteps)
                setNSteps(rtp.mNumberOfOuterTimeSteps);
            if (systemid && rtp.mSourceID)
                cerr << "System ID specified as " << systemid << " and nonzero source id " << rtp.mSourceID << " in rtp.  Using SystemID." << endl;
            rtpknown = 1;
        } else {
            cerr << "Duplicate rtp packet ignored." << endl;
        }
    }

    // void setControlReplicaExchangePeriodInOTS(ED_ControlReplicaExchangePeriodInOTS &arepex) {
      // FIXME
    // }


    void setBMTName(char *name) {
  strcpy(bmtname, name);
  dvsfrombmt = 1;
    }

    void setBMTTimeStep(int n) {
  bmttimestep = n;
  bmttimestepknown = 1;
    }

    void setBMTSimulationTime(double t) {
  bmtsimulationtime = t;
  bmttimestepknown = 1;
    }

    void setBMTStart() {
  bmtstart = 1;
  bmtend = 0;
    }

    void setBMTEnd() {
  bmtend = 1;
  bmtstart = 0;
    }

    void setRandomScatter(int n) {
        randomscatter = n;
        needdbase = 1;
    }

    void setZeroMomentum()
    {
        zeromomentum = 1;
        needdbase = 1;
    }

    void setZeroProteinMomentum()
    {
        zeroproteinmomentum = 1;
        needdbase = 1;
    }

    void setZeroWaterMomentum()
    {
        zerowatermomentum = 1;
        needdbase = 1;
    }

    void setZeroNonWaterMomentum()
    {
        zerononwatermomentum = 1;
        needdbase = 1;
    }

    void setZeroProtMemWatMomentum()
    {
        zeroprotmemwatmomentum = 1;
        needdbase = 1;
    }

    void setVShift(XYZ v)
    {
        vshift = v;
        shiftv = 1;
    }

    void setPShift(XYZ v)
    {
        pshift = v;
        shiftp = 1;
    }
    
    void setInvertV()
    {
        invertv = 1;
    }

    void setInvertPV()
    {
        invertpv = 1;
    }

 
// AK Include back methods from old version of the code

    void setInitTemp(double temp)
    {
        inittemp = 1;
        setRandTemp = temp;
        needdbase = 1;
        setZeroMomentum();
    }

    void setInitTempRandSeed(long seed)
    {
        inittempRandSeed = seed;
    }

    void setRotation(int a, int b, int c) {
        dorotate = 1;
        rotation.mX = a;
        rotation.mY = b;
        rotation.mZ = c;
    }

    void NewVelocity(XYZ &vel, double mass, double temp)
    {
         double btm = sqrt( SciConst::KBoltzmann_IU * temp / mass );
         vel.mX = btm * RanNumGenIF.GasDev();
         vel.mY = btm * RanNumGenIF.GasDev();
         vel.mZ = btm * RanNumGenIF.GasDev();
         if (dorotate)
           vel.ReOrder(rotation);
    }

// AK End of inclusion of methods


   void setDither(double d)
    {
        dither = d;
    }



    void setDitherMolecule(double d)
    {
        dithermolecule = d;
        needdbase = 1;
    }

    void setDBaseName(char *s)
    {
        strcpy(dbasename, s);
    }

    void setMapIntoBox()
    {
        mapintobox = 1;
        needdbase = 1;
    }

    void setSystemID(int n)
    {
        systemid = n;
    }

    void setSourceID(int n)
    {
        sourceid = n;
    }

    void setControlPressure(ED_ControlPressure &apressure)
    {
        if (!pressureknown) {
            pressure = apressure;
            pressureknown = 1;
        } else {
            cerr << "Duplicate pressure packet ignored." << endl;
        }
    }

    void setControlReplicaExchangePeriodInOTS(ED_ControlReplicaExchangePeriodInOTS &arepexperiod)
    {
        if (!repexperiodinotsknown) {
            repexperiod = arepexperiod;
            repexperiodinotsknown = 1;
        } else {
            cerr << "Duplicate repexperiod packet ignored." << endl;
        }
    }

    void setBoundingBox(double x0, double y0, double z0, double x1, double y1, double z1)
    {
        if (!boxknown) {
            xmin = x0;
            ymin = y0;
            zmin = z0;
            xmax = x1;
            ymax = y1;
            zmax = z1;
            boxknown = 1;
        } else {
            cerr << "Snapshot bounding box setting ignored - using command-line value." << endl;
        }
    }

    void setVelocityResampleRandomSeed(unsigned seed)
    {
        if (!velocityresamplerandomseedknown) {
            velocityresamplerandomseed = seed;
            velocityresamplerandomseedknown = 1;
        } else {
            cerr << "Snapshot VelocityResampleRandomSeed " << seed << " ignored - using " << velocityresamplerandomseed << endl;
        }
    }

    void setVelocityResamplePeriod(unsigned period)
    {
        if (!velocityresampleperiodinotsknown) {
            velocityresampleperiodinots = period;
            velocityresampleperiodinotsknown = 1;
        } else {
            cerr << "Snapshot VelocityResamplePeriod " << period << " ignored - using " << velocityresampleperiodinots << endl;
        }
    }

    void setSnapshotPeriod(unsigned period)
    {
        if (!snapshotperiodinotsknown) {
            snapshotperiodinots = period;
            snapshotperiodinotsknown = 1;
        } else {
            cerr << "Snapshot SnapshotPeriodInOTS " << period << " ignored - using " << snapshotperiodinots << endl;
        }
    }

    void setRepExPeriod(unsigned period)
    {
        if (!repexperiodinotsknown) {
            repexperiodinots = period;
            repexperiodinotsknown = 1;
        } else {
            cerr << "Snapshot RepExPeriodInOTS " << period << " ignored - using " << repexperiodinots << endl;
        }
    }

    void setTemp(double T)
    {
        if (velocityresampletargettemperatureknown) {
            if (T != velocityresampletargettemperature) {
                cerr << "Snapshot file Temp " << T << " ignored - using " << velocityresampletargettemperature << endl;
            }
            return;
        }
        velocityresampletargettemperature = T;
        velocityresampletargettemperatureknown = 1;
    }

    void setCurrentStep(unsigned N)
    {
        if (currentstepknown) {
            if (N != currentstep) {
                cerr << "Snapshot file CurrentStep " << N << " ignored - using " << currentstep << endl;
            }
            return;
        }
        currentstep = N;
        currentstepknown = 1;
    }

    void setNSteps(unsigned N)
    {
        if (numberofoutertimestepsknown) {
            if (N != numberofoutertimesteps) {
                cerr << "dvs file NSteps " << N << " ignored - using " << numberofoutertimesteps << endl;
            }
            return;
        }
        numberofoutertimesteps = N;
        numberofoutertimestepsknown = 1;
    }

    void reset()
    {
        cerr << "resetting to zero:  temp, nsteps, currentstep, resampleperiod, pressurecontroltarget, resampletemperature, snapshotperiod, energyloss; also blocking any randomstate packets" << endl;
        numberofoutertimestepsknown = 1;
        currentstepknown = 1;
        velocityresampleperiodinotsknown = 1;
        velocityresampletargettemperatureknown = 1;
        snapshotperiodinotsknown = 1;
        resampleenergylossknown = 1;
  pressurecontroltargetknown = 1;
  blockrandomstate = 1;
    }

    void resetAll()
    {
        reset();
        xmin = 0;
        ymin = 0;
        zmin = 0;
        xmax = 0;
        ymax = 0;
        zmax = 0;
        cerr << "resetting to zero:  bounding box" << endl;
        boxknown = 1;
    }

    virtual void velocityResampleTargetTemperature(ED_RTPValuesVelocityResampleTargetTemperature &t)
    {
        setTemp(t.mValue);
    }

    virtual void velocityResampleRandomSeed(ED_RTPValuesVelocityResampleRandomSeed &t)
    {
    }

    virtual void numberOfOuterTimeSteps(ED_RTPValuesNumberOfOuterTimeSteps &t)
    {
        setNSteps(t.mValue);
    }

    virtual void snapshotPeriodInOTS(ED_RTPValuesSnapshotPeriodInOTS &t)
    {
    }

    virtual void informationRTP(ED_InformationRTP &rtp)
    {
  cerr << "From DVS have: " << endl
       << "OuterTimestep " << rtp.mOuterTimeStepInPicoSeconds << endl
       << "NumberOfOuterTimeSteps " << rtp.mNumberOfOuterTimeSteps << endl
       << "PistonMass " << rtp.mPressureControlPistonMass << endl
       << "PressureTarget " << rtp.mPressureControlTarget << endl
       << "TargetTemp " << rtp.mVelocityResampleTargetTemperature << endl
       << "ResamplePeriod " << rtp.mVelocityResamplePeriodInOTS << endl
       << "SnapshotPeriod " << rtp.mSnapshotPeriodInOTS << endl
       << "SourceID " << rtp.mSourceID << endl
       << "ExecutableID " << rtp.mExecutableID << endl
       << "Num2ChainThermos " << rtp.mNumberOfTwoChainThermostats << endl
       << "BoundingMin " << rtp.mDynamicBoundingBoxMin << endl
       << "BoundingMax " << rtp.mDynamicBoundingBoxMax << endl << endl;

        setInformationRTP(rtp);
    }

    virtual void controlPressure(ED_ControlPressure &pressure)
    {
        setControlPressure(pressure);
    }

    virtual void init()
    {
    }

    void GetShiftVector(XYZ &v, XYZ &box, int aNonZero = 0)
    {
        int i = int((dvsrand()*2 - 1)*randomscatter);
        int j = int((dvsrand()*2 - 1)*randomscatter);
        int k = int((dvsrand()*2 - 1)*randomscatter);

        while (aNonZero && (i == 0 || j == 0 || k == 0)) {
            i = int((dvsrand()*2 - 1)*randomscatter);
            j = int((dvsrand()*2 - 1)*randomscatter);
            k = int((dvsrand()*2 - 1)*randomscatter);
        }

        // cerr << i << " " << j << " " << k << endl;
        v.mX = i * box.mX;
        v.mY = j * box.mY;
        v.mZ = k * box.mZ; 
        // cerr << v.mX << endl;
    }

    void ExplodeFragments()
    {
        XYZ v;
  int first = 1;
  int nsite = 0;
  for (vector<MoleculeRun>::iterator r = moleculeruns.begin(); r != moleculeruns.end(); r++) {
      for (int i=0; i<r->m_Count; i++) {
    GetShiftVector(v, boxsize, first);
    first = 0;
    for (int j=0; j<r->m_Size; j++) {
        sites[nsite++].mPosition += v;
    }
      }
  }
    }

    void mapIntoLine(double x, double &v, double a, double l)
    {
  double dx = x-a;
  int k = fabs(dx/l);
  v = k*l;
  if (dx < 0)
      v = -v - l;
    }

    void mapIntoBox(XYZ &com, XYZ &delta, XYZ &boxmin, XYZ &size)
    {
  // cerr << "in: " << com << " ";
  mapIntoLine(com.mX, delta.mX, boxmin.mX, size.mX);
  mapIntoLine(com.mY, delta.mY, boxmin.mY, size.mY);
  mapIntoLine(com.mZ, delta.mZ, boxmin.mZ, size.mZ);
  // cerr << delta << endl;
    }


    virtual void newSitePacket(ED_DynamicVariablesSite &dvs)
    {
        sites.set(dvs.mSiteId, dvs);
    }

    virtual void newUDFd1(ED_UDFd1 &udf)
    {
  if (udf.mCode == UDF_Binding::ResampleEnergyLoss_Code) {
      if (resampleenergylossknown) {
    cerr << "ResampleEnergyLoss " << udf.mValue << " ignored - using " << resampleenergyloss << endl;
    return;
      }
      resampleenergyloss = udf.mValue;
      resampleenergylossknown = 1;
  }
    }

    virtual void newDynamicVariablesRandomState(ED_DynamicVariablesRandomState &rs)
    {
  if (blockrandomstate) {
      cerr << "RandomState in dvs ignored.  Not passed on to restart file" << endl;
      return;
  }
  memcpy((char *)&randomstate, (char *)&rs, sizeof(ED_DynamicVariablesRandomState));
  randomstateknown = 1;
    }

    virtual void newOneChainThermostatPacket(ED_DynamicVariablesOneChainThermostat &oct)
    {
  memcpy((char *)&onechainthermo, (char *)&oct, sizeof(ED_DynamicVariablesOneChainThermostat));
  onechainthermoknown = 1;
    }

    virtual void newTwoChainFragmentThermostatPacket(ED_DynamicVariablesTwoChainFragmentThermostat &tct)
    {
  twochainthermolist.set(tct.mFragmentNumber, tct);
    }

    virtual void loadBMT()
    {
  BMTReader bmtr(bmtname);
  if (bmttimestepknown) {
      if (bmtr.ReadFrameByTimeStep(bmttimestep)) {
    cerr << "Error reading bmt " << bmtname << " at timestep " << bmttimestep << endl;
    exit(-1);
      }
  } else if (bmtsimulationtimeknown) {
      if (bmtr.ReadFrameBySimulationTime(bmtsimulationtime)) {
    cerr << "Error reading bmt " << bmtname << " at simulation time " << bmtsimulationtime << endl;
    exit(-1);
      }
  } else if (bmtstart) {
      if (bmtr.ReadFrame()) {
    cerr << "Error reading first frame of bmt " << bmtname << endl;
    exit(-1);
      }
  } else {
      // default will read last frame
      cerr << "Reading from final frame of bmt " << bmtname << endl;
      while (!bmtr.ReadFrame())
    ;
  }

  bmtr.getFrameAttrib("Step", &currentstep);
  double simtime;
  bmtr.getFrameAttrib("SimulationTime", &simtime);
  bmtr.closeFileHandle();
  XYZ *p = bmtr.getPositions();
  if (!p) {
      cerr << "No positions found in bmt" << endl;
      exit(-1);
  }
  if (!bmtr.getIncludeVelocities()) {
      cerr << "bmt does not contain velocities needed for restart file" << endl;
      exit(-1);
  }
  XYZ *v = bmtr.getVelocities();
  if (!v) {
      cerr << "No velocities found in bmt" << endl;
      exit(-1);
  }
  for (int i=0; i<sites.getSize(); i++, p++, v++) {
      sites[i].mPosition = *p;
      sites[i].mVelocity = *v;
  }
  cerr << "Read frame from bmt " << bmtname << " at step " << currentstep << " and simulation time " << simtime << endl;
    }


    virtual void final(int status = 1)
    {
        char buff[512];
        int i;

  if (info)
      needdbase = 1;

  if (dvsfrombmt) {
      loadBMT();
  }

  cerr << endl;
  cerr << "Values for restart:" << endl;
  cerr << "CurrentStep " << currentstep << endl;

        ED_FillPacket_ControlSyncId(
            buff,
            eInvariantMagicNumber,
            currentstep,
            eVariableMagicNumber,
            eInvariantMagicNumber,
            0
        );
  if (!info)
      fwrite(buff, ED_ControlSyncId::PackedSize, 1, stdout);

        if (numberofoutertimestepsknown)
            rtp.mNumberOfOuterTimeSteps = numberofoutertimesteps;

        if (boxknown) {
            rtp.mDynamicBoundingBoxMin.mX = xmin;
            rtp.mDynamicBoundingBoxMin.mY = ymin;
            rtp.mDynamicBoundingBoxMin.mZ = zmin;
            rtp.mDynamicBoundingBoxMax.mX = xmax;
            rtp.mDynamicBoundingBoxMax.mY = ymax;
            rtp.mDynamicBoundingBoxMax.mZ = zmax;
        }

  if (dorotate) {
      rtp.mDynamicBoundingBoxMin.ReOrderAbs(rotation);
      rtp.mDynamicBoundingBoxMax.ReOrderAbs(rotation);
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition.ReOrder(rotation);
                sites[i].mVelocity.ReOrder(rotation);
            }
  }


  // if random seed is specified on command line, block passage of the random state
        if (velocityresamplerandomseedknown) {
            rtp.mVelocityResampleRandomSeed = velocityresamplerandomseed;
      blockrandomstate = 1;
  }
        if (velocityresampleperiodinotsknown)
            rtp.mVelocityResamplePeriodInOTS = velocityresampleperiodinots;
        if (velocityresampletargettemperatureknown)
            rtp.mVelocityResampleTargetTemperature = velocityresampletargettemperature;
        if (snapshotperiodinotsknown)
            rtp.mSnapshotPeriodInOTS = snapshotperiodinots;
        if (numberofoutertimestepsknown)
            rtp.mNumberOfOuterTimeSteps = numberofoutertimesteps;
  if (pressurecontroltargetknown)
      rtp.mPressureControlTarget = pressurecontroltarget;

        if (pressureknown) {
            pressure.mFullOuterTimeStep = currentstep;
      if (invertv) {
    pressure.mVolumeVelocity *= -1;
    pressure.mVolumeAcceleration *= -1;
      }
            ED_FillPacket_ControlPressure(buff, currentstep, pressure.mVirial, pressure.mVolumePosition,
                                          pressure.mVolumeVelocity, pressure.mVolumeAcceleration, pressure.mInternalPressure);
            if (!info)
    fwrite(buff, ED_ControlPressure::PackedSize, 1, stdout);
      cerr << "Virial " << pressure.mVirial << endl
     << "Volume " << pressure.mVolumePosition << endl
     << "VolVel " << pressure.mVolumeVelocity << endl
     << "IntPressure " << pressure.mInternalPressure << endl;
        }

        if (repexperiodinotsknown) {
            ED_FillPacket_ControlReplicaExchangePeriodInOTS(buff, currentstep, repexperiodinots);
            if (!info)
    fwrite(buff, ED_ControlReplicaExchangePeriodInOTS::PackedSize, 1, stdout);
      cerr << "RepExPeriodInOTS " << repexperiodinots << endl;
        }

        boxsize = rtp.mDynamicBoundingBoxMax - rtp.mDynamicBoundingBoxMin;

        // if (randomwaterbox)
        //     ExplodeFragments();

        if (shiftv || shiftp) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition += pshift;
                sites[i].mVelocity += vshift;
            }
        }

        if (invertv) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mVelocity *= -1;
            }
        }

        if (invertpv) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition *= -1;
                sites[i].mVelocity *= -1;
            }
        }

        if (dither > 0) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition.mX += dither*(dvsrand()*2 - 1);
                sites[i].mPosition.mY += dither*(dvsrand()*2 - 1);
                sites[i].mPosition.mZ += dither*(dvsrand()*2 - 1);
            }
        }

        if (needdbase) {
            // find sysid; get connectivity and mass list
            havesysinfo = getSystemInfo();
        }

        if (inittemp && havesysinfo) {
                // initialize the random number generator
                RanNumGenIF.Init();
                RandomNumberState tState = RanNumGenIF.GetRandomNumberState();
                tState.mSeed = inittempRandSeed;
                RanNumGenIF.InitState( tState );
                tState = RanNumGenIF.GetRandomNumberState();

                for (int i=0; i<sites.getSize(); i++) {
                   NewVelocity(sites[i].mVelocity, masses[i], setRandTemp);
                }

                // set up to pass on the random state to the restart file
                blockrandomstate = 0;
                randomstateknown = 1;
                // copy the state of the random number generator
                // I got this by reversing what's done in DVSFile.hpp
                // where the RandomNumberState is initialized from the
                // ED_DynamicVariablesRandomState from the dvs
                tState = RanNumGenIF.GetRandomNumberState();

                setVelocityResampleRandomSeed(tState.mSeed);
                velocityresamplerandomseedknown = 1;

                randomstate.mSeed = tState.mSeed;
                randomstate.mGaussToggle = tState.mIset;
                randomstate.mGaussStoredValue = tState.mGset;
                randomstate.mLocalInt0 = tState.mIdum2;
                randomstate.mLocalInt1 = tState.mIy;

                randomstate.mTable0  = tState.mIv[ 0 ];
                randomstate.mTable1  = tState.mIv[ 1 ];
                randomstate.mTable2  = tState.mIv[ 2 ];
                randomstate.mTable3  = tState.mIv[ 3 ];
                randomstate.mTable4  = tState.mIv[ 4 ];
                randomstate.mTable5  = tState.mIv[ 5 ];
                randomstate.mTable6  = tState.mIv[ 6 ];
                randomstate.mTable7  = tState.mIv[ 7 ];
                randomstate.mTable8  = tState.mIv[ 8 ];
                randomstate.mTable9  = tState.mIv[ 9 ];
                randomstate.mTable10 = tState.mIv[ 10 ];
                randomstate.mTable11 = tState.mIv[ 11 ];
                randomstate.mTable12 = tState.mIv[ 12 ];
                randomstate.mTable13 = tState.mIv[ 13 ];
                randomstate.mTable14 = tState.mIv[ 14 ];
                randomstate.mTable15 = tState.mIv[ 15 ];
                randomstate.mTable16 = tState.mIv[ 16 ];
                randomstate.mTable17 = tState.mIv[ 17 ];
                randomstate.mTable18 = tState.mIv[ 18 ];
                randomstate.mTable19 = tState.mIv[ 19 ];
                randomstate.mTable20 = tState.mIv[ 20 ];
                randomstate.mTable21 = tState.mIv[ 21 ];
                randomstate.mTable22 = tState.mIv[ 22 ];
                randomstate.mTable23 = tState.mIv[ 23 ];
                randomstate.mTable24 = tState.mIv[ 24 ];
                randomstate.mTable25 = tState.mIv[ 25 ];
                randomstate.mTable26 = tState.mIv[ 26 ];
                randomstate.mTable27 = tState.mIv[ 27 ];
                randomstate.mTable28 = tState.mIv[ 28 ];
                randomstate.mTable29 = tState.mIv[ 29 ];
                randomstate.mTable30 = tState.mIv[ 30 ];
                randomstate.mTable31 = tState.mIv[ 31 ];
        }

        XYZ mom = XYZ::ZERO_VALUE();
        double TotalMass = 0;
        if (havesysinfo) {
            for (int i=0; i<sites.getSize(); i++) {
                mom += masses[i]*sites[i].mVelocity;
                TotalMass += masses[i];
            }
    cerr << "Momentum " << mom << endl;
    cerr << "COM Velocity " << mom/TotalMass << endl;
    }

  if (randomscatter) {
      ExplodeFragments();
  }

  if (dithermolecule > 0) {
      int nsite = 0;
      for (vector<MoleculeRun>::iterator r = moleculeruns.begin(); r != moleculeruns.end(); r++) {
    XYZ v;
    for (int i=0; i<r->m_Count; i++) {
        v.mX = dithermolecule*(dvsrand()*2 - 1);
        v.mY = dithermolecule*(dvsrand()*2 - 1);
        v.mZ = dithermolecule*(dvsrand()*2 - 1);
        for (int j=0; j<r->m_Size; j++) {
      sites[nsite++].mPosition += v;
        }
    }
      }
  }

  if (mapintobox) {
      int mstart = 0;
      for (vector<MoleculeRun>::iterator r = moleculeruns.begin(); r != moleculeruns.end(); r++) {
    for (int i=0; i<r->m_Count; i++) {
        XYZ com = XYZ::ZERO_VALUE();
        double m = 0;
        for (int j=0; j<r->m_Size; j++) {
      com += masses[mstart+j]*sites[mstart+j].mPosition;
      m += masses[mstart+j];
        }
        com.mX /= m;
        com.mY /= m;
        com.mZ /= m;
        XYZ delta;
        mapIntoBox(com, delta, rtp.mDynamicBoundingBoxMin, boxsize);
        for (int j=0; j<r->m_Size; j++) {
      sites[mstart+j].mPosition -= delta;
        }
        mstart += r->m_Size;
    }
      }
  }

  if (zeromomentum) {
      for (int i=0; i<sites.getSize(); i++) {
    sites[i].mVelocity -= mom/TotalMass;
      }
      cerr << "Removed COM velocity " << mom/TotalMass << endl;
  }

  if (zeroproteinmomentum) {
            MoleculeRun *mr = &moleculeruns[0];
      XYZ pmom = XYZ::ZERO_VALUE();
      double pmass = 0;
      for (int i=0; i<mr->m_Size; i++) {
    pmom += sites[i].mVelocity*masses[i];
    pmass += masses[i];
      }
      XYZ pvel = pmom/pmass;
      for (int j=0; j<sites.getSize(); j++) {
    sites[j].mVelocity -= pvel;
      }
      cerr << "Removed protein velocity " << pvel << endl;
  }

  if (zerowatermomentum) {
      int nruns = moleculeruns.size();
            MoleculeRun *mr = &moleculeruns[nruns-1];
      int nwateratoms = mr->m_Count*mr->m_Size;
      int startwater = sites.getSize()-nwateratoms;
      XYZ pmom = XYZ::ZERO_VALUE();
      double pmass = 0;
      for (int i=startwater; i<sites.getSize(); i++) {
    pmom += sites[i].mVelocity*masses[i];
    pmass += masses[i];
      }
      XYZ pvel = pmom/pmass;
      for (int j=0; j<sites.getSize(); j++) {
    sites[j].mVelocity -= pvel;
      }
      cerr << "Removed water velocity " << pvel << endl;
  }

  if (zerononwatermomentum) {
      int nruns = moleculeruns.size();
            MoleculeRun *mr = &moleculeruns[nruns-1];
      int nwateratoms = mr->m_Count*mr->m_Size;
      int startwater = sites.getSize()-nwateratoms;
      XYZ pmom = XYZ::ZERO_VALUE();
      double pmass = 0;
      for (int i=0; i<startwater; i++) {
    pmom += sites[i].mVelocity*masses[i];
    pmass += masses[i];
      }
      XYZ pvel = pmom/pmass;
      for (int j=0; j<sites.getSize(); j++) {
    sites[j].mVelocity -= pvel;
      }
      cerr << "Removed nonwater velocity " << pvel << endl;
  }

  if (zeroprotmemwatmomentum) {
      int nruns = moleculeruns.size();
            MoleculeRun *mr = &moleculeruns[nruns-1];
      int nwateratoms = mr->m_Count*mr->m_Size;
      int startwater = sites.getSize()-nwateratoms;

      mr = &moleculeruns[0];
      XYZ pmom = XYZ::ZERO_VALUE();
      double pmass = 0;
      for (int i=0; i<mr->m_Size; i++) {
    pmom += sites[i].mVelocity*masses[i];
    pmass += masses[i];
      }
      XYZ pvel = pmom/pmass;
      for (int j=0; j<mr->m_Size; j++) {
    sites[j].mVelocity -= pvel;
      }
      cerr << "Removed protein velocity from protein: " << pvel << endl;

      pmom = XYZ::ZERO_VALUE();
      pmass = 0;
      for (int i=mr->m_Size; i<startwater; i++) {
    pmom += sites[i].mVelocity*masses[i];
    pmass += masses[i];
      }
      if (pmass > 0)
        pvel = pmom/pmass;
      else
        pvel = XYZ::ZERO_VALUE();
      for(int j=mr->m_Size; j<startwater; j++) {
    sites[j].mVelocity -= pvel;
      }
      cerr << "Removed membrane velocity from membrane: " << pvel << endl; 

      pmom = XYZ::ZERO_VALUE();
      pmass = 0;
      for (int i=startwater; i<sites.getSize(); i++) {
    pmom += sites[i].mVelocity*masses[i];
    pmass += masses[i];
      }
      pvel = pmom/pmass;
      for (int j=startwater; j<sites.getSize(); j++) {
    sites[j].mVelocity -= pvel;
      }
      cerr << "Removed water velocity from water: " << pvel << endl;
  }

        if (needdbase) {
            XYZ com = XYZ::ZERO_VALUE();
            XYZ newmom = XYZ::ZERO_VALUE();
      for (int i=0; i<sites.getSize(); i++) {
    newmom += masses[i]*sites[i].mVelocity;
                com += masses[i]*sites[i].mPosition;
      }
            cerr << "Final Momentum          " << newmom << endl;
            cerr << "Center of mass:         " << com/TotalMass << endl;
            cerr << "COM velocity            " << newmom/TotalMass << endl;
        }

        if (havesysinfo) {
            double ekin = 0.0;
            for (int i=0; i<sites.getSize(); i++) {
                double v2 = sites[i].mVelocity.DotProduct(sites[i].mVelocity);
                ekin += masses[i]*v2;
            }
            double assignedTemp = ekin / 
                                     (3.0*sites.getSize()*SciConst::KBoltzmann_IU);
            cerr << "Current temperature     " << assignedTemp << endl;
        }

        XYZ pmin, pmax, vmin, vmax;
        pmin = sites[0].mPosition;
        pmax = sites[0].mPosition;
        vmin = sites[0].mVelocity;
        vmax = sites[0].mVelocity;
        double maxd = -1;
        double maxv = -1;

        for (int i=1; i<sites.getSize(); i++) {
            if (pmin.mX > sites[i].mPosition.mX)
                pmin.mX = sites[i].mPosition.mX;
            if (pmax.mX < sites[i].mPosition.mX)
                pmax.mX = sites[i].mPosition.mX;
            if (pmin.mY > sites[i].mPosition.mY)
                pmin.mY = sites[i].mPosition.mY;
            if (pmax.mY < sites[i].mPosition.mY)
                pmax.mY = sites[i].mPosition.mY;
            if (pmin.mZ > sites[i].mPosition.mZ)
                pmin.mZ = sites[i].mPosition.mZ;
            if (pmax.mZ < sites[i].mPosition.mZ)
                pmax.mZ = sites[i].mPosition.mZ;

            if (vmin.mX > sites[i].mVelocity.mX)
                vmin.mX = sites[i].mVelocity.mX;
            if (vmax.mX < sites[i].mVelocity.mX)
                vmax.mX = sites[i].mVelocity.mX;
            if (vmin.mY > sites[i].mVelocity.mY)
                vmin.mY = sites[i].mVelocity.mY;
            if (vmax.mY < sites[i].mVelocity.mY)
                vmax.mY = sites[i].mVelocity.mY;
            if (vmin.mZ > sites[i].mVelocity.mZ)
                vmin.mZ = sites[i].mVelocity.mZ;
            if (vmax.mZ < sites[i].mVelocity.mZ)
                vmax.mZ = sites[i].mVelocity.mZ;
            double d = sites[i].mPosition.Length();
            if (maxd < d)
                maxd = d;
            double v = sites[i].mVelocity.Length();
            if (maxv < v)
                maxv = v;
        }

        XYZ asize = pmax - pmin;
        double l = asize.mX;
        if (l < asize.mY)
            l = asize.mY;
        if (l < asize.mZ)
            l = asize.mZ;
        double prec16 = l/65536;
        cerr << "Actual bounding box      " << pmin << " " << pmax << endl;
        cerr << "Actual bounding box size " << asize << endl;
        cerr << "Max dim., 16-bit prec.   " << l << " " << prec16 << endl;
        cerr << "Actual bounding velocity " << vmin << " " << vmax << endl;
        cerr << "Max distance, Max velocity " << maxd << " " << maxv << endl;

        //   POSITIONS AND VELOCITIES CANNOT CHANGE BELOW THIS POINT

        if (sourceid && !rtp.mSourceID)
            rtp.mSourceID = sourceid;

  rtp.mNumberOfTwoChainThermostats = twochainthermolist.getSize();
        ED_FillPacket_InformationRTP(buff,
            currentstep,
            rtp.mCoulombMethod,
            rtp.mInnerTimeStepInPicoSeconds,
            rtp.mNumberOfInnerTimeSteps,
            rtp.mOuterTimeStepInPicoSeconds,
            rtp.mNumberOfOuterTimeSteps,
            rtp.mNumberOfRESPALevels,
            rtp.mPressureControlPistonMass,
            rtp.mPressureControlTarget,
            rtp.mVelocityResampleTargetTemperature,
            rtp.mVelocityResamplePeriodInOTS,
            rtp.mEnsemble,
            rtp.mSnapshotPeriodInOTS,
            rtp.mVelocityResampleRandomSeed,
            rtp.mSourceID,
            rtp.mExecutableID,
            rtp.mNumberOfTwoChainThermostats,
            rtp.mdummy3,
            rtp.mDynamicBoundingBoxMin,
            rtp.mDynamicBoundingBoxMax
            );
        if (!info)
      fwrite(buff, ED_InformationRTP::PackedSize, 1, stdout);

  if (randomstateknown && !blockrandomstate) {
      randomstate.mHeader.mShortTimeStep = currentstep;
      //ED_FillPacket_DynamicVariablesRandomState(buff, randomstate);
        ED_FillPacket_DynamicVariablesRandomState(buff,
                                                  rtp.mNumberOfOuterTimeSteps,
                                                  randomstate.mVoxelId,
                                                  randomstate.mMachineContext,
                                                  randomstate.mMoleculeContext,
                                                  randomstate.mGaussToggle,
                                                  randomstate.mGaussStoredValue,
                                                  randomstate.mSeed,
                                                  randomstate.mLocalInt0,
                                                  randomstate.mLocalInt1,
                                                  randomstate.mTable0,
                                                  randomstate.mTable1,
                                                  randomstate.mTable2,
                                                  randomstate.mTable3,
                                                  randomstate.mTable4,
                                                  randomstate.mTable5,
                                                  randomstate.mTable6,
                                                  randomstate.mTable7,
                                                  randomstate.mTable8,
                                                  randomstate.mTable9,
                                                  randomstate.mTable10,
                                                  randomstate.mTable11,
                                                  randomstate.mTable12,
                                                  randomstate.mTable13,
                                                  randomstate.mTable14,
                                                  randomstate.mTable15,
                                                  randomstate.mTable16,
                                                  randomstate.mTable17,
                                                  randomstate.mTable18,
                                                  randomstate.mTable19,
                                                  randomstate.mTable20,
                                                  randomstate.mTable21,
                                                  randomstate.mTable22,
                                                  randomstate.mTable23,
                                                  randomstate.mTable24,
                                                  randomstate.mTable25,
                                                  randomstate.mTable26,
                                                  randomstate.mTable27,
                                                  randomstate.mTable28,
                                                  randomstate.mTable29,
                                                  randomstate.mTable30,
                                                  randomstate.mTable31
                                                 );
      if (!info)
    fwrite(buff, ED_DynamicVariablesRandomState::PackedSize, 1, stdout);
            if (inittemp)
                cerr << "New random number state output " << endl;
            else
          cerr << "Random number state being output without change " << endl;
  }

        ED_FillPacket_UDFd1(buff,
            currentstep,
            UDF_Binding::ResampleEnergyLoss_Code,
            1,
            resampleenergyloss
        );
  if (!info)
      fwrite(buff, ED_UDFd1::PackedSize, 1, stdout);
  cerr << "ResampleEnergyLoss " << resampleenergyloss << endl;

  if (onechainthermoknown) {
      onechainthermo.mHeader.mShortTimeStep = currentstep;
      ED_FillPacket_DynamicVariablesOneChainThermostat(buff, onechainthermo);
      if (!info)
    fwrite(buff, ED_DynamicVariablesOneChainThermostat::PackedSize, 1, stdout);
  }

  if (twochainthermolist.getSize() > 0 && twochainthermolist.hasGaps()) {
      cerr << "Warning!! Some of the TwoChainThermostats appear to be missing!  Aborting dvs2restart!" << endl;
      exit(-1);
  }
  for (int i=0; i< twochainthermolist.getSize(); i++) {
      twochainthermolist[i].mHeader.mShortTimeStep = currentstep;
      ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(buff, twochainthermolist[i]);
      if (!info)
    fwrite(buff, ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize, 1, stdout);
  }

        for (int i=0; i<sites.getSize(); i++) {
            ED_FillPacket_DynamicVariablesSite(
                buff,
                currentstep,
                sites[i].mVoxelId,
                sites[i].mSiteId,
                sites[i].mPosition,
                sites[i].mVelocity
            );
      if (!info)
    fwrite(buff, ED_DynamicVariablesSite::PackedSize, 1, stdout);
        }

  cerr << "OuterTimestep " << rtp.mOuterTimeStepInPicoSeconds << endl
       << "NumberOfOuterTimeSteps " << rtp.mNumberOfOuterTimeSteps << endl
       << "PistonMass " << rtp.mPressureControlPistonMass << endl
       << "PressureTarget " << rtp.mPressureControlTarget << endl
       << "TargetTemp " << rtp.mVelocityResampleTargetTemperature << endl
       << "ResamplePeriod " << rtp.mVelocityResamplePeriodInOTS << endl
       << "SnapshotPeriod " << rtp.mSnapshotPeriodInOTS << endl
       << "SourceID " << rtp.mSourceID << endl
       << "ExecutableID " << rtp.mExecutableID << endl
       << "Num2ChainThermos " << rtp.mNumberOfTwoChainThermostats << endl
       << "BoundingMin " << rtp.mDynamicBoundingBoxMin << endl
       << "BoundingMax " << rtp.mDynamicBoundingBoxMax << endl << endl;
    }

#ifdef AK_OLD_FINAL_METHOD_FROM_FRANKS_DESK
    virtual void final(int status = 1)
    {
        char buff[512];
        int i;

  if (info)
      needdbase = 1;

  cerr << endl;
  cerr << "Values for restart:" << endl;
  cerr << "CurrentStep " << currentstep << endl;

        ED_FillPacket_ControlSyncId(
            buff,
            eInvariantMagicNumber,
            currentstep,
            eVariableMagicNumber,
            eInvariantMagicNumber,
            0
        );
  if (!info)
      fwrite(buff, ED_ControlSyncId::PackedSize, 1, stdout);

        if (numberofoutertimestepsknown)
            rtp.mNumberOfOuterTimeSteps = numberofoutertimesteps;

        if (boxknown) {
            rtp.mDynamicBoundingBoxMin.mX = xmin;
            rtp.mDynamicBoundingBoxMin.mY = ymin;
            rtp.mDynamicBoundingBoxMin.mZ = zmin;
            rtp.mDynamicBoundingBoxMax.mX = xmax;
            rtp.mDynamicBoundingBoxMax.mY = ymax;
            rtp.mDynamicBoundingBoxMax.mZ = zmax;
        }

  // if random seed is specified on command line, block passage of the random state
        if (velocityresamplerandomseedknown) {
            rtp.mVelocityResampleRandomSeed = velocityresamplerandomseed;
      blockrandomstate = 1;
  }
        if (velocityresampleperiodinotsknown)
            rtp.mVelocityResamplePeriodInOTS = velocityresampleperiodinots;
        if (velocityresampletargettemperatureknown)
            rtp.mVelocityResampleTargetTemperature = velocityresampletargettemperature;
        if (snapshotperiodinotsknown)
            rtp.mSnapshotPeriodInOTS = snapshotperiodinots;
        if (numberofoutertimestepsknown)
            rtp.mNumberOfOuterTimeSteps = numberofoutertimesteps;
  if (pressurecontroltargetknown)
      rtp.mPressureControlTarget = pressurecontroltarget;

        if (pressureknown) {
            pressure.mFullOuterTimeStep = currentstep;
      if (invertv) {
    pressure.mVolumeVelocity *= -1;
    pressure.mVolumeAcceleration *= -1;
      }
            ED_FillPacket_ControlPressure(buff, currentstep, pressure.mVirial, pressure.mVolumePosition,
                                          pressure.mVolumeVelocity, pressure.mVolumeAcceleration, pressure.mInternalPressure);
            if (!info)
    fwrite(buff, ED_ControlPressure::PackedSize, 1, stdout);
      cerr << "Virial " << pressure.mVirial << endl
     << "Volume " << pressure.mVolumePosition << endl
     << "VolVel " << pressure.mVolumeVelocity << endl
     << "IntPressure " << pressure.mInternalPressure << endl;
        }

        boxsize = rtp.mDynamicBoundingBoxMax - rtp.mDynamicBoundingBoxMin;

        // if (randomwaterbox)
        //     ExplodeFragments();

        if (shiftv || shiftp) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition += pshift;
                sites[i].mVelocity += vshift;
            }
        }

        if (invertv) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mVelocity *= -1;
            }
        }

        if (invertpv) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition *= -1;
                sites[i].mVelocity *= -1;
            }
        }

        if (dither > 0) {
            for (int i=0; i<sites.getSize(); i++) {
                sites[i].mPosition.mX += dither*(dvsrand()*2 - 1);
                sites[i].mPosition.mY += dither*(dvsrand()*2 - 1);
                sites[i].mPosition.mZ += dither*(dvsrand()*2 - 1);
            }
        }

  XYZ mom = XYZ::ZERO_VALUE();
  double TotalMass = 0;
        if (needdbase) {
            // find sysid; get connectivity and mass list
            havesysinfo = getSystemInfo();
      if (havesysinfo) {
    for (int i=0; i<sites.getSize(); i++) {
        mom += masses[i]*sites[i].mVelocity;
        TotalMass += masses[i];
    }
    cerr << "Momentum " << mom << endl;
    cerr << "COM Velocity " << mom/TotalMass << endl;
      }
        }

  if (randomscatter) {
      ExplodeFragments();
  }

  if (dithermolecule > 0) {
      int nsite = 0;
      for (vector<MoleculeRun>::iterator r = moleculeruns.begin(); r != moleculeruns.end(); r++) {
    XYZ v;
    for (int i=0; i<r->m_Count; i++) {
        v.mX = dithermolecule*(dvsrand()*2 - 1);
        v.mY = dithermolecule*(dvsrand()*2 - 1);
        v.mZ = dithermolecule*(dvsrand()*2 - 1);
        for (int j=0; j<r->m_Size; j++) {
      sites[nsite++].mPosition += v;
        }
    }
      }
  }

  if (mapintobox) {
      int mstart = 0;
      for (vector<MoleculeRun>::iterator r = moleculeruns.begin(); r != moleculeruns.end(); r++) {
    for (int i=0; i<r->m_Count; i++) {
        XYZ com = XYZ::ZERO_VALUE();
        double m = 0;
        for (int j=0; j<r->m_Size; j++) {
      com += masses[mstart+j]*sites[mstart+j].mPosition;
      m += masses[mstart+j];
        }
        com.mX /= m;
        com.mY /= m;
        com.mZ /= m;
        XYZ delta;
        mapIntoBox(com, delta, rtp.mDynamicBoundingBoxMin, boxsize);
        for (int j=0; j<r->m_Size; j++) {
      sites[mstart+j].mPosition -= delta;
        }
        mstart += r->m_Size;
    }
      }
  }

  if (zeromomentum) {
      for (int i=0; i<sites.getSize(); i++) {
    sites[i].mVelocity -= mom/TotalMass;
      }
  }

        if (needdbase) {
            XYZ com = XYZ::ZERO_VALUE();
            XYZ newmom = XYZ::ZERO_VALUE();
      for (int i=0; i<sites.getSize(); i++) {
    newmom += masses[i]*sites[i].mVelocity;
                com += masses[i]*sites[i].mPosition;
      }
            cerr << "Final Momentum          " << newmom << endl;
            cerr << "Center of mass:         " << com/TotalMass << endl;
            cerr << "COM velocity            " << newmom/TotalMass << endl;
        }

        XYZ pmin, pmax, vmin, vmax;
        pmin = sites[0].mPosition;
        pmax = sites[0].mPosition;
        vmin = sites[0].mVelocity;
        vmax = sites[0].mVelocity;
        double maxd = -1;
        double maxv = -1;

        for (int i=1; i<sites.getSize(); i++) {
            if (pmin.mX > sites[i].mPosition.mX)
                pmin.mX = sites[i].mPosition.mX;
            if (pmax.mX < sites[i].mPosition.mX)
                pmax.mX = sites[i].mPosition.mX;
            if (pmin.mY > sites[i].mPosition.mY)
                pmin.mY = sites[i].mPosition.mY;
            if (pmax.mY < sites[i].mPosition.mY)
                pmax.mY = sites[i].mPosition.mY;
            if (pmin.mZ > sites[i].mPosition.mZ)
                pmin.mZ = sites[i].mPosition.mZ;
            if (pmax.mZ < sites[i].mPosition.mZ)
                pmax.mZ = sites[i].mPosition.mZ;

            if (vmin.mX > sites[i].mVelocity.mX)
                vmin.mX = sites[i].mVelocity.mX;
            if (vmax.mX < sites[i].mVelocity.mX)
                vmax.mX = sites[i].mVelocity.mX;
            if (vmin.mY > sites[i].mVelocity.mY)
                vmin.mY = sites[i].mVelocity.mY;
            if (vmax.mY < sites[i].mVelocity.mY)
                vmax.mY = sites[i].mVelocity.mY;
            if (vmin.mZ > sites[i].mVelocity.mZ)
                vmin.mZ = sites[i].mVelocity.mZ;
            if (vmax.mZ < sites[i].mVelocity.mZ)
                vmax.mZ = sites[i].mVelocity.mZ;
            double d = sites[i].mPosition.Length();
            if (maxd < d)
                maxd = d;
            double v = sites[i].mVelocity.Length();
            if (maxv < v)
                maxv = v;
        }

        XYZ asize = pmax - pmin;
        double l = asize.mX;
        if (l < asize.mY)
            l = asize.mY;
        if (l < asize.mZ)
            l = asize.mZ;
        double prec16 = l/65536;
        cerr << "Actual bounding box      " << pmin << " " << pmax << endl;
        cerr << "Actual bounding box size " << asize << endl;
        cerr << "Max dim., 16-bit prec.   " << l << " " << prec16 << endl;
        cerr << "Actual bounding velocity " << vmin << " " << vmax << endl;
        cerr << "Max distance, Max velocity " << maxd << " " << maxv << endl;

        //   POSITIONS AND VELOCITIES CANNOT CHANGE BELOW THIS POINT

        if (sourceid && !rtp.mSourceID)
            rtp.mSourceID = sourceid;

  rtp.mNumberOfTwoChainThermostats = twochainthermolist.getSize();
        ED_FillPacket_InformationRTP(buff,
            currentstep,
            rtp.mCoulombMethod,
            rtp.mInnerTimeStepInPicoSeconds,
            rtp.mNumberOfInnerTimeSteps,
            rtp.mOuterTimeStepInPicoSeconds,
            rtp.mNumberOfOuterTimeSteps,
            rtp.mNumberOfRESPALevels,
            rtp.mPressureControlPistonMass,
            rtp.mPressureControlTarget,
            rtp.mVelocityResampleTargetTemperature,
            rtp.mVelocityResamplePeriodInOTS,
            rtp.mEnsemble,
            rtp.mSnapshotPeriodInOTS,
            rtp.mVelocityResampleRandomSeed,
            rtp.mSourceID,
            rtp.mExecutableID,
            rtp.mNumberOfTwoChainThermostats,
            rtp.mdummy3,
            rtp.mDynamicBoundingBoxMin,
            rtp.mDynamicBoundingBoxMax
            );
        if (!info)
      fwrite(buff, ED_InformationRTP::PackedSize, 1, stdout);

  if (randomstateknown && !blockrandomstate) {
      randomstate.mHeader.mShortTimeStep = currentstep;
      ED_FillPacket_DynamicVariablesRandomState(buff, randomstate);
      if (!info)
    fwrite(buff, ED_DynamicVariablesRandomState::PackedSize, 1, stdout);
      cerr << "Random number state being output without change " << endl;
  }

        ED_FillPacket_UDFd1(buff,
            currentstep,
            UDF_Binding::ResampleEnergyLoss_Code,
            1,
            resampleenergyloss
        );
  if (!info)
      fwrite(buff, ED_UDFd1::PackedSize, 1, stdout);
  cerr << "ResampleEnergyLoss " << resampleenergyloss << endl;

  if (onechainthermoknown) {
      onechainthermo.mHeader.mShortTimeStep = currentstep;
      ED_FillPacket_DynamicVariablesOneChainThermostat(buff, onechainthermo);
      if (!info)
    fwrite(buff, ED_DynamicVariablesOneChainThermostat::PackedSize, 1, stdout);
  }

  if (twochainthermolist.getSize() > 0 && twochainthermolist.hasGaps()) {
      cerr << "Warning!! Some of the TwoChainThermostats appear to be missing!  Aborting dvs2restart!" << endl;
      exit(-1);
  }
  for (int i=0; i< twochainthermolist.getSize(); i++) {
      twochainthermolist[i].mHeader.mShortTimeStep = currentstep;
      ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(buff, twochainthermolist[i]);
      if (!info)
    fwrite(buff, ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize, 1, stdout);
  }

        for (int i=0; i<sites.getSize(); i++) {
            ED_FillPacket_DynamicVariablesSite(
                buff,
                currentstep,
                sites[i].mVoxelId,
                sites[i].mSiteId,
                sites[i].mPosition,
                sites[i].mVelocity
            );
      if (!info)
    fwrite(buff, ED_DynamicVariablesSite::PackedSize, 1, stdout);
        }

  cerr << "OuterTimestep " << rtp.mOuterTimeStepInPicoSeconds << endl
       << "NumberOfOuterTimeSteps " << rtp.mNumberOfOuterTimeSteps << endl
       << "PistonMass " << rtp.mPressureControlPistonMass << endl
       << "PressureTarget " << rtp.mPressureControlTarget << endl
       << "TargetTemp " << rtp.mVelocityResampleTargetTemperature << endl
       << "ResamplePeriod " << rtp.mVelocityResamplePeriodInOTS << endl
       << "SnapshotPeriod " << rtp.mSnapshotPeriodInOTS << endl
       << "SourceID " << rtp.mSourceID << endl
       << "ExecutableID " << rtp.mExecutableID << endl
       << "Num2ChainThermos " << rtp.mNumberOfTwoChainThermostats << endl
       << "BoundingMin " << rtp.mDynamicBoundingBoxMin << endl
       << "BoundingMax " << rtp.mDynamicBoundingBoxMax << endl << endl;
    }
#endif
    

// AK : getSystemInfo, adjVisit and getMoleculeStringAndList 
//	are already present in MolecularSystem class
//	Try to reuse that.

    virtual int getSystemInfo()
    {
        char buff[512];
        if (!systemid && !rtpknown) {
            cerr << "system id was not specified and rtp is not known (vanilla dvs), so cannot access database" << endl;
            cerr << "If sysid is not in dvs, you must specify it or sourceid on command line" << endl;
            return 0;
        }
        if (!sourceid)
            sourceid = rtp.mSourceID;
        cerr << "source id is " << sourceid << endl;
        cerr << "systemid is " << systemid << endl;
        cerr << "dbname is " << dbasename << endl;
        db2::DBName::instance(dbasename);
        db2::ConnHandle& conn = db2::DBName::instance()->connection();
        if (!systemid) {
            if (!sourceid) {
                cerr << "source id from the rtp packet is 0, so cannot access database" << endl;
                return 0;
            }
            sprintf(buff, "select sys_id from experiment.system_source where source_id=%d", sourceid);
            db2::StmtHandle stmt(conn);
            stmt.prepare(buff);
            SQLINTEGER id;
            stmt.bindCol(1, id);
            stmt.execute();
            stmt.fetch();
            systemid = id;
            cerr << "sysID is " << systemid << endl;
            stmt.close();
        }
        db2::BondList bList;
        int siteId = 0;
        bList.init(systemid);
        int nSites = bList.asAdjList().size();
        int *hits = new int[nSites];
        cerr << "nSites is " << nSites << endl;
        for (int i=0; i<nSites; i++)
            hits[i] = -1;
        int mol = 0;
        for (int i=0; i<nSites; i++) {
            // cerr << "checking " << i << endl;
            if (hits[i] == -1) {
                // cerr << "visiting " << i << endl;
                adjVisit(hits, bList.asAdjList(), i, mol);
                // cerr << "got " << i << " " << mol << endl;
                mol++;
            }
        }
        cerr << "NFrags: " << mol << endl;
        getMoleculeStringAndList(moleculerunstring, moleculeruns, hits, nSites);
        // cerr << list.size() << endl; 
        // for (vector<MoleculeRun>::iterator p = list.begin(); p != list.end(); p++)
        //    cerr << p->count << " " << p->size << endl;

        cerr << "Molecule connectivity " << moleculerunstring << endl;

        masses = new double[nSites];
        sprintf(buff, "select atomic_mass from mdsystem.site where sys_id=%d order by site_id", systemid);
        db2::StmtHandle stmt(conn);
        stmt.prepare(buff);
        SQLDOUBLE mass;
        stmt.bindCol(1, mass);
        stmt.execute();
        int i = 0;
        while (stmt.fetch()) {
            masses[i++] = mass;
        }
        stmt.close();

        delete [] hits;
        return 1;
    }

    inline void adjVisit(int *hits, const vector<vector<int> > &bl, int site, int id)
    {
        // cerr << "visiting " << site << " " << id << endl;
        if (hits[site] == -1) {
            // cerr << "setting " << site << endl;
            hits[site] = id;
            for (vector<int>::const_iterator iter = bl[site].begin(); iter != bl[site].end(); iter++) {
                adjVisit(hits, bl, *iter, id);
            }
        }
    }

    void getMoleculeStringAndList(char *buff, vector<MoleculeRun> &list, int *hits, int nSites)
    {
        char s[1024];
        char t[1024];
        s[0] = '\0';
        vector<int> mols;
        vector<int> molsize;
        vector<int> molcount;
        int prevmol = -1;
        int run = 0;
        for (int i=0; i < nSites; i++) {
            if (hits[i] != prevmol) {
                if (prevmol != -1)
                    mols.push_back(run);
                run = 0;
                prevmol = hits[i];
            }
            run++;
        }
        if (run > 0)
            mols.push_back(run);
        int firsttime = 1;
        int count;
        for (vector<int>::iterator i = mols.begin(); i != mols.end(); i++) {
            if (firsttime) {
                firsttime = 0;
                prevmol = *i;
                count = 1;
            } else {
                if (*i == prevmol) {
                    count++;
                } else {
                    molsize.push_back(prevmol);
                    molcount.push_back(count);
                    if (strlen(s))
                        sprintf(t, "%s+%d*%d", s, count, prevmol);
                    else
                        sprintf(t, "%d*%d", count, prevmol);
                    strcpy(s, t);
                    count = 1;
                    prevmol = *i;
                }
            }
        }
        if (strlen(s))
            sprintf(t, "%s+%d*%d", s, count, prevmol);
        else
            sprintf(t, "%d*%d", count, prevmol);
        strcpy(buff, t);
        molsize.push_back(prevmol);
        molcount.push_back(count);
        // cerr << list.size() << endl;
        for (int i=0; i<molsize.size(); i++) {
            MoleculeRun mr(molcount[i], molsize[i]);
            // p[0] = molcount[i];
            // p[1] = molsize[i];
            list.push_back(mr);
        }
        // cerr << list.size() << endl;
    }


#endif

};

