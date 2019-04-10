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
 #ifndef _BL_ERRORS_H

//
// Master file containing Blue light errors.
//
// NOTE: All error codes are < 0...
//

#define BLERR_SUCCESS 0

#define BLERR_BASE      0

#define BLERR_INPROG    1                   // device is busy (not an error).
//
#define BLERR_NO_MEMORY     (BLERR_BASE-1) // Out of memory

#define BLERR_UNIMPLEMENTED (BLERR_BASE-2) // feature unimplemented
#define BLERR_FILEOPEN_ERR  (BLERR_BASE-3) // generic file open error
#define BLERR_INVALID_DATA  (BLERR_BASE-4) // generic invalid data error
#define BLERR_INTERNAL_ERR  (BLERR_BASE-5) // generic internal error.
#define BLERR_FILEREAD_ERR  (BLERR_BASE-6) // generic file read error
#define BLERR_PARAM_ERR     (BLERR_BASE-7) // generic parameter error
#define BLERR_NOT_FOUND     (BLERR_BASE-8) // whatever you were were looling for 
                                           // it was not found.
#define BLERR_STRUCT_TOO_SMALL (BLERR_BASE-9)   // structure or array passed in was too small.
#define BLERR_DUPLICATE_ENTRY  (BLERR_BASE-10)  // duplicate entry in some structure.
#define BLERR_INVALID_HANDLE   (BLERR_BASE-11)  // invalid handle.
//
#define BLERR_JTAG                      (-1000)
#define BLERR_JTAG_LICENSE_UNIQUE_NOT   (BLERR_JTAG - 1)  // License plate is not unique
#define BLERR_JTAG_IP_UNIQUE_NOT        (BLERR_JTAG - 2)  // Ip address is not unique.
#define BLERR_JTAG_INTERNAL_ERR         (BLERR_JTAG - 3)  // Internal error
#define BLERR_JTAG_SOCKET_FAIL          (BLERR_JTAG - 4)  // Failed to create socket.
#define BLERR_JTAG_BAD_IP_ADDR          (BLERR_JTAG - 5)  // Bad ip address in xxx.xxx.xxx.xxx format
#define BLERR_JTAG_BAD_LICENSE          (BLERR_JTAG - 6)  // License plate not valid
#define BLERR_JTAG_BUFFER_SHORT         (BLERR_JTAG - 7)  // buffer too short.
#define BLERR_JTAG_XMIT_BUSY            (BLERR_JTAG - 8)  // jtag manager is busy.
#define BLERR_JTAG_PAYLOAD_TOO_LARGE    (BLERR_JTAG - 9)  // Payload is too big.
#define BLERR_JTAG_PARAM_ERR            (BLERR_JTAG -10)  // Payload is too big.
#define BLERR_JTAG_PKT_ERROR            (BLERR_JTAG -11)  // packet is in error
                                                          // or not finished.
#define BLERR_JTAG_UNIMPLEMENTED        (BLERR_JTAG -12)  // function un-implemented
#define BLERR_JTAG_BLC_IPL_ERROR        (BLERR_JTAG -13)  // BLC IPL ERROR

#define BLERR_BSTRAP                    (-1100)
#define BLERR_BSTRAP_INERROR            (BLERR_BSTRAP-1)    // bootstrap completed in error.

#define BLERR_BLRUDP                    (-1200)             // errors for RUDP
#define BLERR_BLRUDP_CREATE_SOCKET      (BLERR_BLRUDP-1)    // create socket failure.
#define BLERR_BLRUDP_INVALID_ADDR       (BLERR_BLRUDP-2)    // invalid address.
#define BLERR_BLRUDP_DUP_SAP            (BLERR_BLRUDP-3)    // duplicate sap address
#define BLERR_BLRUDP_XMIT_ERR           (BLERR_BLRUDP-4)
#define BLERR_BLRUDP_CONNECT_TIMEOUT    (BLERR_BLRUDP-5)    // connection timeout.
#define BLERR_BLRUDP_INVALID_STATE      (BLERR_BLRUDP-6)    // call is not valid in this context state.
#define BLERR_BLRUDP_INVALID_OPTIONS    (BLERR_BLRUDP-7)    // invalid options....
#define BLERR_BLRUDP_SEND_BUSY          (BLERR_BLRUDP-8)    // Send buffer busy...
#define BLERR_BLRUDP_CONNECT_ABORT      (BLERR_BLRUDP-9)    // remote connection abort.
#define BLERR_BLRUDP_CONNECT_REJECT     (BLERR_BLRUDP-10)   // connection rejected...
#define BLERR_BLRUDP_NO_SEND_BUFFERS    (BLERR_BLRUDP-11)   // no send buffers available
#define BLERR_BLRUDP_DATA_TOOLONG       (BLERR_BLRUDP-12)   // data is too long...
#define BLERR_BLRUDP_NO_RECVDATA        (BLERR_BLRUDP-13)   // no more receive data...
#define BLERR_BLRUDP_INVALID_CONTEXT    (BLERR_BLRUDP-14)   // invalid context... 
#define BLERR_BLRUDP_SOCKET_RCV_ERR     (BLERR_BLRUDP-15)   // socket receive error
#define BLERR_BLRUDP_NO_PKTS            (BLERR_BLRUDP-16)   // no packets available...
#define BLERR_BLRUDP_SOCKET_ERR         (BLERR_BLRUDP-17)   // generic socket error
#define BLERR_BLRUDP_NO_CONTEXTS        (BLERR_BLRUDP-18)   // no contexts active

#endif
