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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          pktrace_perfctr.hpp
 *
 * Purpose:         provide access to BG/L hardware performance counters for PK trace
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         080805 tjcw created.
 *
 *
 ***************************************************************************/

#ifndef __PKTRACE_PERFCTR_HPP__
#define __PKTRACE_PERFCTR_HPP__

extern "C" {
#include <bgl_perfctr.h>
} ;
#include <bglpersonality.h>
#include <unistd.h>

#if !defined(PKFXLOG_PERFCTR)
#define PKFXLOG_PERFCTR (0) 
#endif

#if !defined(PKFXLOG_PERFCTR_SETUP)
#define PKFXLOG_PERFCTR_SETUP (1) 
#endif

class __pktrace_perfctr
{
public: 
   static bgl_perfctr_control_t hw_state ;
   static int counters_configured ;
   static int counter_groups_configured ;
   static char * counter_names[BGL_PERFCTR_NUM_COUNTERS] ;
   static int counter_edges[BGL_PERFCTR_NUM_COUNTERS] ;
   static int counter_groups[BGL_PERFCTR_NUM_COUNTERS] ;
//   static int counter_number ;
  
// Performance counter initialisation as per bgl_perfctr_example.c


   static void __pktrace_perfctr_init() 
	{
		char * pcenv = getenv("PKTRACE_PERF_COUNTER") ;
		int configured=0 ;
		int groups=0 ;
		int counters_in_group=0 ;
		if ( pcenv != NULL )
		{
            int init_rc = bgl_perfctr_init_synch(BGL_PERFCTR_MODE_LOCAL) ;
			for(;;)
			{
				if ( *pcenv == 0 ) break ;
				BGL_PERFCTR_event_t event ;
				char * endptr0 ;
				event.num=(BGL_PERFCTR_event_num)strtol(pcenv, &endptr0, 0) ;
				if ( *endptr0 == 0 ) break ; 
				char * endptr1 ;
				event.edge=strtol(endptr0+1,&endptr1, 0) ;
		        BegLogLine(PKFXLOG_PERFCTR_SETUP) 
		          << "Setting up performace counter pcenv=" << pcenv
		          << " event=" << (int) event.num
		          << " edge=" << (int) event.edge
		          << EndLogLine ;
				int add_rc = bgl_perfctr_add_event(event) ;
				counter_names[configured] = (char *)BGL_PERFCTR_event_table[event.num].event_name ;
				counter_edges[configured] = event.edge ;
				configured += 1 ;
				counters_in_group += 1 ;
				counter_groups[groups] = counters_in_group ; 
				if ( configured >= BGL_PERFCTR_NUM_COUNTERS ) break ;
				if ( *endptr1 != '+' )
				{
					groups += 1;
					counters_in_group = 0 ;
				}
				if ( *endptr1 == 0 ) break ; 
				pcenv=endptr1+1 ;
			}
		   int commit_rc = bgl_perfctr_commit() ;
		}
		counters_configured = configured ;
		counter_groups_configured = groups ; 
//       #if defined(PKTRACE_PERF_COUNTER)
//	   counter_number = ( PKTRACE_PERF_COUNTER ) ;
//	   #if defined(PKTRACE_PERF_COUNTER_EDGE)
//	   counter_edge = (PKTRACE_PERF_COUNTER_EDGE) ;
//	   #else
//	   counter_edge = BGL_PERFCTR_UPC_EDGE_HI ;
//	   #endif
//       #else
//       char * pcenv = getenv("PKTRACE_PERF_COUNTER") ;
//	   counter_number=(NULL != pcenv) ? atoi(pcenv) : 0  ;
//	   #endif
////	   BegLogLine(PKFXLOG_PERFCTR)
////	   	<< "__pktrace_perfctr selecting counter "
////	   	<< counter_number
////	   	<< EndLogLine ;
//	   if ( counter_number >= 0 ) 
//	   {
//		   #if defined(PKTRACE_PERF_COUNTER_EDGE)
//		   int counter_edge= ( PKTRACE_PERF_COUNTER_EDGE ) ;
//		   #else
//		   char * pceenv = getenv("PKTRACE_PERF_COUNTER_EDGE") ;
//		   int counter_edge = ( NULL != pceenv ) ? atoi(pceenv) : 0 ;
//		   #endif
//		   BGL_PERFCTR_event_t event ;
//		   int init_rc = bgl_perfctr_init_synch(BGL_PERFCTR_MODE_LOCAL) ;
//		   event.num = (BGL_PERFCTR_event_num) counter_number ;
//		   event.edge = counter_edge ;
//		   int add_rc = bgl_perfctr_add_event(event) ; 
//		   int commit_rc = bgl_perfctr_commit() ;
//		   int copy_rc = bgl_perfctr_copy_state(&hw_state ,sizeof(hw_state)) ; 
////		   BegLogLine(PKFXLOG_PERFCTR) 
////		     << "Setup returns codes "
////		     << init_rc 
////		     << " " << add_rc 
////		     << " " << commit_rc 
////		     << " " << copy_rc 
////		     << EndLogLine ;
//	   }
	}
   
// Performance counter access as per bgl_perfctr_example.c
//   static long long CurrentTraceMetric(void)  
//	{
//		unsigned long long result ;
//		if  (counter_number >= 0)
//		{
//		   unsigned long long values[BGL_PERFCTR_NUM_COUNTERS];
//		   bgl_perfctr_update() ;
//		   bgl_perfctr_copy_counters(values, sizeof(values)) ;
//		   result = values[hw_state.map[0].counter_register] ;
//		} else {
//		   result = Platform::Clock::Oscillator() ;
//		}   
//		
//		BegLogLine(PKFXLOG_PERFCTR) 
//		  << "CurrentTraceMetric "
//		  << (unsigned long) (result >> 32 )
//		  << " " << (unsigned long) result
//		  << EndLogLine ;
//		return result ;
//	}  
   
}  ;

class __pktrace_perfctr_snap
{
	public: 
	unsigned long long mValues[BGL_PERFCTR_NUM_COUNTERS] ;
	__pktrace_perfctr_snap()
	{
		   int cConfigured = __pktrace_perfctr::counters_configured ;
		   if ( cConfigured > 0 ) 
		   {
		     bgl_perfctr_update() ;
		     bgl_perfctr_copy_counters(mValues, cConfigured*sizeof(unsigned long long)) ;
		   }
	} 
	unsigned long long Metric(int MetricNumber) const 
	{
		unsigned long long result=mValues[MetricNumber];
		BegLogLine(PKFXLOG_PERFCTR) 
		  << "CurrentTraceMetric MetricNumber=" << MetricNumber
		  << " " << (unsigned long) (result >> 32 )
		  << " " << (unsigned long) result
		  << EndLogLine ;
		return result ;
	}
	
} ;

#if !defined(LEAVE_SYMS_EXTERN)
bgl_perfctr_control_t __pktrace_perfctr::hw_state ;
//int __pktrace_perfctr::counter_number ;
//__pktrace_perfctr __pktrace_perfctr_dummy ;
int __pktrace_perfctr::counters_configured ;
int __pktrace_perfctr::counter_groups_configured ;
char *__pktrace_perfctr::counter_names[BGL_PERFCTR_NUM_COUNTERS] ;
int __pktrace_perfctr::counter_edges[BGL_PERFCTR_NUM_COUNTERS] ;
int __pktrace_perfctr::counter_groups[BGL_PERFCTR_NUM_COUNTERS] ;
#endif

#endif
