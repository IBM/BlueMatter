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
 #ifndef __PKTRACE_CONTROL__
#define __PKTRACE_CONTROL__

// When BASIC_TRACING is defined we only get the most 
// bare bone trace points. This includes information about 
// the timestep, real space, kspace, force reduction, position globalization
// The trace points which are set to 1 will be reported when BASIC_TRACING 
// is defined. 
#ifdef BASIC_TRACING
#define TRACE_DETAIL 0
#else
#define TRACE_DETAIL 1
#endif

/**************************************************************
 * DON'T TURN THIS ONE OFF - BOBS TOOLS DEPEND ON IT
 * This reports the total time for an MD timestep
 **************************************************************/
#ifndef PKTRACE_TIMESTEP
#define PKTRACE_TIMESTEP ( 1 )
#endif
/**************************************************************/


/**************************************************************
 * These trace points deal with Replica Exchange. 
 **************************************************************/
#ifndef PKTRACE_REMD_SWAPPING_STEP
#define PKTRACE_REMD_SWAPPING_STEP ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_REMD_ALLREDUCE_ENERGIES
#define PKTRACE_REMD_ALLREDUCE_ENERGIES ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_REMD_COPY_ENERGIES
#define PKTRACE_REMD_COPY_ENERGIES ( TRACE_DETAIL )
#endif
/**************************************************************/


/**************************************************************
 *  Position Globalization Trace Points
 **************************************************************/
// Total time in the position globalization
#ifndef PKTRACE_GLOBALIZEPOSITIONS
#define PKTRACE_GLOBALIZEPOSITIONS  ( 1 )
#endif

// Total time in the neighborhood communication driver broadcast 
#ifndef PKTRACE_NEIGHBORHOOD_COMM_DRIVER_BROADCAST
#define PKTRACE_NEIGHBORHOOD_COMM_DRIVER_BROADCAST ( TRACE_DETAIL )
#endif

// Total time in the message communication layer broadcast
// This is used for V4 MPI and V5 bonded comm 
#ifndef PKTRACE_MSG_COMM_BCAST
#define PKTRACE_MSG_COMM_BCAST ( TRACE_DETAIL )
#endif

// Timing of the alltoallv used by the message communication layer broadcast
#ifndef PKTRACE_MSG_COMM_BCAST_ALLTOALLV
#define PKTRACE_MSG_COMM_BCAST_ALLTOALLV ( TRACE_DETAIL )
#endif

// Total time in the packet throbber broadcast
#ifndef PKTRACE_THROBBER_BCAST
#define PKTRACE_THROBBER_BCAST ( TRACE_DETAIL )
#endif
/**************************************************************/


/**************************************************************
 *  Force Reduction Trace Points
 **************************************************************/
// Total time in the force reduction
#ifndef PKTRACE_REDUCEFORCES
#define PKTRACE_REDUCEFORCES  ( 1 )
#endif

// Total time in the neighborhood communication driver reduce
#ifndef PKTRACE_NEIGHBORHOOD_COMM_DRIVER_REDUCE
#define PKTRACE_NEIGHBORHOOD_COMM_DRIVER_REDUCE ( TRACE_DETAIL )
#endif

// Total time in the message communication layer reduce
// This is used for V4 MPI and V5 bonded comm 
#ifndef PKTRACE_MSG_COMM_REDUCE
#define PKTRACE_MSG_COMM_REDUCE ( TRACE_DETAIL )
#endif

// Timing of the alltoallv used by the message communication layer reduce
#ifndef PKTRACE_MSG_COMM_REDUCE_ALLTOALLV
#define PKTRACE_MSG_COMM_REDUCE_ALLTOALLV ( TRACE_DETAIL )
#endif

// Total time in the packet throbber reduce
#ifndef PKTRACE_THROBBER_REDUCE
#define PKTRACE_THROBBER_REDUCE ( TRACE_DETAIL )
#endif
/**************************************************************/


/**************************************************************
 *  KSpace Trace Points in the order of execution
 **************************************************************/
#ifndef PKTRACE_KSPACE_TOTAL
#define PKTRACE_KSPACE_TOTAL ( 1 )
#endif

#ifndef PKTRACE_KSPACE_BCAST 
#define PKTRACE_KSPACE_BCAST ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_KSPACE_BUILD_FRAGMENT_LIST
#define PKTRACE_KSPACE_BUILD_FRAGMENT_LIST ( 0 )
#endif

#ifndef PKTRACE_KSPACE_ASSIGN_CHARGE
#define PKTRACE_KSPACE_ASSIGN_CHARGE ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_KSPACE_FFT_FWD
#define PKTRACE_KSPACE_FFT_FWD  ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_KSPACE_CONVOLVE
#define PKTRACE_KSPACE_CONVOLVE ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_KSPACE_REV_FFT
#define PKTRACE_KSPACE_REV_FFT ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_KSPACE_INTERPOLATE
#define PKTRACE_KSPACE_INTERPOLATE ( TRACE_DETAIL )
#endif

#ifndef PKTRACE_KSPACE_REDUCE 
#define PKTRACE_KSPACE_REDUCE ( TRACE_DETAIL )
#endif
/**************************************************************/

// These are kspace highly detailed trace points. Use with caution.
#ifndef PKTRACE_ASSIGN_FRAGMENT_CHARGE
#define PKTRACE_ASSIGN_FRAGMENT_CHARGE ( 0 )
#endif

#ifndef PKTRACE_ASSIGN_CHARGE_DETAILS
#define PKTRACE_ASSIGN_CHARGE_DETAILS ( 0 )
#endif
/**************************************************************/


/**************************************************************
 * Local Fragment Direct Driver trace point
 **************************************************************/
#ifndef PKTRACE_LOCAL_FRAGMENT_DIRECT_DRIVER
#define PKTRACE_LOCAL_FRAGMENT_DIRECT_DRIVER ( TRACE_DETAIL )
#endif
/**************************************************************/


/**************************************************************
 * Local Tuple List Driver trace point
 **************************************************************/
#ifndef PKTRACE_LOCAL_TUPLE_LIST_DRIVER
#define PKTRACE_LOCAL_TUPLE_LIST_DRIVER ( TRACE_DETAIL )
#endif
/**************************************************************/


/*************************************************************
 * Fragment Migration
 **************************************************************/
// Timing of the guard zone violation signal
// This a global reduction of an integer
#ifndef PKTRACE_GUARDZONE_VIOLATION_REDUCTION
#define PKTRACE_GUARDZONE_VIOLATION_REDUCTION  ( TRACE_DETAIL )
#endif

// Timing of the cost of fragment migration on a guard zone 
// violation
#ifndef PKTRACE_FRAGMENT_MIGRATION
#define PKTRACE_FRAGMENT_MIGRATION  ( TRACE_DETAIL )
#endif
/**************************************************************/


/*************************************************************
 * Real Space Computation trace points
 **************************************************************/
// Total time in realspace
#ifndef PKTRACE_REAL_SPACE
#define PKTRACE_REAL_SPACE ( 1 )
#endif

// Timing of verlet list generation
// This includes the timing of the assignment recook time steps
// and the verlet list recook time steps
#ifndef PKTRACE_REAL_SPACE_FAT
#define PKTRACE_REAL_SPACE_FAT ( TRACE_DETAIL )
#endif

// Timing of the portion of real space that runs verlet lists
#ifndef PKTRACE_REAL_SPACE_MEAT
#define PKTRACE_REAL_SPACE_MEAT ( 1 )
#endif
/**************************************************************/


/*************************************************************
 * Flushing of the Raw Datagram (RDG) 
 **************************************************************/
#ifndef PKTRACE_RDG_IO
#define PKTRACE_RDG_IO ( 0 )
#endif
/**************************************************************/


/**************************************************************
 * These trace points deal with Ewald 
 **************************************************************/
#ifndef PKTRACE_EWALD_KSPACE
#define PKTRACE_EWALD_KSPACE ( 0 )
#endif

#ifndef PKTRACE_EWALD_EIKS
#define PKTRACE_EWALD_EIKS ( 0 )
#endif

#ifndef PKTRACE_EWALD_LOOP
#define PKTRACE_EWALD_LOOP ( 0 )
#endif

#ifndef PKTRACE_EWALD_ALLREDUCE
#define PKTRACE_EWALD_ALLREDUCE ( 0 )
#endif
/**************************************************************/

#endif
