//*******************************************************************
//*
//*  (c) Copyright International Business Machines Corp. 2003
//*  All Rights Reserved
//*
//*******************************************************************
//*
//*   COBOL COMPILE
//*
//*******************************************************************
//*
//COBOL    EXEC PGM=IGYCRCTL,REGION=&REGION,
//  PARM=&CBLCFLAGS
//* PARM='APOST,LIB,MAP,NOSEQ,BUFSIZE(30K),LIST,NORENT'
//*
//STEPLIB  DD &CBL_STEPLIB,DISP=SHR
//*
//SYSPRINT DD DSN=&&CBLLIST,DISP=(NEW,PASS),SPACE=(CYL,(1,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//SYSUT1   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//SYSUT2   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//SYSUT3   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//SYSUT4   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//SYSUT5   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//SYSUT6   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//SYSUT7   DD  UNIT=SYSDA,SPACE=(CYL,(1,1))
//COB.SYSIN         DD DISP=SHR,
//           DSN=DATASET.NAME.HERE
//COB.SYSLIB        DD DISP=SHR,
//           DSN=DATASET.MACLOCAL.HERE
//                  DD DISP=SHR,
//           DSN=DATASET.MACEXPT.HERE
//                  DD DISP=SHR,
//           DSN=DATASET.SRC.HERE
//             DD DSN=SYS1.MACLIB,DISP=SHR
//             DD DSN=SYS1.MODGEN,DISP=SHR
//COB.SYSLIN        DD DISP=(NEW,PASS),
//             DCB=(RECFM=FB,LRECL=80,BLKSIZE=3200),
//             SPACE=(CYL,(1,10)),UNIT=SYSDA,
//             DSN=&&OBJ
//LIST     EXEC PGM=IEBGENER,COND=(20,LE,COBOL)
//*
//SYSUT1   DD DISP=(OLD,DELETE),DSN=&&CBLLIST,UNIT=SYSDA
//LST.SYSUT2        DD DSN=&&LST,DISP=(NEW,PASS),SPACE=(CYL,(2,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//SYSIN    DD DUMMY
//SYSPRINT DD DUMMY
//*
