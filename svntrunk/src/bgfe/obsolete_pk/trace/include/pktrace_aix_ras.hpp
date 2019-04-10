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
 #ifndef __PKTRACE_AIXRAS_HPP__
#define __PKTRACE_AIXRAS_HPP__

// 1997/07/24 tjcw

// FoxTraceBase has no data members; it is for trace-related 'enum's
class FoxTraceBase
  {
     public:
     enum {
        Enable = 1  // Whether to build trace, usually 'yes'
        } ;
     enum {
        FoxHook = 0x010  // AIX Trace hook for 'product' trace
        } ;
     // 'Hookdata' value ranges, used to distinguish which trace point has been reached
     enum {
        HookData_PK = 0xffe0 ,  // 'PK' style of tracing
        HookData_IF = 0xfff0 ,  // 'informal' style of tracing
        HookData_FO = 0x0000    // 'formal by context' style of tracing
        } ;
  } ;

// This version of the trace classes has no data members; use it for
// (e.g.) AIX trace, where there is no context to be kept
class FoxTrace: public FoxTraceBase
  {
     public:
     inline void trace(TraceBit Mask,
                       unsigned int datum_0
                      ) ;
     inline void trace(TraceBit Mask,
                       unsigned int datum_0,
                       unsigned int datum_1
                      ) ;
     inline void trace(TraceBit Mask,
                       unsigned int datum_0,
                       unsigned int datum_1,
                       unsigned int datum_2
                      ) ;
     inline void trace(TraceBit Mask,
                       unsigned int datum_0,
                       unsigned int datum_1,
                       unsigned int datum_2,
                       unsigned int datum_3
                      ) ;
     inline void trace(TraceBit Mask,
                       unsigned int datum_0,
                       unsigned int datum_1,
                       unsigned int datum_2,
                       unsigned int datum_3,
                       unsigned int datum_4
                      ) ;
     inline void trace(TraceBit Mask,
                       unsigned int datum_0,
                       unsigned int datum_1,
                       unsigned int datum_2,
                       unsigned int datum_3,
                       unsigned int datum_4,
                       unsigned int datum_5
                      ) ;
  } ;

// Implementations to AIX trace
extern "C" {
  void utrchook(unsigned int, ...) ;
  }

inline unsigned int AIXHookWord(unsigned int HookID,
                                unsigned int TimeStamp,
                                unsigned int Length,
                                unsigned int Data_0)
{
   unsigned int FixedLength = ( Length == 0 ) ? 1 :
                              ( Length == 1 ) ? 1 :
                              ( Length == 2 ) ? 2 : 6 ;
   return ( HookID << 20 )
        | ( (TimeStamp != 0) << 19 )
        | ( FixedLength << 16 )
        | ( Data_0 & 0x0000ffff ) ;

}

inline void FoxTrace::trace(TraceBit Mask,
                               unsigned int datum_0
                              )
{
   if (Enable && Mask)
   {
      utrchook( AIXHookWord( FoxHook, 1,  0, datum_0 ) ) ;
   } /* endif */
}

inline void FoxTrace::trace(TraceBit Mask,
                               unsigned int datum_0,
                               unsigned int datum_1
                              )
{
   if (Enable && Mask)
   {
      utrchook( AIXHookWord( FoxHook, 1,  2, datum_0 ),
                datum_1                                 ) ;
   } /* endif */
}

inline void FoxTrace::trace(TraceBit Mask,
                               unsigned int datum_0,
                               unsigned int datum_1,
                               unsigned int datum_2
                              )
{
   if (Enable && Mask)
   {
      utrchook( AIXHookWord( FoxHook, 1,  3, datum_0 ),
                datum_1, datum_2,      0,      0,      0 ) ;
   } /* endif */
}

inline void FoxTrace::trace(TraceBit Mask,
                               unsigned int datum_0,
                               unsigned int datum_1,
                               unsigned int datum_2,
                               unsigned int datum_3
                              )
{
   if (Enable && Mask)
   {
      utrchook( AIXHookWord( FoxHook, 1,  4, datum_0 ),
                datum_1, datum_2, datum_3,      0,      0 ) ;
   } /* endif */
}

inline void FoxTrace::trace(TraceBit Mask,
                               unsigned int datum_0,
                               unsigned int datum_1,
                               unsigned int datum_2,
                               unsigned int datum_3,
                               unsigned int datum_4
                              )
{
   if (Enable && Mask)
   {
      utrchook( AIXHookWord( FoxHook, 1,  5, datum_0 ),
                datum_1, datum_2, datum_3, datum_4,      0 ) ;
   } /* endif */
}

inline void FoxTrace::trace(TraceBit Mask,
                               unsigned int datum_0,
                               unsigned int datum_1,
                               unsigned int datum_2,
                               unsigned int datum_3,
                               unsigned int datum_4,
                               unsigned int datum_5
                              )
{
   if (Enable && Mask)
   {
      utrchook( AIXHookWord( FoxHook, 1,  6, datum_0 ),
                datum_1, datum_2, datum_3, datum_4, datum_5 ) ;
   } /* endif */
}

// Here are the 'pktrace' mappings onto AIX trace
class __pkTraceStream: public FoxTrace
  {
     enum {
        tr_Construct = HookData_PK,
        tr_Destruct,
        tr_Trace,
        tr_TraceFileLine
        } ;
     public:
     __pkTraceStream() { trace(TraceEnabled(), tr_Construct, (int) this) ; } ;
     void __pkTraceStreamSource( int LineNo,
                                 const char * FileNameEnd,
                                 unsigned int TpClass,
                                 const char * TpContext,
                                 unsigned int TpOrdinal,
                                 const char * TpName)
       {
          TraceBit Flag = TraceEnabled() ;
          trace(Flag, tr_Trace, (int) this, TpClass, *(int *)TpContext, TpOrdinal, *(int *)TpName) ;
          trace(Flag, tr_TraceFileLine, LineNo, *(int *)(FileNameEnd-16),
                                                *(int *)(FileNameEnd-12),
                                                *(int *)(FileNameEnd-8),
                                                *(int *)(FileNameEnd-4) ) ;
       } ;
  } ;
#define PkTraceStreamDclAndSource(TraceStreamName,TraceStreamClass) \
                   PkTraceStreamSourceN(TraceStreamClass,TraceStreamName)

#define PkTraceStreamDcl  __pkTraceStream

#define PkTraceStreamSource(TraceStreamClass)  \
        __pkTraceStreamSource(__LINE__,__FILE__+sizeof(__FILE__) ,TraceStreamClass,"",-1,"")

#define PkTraceStreamSourceN(TraceStreamClass,TSName)  \
        __pkTraceStreamSource(__LINE__,__FILE__+sizeof(__FILE__),TraceStreamClass,"",-1,TSName)

#define PkTraceStreamSourceCN(TraceStreamClass,TSContext,TSName)  \
        __pkTraceStreamSource(__LINE__,__FILE__+sizeof(__FILE__),TraceStreamClass,TSContext,-1,TSName)

#define PkTraceStreamSourceCON(TraceStreamClass,TSContext,TSOrdinal,TSName)  \
        __pkTraceStreamSource(__LINE__,__FILE__+sizeof(__FILE__),TraceStreamClass,TSContext,TSOrdinal,TSName)

// Here are the mappings from 'merged' trace to AIX trace
class TraceClient: public FoxTrace
{
   enum {
     trace_Formal = HookData_FO ,
     trace_Informal = HookData_IF
   } ;
public:
   void TracePoint(TraceBit Flag, unsigned int TraceClass, int Ordinal, unsigned int EventName)
   {
      trace(Flag,HookData_FO + TraceClass,EventName,Ordinal) ;
   } ;
   void TracePoint(TraceBit Flag, unsigned int TraceClass, int Ordinal, unsigned int EventName, int Integer)
   {
      trace(Flag,HookData_FO + TraceClass,EventName,Ordinal,Integer) ;
   } ;

   void TracePointC(TraceBit Flag, const char * ContextName, int Ordinal, const char * EventName)
   {
      trace(Flag,HookData_IF,*(unsigned int *) ContextName,
                             *(((unsigned int *) ContextName)+1),
                             *(unsigned int *)EventName,Ordinal) ;
   } ;
   void TracePointC(TraceBit Flag, const char * ContextName, int Ordinal, const char * EventName, int Integer)
   {
      trace(Flag,HookData_IF,*(unsigned int *) ContextName,
                             *(((unsigned int *) ContextName)+1),
                             *(unsigned int *)EventName,Ordinal,Integer) ;
   } ;
} ;

#if defined(FORMAL_TRACE)

#define HitOE(Flag,Class,ContextName,Ordinal,EventName) \
  TracePoint(Flag,TraceClasses::Class, Ordinal, TraceNames::EventName)
#define HitOEI(Flag,Class,ContextName,Ordinal,EventName,Integer) \
  TracePoint(Flag,TraceClasses::Class, Ordinal, TraceNames::EventName,Integer)

#define TracePointCO(Flag,Name,Context,Ordinal) \
  Name.TracePoint(Flag,TraceNames::Name,TraceContexts::Context,Ordinal)
#define TracePointCOI(Flag,Name,Context,Ordinal,Integer) \
  Name.TracePoint(Flag,TraceNames::Name,TraceContexts::Context,Ordinal,Integer)

#else

#define HitOE(Flag,Class,ContextName,Ordinal,EventName) \
  TracePointC(Flag, ContextName, Ordinal, #EventName)
#define HitOEI(Flag,Class,ContextName,Ordinal,EventName,Integer) \
  TracePointC(Flag, ContextName, Ordinal, #EventName,Integer)

#define TracePointCO(Flag,Name,Context,Ordinal) \
  Name.TracePointC(Flag,#Name,#Context,Ordinal)
#define TracePointCOI(Flag,Name,Context,Ordinal,Integer) \
  Name.TracePointC(Flag,#Name,#Context,Ordinal,Integer)

#endif

#endif /* __PKTRACE_AIXRAS_HPP__ */
