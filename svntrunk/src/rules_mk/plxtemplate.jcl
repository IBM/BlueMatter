//*******************************************************************
//*
//*  (c) Copyright International Business Machines Corp. 2003
//*  All Rights Reserved
//*
//*******************************************************************
//*
//*   PLX COMPILE
//*
//*******************************************************************
//*
//PLX      EXEC PGM=AKHPLX,REGION=&REGION,
//  PARM=&PLXCFLAGS
//* PARM=('MACPARM(''MVS/ESA670''),NODOBARS,OBJECT,MONITOR,IDR',
//*      'IMPLICIT,CONV,FLAG(I),SEC(IC),LC(55)'),
//*
//STEPLIB  DD &PLX_STEPLIB,DISP=SHR
//*
//SYSPRINT DD DSN=&&PLXLIST,DISP=(NEW,PASS),SPACE=(CYL,(1,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//UNIDATA  DD SYSOUT=H
//SYSUT1   DD UNIT=SYSDA,
//            SPACE=(CYL,(4,4))
//SYSUT2   DD UNIT=SYSDA,
//            SPACE=(CYL,(5,5))
//SYSUT3   DD UNIT=SYSDA,
//            SPACE=(CYL,(5,5))
//SYSUT4   DD UNIT=SYSDA,
//            SPACE=(CYL,(5,5))
//SYSUT5   DD DISP=(,DELETE),
//            UNIT=SYSDA,
//            SPACE=(CYL,(5,5,5)),
//            DCB=(RECFM=FB,LRECL=80,BLKSIZE=12960)
//ASMPRINT DD DSN=&&ASMLIST,DISP=(NEW,PASS),SPACE=(CYL,(1,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//ASMUT1   DD UNIT=SYSDA,
//            SPACE=(CYL,(5,10))
//*
//PLX.SYSIN         DD DISP=SHR,
//           DSN=DATASET.NAME.HERE
//PLX.SYSLIB        DD DISP=SHR,
//           DSN=DATASET.MACLOCAL.HERE
//                  DD DISP=SHR,
//           DSN=DATASET.MACEXPT.HERE
//             DD DSN=SYS1.MACLIB,DISP=SHR
//             DD DSN=SYS1.MODGEN,DISP=SHR
//PLX.ASMLIB        DD DISP=SHR,
//           DSN=DATASET.MACLOCAL.HERE
//                  DD DISP=SHR,
//           DSN=DATASET.MACEXPT.HERE
//             DD DSN=SYS1.MACLIB,DISP=SHR
//             DD DSN=SYS1.MODGEN,DISP=SHR
//PLX.ASMLIN        DD DISP=(NEW,PASS),
//             DCB=(RECFM=FB,LRECL=80,BLKSIZE=3200),
//             SPACE=(CYL,(1,10)),UNIT=SYSDA,
//             DSN=&&OBJ
//LIST     EXEC PGM=IEBGENER,COND=(20,LE,PLX)
//*
//SYSUT1   DD DISP=(OLD,DELETE),DSN=&&PLXLIST,UNIT=SYSDA
//         DD DISP=(OLD,DELETE),DSN=&&ASMLIST,UNIT=SYSDA
//LST.SYSUT2        DD DSN=&&LST,DISP=(NEW,PASS),SPACE=(CYL,(2,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//SYSIN    DD DUMMY
//SYSPRINT DD DUMMY
//*
