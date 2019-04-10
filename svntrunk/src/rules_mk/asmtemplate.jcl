//*******************************************************************
//*
//*  (c) Copyright International Business Machines Corp. 2003
//*  All Rights Reserved
//*
//*******************************************************************
//*
//*   ASM COMPILE
//*
//*******************************************************************
//*
//ASM      EXEC PGM=ASMA90,REGION=&REGION,
//  PARM=&ASMCFLAGS
//* PARM=('OBJECT,NODECK,LIST,NORENT',
//*     'SYSPARM(''Y IPC 0100 IPC-199 2''),EXIT(LIBEXIT(SDYNASM))'),
//*
//STEPLIB  DD &ASM_STEPLIB,DISP=SHR
//*
//SYSPRINT DD DSN=&&ASMLIST,DISP=(NEW,PASS),SPACE=(CYL,(1,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//SYSUT1   DD UNIT=VIO,SPACE=(CYL,(5,1))
//*
//ASM.SYSIN         DD DISP=SHR,
//           DSN=DATASET.NAME.HERE
//ASM.SYSLIB        DD DISP=SHR,
//           DSN=DATASET.MACLOCAL.HERE
//                  DD DISP=SHR,
//           DSN=DATASET.MACEXPT.HERE
//             DD DSN=DATASET.MACLIB.HERE,DISP=SHR
//             DD DSN=CEE.SCEEMAC,DISP=SHR
//             DD DSN=SYS1.MACLIB,DISP=SHR
//             DD DSN=SYS1.MODGEN,DISP=SHR
//ASM.SYSLIN        DD DISP=(NEW,PASS),
//             DCB=(RECFM=FB,LRECL=80),
//             SPACE=(CYL,(1,10)),UNIT=SYSDA,
//             DSN=&&OBJ
//LIST     EXEC PGM=IEBGENER,COND=(20,LE,ASM)
//*
//SYSUT1   DD DISP=(OLD,DELETE),DSN=&&ASMLIST,UNIT=SYSDA
//LST.SYSUT2        DD DSN=&&LST,DISP=(NEW,PASS),SPACE=(CYL,(2,15)),
//            DCB=(RECFM=FBM,LRECL=121),UNIT=SYSDA
//SYSIN    DD DUMMY
//SYSPRINT DD DUMMY
//*
