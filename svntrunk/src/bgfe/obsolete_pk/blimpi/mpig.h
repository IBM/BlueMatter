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
 * Project:         MPI Geometric
 *
 * Module:          mpig.h
 *
 * Purpose:         Header file for MPI Geometric (MPIG)
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Mar 14, 2003 BGF Created.
 *
 ***************************************************************************/

#ifndef __MPIG_H__
#define __MPIG_H__

#include <mpi.h>

/*
 **********************************************************************************
 * Begin all a block of code which is everything in /usr/lpp/ppe.poe/include/mpi.h
 * but with MPI changed to MPIG
 ***********************************************************************************
 */


         /* IBM_PROLOG_BEGIN_TAG                                                   */
         /* This is an automatically generated prolog.                             */
         /*                                                                        */
         /*                                                                        */
         /*                                                                        */
         /* Licensed Materials - Property of IBM                                   */
         /*                                                                        */
         /* (C) COPYRIGHT International Business Machines Corp. 1994,2000          */
         /* All Rights Reserved                                                    */
         /*                                                                        */
         /* US Government Users Restricted Rights - Use, duplication or            */
         /* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.      */
         /*                                                                        */
         /* IBM_PROLOG_END_TAG                                                     */
         #ifndef _H_MPIG
         #define _H_MPIG
         /****************************************************************************
         @(#) 1.31.1.18 src/ppe/poe/include/mpi.h, ppe.poe.mpi, ppe_rmoh, rmoht5du 00/06/12 10:08:52

          Name: mpi.h

          Description:  C/C++ header file describing the Message Passing Interface
                        implemented by the Parallel Environment

          Notes:  "#ifdef _THREAD_SAFE" is used when making MPIG-2 definitions etc.
                  that will be available in the MPIG threaded library but not the
                  signal library.
                  On AIX "_THREAD_SAFE" is set when the mpcc_r or mpCC_r script is
                  used to compile an application.
                  OS/390 supports only one libary version:
                  V2R4 - V2R6: the signal libray (i.e. "_THREAD_SAFE must not be used)
                  V2R7 and following releases: the threaded library
                      (i.e. "_THREAD_SAFE"  must always be used -- mpcc and mpCC set
                      "_THREAD_SAFE" by default.).

         ****************************************************************************/

         #define MPIG_VERSION 1
         #define MPIG_SUBVERSION 2

         #define MPIG_SUCCESS 0
         #ifdef _THREAD_SAFE
         enum {MPIG_ERR_BUFFER=50,MPIG_ERR_COUNT,MPIG_ERR_TYPE,MPIG_ERR_TAG,
               MPIG_ERR_COMM,MPIG_ERR_RANK,MPIG_ERR_REQUEST,MPIG_ERR_ROOT,MPIG_ERR_GROUP,
               MPIG_ERR_OP,MPIG_ERR_TOPOLOGY,MPIG_ERR_DIMS,MPIG_ERR_ARG,MPIG_ERR_UNKNOWN,
               MPIG_ERR_TRUNCATE,MPIG_ERR_OTHER,MPIG_ERR_INTERN,MPIG_ERR_IN_STATUS,
               MPIG_ERR_PENDING,MPIG_ERR_INFO_KEY,MPIG_ERR_INFO_VALUE,MPIG_ERR_INFO_NOKEY,
               MPIG_ERR_INFO,MPIG_ERR_FILE,MPIG_ERR_NOT_SAME,MPIG_ERR_AMODE,
               MPIG_ERR_UNSUPPORTED_DATAREP,MPIG_ERR_UNSUPPORTED_OPERATION,
               MPIG_ERR_NO_SUCH_FILE,MPIG_ERR_FILE_EXISTS,MPIG_ERR_BAD_FILE,MPIG_ERR_ACCESS,
               MPIG_ERR_NO_SPACE,MPIG_ERR_QUOTA,MPIG_ERR_READ_ONLY,MPIG_ERR_FILE_IN_USE,
               MPIG_ERR_DUP_DATAREP,MPIG_ERR_CONVERSION,MPIG_ERR_IO,MPIG_ERR_WIN,
               MPIG_ERR_BASE,MPIG_ERR_SIZE,MPIG_ERR_DISP,MPIG_ERR_LOCKTYPE,MPIG_ERR_ASSERT,
               MPIG_ERR_RMA_CONFLICT,MPIG_ERR_RMA_SYNC,MPIG_ERR_NO_MEM};
         #define MPIG_ERR_LASTCODE 500
         #else
         enum {MPIG_ERR_BUFFER=50,MPIG_ERR_COUNT,MPIG_ERR_TYPE,MPIG_ERR_TAG,
               MPIG_ERR_COMM,MPIG_ERR_RANK,MPIG_ERR_REQUEST,MPIG_ERR_ROOT,MPIG_ERR_GROUP,
               MPIG_ERR_OP,MPIG_ERR_TOPOLOGY,MPIG_ERR_DIMS,MPIG_ERR_ARG,MPIG_ERR_UNKNOWN,
               MPIG_ERR_TRUNCATE,MPIG_ERR_OTHER,MPIG_ERR_INTERN,MPIG_ERR_IN_STATUS,
               MPIG_ERR_PENDING,MPIG_ERR_NOT_SAME=74};
         #define MPIG_ERR_LASTCODE 250
         #endif
         #define MPIG_PENDING MPIG_ERR_PENDING

         #define MPIG_BOTTOM ((void*) 0)
         #define MPIG_PROC_NULL -3
         #define MPIG_ANY_SOURCE -1
         #define MPIG_ANY_TAG -1
         #define MPIG_UNDEFINED -1

         #define MPIG_SOURCE source
         #define MPIG_TAG tag
         #define MPIG_ERROR error

/*
 * Name MPE_ERRORS_WARN in following enum was not prefix with MPIG so BGF was added.
 */
         enum { MPIG_ERRORS_ARE_FATAL, MPIG_ERRORS_RETURN, BGF_MPE_ERRORS_WARN };

         #define MPIG_MAX_PROCESSOR_NAME 256
         #define MPIG_MAX_ERROR_STRING   128
         #define MPIG_BSEND_OVERHEAD      23
         #ifdef _THREAD_SAFE
         #define MPIG_MAX_FILE_NAME      1023
         #define MPIG_MAX_DATAREP_STRING 255
         #define MPIG_MAX_INFO_KEY       127
         #define MPIG_MAX_INFO_VAL       1023
         #endif

         #ifdef _THREAD_SAFE

         /* combiner values used for datatype decoding functions */
         #define MPIG_COMBINER_NAMED            0  /* a named predefined datatype    */
         #define MPIG_COMBINER_DUP              1  /* MPIG_TYPE_DUP                   */
         #define MPIG_COMBINER_CONTIGUOUS       2  /* MPIG_TYPE_CONTIGUOUS            */
         #define MPIG_COMBINER_VECTOR           3  /* MPIG_TYPE_VECTOR                */
         #define MPIG_COMBINER_HVECTOR_INTEGER  4  /* MPIG_TYPE_HVECTOR from Fortran  */
         #define MPIG_COMBINER_HVECTOR          5  /* MPIG_TYPE_HVECTOR from C and    */
                                                  /*   C++ and in some case Fortran */
                                                  /*   or MPIG_TYPE_CREATE_HVECTOR   */
         #define MPIG_COMBINER_INDEXED          6  /* MPIG_TYPE_INDEXED               */
         #define MPIG_COMBINER_HINDEXED_INTEGER 7  /* MPIG_TYPE_HINDEXED from Fortran */
         #define MPIG_COMBINER_HINDEXED         8  /* MPIG_TYPE_HINDEXED from C and   */
                                                  /*   C++ and in some case Fortran */
                                                  /*   or MPIG_TYPE_CREATE_HINDEXED  */
         #define MPIG_COMBINER_INDEXED_BLOCK    9  /* MPIG_TYPE_CREATE_INDEXED_BLOCK  */
         #define MPIG_COMBINER_STRUCT_INTEGER   10 /* MPIG_TYPE_STRUCT from Fortran   */
         #define MPIG_COMBINER_STRUCT           11 /* MPIG_TYPE_STRUCT from C and     */
                                                  /*   C++ and in some case Fortran */
                                                  /*   or MPIG_TYPE_CREATE_STRUCT    */
         #define MPIG_COMBINER_SUBARRAY         12 /* MPIG_TYPE_CREATE_SUBARRAY       */
         #define MPIG_COMBINER_DARRAY           13 /* MPIG_TYPE_CREATE_DARRAY         */
         #define MPIG_COMBINER_F90_REAL         14 /* MPIG_TYPE_CREATE_F90_REAL       */
         #define MPIG_COMBINER_F90_COMPLEX      15 /* MPIG_TYPE_CREATE_F90_COMPLEX    */
         #define MPIG_COMBINER_F90_INTEGER      16 /* MPIG_TYPE_CREATE_F90_INTEGER    */
         #define MPIG_COMBINER_RESIZED          17 /* MPIG_TYPE_CREATE_RESIZED        */

         #endif /* _THREAD_SAFE */

         /* MPIG special purpose datatypes  */
         #define MPIG_LB                 0
         #define MPIG_UB                 1
         #define MPIG_BYTE               2
         #define MPIG_PACKED             3
         /* MPIG Predefined Datatypes for C language bindings */
         #define MPIG_CHAR               4
         #define MPIG_UNSIGNED_CHAR      5
         #define MPIG_SIGNED_CHAR        6
         #define MPIG_SHORT              7
         #define MPIG_INT                8
         #define MPIG_LONG               9
         #define MPIG_UNSIGNED_SHORT     10
         #define MPIG_UNSIGNED           11
         #define MPIG_UNSIGNED_LONG      12
         #define MPIG_FLOAT              13
         #define MPIG_DOUBLE             14
         #define MPIG_LONG_DOUBLE        15

         #define MPIG_LONG_LONG_INT      39
         #define MPIG_LONG_LONG          MPIG_LONG_LONG_INT
         #define MPIG_UNSIGNED_LONG_LONG 40
         #define MPIG_WCHAR              41

         /* MPIG Predefined Datatypes for Fortran language bindings */
         #define MPIG_INTEGER1           16
         #define MPIG_INTEGER2           17
         #define MPIG_INTEGER4           18
         #define MPIG_INTEGER            MPIG_INTEGER4
         #define MPIG_REAL4              19
         #define MPIG_REAL               MPIG_REAL4
         #define MPIG_REAL8              20
         #define MPIG_DOUBLE_PRECISION   MPIG_REAL8
         #define MPIG_REAL16             21
         #define MPIG_COMPLEX8           22
         #define MPIG_COMPLEX            MPIG_COMPLEX8
         #define MPIG_COMPLEX16          23
         #define MPIG_DOUBLE_COMPLEX     MPIG_COMPLEX16
         #define MPIG_COMPLEX32          24
         #define MPIG_LOGICAL1           25
         #define MPIG_LOGICAL2           26
         #define MPIG_LOGICAL4           27
         #define MPIG_LOGICAL            MPIG_LOGICAL4
         #define MPIG_CHARACTER          28

         #define MPIG_INTEGER8           42
         #define MPIG_LOGICAL8           43

         /* MPIG Predefined datatypes for reduction functions */
                    /* C  Reduction Types  */
         #define MPIG_FLOAT_INT          29  /* {MPIG_FLOAT, MPIG_INT} */
         #define MPIG_DOUBLE_INT         30  /* {MPIG_DOUBLE, MPIG_INT} */
         #define MPIG_LONG_INT           31  /* {MPIG_LONG, MPIG_INT} */
         #define MPIG_2INT               32  /* {MPIG_INT, MPIG_INT} */
         #define MPIG_SHORT_INT          33  /* {MPIG_SHORT, MPIG_INT */
         #define MPIG_LONG_DOUBLE_INT    34  /* {MPIG_LONG_DOUBLE, MPIG_INT} */
                    /* Fortran Reduction Types */
         #define MPIG_2REAL              35  /* {MPIG_REAL, MPIG_REAL} */
         #define MPIG_2DOUBLE_PRECISION  36  /* {MPIG_DOUBLE_PRECISION, MPIG_DOUBLE_PRECISION} */
         #define MPIG_2INTEGER           37  /* {MPIG_INTEGER, MPIG_INTEGER} */
         #define MPIG_2COMPLEX           38  /* {MPIG_COMPLEX, MPIG_COMPLEX} */

         enum { MPIG_COMM_WORLD, MPIG_COMM_SELF };

         /* the order of these is important! */
         enum { MPIG_IDENT, MPIG_CONGRUENT, MPIG_SIMILAR, MPIG_UNEQUAL };

         enum { MPIG_TAG_UB, MPIG_IO, MPIG_HOST, MPIG_WTIME_IS_GLOBAL, MPIG_WIN_BASE, MPIG_WIN_SIZE,
                MPIG_WIN_DISP_UNIT };
/*
 * Name MAX_OP in following enum was not prefix with MPIG so BGF was added.
 */
         enum { MPIG_MAX, MPIG_MIN, MPIG_SUM, MPIG_PROD, MPIG_MAXLOC, MPIG_MINLOC,
                MPIG_BAND, MPIG_BOR, MPIG_BXOR, MPIG_LAND, MPIG_LOR, MPIG_LXOR, MPIG_REPLACE, BGF_MAX_OP };

         #define MPIG_HANDLE_NULL     -1
         #define MPIG_GROUP_NULL      MPIG_HANDLE_NULL
         #define MPIG_COMM_NULL       MPIG_HANDLE_NULL
         #define MPIG_DATATYPE_NULL   MPIG_HANDLE_NULL
         #define MPIG_REQUEST_NULL    MPIG_HANDLE_NULL
         #define MPIG_OP_NULL         MPIG_HANDLE_NULL
         #define MPIG_ERRHANDLER_NULL MPIG_HANDLE_NULL
         #define MPIG_KEYVAL_INVALID  MPIG_HANDLE_NULL /* this is missing from the spec */
         #ifdef _THREAD_SAFE
         #define MPIG_INFO_NULL       MPIG_HANDLE_NULL
         #define MPIG_FILE_NULL       MPIG_HANDLE_NULL
         #define MPIG_WIN_NULL        MPIG_HANDLE_NULL
         #endif

         #define MPIG_GROUP_EMPTY 0

         #ifdef _THREAD_SAFE
         #define MPIG_NON_ATOMIC              0
         #define MPIG_ATOMIC                  1

         #define MPIG_DISPLACEMENT_CURRENT    -1LL

         #define MPIG_DISTRIBUTE_NONE         0
         #define MPIG_DISTRIBUTE_BLOCK        1
         #define MPIG_DISTRIBUTE_CYCLIC       2
         #define MPIG_DISTRIBUTE_DFLT_DARG    0

         #define MPIG_ORDER_C                 1
         #define MPIG_ORDER_FORTRAN           2

         #define MPIG_SEEK_SET                0
         #define MPIG_SEEK_CUR                1
         #define MPIG_SEEK_END                2

         #define MPIG_MODE_RDONLY             0x000001
         #define MPIG_MODE_WRONLY             0x000002
         #define MPIG_MODE_RDWR               0x000004
         #define MPIG_MODE_CREATE             0x000008
         #define MPIG_MODE_APPEND             0x000010
         #define MPIG_MODE_EXCL               0x000020
         #define MPIG_MODE_DELETE_ON_CLOSE    0x000040
         #define MPIG_MODE_UNIQUE_OPEN        0x000080
         #define MPIG_MODE_SEQUENTIAL         0x000100

         #define MPIG_LOCK_EXCLUSIVE          0
         #define MPIG_LOCK_SHARED             1

         #define MPIG_MODE_NOCHECK            0x000200
         #define MPIG_MODE_NOSTORE            0x000400
         #define MPIG_MODE_NOPUT              0x000800
         #define MPIG_MODE_NOPRECEDE          0x001000
         #define MPIG_MODE_NOSUCCEED          0x002000
         #endif

         enum { MPIG_GRAPH, MPIG_CART };

         typedef long MPIG_Aint;
         typedef int MPIG_Handle;
         typedef MPIG_Handle MPIG_Group;
         typedef MPIG_Handle MPIG_Comm;
         typedef MPIG_Handle MPIG_Datatype;
         typedef MPIG_Handle MPIG_Request;
         typedef MPIG_Handle MPIG_Op;
         typedef MPIG_Handle MPIG_Errhandler;        /* this is missing from the spec */

         typedef struct {
            int source;
            int tag;
            int error;
            MPIG_Aint val1;
            int val2;
            int val3;
            int val4;
            int val5;
         } MPIG_Status;

         #ifdef _THREAD_SAFE
         typedef MPIG_Handle MPIG_Info;
         typedef MPIG_Handle MPIG_File;
         typedef MPIG_Handle MPIG_Win;
         typedef long long int MPIG_Offset;
         #endif

         #if defined(__cplusplus)
            extern "C" {
         #endif

         typedef int MPIG_Copy_function(MPIG_Comm,int,void *,void *,void *,int *);
         typedef int MPIG_Delete_function(MPIG_Comm,int,void *,void *);

         typedef void MPIG_Handler_function(MPIG_Comm *,int *,...);

         typedef void MPIG_User_function(void *invec,void *inoutvec,int *len,
                                        MPIG_Datatype *datatype);
         #ifdef _THREAD_SAFE
         typedef int MPIG_Comm_copy_attr_function(MPIG_Comm oldcomm, int comm_keyval, void *extra_state,
                       void *attribute_val_in, void *attribute_val_out, int *flag);
         typedef int MPIG_Comm_delete_attr_function(MPIG_Comm comm, int comm_keyval, void *attribute_val,
                       void *extra_state);
         typedef int MPIG_Win_copy_attr_function(MPIG_Win oldwin, int win_keyval, void *extra_state,
                       void *attribute_val_in, void *attribute_val_out, int *flag);
         typedef int MPIG_Win_delete_attr_function(MPIG_Win win, int win_keyval, void *attribute_val,
                       void *extra_state);
         typedef int MPIG_Type_copy_attr_function(MPIG_Datatype oldwin, int win_keyval, void *extra_state,
                       void *attribute_val_in, void *attribute_val_out, int *flag);
         typedef int MPIG_Type_delete_attr_function(MPIG_Datatype win, int win_keyval, void *attribute_val,
                       void *extra_state);

         typedef void MPIG_File_errhandler_fn(MPIG_File *,int *,...);
         typedef void MPIG_Win_errhandler_fn(MPIG_Win *,int *,...);
         typedef void MPIG_Comm_errhandler_fn(MPIG_Comm *,int *,...);

         typedef int MPIG_Datarep_extent_function(MPIG_Datatype,MPIG_Aint *,void *);
         typedef int MPIG_Datarep_conversion_function(void *,MPIG_Datatype,int,void *,
                                                     MPIG_Offset,void *);
         #define MPIG_CONVERSION_FN_NULL  0
         #endif

         #if defined(__cplusplus)
            }
         #endif

         #define MPIG_NULL_COPY_FN         0
         #define MPIG_NULL_DELETE_FN       0
         #define MPIG_COMM_NULL_COPY_FN    0
         #define MPIG_COMM_NULL_DELETE_FN  0
         #define MPIG_TYPE_NULL_COPY_FN    0
         #define MPIG_TYPE_NULL_DELETE_FN  0
         #define MPIG_WIN_NULL_COPY_FN     0
         #define MPIG_WIN_NULL_DELETE_FN   0
         MPIG_Copy_function       _mpi_dup_fn;
         #define MPIG_DUP_FN      _mpi_dup_fn
         #define MPIG_COMM_DUP_FN _mpi_dup_fn
         #define MPIG_TYPE_DUP_FN _mpi_dup_fn
         #define MPIG_WIN_DUP_FN  _mpi_dup_fn

         #if defined(__cplusplus)
           extern "C" {
         #endif


            /* C Bindings for Point-to-Point Communication */

         int MPIG_Send(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int MPIG_Recv(void* buf, int count, MPIG_Datatype datatype, int source, int tag,
                      MPIG_Comm comm, MPIG_Status *status);
         int MPIG_Get_count(MPIG_Status *status, MPIG_Datatype datatype, int *count);
         int MPIG_Bsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int MPIG_Ssend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int MPIG_Rsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int MPIG_Buffer_attach( void* buffer, int size);
         int MPIG_Buffer_detach( void* buffer, int* size);
         int MPIG_Isend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Ibsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Issend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Irsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Irecv(void* buf, int count, MPIG_Datatype datatype, int source, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Wait(MPIG_Request *request, MPIG_Status *status);
         int MPIG_Test(MPIG_Request *request, int *flag, MPIG_Status *status);
         int MPIG_Request_free(MPIG_Request *request);
         int MPIG_Waitany(int count, MPIG_Request *array_of_requests, int *index,
                       MPIG_Status *status);
         int MPIG_Testany(int count, MPIG_Request *array_of_requests, int *index, int *flag,
                       MPIG_Status *status);
         int MPIG_Waitall(int count, MPIG_Request *array_of_requests,
                       MPIG_Status *array_of_statuses);
         int MPIG_Testall(int count, MPIG_Request *array_of_requests, int *flag,
                       MPIG_Status *array_of_statuses);
         int MPIG_Waitsome(int incount, MPIG_Request *array_of_requests, int *outcount,
                       int *array_of_indices, MPIG_Status *array_of_statuses);
         int MPIG_Testsome(int incount, MPIG_Request *array_of_requests, int *outcount,
                       int *array_of_indices, MPIG_Status *array_of_statuses);
         int MPIG_Iprobe(int source, int tag, MPIG_Comm comm, int *flag, MPIG_Status *status);
         int MPIG_Probe(int source, int tag, MPIG_Comm comm, MPIG_Status *status);
         int MPIG_Cancel(MPIG_Request *request);
         int MPIG_Test_cancelled(MPIG_Status *status, int *flag);
         int MPIG_Send_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Bsend_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Ssend_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Rsend_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Recv_init(void* buf, int count, MPIG_Datatype datatype, int source, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int MPIG_Start(MPIG_Request *request);
         int MPIG_Startall(int count, MPIG_Request *array_of_requests);
         int MPIG_Sendrecv(void *sendbuf, int sendcount, MPIG_Datatype sendtype, int dest,
                       int sendtag, void *recvbuf, int recvcount, MPIG_Datatype recvtype,
                       int source, int recvtag, MPIG_Comm comm, MPIG_Status *status);
         int MPIG_Sendrecv_replace(void* buf, int count, MPIG_Datatype datatype, int dest,
                       int sendtag, int source, int recvtag, MPIG_Comm comm, MPIG_Status *status);
         int MPIG_Type_contiguous(int count, MPIG_Datatype oldtype, MPIG_Datatype *newtype);
         int MPIG_Type_vector(int count, int blocklength, int stride, MPIG_Datatype oldtype,
                       MPIG_Datatype *newtype);
         int MPIG_Type_hvector(int count, int blocklength, MPIG_Aint stride, MPIG_Datatype oldtype,
                       MPIG_Datatype *newtype);
         int MPIG_Type_indexed(int count, int *array_of_blocklengths, int *array_of_displacements,
                       MPIG_Datatype oldtype, MPIG_Datatype *newtype);
         int MPIG_Type_hindexed(int count, int *array_of_blocklengths, MPIG_Aint *array_of_displacements,
                       MPIG_Datatype oldtype, MPIG_Datatype *newtype);
         int MPIG_Type_struct(int count, int *array_of_blocklengths, MPIG_Aint *array_of_displacements,
                       MPIG_Datatype *array_of_types, MPIG_Datatype *newtype);

         #ifdef _THREAD_SAFE
         int MPIG_Type_get_envelope(MPIG_Datatype datatype, int *num_integers, int *num_addresses,
                       int *num_datatypes, int *combiner );
         int MPIG_Type_get_contents(MPIG_Datatype datatype, int max_integers, int max_addresses,
                       int max_datatypes, int array_of_integers[], MPIG_Aint array_of_addresses[],
                       MPIG_Datatype array_of_datatypes[] );
         int MPIG_Type_create_keyval(MPIG_Type_copy_attr_function *type_copy_attr_fn,
                       MPIG_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval,
                       void *extra_state);
         int MPIG_Type_delete_attr(MPIG_Datatype type, int type_keyval);
         int MPIG_Type_free_keyval(int *type_keyval);
         int MPIG_Type_get_attr(MPIG_Datatype type, int type_keyval, void *attribute_val, int *flag);
         int MPIG_Type_set_attr(MPIG_Datatype type, int type_keyval, void *attribute_val);
         int MPIG_Type_dup(MPIG_Datatype type, MPIG_Datatype *newtype);
         #endif

         int MPIG_Address(void* location, MPIG_Aint *address);
         int MPIG_Type_extent(MPIG_Datatype datatype, MPIG_Aint *extent);
         int MPIG_Type_size(MPIG_Datatype datatype, int *size);
         int MPIG_Type_lb(MPIG_Datatype datatype, MPIG_Aint *displacement);
         int MPIG_Type_ub(MPIG_Datatype datatype, MPIG_Aint *displacement);
         int MPIG_Type_commit(MPIG_Datatype *datatype);
         int MPIG_Type_free(MPIG_Datatype *datatype);
         int MPIG_Get_elements(MPIG_Status *status, MPIG_Datatype datatype, int *count);
         int MPIG_Pack(void* inbuf, int incount, MPIG_Datatype datatype, void *outbuf, int outsize,
                       int *position,  MPIG_Comm comm);
         int MPIG_Unpack(void* inbuf, int insize, int *position, void *outbuf, int outcount,
                       MPIG_Datatype datatype, MPIG_Comm comm);
         int MPIG_Pack_size(int incount, MPIG_Datatype datatype, MPIG_Comm comm, int *size);

             /*  C Bindings for Collective Communication   */

         int MPIG_Barrier(MPIG_Comm comm );
         int MPIG_Bcast(void* buffer, int count, MPIG_Datatype datatype, int root, MPIG_Comm comm );
         int MPIG_Gather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm) ;
         int MPIG_Gatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, int root,
                       MPIG_Comm comm);
         int MPIG_Scatter(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm);
         int MPIG_Scatterv(void* sendbuf, int *sendcounts, int *displs, MPIG_Datatype sendtype,
                       void* recvbuf, int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm);
         int MPIG_Allgather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm);
         int MPIG_Allgatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, MPIG_Comm comm);
         int MPIG_Alltoall(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm);
         int MPIG_Alltoallv(void* sendbuf, int *sendcounts, int *sdispls, MPIG_Datatype sendtype,
                       void* recvbuf, int *recvcounts, int *rdispls, MPIG_Datatype recvtype,
                       MPIG_Comm comm);
         int MPIG_Reduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, int root, MPIG_Comm comm);
         int MPIG_Op_create(MPIG_User_function *function, int commute, MPIG_Op *op);
         int MPIG_Op_free( MPIG_Op *op);
         int MPIG_Allreduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm);
         int MPIG_Reduce_scatter(void* sendbuf, void* recvbuf, int *recvcounts,
                       MPIG_Datatype datatype, MPIG_Op op, MPIG_Comm comm);
         int MPIG_Scan(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm );

             /* C  Bindings for Groups, Contexts, and Communicators  */

         int MPIG_Group_size(MPIG_Group group, int *size);
         int MPIG_Group_rank(MPIG_Group group, int *rank);
         int MPIG_Group_translate_ranks (MPIG_Group group1, int n, int *ranks1,
                       MPIG_Group group2, int *ranks2);
         int MPIG_Group_compare(MPIG_Group group1,MPIG_Group group2, int *result);
         int MPIG_Comm_group(MPIG_Comm comm, MPIG_Group *group);
         int MPIG_Group_union(MPIG_Group group1, MPIG_Group group2, MPIG_Group *newgroup);
         int MPIG_Group_intersection(MPIG_Group group1, MPIG_Group group2, MPIG_Group *newgroup);
         int MPIG_Group_difference(MPIG_Group group1, MPIG_Group group2, MPIG_Group *newgroup);
         int MPIG_Group_incl(MPIG_Group group, int n, int *ranks, MPIG_Group *newgroup);
         int MPIG_Group_excl(MPIG_Group group, int n, int *ranks, MPIG_Group *newgroup);
         int MPIG_Group_range_incl(MPIG_Group group, int n, int ranges[][3], MPIG_Group *newgroup);
         int MPIG_Group_range_excl(MPIG_Group group, int n, int ranges[][3], MPIG_Group *newgroup);
         int MPIG_Group_free(MPIG_Group *group);
         int MPIG_Comm_size(MPIG_Comm comm, int *size);
         int MPIG_Comm_rank(MPIG_Comm comm, int *rank);
         int MPIG_Comm_compare(MPIG_Comm comm1, MPIG_Comm comm2, int *result);
         int MPIG_Comm_dup(MPIG_Comm comm, MPIG_Comm *newcomm);
         int MPIG_Comm_create(MPIG_Comm comm, MPIG_Group group, MPIG_Comm *newcomm);
         int MPIG_Comm_split(MPIG_Comm comm, int color, int key, MPIG_Comm *newcomm);
         int MPIG_Comm_free(MPIG_Comm *comm);
         int MPIG_Comm_test_inter(MPIG_Comm comm, int *flag);
         int MPIG_Comm_remote_size(MPIG_Comm comm, int *size);
         int MPIG_Comm_remote_group(MPIG_Comm comm, MPIG_Group *group);
         int MPIG_Intercomm_create(MPIG_Comm local_comm, int local_leader, MPIG_Comm peer_comm,
                       int remote_leader, int tag, MPIG_Comm *newintercomm);
         int MPIG_Intercomm_merge(MPIG_Comm intercomm, int high, MPIG_Comm *newintracomm);
         int MPIG_Keyval_create(MPIG_Copy_function *copy_fn, MPIG_Delete_function *delete_fn,
                       int *keyval, void* extra_state);
         int MPIG_Keyval_free(int *keyval);
         int MPIG_Attr_put(MPIG_Comm comm, int keyval, void *attribute_val);
         int MPIG_Attr_get(MPIG_Comm comm, int keyval, void *attribute_val, int *flag);
         int MPIG_Attr_delete(MPIG_Comm comm, int keyval);
         #ifdef _THREAD_SAFE
         int MPIG_Comm_create_errhandler(MPIG_Comm_errhandler_fn *function, MPIG_Errhandler *errhandler);
         int MPIG_Comm_get_errhandler(MPIG_Comm comm, MPIG_Errhandler *errhandler);
         int MPIG_Comm_set_errhandler(MPIG_Comm comm, MPIG_Errhandler errhandler);
         int MPIG_Comm_create_keyval(MPIG_Comm_copy_attr_function *comm_copy_attr_fn,
                       MPIG_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval,
                       void *extra_state);
         int MPIG_Comm_delete_attr(MPIG_Comm comm, int comm_keyval);
         int MPIG_Comm_free_keyval(int *comm_keyval);
         int MPIG_Comm_get_attr(MPIG_Comm comm, int comm_keyval, void *attribute_val, int *flag);
         int MPIG_Comm_set_attr(MPIG_Comm comm, int comm_keyval, void *attribute_val);
         #endif


            /*   C Bindings for Process Topologies   */

         int MPIG_Cart_create(MPIG_Comm comm_old, int ndims, int *dims, int *periods,
                       int reorder, MPIG_Comm *comm_cart);
         int MPIG_Dims_create(int nnodes, int ndims, int *dims);
         int MPIG_Graph_create(MPIG_Comm comm_old, int nnodes, int *index, int *edges,
                       int reorder, MPIG_Comm *comm_graph);
         int MPIG_Topo_test(MPIG_Comm comm, int *status);
         int MPIG_Graphdims_get(MPIG_Comm comm, int *nnodes, int *nedges);
         int MPIG_Graph_get(MPIG_Comm comm, int maxindex, int maxedges, int *index, int *edges);
         int MPIG_Cartdim_get(MPIG_Comm comm, int *ndims);
         int MPIG_Cart_get(MPIG_Comm comm, int maxdims, int *dims, int *periods, int *coords);
         int MPIG_Cart_rank(MPIG_Comm comm, int *coords, int *rank);
         int MPIG_Cart_coords(MPIG_Comm comm, int rank, int maxdims, int *coords);
         int MPIG_Graph_neighbors_count(MPIG_Comm comm, int rank, int *nneighbors);
         int MPIG_Graph_neighbors(MPIG_Comm comm, int rank, int maxneighbors, int *neighbors);
         int MPIG_Cart_shift(MPIG_Comm comm, int direction, int disp, int *rank_source, int *rank_dest);
         int MPIG_Cart_sub(MPIG_Comm comm, int *remain_dims, MPIG_Comm *newcomm);
         int MPIG_Cart_map(MPIG_Comm comm, int ndims, int *dims, int *periods, int *newrank);
         int MPIG_Graph_map(MPIG_Comm comm, int nnodes, int *index, int *edges, int *newrank);


           /*    C bindings for Environmental Inquiry   */

         int MPIG_Get_version(int *version, int *subversion);
         int MPIG_Get_processor_name(char *name, int *resultlen);
         int MPIG_Errhandler_create(MPIG_Handler_function *function, MPIG_Errhandler *errhandler);
         int MPIG_Errhandler_set(MPIG_Comm comm, MPIG_Errhandler errhandler);
         int MPIG_Errhandler_get(MPIG_Comm comm, MPIG_Errhandler *errhandler);
         int MPIG_Errhandler_free(MPIG_Errhandler *errhandler);
         int MPIG_Error_string(int errorcode, char *string, int *resultlen);
         int MPIG_Error_class(int errorcode, int *errorclass);
         double MPIG_Wtime(void);
         double MPIG_Wtick(void);
         int MPIG_Init(int *argc, char ***argv);
         int MPIG_Finalize(void);
         int MPIG_Initialized(int *flag);
         int MPIG_Abort(MPIG_Comm comm, int errorcode);


           /*    C Bindings for Profiling  */

         int MPIG_Pcontrol(const int level,...);

             /*  C Bindings for Non-Blocking Collective Communication   */

         int MPE_Ibarrier(MPIG_Comm comm,MPIG_Request *request);
         int MPE_Ibcast(void* buffer, int count, MPIG_Datatype datatype, int root, MPIG_Comm comm ,MPIG_Request *request);
         int MPE_Igather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm,MPIG_Request *request) ;
         int MPE_Igatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, int root,
                       MPIG_Comm comm,MPIG_Request *request);
         int MPE_Iscatter(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Iscatterv(void* sendbuf, int *sendcounts, int *displs, MPIG_Datatype sendtype,
                       void* recvbuf, int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Iallgather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Iallgatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Ialltoall(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Ialltoallv(void* sendbuf, int *sendcounts, int *sdispls, MPIG_Datatype sendtype,
                       void* recvbuf, int *recvcounts, int *rdispls, MPIG_Datatype recvtype,
                       MPIG_Comm comm,MPIG_Request *request);
         int MPE_Ireduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, int root, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Iallreduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Ireduce_scatter(void* sendbuf, void* recvbuf, int *recvcounts,
                       MPIG_Datatype datatype, MPIG_Op op, MPIG_Comm comm,MPIG_Request *request);
         int MPE_Iscan(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm ,MPIG_Request *request);

         #ifdef _THREAD_SAFE
             /*  C Bindings for MPIG 1-sided */

         int MPIG_Win_create(void *base, MPIG_Aint size, int disp_unit,
                             MPIG_Info info, MPIG_Comm comm, MPIG_Win *win);
         int MPIG_Win_free(MPIG_Win *win);
         int MPIG_Win_get_group(MPIG_Win win, MPIG_Group *group);
         int MPIG_Put(void *origin_addr, int origin_count, MPIG_Datatype origin_datatype,
                     int target_rank, MPIG_Aint target_disp, int target_count,
                     MPIG_Datatype target_datatype, MPIG_Win win);
         int MPIG_Get(void *origin_addr, int origin_count, MPIG_Datatype origin_datatype,
                     int target_rank, MPIG_Aint target_disp, int target_count,
                     MPIG_Datatype target_datatype, MPIG_Win win);
         int MPIG_Accumulate(void *origin_addr, int origin_count, MPIG_Datatype
                            origin_datatype, int target_rank, MPIG_Aint target_disp,
                            int target_count, MPIG_Datatype target_datatype,
                            MPIG_Op op, MPIG_Win win);
         int MPIG_Win_fence(int assert, MPIG_Win win);
         int MPIG_Win_start(MPIG_Group group, int assert, MPIG_Win win);
         int MPIG_Win_complete(MPIG_Win win);
         int MPIG_Win_post(MPIG_Group group, int assert, MPIG_Win win);
         int MPIG_Win_wait(MPIG_Win win);
         int MPIG_Win_test(MPIG_Win win, int *flag);
         int MPIG_Win_lock(int lock_type, int rank, int assert, MPIG_Win win);
         int MPIG_Win_unlock(int rank, MPIG_Win win);
         int MPIG_Win_get_attr(MPIG_Win win, int win_keyval, void *attribute_val,
                                int *flag);
         int MPIG_Win_set_attr(MPIG_Win win, int win_keyval, void *attribute_val);
         int MPIG_Win_create_keyval(MPIG_Win_copy_attr_function *win_copy_attr_fn,
                       MPIG_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval,
                       void *extra_state);
         int MPIG_Win_delete_attr(MPIG_Win win, int win_keyval);
         int MPIG_Win_free_keyval(int *win_keyval);
         int MPIG_Win_create_errhandler(MPIG_Win_errhandler_fn *function, MPIG_Errhandler *errhandler);
         int MPIG_Win_set_errhandler(MPIG_Win win, MPIG_Errhandler err);
         int MPIG_Win_get_errhandler(MPIG_Win win, MPIG_Errhandler *errhandler);

         int MPIG_Alloc_mem(MPIG_Aint size, MPIG_Info info, void *baseptr);
         int MPIG_Free_mem(void *base);

             /*  C Bindings for I/O  */

         int MPIG_File_open(MPIG_Comm comm,char *filename,int amode,MPIG_Info info,
                           MPIG_File *fh);
         int MPIG_File_close(MPIG_File *fh);
         int MPIG_File_delete(char *filename,MPIG_Info info);
         int MPIG_File_set_size(MPIG_File fh,MPIG_Offset size);
         int MPIG_File_get_size(MPIG_File fh,MPIG_Offset *size);
         int MPIG_File_get_group(MPIG_File fh,MPIG_Group *group);
         int MPIG_File_get_amode(MPIG_File fh,int *amode);
         int MPIG_File_set_info(MPIG_File fh,MPIG_Info info);
         int MPIG_File_get_info(MPIG_File fh,MPIG_Info *info_used);
         int MPIG_File_set_view(MPIG_File fh,MPIG_Offset disp,MPIG_Datatype etype,
                               MPIG_Datatype filetype,char *datarep,MPIG_Info info);
         int MPIG_File_get_view(MPIG_File fh,MPIG_Offset *disp,MPIG_Datatype *etype,
                               MPIG_Datatype *filetype,char *datarep);
         int MPIG_File_read_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                              MPIG_Datatype datatype,MPIG_Status *status);
         int MPIG_File_read_at_all(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                  MPIG_Datatype datatype,MPIG_Status *status);
         int MPIG_File_write_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                               MPIG_Datatype datatype,MPIG_Status *status);
         int MPIG_File_write_at_all(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                   MPIG_Datatype datatype,MPIG_Status *status);
         int MPIG_File_iread_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                               MPIG_Datatype datatype,MPIG_Request *request);
         int MPIG_File_iwrite_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                MPIG_Datatype datatype,MPIG_Request *request);
         int MPIG_File_get_atomicity(MPIG_File fh,int *flag);
         int MPIG_File_sync(MPIG_File fh);
         int MPIG_Type_create_subarray(int ndims,int array_of_sizes[],
                                      int array_of_subsizes[],int array_of_starts[],
                                      int order,MPIG_Datatype oldtype,
                                      MPIG_Datatype *newtype);
         int MPIG_Type_create_darray(int size,int rank,int ndims,int array_of_gsizes[],
                                    int array_of_distribs[],int array_of_dargs[],
                                    int array_of_psizes[],int order,MPIG_Datatype oldtype,
                                    MPIG_Datatype *newtype);
         int MPIG_File_create_errhandler(MPIG_File_errhandler_fn *function,
                                        MPIG_Errhandler *errhandler);
         int MPIG_File_set_errhandler(MPIG_File fh,MPIG_Errhandler errhandler);
         int MPIG_File_get_errhandler(MPIG_File fh,MPIG_Errhandler *errhandler);
         int MPIG_File_create_errhandler(MPIG_File_errhandler_fn *function, MPIG_Errhandler *errhandler);
         int MPIG_File_get_byte_offset(MPIG_File fh, MPIG_Offset offset, MPIG_Offset *disp);
         int MPIG_File_get_info(MPIG_File fh, MPIG_Info *info_used);
         int MPIG_File_get_position(MPIG_File fh, MPIG_Offset *offset);
         int MPIG_File_get_position_shared(MPIG_File fh, MPIG_Offset *offset);
         int MPIG_File_get_type_extent(MPIG_File fh, MPIG_Datatype datatype, MPIG_Aint *extent);
         int MPIG_File_iread(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Request *request);
         int MPIG_File_iread_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Request *request);
         int MPIG_File_iwrite(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Request *request);
         int MPIG_File_iwrite_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Request *request);
         int MPIG_File_preallocate(MPIG_File fh, MPIG_Offset size);
         int MPIG_File_read(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Status *status);
         int MPIG_File_read_all(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Status *status);
         int MPIG_File_read_all_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int MPIG_File_read_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int MPIG_File_read_at_all_begin(MPIG_File fh, MPIG_Offset offset, void *buf, int count,
                      MPIG_Datatype datatype);
         int MPIG_File_read_at_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int MPIG_File_read_ordered(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int MPIG_File_read_ordered_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int MPIG_File_read_ordered_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int MPIG_File_read_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int MPIG_File_seek(MPIG_File fh, MPIG_Offset offset, int whence);
         int MPIG_File_seek_shared(MPIG_File fh, MPIG_Offset offset, int whence);
         int MPIG_File_set_atomicity(MPIG_File fh, int flag);
         int MPIG_File_set_info(MPIG_File fh, MPIG_Info info);
         int MPIG_File_write(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Status *status);
         int MPIG_File_write_all(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int MPIG_File_write_all_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int MPIG_File_write_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int MPIG_File_write_at_all_begin(MPIG_File fh, MPIG_Offset offset, void *buf, int count,
                      MPIG_Datatype datatype);
         int MPIG_File_write_at_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int MPIG_File_write_ordered(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int MPIG_File_write_ordered_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int MPIG_File_write_ordered_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int MPIG_File_write_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int MPIG_Register_datarep(char *datarep, MPIG_Datarep_conversion_function *read_conversion_fn,
                      MPIG_Datarep_conversion_function *write_conversion_fn,
                      MPIG_Datarep_extent_function *dtype_file_extent_fn, void *extra_state);
         int MPIG_Info_create(MPIG_Info *info);
         int MPIG_Info_set(MPIG_Info Info,char *key,char *value);
         int MPIG_Info_delete(MPIG_Info info,char *key);
         int MPIG_Info_get(MPIG_Info info,char *key,int valuelen,char *value,int *flag);
         int MPIG_Info_get_valuelen(MPIG_Info info,char *key,int *valuelen,int *flag);
         int MPIG_Info_get_nkeys(MPIG_Info info,int *nkeys);
         int MPIG_Info_get_nthkey(MPIG_Info info,int n,char *key);
         int MPIG_Info_dup(MPIG_Info info, MPIG_Info *newinfo);
         int MPIG_Info_free(MPIG_Info *info);
         #endif

         /****************************************************************************
          ****************  Profiling (PMPIG) Entry Points for C  *********************
          ****************************************************************************/

            /* C Bindings for Point-to-Point Communication */

         int PMPIG_Send(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int PMPIG_Recv(void* buf, int count, MPIG_Datatype datatype, int source, int tag,
                      MPIG_Comm comm, MPIG_Status *status);
         int PMPIG_Get_count(MPIG_Status *status, MPIG_Datatype datatype, int *count);
         int PMPIG_Bsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int PMPIG_Ssend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int PMPIG_Rsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                      MPIG_Comm comm);
         int PMPIG_Buffer_attach( void* buffer, int size);
         int PMPIG_Buffer_detach( void* buffer, int* size);
         int PMPIG_Isend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Ibsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Issend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Irsend(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Irecv(void* buf, int count, MPIG_Datatype datatype, int source, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Wait(MPIG_Request *request, MPIG_Status *status);
         int PMPIG_Test(MPIG_Request *request, int *flag, MPIG_Status *status);
         int PMPIG_Request_free(MPIG_Request *request);
         int PMPIG_Waitany(int count, MPIG_Request *array_of_requests, int *index,
                       MPIG_Status *status);
         int PMPIG_Testany(int count, MPIG_Request *array_of_requests, int *index, int *flag,
                       MPIG_Status *status);
         int PMPIG_Waitall(int count, MPIG_Request *array_of_requests,
                       MPIG_Status *array_of_statuses);
         int PMPIG_Testall(int count, MPIG_Request *array_of_requests, int *flag,
                       MPIG_Status *array_of_statuses);
         int PMPIG_Waitsome(int incount, MPIG_Request *array_of_requests, int *outcount,
                       int *array_of_indices, MPIG_Status *array_of_statuses);
         int PMPIG_Testsome(int incount, MPIG_Request *array_of_requests, int *outcount,
                       int *array_of_indices, MPIG_Status *array_of_statuses);
         int PMPIG_Iprobe(int source, int tag, MPIG_Comm comm, int *flag, MPIG_Status *status);
         int PMPIG_Probe(int source, int tag, MPIG_Comm comm, MPIG_Status *status);
         int PMPIG_Cancel(MPIG_Request *request);
         int PMPIG_Test_cancelled(MPIG_Status *status, int *flag);
         int PMPIG_Send_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Bsend_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Ssend_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Rsend_init(void* buf, int count, MPIG_Datatype datatype, int dest, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Recv_init(void* buf, int count, MPIG_Datatype datatype, int source, int tag,
                       MPIG_Comm comm, MPIG_Request *request);
         int PMPIG_Start(MPIG_Request *request);
         int PMPIG_Startall(int count, MPIG_Request *array_of_requests);
         int PMPIG_Sendrecv(void *sendbuf, int sendcount, MPIG_Datatype sendtype, int dest,
                       int sendtag, void *recvbuf, int recvcount, MPIG_Datatype recvtype,
                       int source, int recvtag, MPIG_Comm comm, MPIG_Status *status);
         int PMPIG_Sendrecv_replace(void* buf, int count, MPIG_Datatype datatype, int dest,
                       int sendtag, int source, int recvtag, MPIG_Comm comm, MPIG_Status *status);
         int PMPIG_Type_contiguous(int count, MPIG_Datatype oldtype, MPIG_Datatype *newtype);
         int PMPIG_Type_vector(int count, int blocklength, int stride, MPIG_Datatype oldtype,
                       MPIG_Datatype *newtype);
         int PMPIG_Type_hvector(int count, int blocklength, MPIG_Aint stride, MPIG_Datatype oldtype,
                       MPIG_Datatype *newtype);
         int PMPIG_Type_indexed(int count, int *array_of_blocklengths, int *array_of_displacements,
                       MPIG_Datatype oldtype, MPIG_Datatype *newtype);
         int PMPIG_Type_hindexed(int count, int *array_of_blocklengths, MPIG_Aint *array_of_displacements,
                       MPIG_Datatype oldtype, MPIG_Datatype *newtype);
         int PMPIG_Type_struct(int count, int *array_of_blocklengths, MPIG_Aint *array_of_displacements,
                       MPIG_Datatype *array_of_types, MPIG_Datatype *newtype);

         #ifdef _THREAD_SAFE
         int PMPIG_Type_get_envelope(MPIG_Datatype datatype, int *num_integers, int *num_addresses,
                      int *num_datatypes, int *combiner );
         int PMPIG_Type_get_contents(MPIG_Datatype datatype, int max_integers, int max_addresses,
                      int max_datatypes, int array_of_integers[], MPIG_Aint array_of_addresses[],
                      MPIG_Datatype array_of_datatypes[] );
         int PMPIG_Type_create_keyval(MPIG_Type_copy_attr_function *type_copy_attr_fn,
                      MPIG_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval,
                      void *extra_state);
         int PMPIG_Type_delete_attr(MPIG_Datatype type, int type_keyval);
         int PMPIG_Type_free_keyval(int *type_keyval);
         int PMPIG_Type_get_attr(MPIG_Datatype type, int type_keyval, void *attribute_val, int *flag);
         int PMPIG_Type_set_attr(MPIG_Datatype type, int type_keyval, void *attribute_val);
         int PMPIG_Type_dup(MPIG_Datatype type, MPIG_Datatype *newtype);
         #endif

         int PMPIG_Address(void* location, MPIG_Aint *address);
         int PMPIG_Type_extent(MPIG_Datatype datatype, MPIG_Aint *extent);
         int PMPIG_Type_size(MPIG_Datatype datatype, int *size);
         int PMPIG_Type_lb(MPIG_Datatype datatype, MPIG_Aint *displacement);
         int PMPIG_Type_ub(MPIG_Datatype datatype, MPIG_Aint *displacement);
         int PMPIG_Type_commit(MPIG_Datatype *datatype);
         int PMPIG_Type_free(MPIG_Datatype *datatype);
         int PMPIG_Get_elements(MPIG_Status *status, MPIG_Datatype datatype, int *count);
         int PMPIG_Pack(void* inbuf, int incount, MPIG_Datatype datatype, void *outbuf, int outsize,
                       int *position,  MPIG_Comm comm);
         int PMPIG_Unpack(void* inbuf, int insize, int *position, void *outbuf, int outcount,
                       MPIG_Datatype datatype, MPIG_Comm comm);
         int PMPIG_Pack_size(int incount, MPIG_Datatype datatype, MPIG_Comm comm, int *size);

             /*  C Bindings for Collective Communication   */

         int PMPIG_Barrier(MPIG_Comm comm );
         int PMPIG_Bcast(void* buffer, int count, MPIG_Datatype datatype, int root, MPIG_Comm comm );
         int PMPIG_Gather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm) ;
         int PMPIG_Gatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, int root,
                       MPIG_Comm comm);
         int PMPIG_Scatter(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm);
         int PMPIG_Scatterv(void* sendbuf, int *sendcounts, int *displs, MPIG_Datatype sendtype,
                       void* recvbuf, int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm);
         int PMPIG_Allgather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm);
         int PMPIG_Allgatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, MPIG_Comm comm);
         int PMPIG_Alltoall(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm);
         int PMPIG_Alltoallv(void* sendbuf, int *sendcounts, int *sdispls, MPIG_Datatype sendtype,
                       void* recvbuf, int *recvcounts, int *rdispls, MPIG_Datatype recvtype,
                       MPIG_Comm comm);
         int PMPIG_Reduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, int root, MPIG_Comm comm);
         int PMPIG_Op_create(MPIG_User_function *function, int commute, MPIG_Op *op);
         int PMPIG_Op_free( MPIG_Op *op);
         int PMPIG_Allreduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm);
         int PMPIG_Reduce_scatter(void* sendbuf, void* recvbuf, int *recvcounts,
                       MPIG_Datatype datatype, MPIG_Op op, MPIG_Comm comm);
         int PMPIG_Scan(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm );

             /* C  Bindings for Groups, Contexts, and Communicators  */

         int PMPIG_Group_size(MPIG_Group group, int *size);
         int PMPIG_Group_rank(MPIG_Group group, int *rank);
         int PMPIG_Group_translate_ranks (MPIG_Group group1, int n, int *ranks1,
                       MPIG_Group group2, int *ranks2);
         int PMPIG_Group_compare(MPIG_Group group1,MPIG_Group group2, int *result);
         int PMPIG_Comm_group(MPIG_Comm comm, MPIG_Group *group);
         int PMPIG_Group_union(MPIG_Group group1, MPIG_Group group2, MPIG_Group *newgroup);
         int PMPIG_Group_intersection(MPIG_Group group1, MPIG_Group group2, MPIG_Group *newgroup);
         int PMPIG_Group_difference(MPIG_Group group1, MPIG_Group group2, MPIG_Group *newgroup);
         int PMPIG_Group_incl(MPIG_Group group, int n, int *ranks, MPIG_Group *newgroup);
         int PMPIG_Group_excl(MPIG_Group group, int n, int *ranks, MPIG_Group *newgroup);
         int PMPIG_Group_range_incl(MPIG_Group group, int n, int ranges[][3], MPIG_Group *newgroup);
         int PMPIG_Group_range_excl(MPIG_Group group, int n, int ranges[][3], MPIG_Group *newgroup);
         int PMPIG_Group_free(MPIG_Group *group);
         int PMPIG_Comm_size(MPIG_Comm comm, int *size);
         int PMPIG_Comm_rank(MPIG_Comm comm, int *rank);
         int PMPIG_Comm_compare(MPIG_Comm comm1, MPIG_Comm comm2, int *result);
         int PMPIG_Comm_dup(MPIG_Comm comm, MPIG_Comm *newcomm);
         int PMPIG_Comm_create(MPIG_Comm comm, MPIG_Group group, MPIG_Comm *newcomm);
         int PMPIG_Comm_split(MPIG_Comm comm, int color, int key, MPIG_Comm *newcomm);
         int PMPIG_Comm_free(MPIG_Comm *comm);
         int PMPIG_Comm_test_inter(MPIG_Comm comm, int *flag);
         int PMPIG_Comm_remote_size(MPIG_Comm comm, int *size);
         int PMPIG_Comm_remote_group(MPIG_Comm comm, MPIG_Group *group);
         int PMPIG_Intercomm_create(MPIG_Comm local_comm, int local_leader, MPIG_Comm peer_comm,
                       int remote_leader, int tag, MPIG_Comm *newintercomm);
         int PMPIG_Intercomm_merge(MPIG_Comm intercomm, int high, MPIG_Comm *newintracomm);
         int PMPIG_Keyval_create(MPIG_Copy_function *copy_fn, MPIG_Delete_function *delete_fn,
                       int *keyval, void* extra_state);
         int PMPIG_Keyval_free(int *keyval);
         int PMPIG_Attr_put(MPIG_Comm comm, int keyval, void *attribute_val);
         int PMPIG_Attr_get(MPIG_Comm comm, int keyval, void *attribute_val, int *flag);
         int PMPIG_Attr_delete(MPIG_Comm comm, int keyval);

         #ifdef _THREAD_SAFE
         int PMPIG_Comm_create_errhandler(MPIG_Comm_errhandler_fn *function, MPIG_Errhandler *errhandler);
         int PMPIG_Comm_get_errhandler(MPIG_Comm comm, MPIG_Errhandler *errhandler);
         int PMPIG_Comm_set_errhandler(MPIG_Comm comm, MPIG_Errhandler errhandler);
         int PMPIG_Comm_create_keyval(MPIG_Comm_copy_attr_function *comm_copy_attr_fn,
                      MPIG_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval,
                      void *extra_state);
         int PMPIG_Comm_delete_attr(MPIG_Comm comm, int comm_keyval);
         int PMPIG_Comm_free_keyval(int *comm_keyval);
         int PMPIG_Comm_get_attr(MPIG_Comm comm, int comm_keyval, void *attribute_val, int *flag);
         int PMPIG_Comm_set_attr(MPIG_Comm comm, int comm_keyval, void *attribute_val);
         #endif


            /*   C Bindings for Process Topologies   */

         int PMPIG_Cart_create(MPIG_Comm comm_old, int ndims, int *dims, int *periods,
                       int reorder, MPIG_Comm *comm_cart);
         int PMPIG_Dims_create(int nnodes, int ndims, int *dims);
         int PMPIG_Graph_create(MPIG_Comm comm_old, int nnodes, int *index, int *edges,
                       int reorder, MPIG_Comm *comm_graph);
         int PMPIG_Topo_test(MPIG_Comm comm, int *status);
         int PMPIG_Graphdims_get(MPIG_Comm comm, int *nnodes, int *nedges);
         int PMPIG_Graph_get(MPIG_Comm comm, int maxindex, int maxedges, int *index, int *edges);
         int PMPIG_Cartdim_get(MPIG_Comm comm, int *ndims);
         int PMPIG_Cart_get(MPIG_Comm comm, int maxdims, int *dims, int *periods, int *coords);
         int PMPIG_Cart_rank(MPIG_Comm comm, int *coords, int *rank);
         int PMPIG_Cart_coords(MPIG_Comm comm, int rank, int maxdims, int *coords);
         int PMPIG_Graph_neighbors_count(MPIG_Comm comm, int rank, int *nneighbors);
         int PMPIG_Graph_neighbors(MPIG_Comm comm, int rank, int maxneighbors, int *neighbors);
         int PMPIG_Cart_shift(MPIG_Comm comm, int direction, int disp, int *rank_source, int *rank_dest);
         int PMPIG_Cart_sub(MPIG_Comm comm, int *remain_dims, MPIG_Comm *newcomm);
         int PMPIG_Cart_map(MPIG_Comm comm, int ndims, int *dims, int *periods, int *newrank);
         int PMPIG_Graph_map(MPIG_Comm comm, int nnodes, int *index, int *edges, int *newrank);


           /*    C bindings for Environmental Inquiry   */

         int PMPIG_Get_version(int *version, int *subversion);
         int PMPIG_Get_processor_name(char *name, int *resultlen);
         int PMPIG_Errhandler_create(MPIG_Handler_function *function, MPIG_Errhandler *errhandler);
         int PMPIG_Errhandler_set(MPIG_Comm comm, MPIG_Errhandler errhandler);
         int PMPIG_Errhandler_get(MPIG_Comm comm, MPIG_Errhandler *errhandler);
         int PMPIG_Errhandler_free(MPIG_Errhandler *errhandler);
         int PMPIG_Error_string(int errorcode, char *string, int *resultlen);
         int PMPIG_Error_class(int errorcode, int *errorclass);
         double PMPIG_Wtime(void);
         double PMPIG_Wtick(void);
         int PMPIG_Init(int *argc, char ***argv);
         int PMPIG_Finalize(void);
         int PMPIG_Initialized(int *flag);
         int PMPIG_Abort(MPIG_Comm comm, int errorcode);


           /*    C Bindings for Profiling  */

         int PMPIG_Pcontrol(const int level,...);

             /*  C Bindings for Non-Blocking Collective Communication   */

         int PMPE_Ibarrier(MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Ibcast(void* buffer, int count, MPIG_Datatype datatype, int root, MPIG_Comm comm ,MPIG_Request *request);
         int PMPE_Igather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm,MPIG_Request *request) ;
         int PMPE_Igatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, int root,
                       MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Iscatter(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Iscatterv(void* sendbuf, int *sendcounts, int *displs, MPIG_Datatype sendtype,
                       void* recvbuf, int recvcount, MPIG_Datatype recvtype, int root, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Iallgather(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Iallgatherv(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int *recvcounts, int *displs, MPIG_Datatype recvtype, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Ialltoall(void* sendbuf, int sendcount, MPIG_Datatype sendtype, void* recvbuf,
                       int recvcount, MPIG_Datatype recvtype, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Ialltoallv(void* sendbuf, int *sendcounts, int *sdispls, MPIG_Datatype sendtype,
                       void* recvbuf, int *recvcounts, int *rdispls, MPIG_Datatype recvtype,
                       MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Ireduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, int root, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Iallreduce(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Ireduce_scatter(void* sendbuf, void* recvbuf, int *recvcounts,
                       MPIG_Datatype datatype, MPIG_Op op, MPIG_Comm comm,MPIG_Request *request);
         int PMPE_Iscan(void* sendbuf, void* recvbuf, int count, MPIG_Datatype datatype,
                       MPIG_Op op, MPIG_Comm comm ,MPIG_Request *request);

         #ifdef _THREAD_SAFE
             /*  C Bindings for MPIG 1-sided */

         int PMPIG_Win_create(void *base, MPIG_Aint size, int disp_unit,
                             MPIG_Info info, MPIG_Comm comm, MPIG_Win *win);
         int PMPIG_Win_free(MPIG_Win *win);
         int PMPIG_Win_get_group(MPIG_Win win, MPIG_Group *group);
         int PMPIG_Put(void *origin_addr, int origin_count, MPIG_Datatype origin_datatype,
                     int target_rank, MPIG_Aint target_disp, int target_count,
                     MPIG_Datatype target_datatype, MPIG_Win win);
         int PMPIG_Get(void *origin_addr, int origin_count, MPIG_Datatype origin_datatype,
                     int target_rank, MPIG_Aint target_disp, int target_count,
                     MPIG_Datatype target_datatype, MPIG_Win win);
         int PMPIG_Accumulate(void *origin_addr, int origin_count, MPIG_Datatype
                            origin_datatype, int target_rank, MPIG_Aint target_disp,
                            int target_count, MPIG_Datatype target_datatype,
                            MPIG_Op op, MPIG_Win win);
         int PMPIG_Win_fence(int assert, MPIG_Win win);
         int PMPIG_Win_start(MPIG_Group group, int assert, MPIG_Win win);
         int PMPIG_Win_complete(MPIG_Win win);
         int PMPIG_Win_post(MPIG_Group group, int assert, MPIG_Win win);
         int PMPIG_Win_wait(MPIG_Win win);
         int PMPIG_Win_test(MPIG_Win win, int *flag);
         int PMPIG_Win_lock(int lock_type, int rank, int assert, MPIG_Win win);
         int PMPIG_Win_unlock(int rank, MPIG_Win win);
         int PMPIG_Win_get_attr(MPIG_Win win, int win_keyval, void *attribute_val, int *flag);
         int PMPIG_Win_set_attr(MPIG_Win win, int win_keyval, void *attribute_val);

         int PMPIG_Alloc_mem(MPIG_Aint size, MPIG_Info info, void *baseptr);
         int PMPIG_Free_mem(void *base);

             /*  C Bindings for I/O  */

         int PMPIG_File_open(MPIG_Comm comm,char *filename,int amode,MPIG_Info info,
                            MPIG_File *fh);
         int PMPIG_File_close(MPIG_File *fh);
         int PMPIG_File_delete(char *filename,MPIG_Info info);
         int PMPIG_File_set_size(MPIG_File fh,MPIG_Offset size);
         int PMPIG_File_get_size(MPIG_File fh,MPIG_Offset *size);
         int PMPIG_File_get_group(MPIG_File fh,MPIG_Group *group);
         int PMPIG_File_get_amode(MPIG_File fh,int *amode);
         int PMPIG_File_set_info(MPIG_File fh,MPIG_Info info);
         int PMPIG_File_get_info(MPIG_File fh,MPIG_Info *info_used);
         int PMPIG_File_set_view(MPIG_File fh,MPIG_Offset disp,MPIG_Datatype etype,
                                MPIG_Datatype filetype,char *datarep,MPIG_Info info);
         int PMPIG_File_get_view(MPIG_File fh,MPIG_Offset *disp,MPIG_Datatype *etype,
                                MPIG_Datatype *filetype,char *datarep);
         int PMPIG_File_read_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                               MPIG_Datatype datatype,MPIG_Status *status);
         int PMPIG_File_read_at_all(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                   MPIG_Datatype datatype,MPIG_Status *status);
         int PMPIG_File_write_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                MPIG_Datatype datatype,MPIG_Status *status);
         int PMPIG_File_write_at_all(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                    MPIG_Datatype datatype,MPIG_Status *status);
         int PMPIG_File_iread_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                MPIG_Datatype datatype,MPIG_Request *request);
         int PMPIG_File_iwrite_at(MPIG_File fh,MPIG_Offset offset,void *buf,int count,
                                 MPIG_Datatype datatype,MPIG_Request *request);
         int PMPIG_File_get_atomicity(MPIG_File fh,int *flag);
         int PMPIG_File_sync(MPIG_File fh);
         int PMPIG_Type_create_subarray(int ndims,int array_of_sizes[],
                                       int array_of_subsizes[],int array_of_starts[],
                                       int order,MPIG_Datatype oldtype,
                                       MPIG_Datatype *newtype);
         int PMPIG_Type_create_darray(int size,int rank,int ndims,int array_of_gsizes[],
                                     int array_of_distribs[],int array_of_dargs[],
                                     int array_of_psizes[],int order,MPIG_Datatype oldtype,
                                     MPIG_Datatype *newtype);
         int PMPIG_File_create_errhandler(MPIG_File_errhandler_fn *function,
                                         MPIG_Errhandler *errhandler);
         int PMPIG_File_set_errhandler(MPIG_File fh,MPIG_Errhandler errhandler);
         int PMPIG_File_get_errhandler(MPIG_File fh,MPIG_Errhandler *errhandler);
         int PMPIG_File_create_errhandler(MPIG_File_errhandler_fn *function, MPIG_Errhandler *errhandler);
         int PMPIG_File_get_byte_offset(MPIG_File fh, MPIG_Offset offset, MPIG_Offset *disp);
         int PMPIG_File_get_info(MPIG_File fh, MPIG_Info *info_used);
         int PMPIG_File_get_position(MPIG_File fh, MPIG_Offset *offset);
         int PMPIG_File_get_position_shared(MPIG_File fh, MPIG_Offset *offset);
         int PMPIG_File_get_type_extent(MPIG_File fh, MPIG_Datatype datatype, MPIG_Aint *extent);
         int PMPIG_File_iread(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Request *request);
         int PMPIG_File_iread_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Request *request);
         int PMPIG_File_iwrite(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Request *request);
         int PMPIG_File_iwrite_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Request *request);
         int PMPIG_File_preallocate(MPIG_File fh, MPIG_Offset size);
         int PMPIG_File_read(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Status *status);
         int PMPIG_File_read_all(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int PMPIG_File_read_all_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int PMPIG_File_read_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int PMPIG_File_read_at_all_begin(MPIG_File fh, MPIG_Offset offset, void *buf, int count,
                      MPIG_Datatype datatype);
         int PMPIG_File_read_at_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int PMPIG_File_read_ordered(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int PMPIG_File_read_ordered_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int PMPIG_File_read_ordered_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int PMPIG_File_read_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int PMPIG_File_seek(MPIG_File fh, MPIG_Offset offset, int whence);
         int PMPIG_File_seek_shared(MPIG_File fh, MPIG_Offset offset, int whence);
         int PMPIG_File_set_atomicity(MPIG_File fh, int flag);
         int PMPIG_File_set_info(MPIG_File fh, MPIG_Info info);
         int PMPIG_File_write(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype, MPIG_Status *status);
         int PMPIG_File_write_all(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int PMPIG_File_write_all_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int PMPIG_File_write_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int PMPIG_File_write_at_all_begin(MPIG_File fh, MPIG_Offset offset, void *buf, int count,
                      MPIG_Datatype datatype);
         int PMPIG_File_write_at_all_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int PMPIG_File_write_ordered(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int PMPIG_File_write_ordered_begin(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype);
         int PMPIG_File_write_ordered_end(MPIG_File fh, void *buf, MPIG_Status *status);
         int PMPIG_File_write_shared(MPIG_File fh, void *buf, int count, MPIG_Datatype datatype,
                      MPIG_Status *status);
         int PMPIG_Register_datarep(char *datarep, MPIG_Datarep_conversion_function *read_conversion_fn,
                      MPIG_Datarep_conversion_function *write_conversion_fn,
                      MPIG_Datarep_extent_function *dtype_file_extent_fn, void *extra_state);
         int PMPIG_Info_create(MPIG_Info *info);
         int PMPIG_Info_set(MPIG_Info Info,char *key,char *value);
         int PMPIG_Info_delete(MPIG_Info info,char *key);
         int PMPIG_Info_get(MPIG_Info info,char *key,int valuelen,char *value,int *flag);
         int PMPIG_Info_get_valuelen(MPIG_Info info,char *key,int *valuelen,int *flag);
         int PMPIG_Info_get_nkeys(MPIG_Info info,int *nkeys);
         int PMPIG_Info_get_nthkey(MPIG_Info info,int n,char *key);
         int PMPIG_Info_dup(MPIG_Info info, MPIG_Info *newinfo);
         int PMPIG_Info_free(MPIG_Info *info);
         #endif

         #if defined(__cplusplus)
            }
         #endif
         #endif /* _H_MPIG */


/*
 ************************************************************************
 * End block changed /usr/lpp/ppe.poe/include/mpi.h
 * Now begin custom function
 ************************************************************************
 */

extern MPI_Comm MPI_CART_COMM_WORLD;

int  MPIG_Init( int *argv, char ***argc );
/*
A call for development purposes which states the size at run time
put  -1 for don't care - fails if volume x*y*z
can't fit roundly in mpi partition
*/
int MPIG_InitSize( int *argc, char ***argv, int x, int y, int z );

int MPIG_GetSize( int *x, int *y, int *z );
int MPIG_GetSizeX();
int MPIG_GetSizeY();
int MPIG_GetSizeZ();

int MPIG_GetCoord( int *x, int *y, int *z );


int MPIG_GetCoordX();
int MPIG_GetCoordY();
int MPIG_GetCoordZ();

int MPIG_RankToCoord( int Rank, int *x, int *y, int *z );

int MPIG_GetSizeX();
int MPIG_GetSizeY();
int MPIG_GetSizeZ();

int MPIG_CoordsToRank( int x, int y, int z, int *Rank  );

int MPIG_GetRank( int x, int y, int z );


#endif
