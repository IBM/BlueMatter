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
 #ifndef __TRACE_CONTROL_HPP__
#define __TRACE_CONTROL_HPP__

#ifdef    BUILD_TYPE_DEV
#define BUILD_TRACE_MASK 0x00000007
#else  /* BUILD_TYPE */
#define BUILD_TRACE_MASK 0x00000001
#endif /* BUILD_TYPE */

// Default to compiling all trace 'out'. Use a -D compiler option to set
// some bits on (corresponding to the ModuleTrace enum) if you want some
// tracing compiled in.
#ifndef   MODULE_TRACE_MASK
#define MODULE_TRACE_MASK 0x00000000
#endif /* MODULE_TRACE_MASK */

// Whether to enable each kind of trace
enum {
   Product     = 0x0000001 ,  // 'product' trace
   Development = 0x0000002 ,  // 'development' trace
   TimeTrace   = 0x0000004   // 'timestamp only' trace
   } ;

// Bit-map of modules, to enable decision of whether trace is 'on' in each module
class ModuleTrace {
public:
  enum {
     ActorChannel = 0x00000001,
     SRSW_Queue = 0x00000002
     } ;
   } ;

// Unique identification for trace contexts and trace point names
class TraceClasses {
public:
  enum {
     ChannelActorDriver = 0 ,
     SRSW_Queue = 1
     } ;
   } ;

class ChannelActorDriverTraceNames {
public:
  enum {
     BeforeExecute = 0 ,
     AfterExecute = 1 ,
     BeforePushFlushWrite = 2,
     AfterPushFlushWrite = 3
     } ;
   } ;

class SRSW_QueueTraceNames {
public:
  enum {
     BeforeBlockingReserveWriteMessageBuffer = 0 ,
     AfterBlockingReserveWriteMessageBuffer = 1 ,
     BeforeBlockingPullReadMessageBuffer = 2 ,
     AfterBlockingPullReadMessageBuffer = 3 ,
     BeforeBlockingPullReadMessageBufferSet = 4 ,
     AfterBlockingPullReadMessageBufferSet = 5
     } ;
   } ;


class TraceNames: public ChannelActorDriverTraceNames,
                  public SRSW_QueueTraceNames
{ } ;

#endif /* __TRACE_CONTROL_HPP__ */
