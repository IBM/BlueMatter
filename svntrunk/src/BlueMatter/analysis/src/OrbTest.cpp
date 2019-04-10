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
 #include <assert.h>
#include <cstdio>
#include <stdlib.h>

#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/OrbSimple.hpp>


// OrbTest msdname dvsname outstem [orbname or L W H]
int main(int argc, char* argv[])
{
    OrbRun *por = new OrbRun;
    double lwc, gz, sd;
    if (argc < 8) {
        printf("OrbTest msdname dvstxtname outstem shortname lowercutoff switchdelta guardzone [orbname] [L W H]\n");
        exit(-1);
    }
    if (argc == 9) {
        lwc = atof(argv[5]);
        sd = atof(argv[6]);
        gz = atof(argv[7]);
        por->Init(argv[1], argv[2], argv[3], argv[4], argv[8], lwc, sd, gz);
    } else if (argc == 11) {
        double l, w, h;
        lwc = atof(argv[5]);
        sd = atof(argv[6]);
        gz = atof(argv[7]);
        l = atof(argv[8]);
        w = atof(argv[9]);
        h = atof(argv[10]);
        por->Init(argv[1], argv[2], argv[3], argv[4], "", lwc, sd, gz, l, w, h);
    } else if (argc == 8) {
        double l, w, h;
        lwc = atof(argv[5]);
        sd = atof(argv[6]);
        gz = atof(argv[7]);
        l = 0;
        w = 0;
        h = 0;
        por->Init(argv[1], argv[2], argv[3], argv[4], "", lwc, sd, gz, l, w, h);
    } else {
        printf("wrong number of command args\n");
        exit(-1);
    }
    // or.Init("/fs/orb/data/light_rhod_pre_gb_forced_exclu.msd" ,"/fs/orb/data/orb_new_tab_512.out", "/fs/orb/data/light_rhod_from_feller.txt");
    por->DumpFragmentPairs();
    por->Dump();

    return 0;
}


