
/*  A Bison parser, made from crisolscript.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	tGLOBAL	258
#define	tCONST	259
#define	tVAR	260
#define	tFUNC	261
#define	tCOMPILE	262
#define	tSCRIPT	263
#define	tNUMBER	264
#define	tSTRING	265
#define	tENTITY	266
#define	tVOID	267
#define	tIF	268
#define	tELSE	269
#define	tWHILE	270
#define	tRETURN	271
#define	tREF	272
#define	tTHEN	273
#define	tDO	274
#define	tIMPORT	275
#define	tFUNCTION	276
#define	tBEGIN	277
#define	tEND	278
#define	tGAMEOBJ	279
#define	tWORLDOBJ	280
#define	tNULL	281
#define	tTRUE	282
#define	tFALSE	283
#define	tRIGHT_HAND_SLOT	284
#define	tLEFT_HAND_SLOT	285
#define	tNORTH_DIRECTION	286
#define	tNORTHEAST_DIRECTION	287
#define	tEAST_DIRECTION	288
#define	tSOUTHEAST_DIRECTION	289
#define	tSOUTH_DIRECTION	290
#define	tSOUTHWEST_DIRECTION	291
#define	tWEST_DIRECTION	292
#define	tNORTHWEST_DIRECTION	293
#define	tNO_COMBAT_ALINGMENT	294
#define	tPLAYER_COMBAT_ALINGMENT	295
#define	tENEMYPLAYER_COMBAT_ALINGMENT	296
#define	tBASE_VALUE	297
#define	tTEMP_VALUE	298
#define	tENTITY_PLAYER	299
#define	tENTITY_SCENE_OBJ	300
#define	tENTITY_NPC	301
#define	tENTITY_WALL	302
#define	tENTITY_ITEM	303
#define	tTEXT_RIGHT_JUSTIFY	304
#define	tTEXT_UP_JUSTIFY	305
#define	tTEXT_LEFT_JUSTIFY	306
#define	tKEY_ESC	307
#define	tKEY_F1	308
#define	tKEY_F2	309
#define	tKEY_F3	310
#define	tKEY_F4	311
#define	tKEY_F5	312
#define	tKEY_F6	313
#define	tKEY_F7	314
#define	tKEY_F8	315
#define	tKEY_F9	316
#define	tKEY_F10	317
#define	tKEY_F11	318
#define	tKEY_F12	319
#define	tKEY_0	320
#define	tKEY_1	321
#define	tKEY_2	322
#define	tKEY_3	323
#define	tKEY_4	324
#define	tKEY_5	325
#define	tKEY_6	326
#define	tKEY_7	327
#define	tKEY_8	328
#define	tKEY_9	329
#define	tKEY_A	330
#define	tKEY_B	331
#define	tKEY_C	332
#define	tKEY_D	333
#define	tKEY_E	334
#define	tKEY_F	335
#define	tKEY_G	336
#define	tKEY_H	337
#define	tKEY_I	338
#define	tKEY_J	339
#define	tKEY_K	340
#define	tKEY_L	341
#define	tKEY_M	342
#define	tKEY_N	343
#define	tKEY_O	344
#define	tKEY_P	345
#define	tKEY_Q	346
#define	tKEY_R	347
#define	tKEY_S	348
#define	tKEY_T	349
#define	tKEY_U	350
#define	tKEY_V	351
#define	tKEY_W	352
#define	tKEY_X	353
#define	tKEY_Y	354
#define	tKEY_Z	355
#define	tKEY_BACK	356
#define	tKEY_TAB	357
#define	tKEY_RETURN	358
#define	tKEY_SPACE	359
#define	tKEY_LCONTROL	360
#define	tKEY_RCONTROL	361
#define	tKEY_LSHIFT	362
#define	tKEY_RSHIFT	363
#define	tKEY_ALT	364
#define	tKEY_ALTGR	365
#define	tKEY_INSERT	366
#define	tKEY_REPAG	367
#define	tKEY_AVPAG	368
#define	tKEY_MINUS_PAD	369
#define	tKEY_ADD_PAD	370
#define	tKEY_DIV_PAD	371
#define	tKEY_MUL_PAD	372
#define	tKEY_0_PAD	373
#define	tKEY_1_PAD	374
#define	tKEY_2_PAD	375
#define	tKEY_3_PAD	376
#define	tKEY_4_PAD	377
#define	tKEY_5_PAD	378
#define	tKEY_6_PAD	379
#define	tKEY_7_PAD	380
#define	tKEY_8_PAD	381
#define	tKEY_9_PAD	382
#define	tKEY_UP	383
#define	tKEY_DOWN	384
#define	tKEY_RIGHT	385
#define	tKEY_LEFT	386
#define	tONSTARTGAME	387
#define	tONCLICKHOURPANEL	388
#define	tONFLEECOMBAT	389
#define	tONKEYPRESSED	390
#define	tONSTARTCOMBATMODE	391
#define	tONENDCOMBATMODE	392
#define	tONNEWHOUR	393
#define	tONENTERINAREA	394
#define	tONWORLDIDLE	395
#define	tONSETINFLOOR	396
#define	tONSETOUTOFFLOOR	397
#define	tONGETITEM	398
#define	tONDROPITEM	399
#define	tONOBSERVEENTITY	400
#define	tONTALKTOENTITY	401
#define	tONMANIPULATEENTITY	402
#define	tONDEATH	403
#define	tONRESURRECT	404
#define	tONINSERTINEQUIPMENTSLOT	405
#define	tONREMOVEFROMEQUIPMENTSLOT	406
#define	tONUSEHABILITY	407
#define	tONACTIVATEDSYMPTOM	408
#define	tONDEACTIVATEDSYMPTOM	409
#define	tONHITENTITY	410
#define	tONSTARTCOMBATTURN	411
#define	tONCRIATUREINRANGE	412
#define	tONCRIATUREOUTOFRANGE	413
#define	tONENTITYIDLE	414
#define	tONUSEITEM	415
#define	tONTRADEITEM	416
#define	tONENTITYCREATED	417
#define	tSETSCRIPT	418
#define	tQUITGAME	419
#define	tWRITETOCONSOLE	420
#define	tACTIVEADVICEDIALOG	421
#define	tACTIVEQUESTIONDIALOG	422
#define	tACTIVETEXTREADERDIALOG	423
#define	tADDOPTIONTOTEXTSELECTORDIALOG	424
#define	tRESETOPTIONSINTEXTSELECTORDIALOG	425
#define	tACTIVETEXTSELECTORDIALOG	426
#define	tPLAYMIDIMUSIC	427
#define	tSTOPMIDIMUSIC	428
#define	tPLAYWAVAMBIENTSOUND	429
#define	tSTOPWAVAMBIENTSOUND	430
#define	tACTIVETRADEITEMSINTERFAZ	431
#define	tADDOPTIONTOCONVERSATORINTERFAZ	432
#define	tRESETOPTIONSINCONVERSATORINTERFAZ	433
#define	tACTIVECONVERSATORINTERFAZ	434
#define	tDESACTIVECONVERSATORINTERFAZ	435
#define	tGETOPTIONFROMCONVERSATORINTERFAZ	436
#define	tSHOWPRESENTATION	437
#define	tBEGINCUTSCENE	438
#define	tENDCUTSCENE	439
#define	tISKEYPRESSED	440
#define	tGETAREANAME	441
#define	tGETAREAID	442
#define	tGETAREAWIDTH	443
#define	tGETAREAHEIGHT	444
#define	tGETHOUR	445
#define	tGETMINUTE	446
#define	tSETHOUR	447
#define	tSETMINUTE	448
#define	tGETENTITY	449
#define	tGETPLAYER	450
#define	tISFLOORVALID	451
#define	tGETNUMITEMSAT	452
#define	tGETITEMAT	453
#define	tGETDISTANCE	454
#define	tCALCULEPATHLENGHT	455
#define	tLOADAREA	456
#define	tCHANGEENTITYLOCATION	457
#define	tATTACHCAMERATOENTITY	458
#define	tATTACHCAMERATOLOCATION	459
#define	tISCOMBATMODEACTIVE	460
#define	tENDCOMBAT	461
#define	tGETCRIATUREINCOMBATTURN	462
#define	tGETCOMBATANT	463
#define	tGETNUMBEROFCOMBATANTS	464
#define	tGETAREALIGHTMODEL	465
#define	tSETIDLESCRIPTTIME	466
#define	tDESTROYENTITY	467
#define	tCREATECRIATURE	468
#define	tCREATEWALL	469
#define	tCREATESCENARYOBJECT	470
#define	tCREATEITEMABANDONED	471
#define	tCREATEITEMWITHOWNER	472
#define	tSETWORLDTIMEPAUSE	473
#define	tISWORLDTIMEINPAUSE	474
#define	tSETELEVATIONAT	475
#define	tGETELEVATIONAT	476
#define	tNEXTTURN	477
#define	tGETLIGHTAT	478
#define	tPLAYWAVSOUND	479
#define	tSETSCRIPTAT	480
#define	tGETNAME	481
#define	tSETNAME	482
#define	tGETTYPE	483
#define	tGETENTITYTYPE	484
#define	tSAY	485
#define	tSHUTUP	486
#define	tISSAYING	487
#define	tATTACHGFX	488
#define	tRELEASEGFX	489
#define	tRELEASEALLGFX	490
#define	tISGFXATTACHED	491
#define	tGETNUMITEMSINCONTAINER	492
#define	tISITEMINCONTAINER	493
#define	tTRANSFERITEMTOCONTAINER	494
#define	tINSERTITEMINCONTAINER	495
#define	tREMOVEITEMOFCONTAINER	496
#define	tSETANIMTEMPLATESTATE	497
#define	tSETPORTRAITANIMTEMPLATESTATE	498
#define	tSETLIGHT	499
#define	tGETLIGHT	500
#define	tGETXPOS	501
#define	tGETYPOS	502
#define	tGETELEVATION	503
#define	tSETELEVATION	504
#define	tGETLOCALATTRIBUTE	505
#define	tSETLOCALATTRIBUTE	506
#define	tGETOWNER	507
#define	tGETCLASS	508
#define	tGETINCOMBATUSECOST	509
#define	tGETGLOBALATTRIBUTE	510
#define	tSETGLOBALATTRIBUTE	511
#define	tGETWALLORIENTATION	512
#define	tBLOCKACCESS	513
#define	tUNBLOCKACCESS	514
#define	tISACCESSBLOCKED	515
#define	tSETSYMPTOM	516
#define	tISSYMPTOMACTIVE	517
#define	tGETGENRE	518
#define	tGETHEALTH	519
#define	tSETHEALTH	520
#define	tGETEXTENDEDATTRIBUTE	521
#define	tSETEXTENDEDATTRIBUTE	522
#define	tGETLEVEL	523
#define	tSETLEVEL	524
#define	tGETEXPERIENCE	525
#define	tSETEXPERIENCE	526
#define	tGETACTIONPOINTS	527
#define	tSETACTIONPOINTS	528
#define	tISHABILITYACTIVE	529
#define	tSETHABILITY	530
#define	tUSEHABILITY	531
#define	tISRUNMODEACTIVE	532
#define	tSETRUNMODE	533
#define	tMOVETO	534
#define	tISMOVING	535
#define	tSTOPMOVING	536
#define	tEQUIPITEM	537
#define	tREMOVEITEMEQUIPPED	538
#define	tISITEMEQUIPPED	539
#define	tDROPITEM	540
#define	tUSEITEM	541
#define	tMANIPULATE	542
#define	tSETTRANSPARENTMODE	543
#define	tISTRANSPARENTMODEACTIVE	544
#define	tCHANGEANIMORIENTATION	545
#define	tGETANIMORIENTATION	546
#define	tSETALINGMENT	547
#define	tSETALINGMENTWITH	548
#define	tSETALINGMENTAGAINST	549
#define	tGETALINGMENT	550
#define	tHITENTITY	551
#define	tGETITEMEQUIPPED	552
#define	tGETINCOMBATACTIONPOINTS	553
#define	tISGHOSTMOVEMODEACTIVE	554
#define	tSETGHOSTMOVEMODE	555
#define	tGETRANGE	556
#define	tISINRANGE	557
#define	tAPIPASSTORGBCOLOR	558
#define	tAPIGETREDCOMPONENT	559
#define	tAPIGETGREENCOMPONENT	560
#define	tAPIGETBLUECOMPONENT	561
#define	tAPIRAND	562
#define	tAPIGETINTEGERVALUE	563
#define	tAPIGETDECIMALVALUE	564
#define	tAPIGETSTRINGSIZE	565
#define	tAPIWRITETOLOGGER	566
#define	tAPIENABLECRISOLSCRIPTWARNINGS	567
#define	tAPIDISABLECRISOLSCRIPTWARNINGS	568
#define	tAPISHOWFPS	569
#define	tAPIWAIT	570
#define	tNUMBER_VALUE	571
#define	tSTRING_VALUE	572
#define	tIDENTIFIER	573
#define	IFX	574
#define	tASSING	575
#define	tOR	576
#define	tAND	577
#define	tEQ	578
#define	tNEQ	579
#define	tGEQ	580
#define	tLEQ	581

#line 6 "crisolscript.y"

 /* Includes */
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "TypesDefs.h"
#include "ASTree.h"
#include "error.h"
 
 /* Prototipos de funciones extern */
extern void SetScriptFileBuffer(sbyte* FileName);
extern void SetImportFileBuffer(sbyte* FileName);
extern char* GetActFileName(void);

 /* Vbles extern */
extern sGlobal* pGlobal;
extern sbyte*   szGlobalsFile; 


#line 27 "crisolscript.y"
typedef union {
 float               fNumber;      /* Valor numerico */
 sbyte*              szString;     /* Cadena         */
 sbyte*              szIdentifier; /* Identificador  */
 struct sGlobal*     pGlobal;      /* Espacio global */
 struct sConst*      pConst;       /* Constantes */
 struct sVar*        pVar;         /* Vbles */
 struct sScript*     pScript;      /* Scripts */
 struct sScriptType* pScriptType;  /* Tipos de scripts */
 struct sImport*     pImport;      /* Ficheros de funciones globales */
 struct sFunc*       pFunc;        /* Funciones */
 struct sType*       pType;        /* Tipos de datos */
 struct sArgument*   pArgument;    /* Argumentos */
 struct sStm*        pStm;         /* Sentencias */
 struct sExp*        pExp;         /* Expresiones */ 
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		1155
#define	YYFLAG		-32768
#define	YYNTBASE	340

#define YYTRANSLATE(x) ((unsigned)(x) <= 581 ? yytranslate[x] : 380)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   323,     2,     2,     2,   334,     2,     2,   337,
   338,   332,   330,   336,   331,   339,   333,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,   335,   329,
     2,   328,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
   126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
   186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
   196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
   206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
   216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
   226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
   236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
   246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
   256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
   266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
   276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
   286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
   296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
   306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
   316,   317,   318,   319,   320,   321,   322,   324,   325,   326,
   327
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     6,     7,    10,    12,    15,    21,    22,    25,
    27,    30,    34,    37,    42,    43,    46,    47,    49,    50,
    54,    55,    60,    64,    72,    77,    82,    87,    92,    97,
   102,   108,   114,   119,   131,   143,   152,   161,   170,   179,
   188,   194,   200,   212,   224,   236,   245,   254,   266,   272,
   278,   290,   302,   308,   317,   326,   327,   329,   330,   334,
   335,   340,   344,   345,   347,   348,   351,   353,   356,   366,
   368,   370,   372,   374,   376,   380,   384,   387,   389,   391,
   393,   396,   400,   402,   405,   407,   410,   414,   421,   430,
   437,   439,   442,   444,   446,   448,   450,   452,   454,   456,
   458,   460,   462,   464,   466,   468,   470,   472,   474,   476,
   478,   480,   482,   484,   486,   488,   490,   492,   494,   496,
   498,   500,   502,   504,   506,   508,   510,   512,   514,   516,
   518,   520,   522,   524,   526,   528,   530,   532,   534,   536,
   538,   540,   542,   544,   546,   548,   550,   552,   554,   556,
   558,   560,   562,   564,   566,   568,   570,   572,   574,   576,
   578,   580,   582,   584,   586,   588,   590,   592,   594,   596,
   598,   600,   602,   604,   606,   608,   610,   612,   614,   616,
   618,   620,   622,   624,   626,   628,   630,   632,   634,   636,
   638,   640,   642,   644,   646,   648,   650,   652,   654,   656,
   658,   660,   662,   664,   666,   668,   670,   672,   674,   676,
   678,   680,   682,   684,   686,   688,   690,   692,   694,   696,
   698,   700,   702,   704,   706,   708,   710,   712,   714,   716,
   718,   720,   722,   726,   730,   734,   738,   742,   746,   750,
   754,   758,   762,   766,   770,   773,   776,   780,   784,   788,
   793,   795,   799,   803,   807,   812,   817,   822,   827,   832,
   837,   842,   847,   852,   857,   862,   867,   872,   877,   882,
   887,   892,   897,   902,   907,   912,   917,   922,   927,   932,
   937,   942,   947,   952,   957,   962,   967,   972,   977,   982,
   987,   992,   997,  1002,  1007,  1012,  1017,  1022,  1027,  1032,
  1037,  1042,  1047,  1052,  1057,  1062,  1067,  1072,  1077,  1082,
  1087,  1092,  1097,  1102,  1107,  1112,  1117,  1122,  1127,  1132,
  1137,  1142,  1147,  1152,  1157,  1162,  1167,  1172,  1177,  1182,
  1187,  1192,  1197,  1202,  1207,  1212,  1217,  1222,  1227,  1232,
  1237,  1242,  1247,  1252,  1257,  1262,  1267,  1272,  1277,  1282,
  1287,  1292,  1297,  1302,  1307,  1312,  1317,  1322,  1327,  1332,
  1337,  1342,  1347,  1352,  1357,  1362,  1367,  1372,  1377,  1382,
  1387,  1392,  1397,  1402,  1407,  1412,  1417,  1422,  1427,  1432,
  1437,  1442,  1447,  1452,  1457,  1462,  1467,  1472,  1477,  1482,
  1487,  1492,  1497,  1502,  1507,  1512,  1517,  1522,  1527,  1532,
  1537,  1542,  1547,  1552,  1557,  1562,  1567,  1572,  1577,  1582,
  1587,  1592,  1593,  1595,  1597
};

static const short yyrhs[] = {    -1,
     3,   341,   344,   349,     0,     0,     4,   342,     0,   343,
     0,   342,   343,     0,   369,   318,   320,   373,   335,     0,
     0,     5,   345,     0,   346,     0,   345,   346,     0,   369,
   347,   335,     0,   318,   348,     0,   318,   348,   336,   347,
     0,     0,   320,   373,     0,     0,   350,     0,     0,   353,
   351,   354,     0,     0,   350,   353,   352,   354,     0,     7,
   317,   335,     0,     8,   355,   356,   341,   344,   362,   370,
     0,   132,   337,    12,   338,     0,   133,   337,    12,   338,
     0,   134,   337,    12,   338,     0,   135,   337,    12,   338,
     0,   136,   337,    12,   338,     0,   137,   337,    12,   338,
     0,   138,   337,     9,   318,   338,     0,   139,   337,     9,
   318,   338,     0,   140,   337,    12,   338,     0,   141,   337,
     9,   318,   336,     9,   318,   336,    11,   318,   338,     0,
   142,   337,     9,   318,   336,     9,   318,   336,    11,   318,
   338,     0,   143,   337,    11,   318,   336,    11,   318,   338,
     0,   144,   337,    11,   318,   336,    11,   318,   338,     0,
   145,   337,    11,   318,   336,    11,   318,   338,     0,   146,
   337,    11,   318,   336,    11,   318,   338,     0,   147,   337,
    11,   318,   336,    11,   318,   338,     0,   148,   337,    11,
   318,   338,     0,   149,   337,    11,   318,   338,     0,   150,
   337,    11,   318,   336,    11,   318,   336,     9,   318,   338,
     0,   151,   337,    11,   318,   336,    11,   318,   336,     9,
   318,   338,     0,   152,   337,    11,   318,   336,     9,   318,
   336,    11,   318,   338,     0,   153,   337,    11,   318,   336,
     9,   318,   338,     0,   154,   337,    11,   318,   336,     9,
   318,   338,     0,   155,   337,    11,   318,   336,     9,   318,
   336,    11,   318,   338,     0,   156,   337,    11,   318,   338,
     0,   159,   337,    11,   318,   338,     0,   160,   337,    11,
   318,   336,    11,   318,   336,    11,   318,   338,     0,   161,
   337,    11,   318,   336,    11,   318,   336,    11,   318,   338,
     0,   162,   337,    11,   318,   338,     0,   157,   337,    11,
   318,   336,    11,   318,   338,     0,   158,   337,    11,   318,
   336,    11,   318,   338,     0,     0,   357,     0,     0,   360,
   358,   361,     0,     0,   357,   360,   359,   361,     0,    20,
   317,   335,     0,     0,   363,     0,     0,     6,   363,     0,
   364,     0,   363,   364,     0,   365,    21,   318,   337,   366,
   338,   341,   344,   370,     0,    12,     0,   369,     0,    12,
     0,   367,     0,   368,     0,   368,   336,   367,     0,    17,
   369,   318,     0,   369,   318,     0,     9,     0,    10,     0,
    11,     0,    22,    23,     0,    22,   371,    23,     0,   372,
     0,   371,   372,     0,   335,     0,    16,   335,     0,    16,
   373,   335,     0,    13,   337,   373,   338,    18,   372,     0,
    13,   337,   373,   338,    18,   372,    14,   372,     0,    15,
   337,   373,   338,    19,   372,     0,   370,     0,   373,   335,
     0,   318,     0,    26,     0,    27,     0,    28,     0,    29,
     0,    30,     0,    31,     0,    32,     0,    33,     0,    34,
     0,    35,     0,    36,     0,    37,     0,    38,     0,    39,
     0,    40,     0,    41,     0,    42,     0,    43,     0,    44,
     0,    45,     0,    46,     0,    47,     0,    48,     0,    49,
     0,    50,     0,    51,     0,   132,     0,   133,     0,   134,
     0,   135,     0,   136,     0,   137,     0,   138,     0,   139,
     0,   140,     0,   141,     0,   142,     0,   143,     0,   144,
     0,   145,     0,   146,     0,   147,     0,   148,     0,   149,
     0,   150,     0,   151,     0,   152,     0,   153,     0,   154,
     0,   155,     0,   156,     0,   157,     0,   158,     0,   159,
     0,   160,     0,   161,     0,   162,     0,    52,     0,    53,
     0,    54,     0,    55,     0,    56,     0,    57,     0,    58,
     0,    59,     0,    60,     0,    61,     0,    62,     0,    63,
     0,    64,     0,    65,     0,    66,     0,    67,     0,    68,
     0,    69,     0,    70,     0,    71,     0,    72,     0,    73,
     0,    74,     0,    75,     0,    76,     0,    77,     0,    78,
     0,    79,     0,    80,     0,    81,     0,    82,     0,    83,
     0,    84,     0,    85,     0,    86,     0,    87,     0,    88,
     0,    89,     0,    90,     0,    91,     0,    92,     0,    93,
     0,    94,     0,    95,     0,    96,     0,    97,     0,    98,
     0,    99,     0,   100,     0,   101,     0,   102,     0,   103,
     0,   104,     0,   105,     0,   106,     0,   107,     0,   108,
     0,   109,     0,   110,     0,   111,     0,   112,     0,   113,
     0,   114,     0,   115,     0,   116,     0,   117,     0,   118,
     0,   119,     0,   120,     0,   121,     0,   122,     0,   123,
     0,   124,     0,   125,     0,   126,     0,   127,     0,   128,
     0,   129,     0,   130,     0,   131,     0,   316,     0,   317,
     0,   318,   320,   373,     0,   373,   324,   373,     0,   373,
   325,   373,     0,   373,   329,   373,     0,   373,   327,   373,
     0,   373,   328,   373,     0,   373,   326,   373,     0,   373,
   330,   373,     0,   373,   331,   373,     0,   373,   332,   373,
     0,   373,   333,   373,     0,   373,   334,   373,     0,   331,
   373,     0,   323,   373,     0,   373,   322,   373,     0,   373,
   321,   373,     0,   337,   373,   338,     0,   318,   337,   378,
   338,     0,   374,     0,    24,   339,   375,     0,    25,   339,
   376,     0,   318,   339,   377,     0,   303,   337,   378,   338,
     0,   304,   337,   378,   338,     0,   305,   337,   378,   338,
     0,   306,   337,   378,   338,     0,   307,   337,   378,   338,
     0,   308,   337,   378,   338,     0,   309,   337,   378,   338,
     0,   310,   337,   378,   338,     0,   311,   337,   378,   338,
     0,   312,   337,   378,   338,     0,   313,   337,   378,   338,
     0,   314,   337,   378,   338,     0,   315,   337,   378,   338,
     0,   164,   337,   378,   338,     0,   165,   337,   378,   338,
     0,   166,   337,   378,   338,     0,   167,   337,   378,   338,
     0,   168,   337,   378,   338,     0,   169,   337,   378,   338,
     0,   170,   337,   378,   338,     0,   171,   337,   378,   338,
     0,   172,   337,   378,   338,     0,   173,   337,   378,   338,
     0,   174,   337,   378,   338,     0,   175,   337,   378,   338,
     0,   176,   337,   378,   338,     0,   177,   337,   378,   338,
     0,   178,   337,   378,   338,     0,   179,   337,   378,   338,
     0,   180,   337,   378,   338,     0,   181,   337,   378,   338,
     0,   182,   337,   378,   338,     0,   183,   337,   378,   338,
     0,   184,   337,   378,   338,     0,   163,   337,   378,   338,
     0,   185,   337,   378,   338,     0,   186,   337,   378,   338,
     0,   187,   337,   378,   338,     0,   188,   337,   378,   338,
     0,   189,   337,   378,   338,     0,   190,   337,   378,   338,
     0,   191,   337,   378,   338,     0,   192,   337,   378,   338,
     0,   193,   337,   378,   338,     0,   194,   337,   378,   338,
     0,   195,   337,   378,   338,     0,   196,   337,   378,   338,
     0,   197,   337,   378,   338,     0,   198,   337,   378,   338,
     0,   199,   337,   378,   338,     0,   200,   337,   378,   338,
     0,   201,   337,   378,   338,     0,   202,   337,   378,   338,
     0,   203,   337,   378,   338,     0,   204,   337,   378,   338,
     0,   205,   337,   378,   338,     0,   206,   337,   378,   338,
     0,   207,   337,   378,   338,     0,   208,   337,   378,   338,
     0,   209,   337,   378,   338,     0,   210,   337,   378,   338,
     0,   211,   337,   378,   338,     0,   163,   337,   378,   338,
     0,   212,   337,   378,   338,     0,   213,   337,   378,   338,
     0,   214,   337,   378,   338,     0,   215,   337,   378,   338,
     0,   216,   337,   378,   338,     0,   217,   337,   378,   338,
     0,   218,   337,   378,   338,     0,   219,   337,   378,   338,
     0,   220,   337,   378,   338,     0,   221,   337,   378,   338,
     0,   222,   337,   378,   338,     0,   223,   337,   378,   338,
     0,   224,   337,   378,   338,     0,   225,   337,   378,   338,
     0,   226,   337,   378,   338,     0,   227,   337,   378,   338,
     0,   229,   337,   378,   338,     0,   228,   337,   378,   338,
     0,   230,   337,   378,   338,     0,   231,   337,   378,   338,
     0,   232,   337,   378,   338,     0,   233,   337,   378,   338,
     0,   234,   337,   378,   338,     0,   235,   337,   378,   338,
     0,   236,   337,   378,   338,     0,   237,   337,   378,   338,
     0,   238,   337,   378,   338,     0,   198,   337,   378,   338,
     0,   239,   337,   378,   338,     0,   240,   337,   378,   338,
     0,   241,   337,   378,   338,     0,   242,   337,   378,   338,
     0,   243,   337,   378,   338,     0,   211,   337,   378,   338,
     0,   244,   337,   378,   338,     0,   245,   337,   378,   338,
     0,   246,   337,   378,   338,     0,   247,   337,   378,   338,
     0,   248,   337,   378,   338,     0,   249,   337,   378,   338,
     0,   250,   337,   378,   338,     0,   251,   337,   378,   338,
     0,   252,   337,   378,   338,     0,   253,   337,   378,   338,
     0,   254,   337,   378,   338,     0,   255,   337,   378,   338,
     0,   256,   337,   378,   338,     0,   257,   337,   378,   338,
     0,   258,   337,   378,   338,     0,   259,   337,   378,   338,
     0,   260,   337,   378,   338,     0,   261,   337,   378,   338,
     0,   262,   337,   378,   338,     0,   263,   337,   378,   338,
     0,   264,   337,   378,   338,     0,   265,   337,   378,   338,
     0,   266,   337,   378,   338,     0,   267,   337,   378,   338,
     0,   268,   337,   378,   338,     0,   269,   337,   378,   338,
     0,   270,   337,   378,   338,     0,   271,   337,   378,   338,
     0,   298,   337,   378,   338,     0,   272,   337,   378,   338,
     0,   273,   337,   378,   338,     0,   274,   337,   378,   338,
     0,   275,   337,   378,   338,     0,   276,   337,   378,   338,
     0,   277,   337,   378,   338,     0,   278,   337,   378,   338,
     0,   279,   337,   378,   338,     0,   280,   337,   378,   338,
     0,   281,   337,   378,   338,     0,   282,   337,   378,   338,
     0,   283,   337,   378,   338,     0,   297,   337,   378,   338,
     0,   284,   337,   378,   338,     0,   285,   337,   378,   338,
     0,   286,   337,   378,   338,     0,   287,   337,   378,   338,
     0,   288,   337,   378,   338,     0,   289,   337,   378,   338,
     0,   290,   337,   378,   338,     0,   291,   337,   378,   338,
     0,   292,   337,   378,   338,     0,   293,   337,   378,   338,
     0,   294,   337,   378,   338,     0,   295,   337,   378,   338,
     0,   296,   337,   378,   338,     0,   163,   337,   378,   338,
     0,   299,   337,   378,   338,     0,   300,   337,   378,   338,
     0,   301,   337,   378,   338,     0,   302,   337,   378,   338,
     0,     0,   379,     0,   373,     0,   373,   336,   379,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   182,   184,   189,   191,   195,   197,   201,   206,   208,   212,
   214,   218,   222,   224,   228,   230,   235,   237,   241,   244,
   245,   248,   251,   255,   259,   261,   263,   265,   267,   269,
   271,   273,   275,   277,   281,   285,   288,   291,   294,   297,
   300,   302,   304,   308,   312,   316,   319,   322,   326,   328,
   330,   334,   338,   340,   343,   349,   351,   355,   358,   359,
   362,   365,   369,   371,   375,   377,   381,   383,   387,   391,
   393,   397,   399,   403,   405,   409,   411,   416,   418,   420,
   425,   427,   431,   433,   437,   439,   441,   443,   445,   447,
   449,   451,   456,   458,   460,   462,   464,   466,   468,   470,
   472,   474,   476,   478,   480,   482,   484,   486,   488,   490,
   492,   494,   496,   498,   500,   502,   504,   506,   508,   510,
   512,   514,   516,   518,   520,   522,   524,   526,   528,   530,
   532,   534,   536,   538,   540,   542,   544,   546,   548,   550,
   552,   554,   556,   558,   560,   562,   564,   566,   568,   570,
   572,   574,   576,   578,   580,   582,   584,   586,   588,   590,
   592,   594,   596,   598,   600,   602,   604,   606,   608,   610,
   612,   614,   616,   618,   620,   622,   624,   626,   628,   630,
   632,   634,   636,   638,   640,   642,   644,   646,   648,   650,
   652,   654,   656,   658,   660,   662,   664,   666,   668,   670,
   672,   674,   676,   678,   680,   682,   684,   686,   688,   690,
   692,   694,   696,   698,   700,   702,   704,   706,   708,   710,
   712,   714,   716,   718,   720,   722,   724,   726,   728,   730,
   732,   734,   736,   738,   740,   742,   744,   746,   748,   750,
   752,   754,   756,   758,   760,   762,   764,   766,   768,   770,
   772,   774,   776,   778,   782,   784,   786,   788,   790,   792,
   794,   796,   798,   800,   802,   804,   806,   810,   812,   814,
   816,   818,   820,   822,   824,   826,   828,   830,   832,   834,
   836,   838,   840,   842,   844,   846,   848,   850,   852,   854,
   858,   860,   862,   864,   866,   868,   870,   872,   874,   876,
   878,   880,   882,   884,   886,   888,   890,   892,   894,   896,
   898,   900,   902,   904,   906,   908,   910,   912,   914,   916,
   918,   920,   922,   924,   926,   928,   930,   932,   934,   936,
   938,   942,   944,   946,   948,   950,   952,   954,   956,   958,
   960,   962,   964,   966,   968,   970,   972,   974,   976,   978,
   980,   982,   984,   986,   988,   990,   992,   994,   996,   998,
  1000,  1002,  1004,  1006,  1008,  1010,  1012,  1014,  1016,  1018,
  1020,  1022,  1024,  1026,  1028,  1030,  1032,  1034,  1036,  1038,
  1040,  1042,  1044,  1046,  1048,  1050,  1052,  1054,  1056,  1058,
  1060,  1062,  1064,  1066,  1068,  1070,  1072,  1074,  1076,  1078,
  1080,  1082,  1084,  1086,  1088,  1090,  1092,  1094,  1096,  1098,
  1100,  1107,  1109,  1113,  1115
};

static const char * const yytname[] = {   "$","error","$undefined.","tGLOBAL",
"tCONST","tVAR","tFUNC","tCOMPILE","tSCRIPT","tNUMBER","tSTRING","tENTITY","tVOID",
"tIF","tELSE","tWHILE","tRETURN","tREF","tTHEN","tDO","tIMPORT","tFUNCTION",
"tBEGIN","tEND","tGAMEOBJ","tWORLDOBJ","tNULL","tTRUE","tFALSE","tRIGHT_HAND_SLOT",
"tLEFT_HAND_SLOT","tNORTH_DIRECTION","tNORTHEAST_DIRECTION","tEAST_DIRECTION",
"tSOUTHEAST_DIRECTION","tSOUTH_DIRECTION","tSOUTHWEST_DIRECTION","tWEST_DIRECTION",
"tNORTHWEST_DIRECTION","tNO_COMBAT_ALINGMENT","tPLAYER_COMBAT_ALINGMENT","tENEMYPLAYER_COMBAT_ALINGMENT",
"tBASE_VALUE","tTEMP_VALUE","tENTITY_PLAYER","tENTITY_SCENE_OBJ","tENTITY_NPC",
"tENTITY_WALL","tENTITY_ITEM","tTEXT_RIGHT_JUSTIFY","tTEXT_UP_JUSTIFY","tTEXT_LEFT_JUSTIFY",
"tKEY_ESC","tKEY_F1","tKEY_F2","tKEY_F3","tKEY_F4","tKEY_F5","tKEY_F6","tKEY_F7",
"tKEY_F8","tKEY_F9","tKEY_F10","tKEY_F11","tKEY_F12","tKEY_0","tKEY_1","tKEY_2",
"tKEY_3","tKEY_4","tKEY_5","tKEY_6","tKEY_7","tKEY_8","tKEY_9","tKEY_A","tKEY_B",
"tKEY_C","tKEY_D","tKEY_E","tKEY_F","tKEY_G","tKEY_H","tKEY_I","tKEY_J","tKEY_K",
"tKEY_L","tKEY_M","tKEY_N","tKEY_O","tKEY_P","tKEY_Q","tKEY_R","tKEY_S","tKEY_T",
"tKEY_U","tKEY_V","tKEY_W","tKEY_X","tKEY_Y","tKEY_Z","tKEY_BACK","tKEY_TAB",
"tKEY_RETURN","tKEY_SPACE","tKEY_LCONTROL","tKEY_RCONTROL","tKEY_LSHIFT","tKEY_RSHIFT",
"tKEY_ALT","tKEY_ALTGR","tKEY_INSERT","tKEY_REPAG","tKEY_AVPAG","tKEY_MINUS_PAD",
"tKEY_ADD_PAD","tKEY_DIV_PAD","tKEY_MUL_PAD","tKEY_0_PAD","tKEY_1_PAD","tKEY_2_PAD",
"tKEY_3_PAD","tKEY_4_PAD","tKEY_5_PAD","tKEY_6_PAD","tKEY_7_PAD","tKEY_8_PAD",
"tKEY_9_PAD","tKEY_UP","tKEY_DOWN","tKEY_RIGHT","tKEY_LEFT","tONSTARTGAME","tONCLICKHOURPANEL",
"tONFLEECOMBAT","tONKEYPRESSED","tONSTARTCOMBATMODE","tONENDCOMBATMODE","tONNEWHOUR",
"tONENTERINAREA","tONWORLDIDLE","tONSETINFLOOR","tONSETOUTOFFLOOR","tONGETITEM",
"tONDROPITEM","tONOBSERVEENTITY","tONTALKTOENTITY","tONMANIPULATEENTITY","tONDEATH",
"tONRESURRECT","tONINSERTINEQUIPMENTSLOT","tONREMOVEFROMEQUIPMENTSLOT","tONUSEHABILITY",
"tONACTIVATEDSYMPTOM","tONDEACTIVATEDSYMPTOM","tONHITENTITY","tONSTARTCOMBATTURN",
"tONCRIATUREINRANGE","tONCRIATUREOUTOFRANGE","tONENTITYIDLE","tONUSEITEM","tONTRADEITEM",
"tONENTITYCREATED","tSETSCRIPT","tQUITGAME","tWRITETOCONSOLE","tACTIVEADVICEDIALOG",
"tACTIVEQUESTIONDIALOG","tACTIVETEXTREADERDIALOG","tADDOPTIONTOTEXTSELECTORDIALOG",
"tRESETOPTIONSINTEXTSELECTORDIALOG","tACTIVETEXTSELECTORDIALOG","tPLAYMIDIMUSIC",
"tSTOPMIDIMUSIC","tPLAYWAVAMBIENTSOUND","tSTOPWAVAMBIENTSOUND","tACTIVETRADEITEMSINTERFAZ",
"tADDOPTIONTOCONVERSATORINTERFAZ","tRESETOPTIONSINCONVERSATORINTERFAZ","tACTIVECONVERSATORINTERFAZ",
"tDESACTIVECONVERSATORINTERFAZ","tGETOPTIONFROMCONVERSATORINTERFAZ","tSHOWPRESENTATION",
"tBEGINCUTSCENE","tENDCUTSCENE","tISKEYPRESSED","tGETAREANAME","tGETAREAID",
"tGETAREAWIDTH","tGETAREAHEIGHT","tGETHOUR","tGETMINUTE","tSETHOUR","tSETMINUTE",
"tGETENTITY","tGETPLAYER","tISFLOORVALID","tGETNUMITEMSAT","tGETITEMAT","tGETDISTANCE",
"tCALCULEPATHLENGHT","tLOADAREA","tCHANGEENTITYLOCATION","tATTACHCAMERATOENTITY",
"tATTACHCAMERATOLOCATION","tISCOMBATMODEACTIVE","tENDCOMBAT","tGETCRIATUREINCOMBATTURN",
"tGETCOMBATANT","tGETNUMBEROFCOMBATANTS","tGETAREALIGHTMODEL","tSETIDLESCRIPTTIME",
"tDESTROYENTITY","tCREATECRIATURE","tCREATEWALL","tCREATESCENARYOBJECT","tCREATEITEMABANDONED",
"tCREATEITEMWITHOWNER","tSETWORLDTIMEPAUSE","tISWORLDTIMEINPAUSE","tSETELEVATIONAT",
"tGETELEVATIONAT","tNEXTTURN","tGETLIGHTAT","tPLAYWAVSOUND","tSETSCRIPTAT","tGETNAME",
"tSETNAME","tGETTYPE","tGETENTITYTYPE","tSAY","tSHUTUP","tISSAYING","tATTACHGFX",
"tRELEASEGFX","tRELEASEALLGFX","tISGFXATTACHED","tGETNUMITEMSINCONTAINER","tISITEMINCONTAINER",
"tTRANSFERITEMTOCONTAINER","tINSERTITEMINCONTAINER","tREMOVEITEMOFCONTAINER",
"tSETANIMTEMPLATESTATE","tSETPORTRAITANIMTEMPLATESTATE","tSETLIGHT","tGETLIGHT",
"tGETXPOS","tGETYPOS","tGETELEVATION","tSETELEVATION","tGETLOCALATTRIBUTE","tSETLOCALATTRIBUTE",
"tGETOWNER","tGETCLASS","tGETINCOMBATUSECOST","tGETGLOBALATTRIBUTE","tSETGLOBALATTRIBUTE",
"tGETWALLORIENTATION","tBLOCKACCESS","tUNBLOCKACCESS","tISACCESSBLOCKED","tSETSYMPTOM",
"tISSYMPTOMACTIVE","tGETGENRE","tGETHEALTH","tSETHEALTH","tGETEXTENDEDATTRIBUTE",
"tSETEXTENDEDATTRIBUTE","tGETLEVEL","tSETLEVEL","tGETEXPERIENCE","tSETEXPERIENCE",
"tGETACTIONPOINTS","tSETACTIONPOINTS","tISHABILITYACTIVE","tSETHABILITY","tUSEHABILITY",
"tISRUNMODEACTIVE","tSETRUNMODE","tMOVETO","tISMOVING","tSTOPMOVING","tEQUIPITEM",
"tREMOVEITEMEQUIPPED","tISITEMEQUIPPED","tDROPITEM","tUSEITEM","tMANIPULATE",
"tSETTRANSPARENTMODE","tISTRANSPARENTMODEACTIVE","tCHANGEANIMORIENTATION","tGETANIMORIENTATION",
"tSETALINGMENT","tSETALINGMENTWITH","tSETALINGMENTAGAINST","tGETALINGMENT","tHITENTITY",
"tGETITEMEQUIPPED","tGETINCOMBATACTIONPOINTS","tISGHOSTMOVEMODEACTIVE","tSETGHOSTMOVEMODE",
"tGETRANGE","tISINRANGE","tAPIPASSTORGBCOLOR","tAPIGETREDCOMPONENT","tAPIGETGREENCOMPONENT",
"tAPIGETBLUECOMPONENT","tAPIRAND","tAPIGETINTEGERVALUE","tAPIGETDECIMALVALUE",
"tAPIGETSTRINGSIZE","tAPIWRITETOLOGGER","tAPIENABLECRISOLSCRIPTWARNINGS","tAPIDISABLECRISOLSCRIPTWARNINGS",
"tAPISHOWFPS","tAPIWAIT","tNUMBER_VALUE","tSTRING_VALUE","tIDENTIFIER","IFX",
"tASSING","tOR","tAND","'!'","tEQ","tNEQ","tGEQ","tLEQ","'>'","'<'","'+'","'-'",
"'*'","'/'","'%'","';'","','","'('","')'","'.'","global","const","const_decl_seq",
"const_decl","var","var_decl_seq","var_decl","var_identifier_seq","var_init_decl",
"scripts","script_decl_seq","@1","@2","compile_script","script_decl","script_type",
"import","import_seq","@3","@4","import_decl","g_function","l_function","function_decl_seq",
"function_decl","return_type","arguments","argument_decl_seq","argument_decl",
"type","scope_stm","stm_seq","stm","exp","api_function_call","gameobj_method_call",
"worldobj_method_call","entityobj_method_call","params","params_seq",""
};
#endif

static const short yyr1[] = {     0,
   340,   340,   341,   341,   342,   342,   343,   344,   344,   345,
   345,   346,   347,   347,   348,   348,   349,   349,   351,   350,
   352,   350,   353,   354,   355,   355,   355,   355,   355,   355,
   355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
   355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
   355,   355,   355,   355,   355,   356,   356,   358,   357,   359,
   357,   360,   361,   361,   362,   362,   363,   363,   364,   365,
   365,   366,   366,   367,   367,   368,   368,   369,   369,   369,
   370,   370,   371,   371,   372,   372,   372,   372,   372,   372,
   372,   372,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   373,   373,   373,   373,   373,   373,
   373,   373,   373,   373,   374,   374,   374,   374,   374,   374,
   374,   374,   374,   374,   374,   374,   374,   375,   375,   375,
   375,   375,   375,   375,   375,   375,   375,   375,   375,   375,
   375,   375,   375,   375,   375,   375,   375,   375,   375,   375,
   376,   376,   376,   376,   376,   376,   376,   376,   376,   376,
   376,   376,   376,   376,   376,   376,   376,   376,   376,   376,
   376,   376,   376,   376,   376,   376,   376,   376,   376,   376,
   376,   376,   376,   376,   376,   376,   376,   376,   376,   376,
   376,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   377,   377,   377,   377,   377,   377,   377,   377,   377,
   377,   378,   378,   379,   379
};

static const short yyr2[] = {     0,
     0,     4,     0,     2,     1,     2,     5,     0,     2,     1,
     2,     3,     2,     4,     0,     2,     0,     1,     0,     3,
     0,     4,     3,     7,     4,     4,     4,     4,     4,     4,
     5,     5,     4,    11,    11,     8,     8,     8,     8,     8,
     5,     5,    11,    11,    11,     8,     8,    11,     5,     5,
    11,    11,     5,     8,     8,     0,     1,     0,     3,     0,
     4,     3,     0,     1,     0,     2,     1,     2,     9,     1,
     1,     1,     1,     1,     3,     3,     2,     1,     1,     1,
     2,     3,     1,     2,     1,     2,     3,     6,     8,     6,
     1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     2,     2,     3,     3,     3,     4,
     1,     3,     3,     3,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     0,     1,     1,     3
};

static const short yydefact[] = {     1,
     3,     0,     8,    78,    79,    80,     4,     5,     0,     0,
    17,     6,     0,     9,    10,     0,     0,     2,    18,    19,
     0,    11,    15,     0,     0,    21,     0,     0,     0,    94,
    95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   115,   116,   117,   118,   119,   151,   152,   153,   154,   155,
   156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
   186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
   196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
   206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
   216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
   226,   227,   228,   229,   230,   120,   121,   122,   123,   124,
   125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,   149,   150,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   231,
   232,    93,     0,     0,     0,     0,   251,     0,    13,    12,
    23,     0,     0,    20,     0,     0,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,     0,
   412,     0,   246,   245,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     7,    16,
     0,    22,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    56,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   252,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   253,
   414,     0,   413,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   233,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   254,   249,   248,
   247,   234,   235,   239,   237,   238,   236,   240,   241,   242,
   243,   244,    14,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     3,    57,    58,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,     0,   255,   256,   257,   258,   259,   260,   261,
   262,   263,   264,   265,   266,   267,   250,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
    60,    63,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   415,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    25,    26,    27,
    28,    29,    30,     0,     0,    33,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    62,    65,
    63,    70,    59,    64,    67,     0,    71,   289,   268,   269,
   270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
   280,   281,   282,   283,   284,   285,   286,   287,   288,   290,
   317,   291,   292,   293,   294,   295,   296,   297,   298,   299,
   300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
   310,   311,   312,   313,   314,   315,   316,   318,   319,   320,
   321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
   331,   407,   345,   351,   332,   333,   335,   334,   336,   337,
   338,   339,   340,   341,   342,   343,   344,   346,   347,   348,
   349,   350,   352,   353,   354,   355,   356,   357,   358,   359,
   360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
   370,   371,   372,   373,   374,   375,   376,   377,   378,   379,
   381,   382,   383,   384,   385,   386,   387,   388,   389,   390,
   391,   392,   394,   395,   396,   397,   398,   399,   400,   401,
   402,   403,   404,   405,   406,   393,   380,   408,   409,   410,
   411,    31,    32,     0,     0,     0,     0,     0,     0,     0,
    41,    42,     0,     0,     0,     0,     0,     0,    49,     0,
     0,    50,     0,     0,    53,     0,     0,    61,    68,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    66,     0,    24,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    81,    85,    91,     0,    83,     0,     0,     0,     0,
    36,    37,    38,    39,    40,     0,     0,     0,    46,    47,
     0,    54,    55,     0,     0,     0,     0,    86,     0,    82,
    84,    92,    72,     0,     0,    73,    74,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    87,     0,
     3,     0,    77,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    76,     8,    75,    34,    35,    43,    44,
    45,    48,    51,    52,     0,     0,     0,    88,    90,    69,
     0,    89,     0,     0,     0
};

static const short yydefgoto[] = {  1153,
     3,     7,     8,    11,    14,    15,    24,   189,    18,    19,
    27,   192,    20,   194,   264,   476,   477,   672,   851,   478,
   853,  1027,   854,   855,   856,  1105,  1106,  1107,   857,  1074,
  1075,  1076,   331,   187,   288,   330,   428,   332,   333
};

static const short yypact[] = {     2,
     3,   183,     5,-32768,-32768,-32768,   183,-32768,  -310,   183,
     4,-32768,  -290,   183,-32768,  -267,  -264,-32768,     4,-32768,
  1560,-32768,  -254,  -191,  -147,-32768,   152,  -149,  -126,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -123,  -114,  -113,  -112,
  -111,  -110,  -109,  -108,  -107,   -43,    24,   117,   118,-32768,
-32768,  -308,  1560,  1560,  1560,    38,-32768,  1560,   120,-32768,
-32768,   152,   478,-32768,   108,    45,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  -159,  -269,  -115,  -284,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,-32768,   369,
  -267,-32768,   121,   122,   124,   127,   128,   129,   130,   304,
   305,   306,   307,   322,   340,   355,   367,   368,   370,   371,
   372,   373,   374,   375,   376,   377,   378,   379,   380,   381,
   382,   383,   384,   202,   385,   386,   387,   388,   389,   390,
   391,   392,   393,   394,   395,   396,   397,   398,   399,   400,
   401,   402,   403,   404,   405,   406,   407,-32768,   408,   409,
   410,   411,   412,   429,   430,   431,   432,   434,   435,   436,
   437,   438,   439,   440,   442,   443,   444,   446,   448,   449,
   450,   452,   455,   456,   457,   458,   459,   601,   602,   603,
   604,   605,   606,   607,   608,   609,   610,   611,   612,-32768,
  -175,   119,-32768,   613,   614,   615,   616,   617,   618,   619,
   620,   621,   622,   623,   624,   369,   625,   627,   628,   629,
   630,   631,   632,   633,   634,   635,   636,   637,   638,   639,
   640,   641,   642,   643,   644,   645,   646,   647,   648,   649,
   650,   651,   652,   653,   654,   655,   656,   657,   658,   659,
   660,   661,   662,   663,   664,   665,   666,   667,   668,   669,
   670,   671,   672,   673,   674,   675,   676,   677,   678,   679,
   680,   681,   682,   683,   684,   685,   686,   687,   688,   689,
   690,   692,   693,   694,   695,   696,   697,   698,   699,   700,
   701,   702,   703,   704,   705,   706,   707,-32768,-32768,  -269,
  -269,  -298,  -298,  -298,  -298,  -298,  -298,  -115,  -115,   711,
   711,-32768,-32768,   938,  1034,  1035,  1036,  1037,  1039,  1043,
  1044,  1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,
  1054,  1055,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,
  1083,  1084,  1085,  1086,   741,     3,   202,-32768,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,  1560,
  1560,  1560,  1560,  1560,  1560,  1560,  1560,   761,   762,   763,
   764,   766,   767,   789,   790,   771,   793,   795,   796,   797,
   798,   799,   800,   801,   808,   809,   810,  1091,  1108,  1109,
  1110,  1111,  1113,  1114,  1115,  1116,  1117,  1118,  1102,     5,
-32768,   200,  1101,  1103,  1105,  1112,  1119,  1120,  1121,  1122,
  1123,  1124,  1125,  1126,  1127,  1128,  1129,  1130,  1131,  1132,
  1133,  1134,  1135,  1136,  1137,  1138,  1139,  1140,  1141,  1142,
  1143,  1144,  1145,  1146,  1147,  1148,  1149,  1150,  1151,  1152,
  1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,  1161,  1162,
  1163,  1164,  1165,  1166,  1167,  1168,  1169,  1170,  1171,  1172,
  1173,  1174,  1175,  1176,  1177,  1178,-32768,  1179,  1180,  1181,
  1182,  1183,  1184,  1185,  1186,  1187,  1188,  1189,  1190,  1191,
  1192,  1193,  1194,  1195,  1196,  1197,  1198,  1199,  1200,  1201,
  1202,  1203,  1204,  1205,  1206,  1207,  1208,  1209,  1210,  1227,
  1228,  1229,  1230,  1232,  1233,  1234,  1235,  1236,  1237,  1238,
  1240,  1241,  1242,  1244,  1385,  1386,  1387,  1388,  1389,  1390,
  1391,  1392,  1393,  1394,  1395,  1396,  1397,  1398,  1399,  1400,
  1401,  1402,  1403,  1404,  1405,  1406,  1407,  1408,  1409,  1410,
  1411,  1412,  1413,  1414,  1415,  1416,  1417,-32768,-32768,-32768,
-32768,-32768,-32768,  1418,  1419,-32768,  1104,  1422,  1423,  1424,
  1425,  1426,  1427,  1428,  1429,  1432,  1433,  1434,  1435,  1436,
  1437,  1438,  1439,  1441,  1440,  1443,  1444,  1445,-32768,  1442,
   200,-32768,-32768,   200,-32768,  1430,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  1446,  1447,  1753,  1754,  1763,  1770,  1771,
-32768,-32768,  1773,  1774,  1777,  1778,  1779,  1780,-32768,  1781,
  1782,-32768,  1783,  1784,-32768,   200,  1431,-32768,-32768,  1472,
  1473,  1478,  1479,  1480,  1481,  1482,  1483,  1484,  1485,  1486,
  1487,  1488,  1489,  1490,  1491,  1492,  1493,   200,   447,-32768,
  1475,  1477,  1494,  1476,  1495,  1496,  1497,  1498,  1501,  1502,
  1503,  1504,  1505,  1508,  1507,  1509,  1510,  1512,  1513,  1514,
  1246,-32768,-32768,-32768,   775,-32768,   339,   174,  1804,  1805,
-32768,-32768,-32768,-32768,-32768,  1808,  1809,  1810,-32768,-32768,
  1811,-32768,-32768,  1812,  1813,  1560,  1560,-32768,   354,-32768,
-32768,-32768,-32768,   183,  1511,-32768,  1516,  1522,  1523,  1535,
  1536,  1537,  1538,  1539,  1540,  1541,  -156,   324,-32768,  1542,
     3,   170,-32768,  1524,  1543,  1544,  1546,  1547,  1548,  1549,
  1550,  1801,  1806,-32768,     5,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  1107,  1107,  1431,  1814,-32768,-32768,
  1107,-32768,  1449,  1452,-32768
};

static const short yypgoto[] = {-32768,
  -474,-32768,  1819,  -667,-32768,  1815,   475,-32768,-32768,-32768,
-32768,-32768,  1842,  1253,-32768,-32768,-32768,-32768,-32768,   969,
   596,-32768,   428,  -828,-32768,-32768,   709,-32768,    -1,  -999,
-32768,  -930,   -21,-32768,-32768,-32768,-32768,  -184,  1277
};


#define	YYLAST		1897


static const short yytable[] = {   186,
     9,   670,   850,   348,     1,     9,     2,    13,    16,    10,
    17,   210,    16,   334,   335,   336,   337,   338,   339,   340,
   341,   342,   343,   344,   345,  1029,   347,  1050,   211,    21,
   212,   224,   225,   226,   227,   228,   216,   217,   349,   218,
   219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
    23,   350,    25,   429,   218,   219,   220,   221,   222,   223,
   224,   225,   226,   227,   228,   188,   351,   352,   353,   354,
   355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
   365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
   375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
   385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
   395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
   405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
   415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
   425,   426,   427,   190,  1101,   216,   217,  1150,   218,   219,
   220,   221,   222,   223,   224,   225,   226,   227,   228,   193,
   543,   213,   214,   215,   216,   217,   230,   218,   219,   220,
   221,   222,   223,   224,   225,   226,   227,   228,     4,     5,
     6,  1132,     4,     5,     6,  1103,  1104,   191,   346,   195,
  1104,     4,     5,     6,   430,   431,   432,   433,   434,   435,
   436,   437,   438,   439,   440,   441,   442,   289,     4,     5,
     6,   852,   196,   197,  1148,  1149,   226,   227,   228,  1029,
  1152,   475,   198,   199,   200,   201,   202,   203,   204,   205,
   290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
   300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
   310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
   320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
   265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
   275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
   285,   286,   287,   206,   673,   674,   675,   676,   677,   678,
   679,   680,   681,   682,   683,   684,   685,   686,   687,   688,
   689,   690,   691,   692,   693,   694,   695,   696,   697,   698,
   699,   700,   701,   702,   703,   704,   705,   706,   707,   708,
   709,   710,   711,   712,   713,   714,   715,   716,   717,   718,
   719,   720,   721,   722,   723,   724,   725,   726,   727,   728,
   729,   730,   731,   732,   733,   734,   735,   736,   216,   217,
   207,   218,   219,   220,   221,   222,   223,   224,   225,   226,
   227,   228,   229,   738,   739,   740,   741,   742,   743,   744,
   745,   746,   747,   748,   749,   750,   751,   752,   753,   754,
   755,   756,   757,   758,   759,   760,   761,   762,   763,   764,
   765,   766,   767,   768,   769,   770,   771,   772,   773,   774,
   775,   776,   777,   778,   779,   780,   781,   782,   783,   784,
   785,   786,   787,   788,   789,   790,   791,   792,   793,   794,
   795,   796,   797,   798,   799,   800,   801,   802,   803,   804,
   805,   806,   807,   808,   809,   810,   811,   812,   813,   814,
   815,   816,   817,   208,   209,   231,   544,   444,   445,  1069,
   446,  1070,  1071,   447,   448,   449,   450,  1147,  1049,  1072,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
    98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
   108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
   128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
   148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
   158,   159,   160,   161,   162,   163,   164,   165,   166,   233,
   234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
   244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
   254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
   451,   452,   453,   454,   216,   217,  1135,   218,   219,   220,
   221,   222,   223,   224,   225,   226,   227,   228,   455,   216,
   217,  1133,   218,   219,   220,   221,   222,   223,   224,   225,
   226,   227,   228,  1102,   216,   217,   456,   218,   219,   220,
   221,   222,   223,   224,   225,   226,   227,   228,  1119,   216,
   217,   457,   218,   219,   220,   221,   222,   223,   224,   225,
   226,   227,   228,   458,   459,   443,   460,   461,   462,   463,
   464,   465,   466,   467,   468,   469,   470,   471,   472,   473,
   474,   479,   480,   481,   482,   483,   484,   485,   486,   487,
   488,   489,   490,   491,   492,   493,   494,   495,   496,   497,
   498,   499,   500,   501,   502,   503,   504,   505,   506,   167,
   168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
   178,   179,   180,   181,   182,   507,   508,   509,   510,   183,
   511,   512,   513,   514,   515,   516,   517,   184,   518,   519,
   520,  1073,   521,   185,   522,   523,   524,  1069,   525,  1070,
  1071,   526,   527,   528,   529,   530,  1049,  1100,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
    90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
   100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
   110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
   120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
   150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
   160,   161,   162,   163,   164,   165,   166,   531,   532,   533,
   534,   535,   536,   537,   538,   539,   540,   541,   542,   638,
   545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
   555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
   565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
   575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
   585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
   595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
   605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
   615,   616,   617,   618,   619,   620,   621,  1077,   622,   623,
   624,   625,   626,   627,   628,   629,   630,   631,   632,   633,
   634,   635,   636,   637,   228,   639,   640,   641,   642,  1099,
   643,   644,   645,  1077,   647,   648,   646,   669,   649,   650,
   651,   652,   653,   654,   655,   656,   657,   658,   659,   660,
   661,   662,   663,   664,  1117,  1118,  1108,   167,   168,   169,
   170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
   180,   181,   182,   665,   666,   667,   668,   183,   818,   819,
   820,   821,  1120,   822,   823,   184,   824,   825,   826,  1073,
   827,   185,   828,   829,   830,   831,   832,   833,   834,  1069,
  1108,  1070,  1071,  1077,  1077,   835,   836,   837,  1049,  1077,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
    98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
   108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
   128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
   148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
   158,   159,   160,   161,   162,   163,   164,   165,   166,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
    89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
    99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
   109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
   119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
   149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
   159,   160,   161,   162,   163,   164,   165,   166,   838,   167,
   168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
   178,   179,   180,   181,   182,   839,   840,   841,   842,   183,
   843,   844,   845,   846,   847,   848,   849,   184,   858,  1004,
   859,  1073,   860,   185,   232,   671,  1028,  1026,  1154,   861,
  1030,  1155,  1049,  1048,  1031,  1032,   862,   863,   864,   865,
   866,   867,   868,   869,   870,   871,   872,   873,   874,   875,
   876,   877,   878,   879,   880,   881,   882,   883,   884,   885,
   886,   887,   888,   889,   890,   891,   892,   893,   894,   895,
   896,   897,   898,   899,   900,   901,   902,   903,   904,   905,
   906,   907,   908,   909,   910,   911,   912,   913,   914,   915,
   916,   917,   918,   919,   920,   921,   922,   923,   924,   925,
   926,   927,   928,   929,   930,   931,   932,   933,   934,   935,
   936,   937,   938,   939,   940,   941,   942,   943,   944,   945,
   946,   947,   948,   949,   950,   951,   952,   953,   167,   168,
   169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
   179,   180,   181,   182,   954,   955,   956,   957,   183,   958,
   959,   960,   961,   962,   963,   964,   184,   965,   966,   967,
  1098,   968,   185,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
   125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   969,   970,   971,   972,   973,   974,   975,   976,
   977,   978,   979,   980,   981,   982,   983,   984,   985,   986,
   987,   988,   989,   990,   991,   992,   993,   994,   995,   996,
   997,   998,   999,  1000,  1001,  1002,  1003,  1005,  1006,  1007,
  1008,  1009,  1010,  1033,  1034,  1011,  1012,  1013,  1014,  1015,
  1016,  1017,  1018,  1035,  1020,  1019,  1021,  1022,  1023,  1024,
  1036,  1037,  1025,  1038,  1039,  1040,  1041,  1042,  1043,  1051,
  1052,  1044,  1045,  1046,  1047,  1053,  1054,  1055,  1056,  1057,
  1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,
  1068,  1078,  1079,  1081,  1109,  1110,  1111,  1112,  1145,   737,
  1113,  1114,  1115,  1116,  1146,    12,     0,  1151,    22,  1080,
  1136,     0,  1082,  1083,  1084,  1085,  1086,  1087,  1088,  1123,
  1124,  1089,  1090,  1091,  1092,  1094,  1093,  1095,  1121,  1096,
  1097,  1122,  1125,  1126,  1127,  1128,  1129,  1130,  1131,  1134,
    26,  1137,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   177,   178,   179,   180,   181,   182,     0,     0,
  1138,  1139,   183,  1140,  1141,  1142,  1143,  1144,     0,     0,
   184,     0,     0,     0,     0,     0,   185
};

static const short yycheck[] = {    21,
     2,   476,   670,   163,     3,     7,     4,   318,    10,     5,
     7,   320,    14,   198,   199,   200,   201,   202,   203,   204,
   205,   206,   207,   208,   209,   854,   211,  1027,   337,   320,
   339,   330,   331,   332,   333,   334,   321,   322,   198,   324,
   325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
   318,   211,   317,   338,   324,   325,   326,   327,   328,   329,
   330,   331,   332,   333,   334,   320,   226,   227,   228,   229,
   230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
   240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
   250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
   260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
   270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
   280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
   290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
   300,   301,   302,   335,  1075,   321,   322,  1147,   324,   325,
   326,   327,   328,   329,   330,   331,   332,   333,   334,     8,
   336,   183,   184,   185,   321,   322,   188,   324,   325,   326,
   327,   328,   329,   330,   331,   332,   333,   334,     9,    10,
    11,   338,     9,    10,    11,    12,    17,   335,   210,   339,
    17,     9,    10,    11,   216,   217,   218,   219,   220,   221,
   222,   223,   224,   225,   226,   227,   228,   163,     9,    10,
    11,    12,   339,   337,  1145,  1146,   332,   333,   334,  1048,
  1151,    20,   337,   337,   337,   337,   337,   337,   337,   337,
   186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
   196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
   206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
   216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
   163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
   173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
   183,   184,   185,   337,   479,   480,   481,   482,   483,   484,
   485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
   495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
   505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
   515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
   525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
   535,   536,   537,   538,   539,   540,   541,   542,   321,   322,
   337,   324,   325,   326,   327,   328,   329,   330,   331,   332,
   333,   334,   335,   558,   559,   560,   561,   562,   563,   564,
   565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
   575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
   585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
   595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
   605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
   615,   616,   617,   618,   619,   620,   621,   622,   623,   624,
   625,   626,   627,   628,   629,   630,   631,   632,   633,   634,
   635,   636,   637,   337,   337,   336,   338,   337,   337,    13,
   337,    15,    16,   337,   337,   337,   337,  1135,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
   114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
   124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
   154,   155,   156,   157,   158,   159,   160,   161,   162,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
   153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
   337,   337,   337,   337,   321,   322,  1121,   324,   325,   326,
   327,   328,   329,   330,   331,   332,   333,   334,   337,   321,
   322,   338,   324,   325,   326,   327,   328,   329,   330,   331,
   332,   333,   334,   335,   321,   322,   337,   324,   325,   326,
   327,   328,   329,   330,   331,   332,   333,   334,   335,   321,
   322,   337,   324,   325,   326,   327,   328,   329,   330,   331,
   332,   333,   334,   337,   337,   231,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   303,
   304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
   314,   315,   316,   317,   318,   337,   337,   337,   337,   323,
   337,   337,   337,   337,   337,   337,   337,   331,   337,   337,
   337,   335,   337,   337,   337,   337,   337,    13,   337,    15,
    16,   337,   337,   337,   337,   337,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
   126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,   160,   161,   162,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,    12,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   337,   337,   337,  1049,   337,   337,
   337,   337,   337,   337,   337,   337,   337,   337,   337,   337,
   337,   337,   337,   337,   334,    12,    12,    12,    12,  1071,
    12,     9,     9,  1075,     9,     9,    12,   317,    11,    11,
    11,    11,    11,    11,    11,    11,    11,    11,    11,    11,
    11,    11,    11,    11,  1096,  1097,  1078,   303,   304,   305,
   306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
   316,   317,   318,    11,    11,    11,    11,   323,   338,   338,
   338,   338,  1104,   338,   338,   331,   318,   318,   338,   335,
   318,   337,   318,   318,   318,   318,   318,   318,   318,    13,
  1122,    15,    16,  1145,  1146,   318,   318,   318,    22,  1151,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
   114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
   124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
   154,   155,   156,   157,   158,   159,   160,   161,   162,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
   125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,   159,   160,   161,   162,   318,   303,
   304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
   314,   315,   316,   317,   318,   318,   318,   318,   318,   323,
   318,   318,   318,   318,   318,   318,   335,   331,   338,   336,
   338,   335,   338,   337,   192,   477,   851,     6,     0,   338,
    21,     0,    22,  1026,     9,     9,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   303,   304,
   305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
   315,   316,   317,   318,   338,   338,   338,   338,   323,   338,
   338,   338,   338,   338,   338,   338,   331,   338,   338,   338,
   335,   338,   337,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
   101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
   121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
   151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
   161,   162,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
   338,   338,   338,   338,   338,   338,   338,   336,   336,   336,
   336,   336,   336,    11,    11,   338,   338,   336,   336,   336,
   336,   336,   336,    11,   336,   338,   336,   338,   336,   336,
    11,    11,   338,    11,    11,     9,     9,     9,     9,   318,
   318,    11,    11,    11,    11,   318,   318,   318,   318,   318,
   318,   318,   318,   318,   318,   318,   318,   318,   318,   318,
   318,   337,   336,   338,    11,    11,     9,     9,    18,   543,
    11,    11,    11,    11,    19,     7,    -1,    14,    14,   336,
  1122,    -1,   338,   338,   338,   338,   336,   336,   336,   318,
   318,   338,   338,   336,   338,   336,   338,   336,   338,   337,
   337,   336,   318,   318,   318,   318,   318,   318,   318,   318,
    19,   338,   303,   304,   305,   306,   307,   308,   309,   310,
   311,   312,   313,   314,   315,   316,   317,   318,    -1,    -1,
   338,   338,   323,   338,   338,   338,   338,   338,    -1,    -1,
   331,    -1,    -1,    -1,    -1,    -1,   337
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 183 "crisolscript.y"
{ pGlobal = NULL; ;
    break;}
case 2:
#line 185 "crisolscript.y"
{ pGlobal = MakeGlobal(szGlobalsFile, yyvsp[-2].pConst, yyvsp[-1].pVar, yyvsp[0].pScript); ;
    break;}
case 3:
#line 190 "crisolscript.y"
{ yyval.pConst = NULL; ;
    break;}
case 4:
#line 192 "crisolscript.y"
{ yyval.pConst = yyvsp[0].pConst; ;
    break;}
case 5:
#line 196 "crisolscript.y"
{ yyval.pConst = yyvsp[0].pConst; ;
    break;}
case 6:
#line 198 "crisolscript.y"
{ yyval.pConst = MakeConstDeclSeq(yyvsp[-1].pConst, yyvsp[0].pConst); ;
    break;}
case 7:
#line 202 "crisolscript.y"
{ yyval.pConst = MakeConstDecl(yyvsp[-4].pType, yyvsp[-3].szIdentifier, yyvsp[-1].pExp); ;
    break;}
case 8:
#line 207 "crisolscript.y"
{ yyval.pVar = NULL; ;
    break;}
case 9:
#line 209 "crisolscript.y"
{ yyval.pVar = yyvsp[0].pVar; ;
    break;}
case 10:
#line 213 "crisolscript.y"
{ yyval.pVar = yyvsp[0].pVar; ;
    break;}
case 11:
#line 215 "crisolscript.y"
{ yyval.pVar = MakeVarTypeDeclSeq(yyvsp[-1].pVar, yyvsp[0].pVar); ;
    break;}
case 12:
#line 219 "crisolscript.y"
{ yyval.pVar = MakeVarTypeDecl(yyvsp[-2].pType, yyvsp[-1].pVar); ;
    break;}
case 13:
#line 223 "crisolscript.y"
{ yyval.pVar = MakeVarIdentDeclSeq(yyvsp[-1].szIdentifier, yyvsp[0].pExp, NULL); ;
    break;}
case 14:
#line 225 "crisolscript.y"
{ yyval.pVar = MakeVarIdentDeclSeq(yyvsp[-3].szIdentifier, yyvsp[-2].pExp, yyvsp[0].pVar); ;
    break;}
case 15:
#line 229 "crisolscript.y"
{ yyval.pExp = NULL; ;
    break;}
case 16:
#line 231 "crisolscript.y"
{ yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 17:
#line 236 "crisolscript.y"
{ yyval.pScript = NULL; ;
    break;}
case 18:
#line 238 "crisolscript.y"
{ yyval.pScript = yyvsp[0].pScript; ;
    break;}
case 19:
#line 242 "crisolscript.y"
{ SetScriptFileBuffer(yyvsp[0].szString); ;
    break;}
case 20:
#line 244 "crisolscript.y"
{ yyval.pScript = yyvsp[0].pScript; ;
    break;}
case 21:
#line 246 "crisolscript.y"
{ SetScriptFileBuffer(yyvsp[0].szString); ;
    break;}
case 22:
#line 248 "crisolscript.y"
{ yyval.pScript = MakeScriptSeq(yyvsp[-3].pScript, yyvsp[0].pScript); ;
    break;}
case 23:
#line 252 "crisolscript.y"
{ yyval.szString = yyvsp[-1].szString; ;
    break;}
case 24:
#line 256 "crisolscript.y"
{ yyval.pScript = MakeScriptDecl(GetActFileName(), yyvsp[-5].pScriptType, yyvsp[-4].pImport, yyvsp[-3].pConst, yyvsp[-2].pVar, yyvsp[-1].pFunc, yyvsp[0].pStm); ;
    break;}
case 25:
#line 260 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnStartGame(); ;
    break;}
case 26:
#line 262 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnClickHourPanel(); ;
    break;}
case 27:
#line 264 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnFleeCombat(); ;
    break;}
case 28:
#line 266 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnKeyPressed(); ;
    break;}
case 29:
#line 268 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnStartCombatMode(); ;
    break;}
case 30:
#line 270 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnEndCombatMode(); ;
    break;}
case 31:
#line 272 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnNewHour(yyvsp[-1].szIdentifier); ;
    break;}
case 32:
#line 274 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnEnterInArea(yyvsp[-1].szIdentifier); ;
    break;}
case 33:
#line 276 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnWorldIdle(); ;
    break;}
case 34:
#line 280 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnSetInFloor(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 35:
#line 284 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnSetOutOfFloor(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 36:
#line 287 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnGetItem(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 37:
#line 290 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnDropItem(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 38:
#line 293 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnObserveEntity(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 39:
#line 296 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnTalkToEntity(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 40:
#line 299 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnManipulateEntity(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 41:
#line 301 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnDeath(yyvsp[-1].szIdentifier); ;
    break;}
case 42:
#line 303 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnResurrect(yyvsp[-1].szIdentifier); ;
    break;}
case 43:
#line 307 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnInsertInEquipmentSlot(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 44:
#line 311 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnRemoveFromEquipmentSlot(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 45:
#line 315 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnUseHability(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 46:
#line 318 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnActivatedSymptom(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 47:
#line 321 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnDeactivatedSymptom(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 48:
#line 325 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnHitEntity(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 49:
#line 327 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnStartCombatTurn(yyvsp[-1].szIdentifier); ;
    break;}
case 50:
#line 329 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnEntityIdle(yyvsp[-1].szIdentifier); ;
    break;}
case 51:
#line 333 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnUseItem(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 52:
#line 337 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnTradeItem(yyvsp[-7].szIdentifier, yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 53:
#line 339 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnEntityCreated(yyvsp[-1].szIdentifier); ;
    break;}
case 54:
#line 342 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnCriatureInRange(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 55:
#line 345 "crisolscript.y"
{ yyval.pScriptType = MakeScriptTypeOnCriatureOutOfRange(yyvsp[-4].szIdentifier, yyvsp[-1].szIdentifier); ;
    break;}
case 56:
#line 350 "crisolscript.y"
{ yyval.pImport = NULL; ;
    break;}
case 57:
#line 352 "crisolscript.y"
{ yyval.pImport = yyvsp[0].pImport; ;
    break;}
case 58:
#line 356 "crisolscript.y"
{ SetImportFileBuffer(yyvsp[0].szString); ;
    break;}
case 59:
#line 358 "crisolscript.y"
{ yyval.pImport = MakeImportFunc(yyvsp[-2].szString, yyvsp[0].pFunc); ;
    break;}
case 60:
#line 360 "crisolscript.y"
{ SetImportFileBuffer(yyvsp[0].szString); ;
    break;}
case 61:
#line 362 "crisolscript.y"
{ yyval.pImport = MakeImportSeq(yyvsp[-3].pImport, MakeImportFunc(yyvsp[-2].szString, yyvsp[0].pFunc)); ;
    break;}
case 62:
#line 365 "crisolscript.y"
{ yyval.szString = yyvsp[-1].szString; ;
    break;}
case 63:
#line 370 "crisolscript.y"
{ yyval.pFunc = NULL; ;
    break;}
case 64:
#line 372 "crisolscript.y"
{ yyval.pFunc = yyvsp[0].pFunc; ;
    break;}
case 65:
#line 376 "crisolscript.y"
{ yyval.pFunc = NULL; ;
    break;}
case 66:
#line 378 "crisolscript.y"
{ yyval.pFunc = yyvsp[0].pFunc; ;
    break;}
case 67:
#line 382 "crisolscript.y"
{ yyval.pFunc = yyvsp[0].pFunc; ;
    break;}
case 68:
#line 384 "crisolscript.y"
{ yyval.pFunc = MakeFuncSeq(yyvsp[-1].pFunc, yyvsp[0].pFunc); ;
    break;}
case 69:
#line 388 "crisolscript.y"
{ yyval.pFunc = MakeFuncDecl(yyvsp[-8].pType, yyvsp[-6].szIdentifier, yyvsp[-4].pArgument, yyvsp[-2].pConst, yyvsp[-1].pVar, yyvsp[0].pStm); ;
    break;}
case 70:
#line 392 "crisolscript.y"
{ yyval.pType = MakeTypeVoid(); ;
    break;}
case 71:
#line 394 "crisolscript.y"
{ yyval.pType = yyvsp[0].pType; ;
    break;}
case 72:
#line 398 "crisolscript.y"
{ yyval.pArgument = NULL ;
    break;}
case 73:
#line 400 "crisolscript.y"
{ yyval.pArgument = yyvsp[0].pArgument; ;
    break;}
case 74:
#line 404 "crisolscript.y"
{ yyval.pArgument = yyvsp[0].pArgument; ;
    break;}
case 75:
#line 406 "crisolscript.y"
{ yyvsp[-2].pArgument->pSigArgument = yyvsp[0].pArgument; yyval.pArgument = yyvsp[-2].pArgument; ;
    break;}
case 76:
#line 410 "crisolscript.y"
{ yyval.pArgument = MakeArgumentByRef(yyvsp[-1].pType, yyvsp[0].szIdentifier, NULL); ;
    break;}
case 77:
#line 412 "crisolscript.y"
{ yyval.pArgument = MakeArgumentByValue(yyvsp[-1].pType, yyvsp[0].szIdentifier, NULL); ;
    break;}
case 78:
#line 417 "crisolscript.y"
{ yyval.pType = MakeTypeNumber(); ;
    break;}
case 79:
#line 419 "crisolscript.y"
{ yyval.pType = MakeTypeString(); ;
    break;}
case 80:
#line 421 "crisolscript.y"
{ yyval.pType = MakeTypeEntity(); ;
    break;}
case 81:
#line 426 "crisolscript.y"
{ yyval.pStm = NULL; ;
    break;}
case 82:
#line 428 "crisolscript.y"
{ yyval.pStm = yyvsp[-1].pStm; ;
    break;}
case 83:
#line 432 "crisolscript.y"
{ yyval.pStm = yyvsp[0].pStm; ;
    break;}
case 84:
#line 434 "crisolscript.y"
{ yyval.pStm = MakeStmSeq(yyvsp[-1].pStm, yyvsp[0].pStm); ;
    break;}
case 85:
#line 438 "crisolscript.y"
{ yyval.pStm = MakeStmExp(NULL); ;
    break;}
case 86:
#line 440 "crisolscript.y"
{ yyval.pStm = MakeStmReturn(NULL); ;
    break;}
case 87:
#line 442 "crisolscript.y"
{ yyval.pStm = MakeStmReturn(yyvsp[-1].pExp); ;
    break;}
case 88:
#line 444 "crisolscript.y"
{ yyval.pStm = MakeStmIf(yyvsp[-3].pExp, yyvsp[0].pStm); ;
    break;}
case 89:
#line 446 "crisolscript.y"
{ yyval.pStm = MakeStmIfElse(yyvsp[-5].pExp, yyvsp[-2].pStm, yyvsp[0].pStm); ;
    break;}
case 90:
#line 448 "crisolscript.y"
{ yyval.pStm = MakeStmWhile(yyvsp[-3].pExp, yyvsp[0].pStm); ;
    break;}
case 91:
#line 450 "crisolscript.y"
{ yyval.pStm = yyvsp[0].pStm; ;
    break;}
case 92:
#line 452 "crisolscript.y"
{ yyval.pStm = MakeStmExp(yyvsp[-1].pExp); ;
    break;}
case 93:
#line 457 "crisolscript.y"
{ yyval.pExp = MakeExpIdentifier(yyvsp[0].szIdentifier); ;
    break;}
case 94:
#line 459 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstEntity(0); ;
    break;}
case 95:
#line 461 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 96:
#line 463 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 97:
#line 465 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 98:
#line 467 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 99:
#line 469 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 100:
#line 471 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 101:
#line 473 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(2.0f); ;
    break;}
case 102:
#line 475 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(3.0f); ;
    break;}
case 103:
#line 477 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(4.0f); ;
    break;}
case 104:
#line 479 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(5.0f); ;
    break;}
case 105:
#line 481 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(6.0f); ;
    break;}
case 106:
#line 483 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(7.0f); ;
    break;}
case 107:
#line 485 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 108:
#line 487 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 109:
#line 489 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(2.0f); ;
    break;}
case 110:
#line 491 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 111:
#line 493 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 112:
#line 495 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 113:
#line 497 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(2.0f); ;
    break;}
case 114:
#line 499 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(3.0f); ;
    break;}
case 115:
#line 501 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(4.0f); ;
    break;}
case 116:
#line 503 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(5.0f); ;
    break;}
case 117:
#line 505 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 118:
#line 507 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 119:
#line 509 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(2.0f); ;
    break;}
case 120:
#line 511 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 121:
#line 513 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 122:
#line 515 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(2.0f); ;
    break;}
case 123:
#line 517 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(3.0f); ;
    break;}
case 124:
#line 519 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(4.0f); ;
    break;}
case 125:
#line 521 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(5.0f); ;
    break;}
case 126:
#line 523 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(6.0f); ;
    break;}
case 127:
#line 525 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(7.0f); ;
    break;}
case 128:
#line 527 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(8.0f); ;
    break;}
case 129:
#line 529 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(9.0f); ;
    break;}
case 130:
#line 531 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(10.0f); ;
    break;}
case 131:
#line 533 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(11.0f); ;
    break;}
case 132:
#line 535 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(12.0f); ;
    break;}
case 133:
#line 537 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(13.0f); ;
    break;}
case 134:
#line 539 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(14.0f); ;
    break;}
case 135:
#line 541 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(15.0f); ;
    break;}
case 136:
#line 543 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(16.0f); ;
    break;}
case 137:
#line 545 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(17.0f); ;
    break;}
case 138:
#line 547 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(18.0f); ;
    break;}
case 139:
#line 549 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(19.0f); ;
    break;}
case 140:
#line 551 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(20.0f); ;
    break;}
case 141:
#line 553 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(21.0f); ;
    break;}
case 142:
#line 555 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(22.0f); ;
    break;}
case 143:
#line 557 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(23.0f); ;
    break;}
case 144:
#line 559 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(24.0f); ;
    break;}
case 145:
#line 561 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(25.0f); ;
    break;}
case 146:
#line 563 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(26.0f); ;
    break;}
case 147:
#line 565 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(27.0f); ;
    break;}
case 148:
#line 567 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(28.0f); ;
    break;}
case 149:
#line 569 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(29.0f); ;
    break;}
case 150:
#line 571 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(30.0f); ;
    break;}
case 151:
#line 573 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(0.0f); ;
    break;}
case 152:
#line 575 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(1.0f); ;
    break;}
case 153:
#line 577 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(2.0f); ;
    break;}
case 154:
#line 579 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(3.0f); ;
    break;}
case 155:
#line 581 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(4.0f); ;
    break;}
case 156:
#line 583 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(5.0f); ;
    break;}
case 157:
#line 585 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(6.0f); ;
    break;}
case 158:
#line 587 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(7.0f); ;
    break;}
case 159:
#line 589 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(8.0f); ;
    break;}
case 160:
#line 591 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(9.0f); ;
    break;}
case 161:
#line 593 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(10.0f); ;
    break;}
case 162:
#line 595 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(11.0f); ;
    break;}
case 163:
#line 597 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(12.0f); ;
    break;}
case 164:
#line 599 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(13.0f); ;
    break;}
case 165:
#line 601 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(14.0f); ;
    break;}
case 166:
#line 603 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(15.0f); ;
    break;}
case 167:
#line 605 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(16.0f); ;
    break;}
case 168:
#line 607 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(17.0f); ;
    break;}
case 169:
#line 609 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(18.0f); ;
    break;}
case 170:
#line 611 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(19.0f); ;
    break;}
case 171:
#line 613 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(20.0f); ;
    break;}
case 172:
#line 615 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(21.0f); ;
    break;}
case 173:
#line 617 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(22.0f); ;
    break;}
case 174:
#line 619 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(23.0f); ;
    break;}
case 175:
#line 621 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(24.0f); ;
    break;}
case 176:
#line 623 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(25.0f); ;
    break;}
case 177:
#line 625 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(26.0f); ;
    break;}
case 178:
#line 627 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(27.0f); ;
    break;}
case 179:
#line 629 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(28.0f); ;
    break;}
case 180:
#line 631 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(29.0f); ;
    break;}
case 181:
#line 633 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(30.0f); ;
    break;}
case 182:
#line 635 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(31.0f); ;
    break;}
case 183:
#line 637 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(32.0f); ;
    break;}
case 184:
#line 639 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(33.0f); ;
    break;}
case 185:
#line 641 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(34.0f); ;
    break;}
case 186:
#line 643 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(35.0f); ;
    break;}
case 187:
#line 645 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(36.0f); ;
    break;}
case 188:
#line 647 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(37.0f); ;
    break;}
case 189:
#line 649 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(38.0f); ;
    break;}
case 190:
#line 651 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(39.0f); ;
    break;}
case 191:
#line 653 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(40.0f); ;
    break;}
case 192:
#line 655 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(41.0f); ;
    break;}
case 193:
#line 657 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(42.0f); ;
    break;}
case 194:
#line 659 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(43.0f); ;
    break;}
case 195:
#line 661 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(44.0f); ;
    break;}
case 196:
#line 663 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(45.0f); ;
    break;}
case 197:
#line 665 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(46.0f); ;
    break;}
case 198:
#line 667 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(47.0f); ;
    break;}
case 199:
#line 669 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(48.0f); ;
    break;}
case 200:
#line 671 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(49.0f); ;
    break;}
case 201:
#line 673 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(50.0f); ;
    break;}
case 202:
#line 675 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(51.0f); ;
    break;}
case 203:
#line 677 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(52.0f); ;
    break;}
case 204:
#line 679 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(53.0f); ;
    break;}
case 205:
#line 681 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(54.0f); ;
    break;}
case 206:
#line 683 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(55.0f); ;
    break;}
case 207:
#line 685 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(56.0f); ;
    break;}
case 208:
#line 687 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(57.0f); ;
    break;}
case 209:
#line 689 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(58.0f); ;
    break;}
case 210:
#line 691 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(59.0f); ;
    break;}
case 211:
#line 693 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(60.0f); ;
    break;}
case 212:
#line 695 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(61.0f); ;
    break;}
case 213:
#line 697 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(62.0f); ;
    break;}
case 214:
#line 699 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(63.0f); ;
    break;}
case 215:
#line 701 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(64.0f); ;
    break;}
case 216:
#line 703 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(65.0f); ;
    break;}
case 217:
#line 705 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(66.0f); ;
    break;}
case 218:
#line 707 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(67.0f); ;
    break;}
case 219:
#line 709 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(68.0f); ;
    break;}
case 220:
#line 711 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(69.0f); ;
    break;}
case 221:
#line 713 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(70.0f); ;
    break;}
case 222:
#line 715 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(71.0f); ;
    break;}
case 223:
#line 717 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(72.0f); ;
    break;}
case 224:
#line 719 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(73.0f); ;
    break;}
case 225:
#line 721 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(74.0f); ;
    break;}
case 226:
#line 723 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(75.0f); ;
    break;}
case 227:
#line 725 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(76.0f); ;
    break;}
case 228:
#line 727 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(77.0f); ;
    break;}
case 229:
#line 729 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(78.0f); ;
    break;}
case 230:
#line 731 "crisolscript.y"
{ yyval.pExp = MakeExpGlobalConstNumber(79.0f); ;
    break;}
case 231:
#line 733 "crisolscript.y"
{ yyval.pExp = MakeExpNumberValue(yyvsp[0].fNumber); ;
    break;}
case 232:
#line 735 "crisolscript.y"
{ yyval.pExp = MakeExpStringValue(yyvsp[0].szString); ;
    break;}
case 233:
#line 737 "crisolscript.y"
{ yyval.pExp = MakeExpAssing(yyvsp[-2].szIdentifier, yyvsp[0].pExp); ;
    break;}
case 234:
#line 739 "crisolscript.y"
{ yyval.pExp = MakeExpEqual(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 235:
#line 741 "crisolscript.y"
{ yyval.pExp = MakeExpNoEqual(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 236:
#line 743 "crisolscript.y"
{ yyval.pExp = MakeExpLess(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 237:
#line 745 "crisolscript.y"
{ yyval.pExp = MakeExpLessEqual(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 238:
#line 747 "crisolscript.y"
{ yyval.pExp = MakeExpGreater(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 239:
#line 749 "crisolscript.y"
{ yyval.pExp = MakeExpGreaterEqual(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 240:
#line 751 "crisolscript.y"
{ yyval.pExp = MakeExpAdd(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 241:
#line 753 "crisolscript.y"
{ yyval.pExp = MakeExpMinus(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 242:
#line 755 "crisolscript.y"
{ yyval.pExp = MakeExpMult(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 243:
#line 757 "crisolscript.y"
{ yyval.pExp = MakeExpDiv(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 244:
#line 759 "crisolscript.y"
{ yyval.pExp = MakeExpModulo(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 245:
#line 761 "crisolscript.y"
{ yyval.pExp = MakeExpUMinus(yyvsp[0].pExp); ;
    break;}
case 246:
#line 763 "crisolscript.y"
{ yyval.pExp = MakeExpNot(yyvsp[0].pExp); ;
    break;}
case 247:
#line 765 "crisolscript.y"
{ yyval.pExp = MakeExpAnd(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 248:
#line 767 "crisolscript.y"
{ yyval.pExp = MakeExpOr(yyvsp[-2].pExp, yyvsp[0].pExp); ;
    break;}
case 249:
#line 769 "crisolscript.y"
{ yyval.pExp = yyvsp[-1].pExp; ;
    break;}
case 250:
#line 771 "crisolscript.y"
{ yyval.pExp = MakeExpUserFuncInvoke(yyvsp[-3].szIdentifier, yyvsp[-1].pExp); ;
    break;}
case 251:
#line 773 "crisolscript.y"
{ yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 252:
#line 775 "crisolscript.y"
{ yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 253:
#line 777 "crisolscript.y"
{ yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 254:
#line 779 "crisolscript.y"
{ yyvsp[0].pExp->ExpEntityObjInvoke.szIdentifier = yyvsp[-2].szIdentifier; yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 255:
#line 783 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_PASSTORGBCOLOR, yyvsp[-1].pExp); ;
    break;}
case 256:
#line 785 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_GETREDCOMPONENT, yyvsp[-1].pExp); ;
    break;}
case 257:
#line 787 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_GETGREENCOMPONENT, yyvsp[-1].pExp); ;
    break;}
case 258:
#line 789 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_GETBLUECOMPONENT, yyvsp[-1].pExp); ;
    break;}
case 259:
#line 791 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_RAND, yyvsp[-1].pExp); ;
    break;}
case 260:
#line 793 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_GETINTEGERVALUE, yyvsp[-1].pExp); ;
    break;}
case 261:
#line 795 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_GETDECIMALVALUE, yyvsp[-1].pExp); ;
    break;}
case 262:
#line 797 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_GETSTRINGSIZE, yyvsp[-1].pExp); ;
    break;}
case 263:
#line 799 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_WRITETOLOGGER, yyvsp[-1].pExp); ;
    break;}
case 264:
#line 801 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_ENABLECRISOLSCRIPTWARNINGS, yyvsp[-1].pExp); ;
    break;}
case 265:
#line 803 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_DISABLECRISOLSCRIPTWARNINGS, yyvsp[-1].pExp); ;
    break;}
case 266:
#line 805 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_SHOWFPS, yyvsp[-1].pExp); ;
    break;}
case 267:
#line 807 "crisolscript.y"
{ yyval.pExp = MakeExpAPIFunc(API_WAIT, yyvsp[-1].pExp); ;
    break;}
case 268:
#line 811 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_QUITGAME, yyvsp[-1].pExp); ;
    break;}
case 269:
#line 813 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_WRITETOCONSOLE, yyvsp[-1].pExp); ;
    break;}
case 270:
#line 815 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ACTIVEADVICEDIALOG, yyvsp[-1].pExp); ;
    break;}
case 271:
#line 817 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ACTIVEQUESTIONDIALOG, yyvsp[-1].pExp); ;
    break;}
case 272:
#line 819 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ACTIVETEXTREADERDIALOG, yyvsp[-1].pExp); ;
    break;}
case 273:
#line 821 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG, yyvsp[-1].pExp); ;
    break;}
case 274:
#line 823 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG, yyvsp[-1].pExp); ;
    break;}
case 275:
#line 825 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ACTIVETEXTSELECTORDIALOG, yyvsp[-1].pExp); ;
    break;}
case 276:
#line 827 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_PLAYMIDIMUSIC, yyvsp[-1].pExp); ;
    break;}
case 277:
#line 829 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_STOPMIDIMUSIC, yyvsp[-1].pExp); ;
    break;}
case 278:
#line 831 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_PLAYWAVAMBIENTSOUND, yyvsp[-1].pExp); ;
    break;}
case 279:
#line 833 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_STOPWAVAMBIENTSOUND, yyvsp[-1].pExp); ;
    break;}
case 280:
#line 835 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ACTIVETRADEITEMSINTERFAZ, yyvsp[-1].pExp); ;
    break;}
case 281:
#line 837 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ, yyvsp[-1].pExp); ;
    break;}
case 282:
#line 839 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ, yyvsp[-1].pExp); ;
    break;}
case 283:
#line 841 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ACTIVECONVERSATORINTERFAZ, yyvsp[-1].pExp); ;
    break;}
case 284:
#line 843 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_DESACTIVECONVERSATORINTERFAZ, yyvsp[-1].pExp); ;
    break;}
case 285:
#line 845 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ, yyvsp[-1].pExp); ;
    break;}
case 286:
#line 847 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_SHOWPRESENTATION, yyvsp[-1].pExp); ;
    break;}
case 287:
#line 849 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_BEGINCUTSCENE, yyvsp[-1].pExp); ;
    break;}
case 288:
#line 851 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ENDCUTSCENE, yyvsp[-1].pExp); ;
    break;}
case 289:
#line 853 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_SETSCRIPT, yyvsp[-1].pExp); ;
    break;}
case 290:
#line 855 "crisolscript.y"
{ yyval.pExp = MakeExpGameObj(GAMEOBJ_ISKEYPRESSED, yyvsp[-1].pExp); ;
    break;}
case 291:
#line 859 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETAREANAME, yyvsp[-1].pExp); ;
    break;}
case 292:
#line 861 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETAREAID, yyvsp[-1].pExp); ;
    break;}
case 293:
#line 863 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETAREAWIDTH, yyvsp[-1].pExp); ;
    break;}
case 294:
#line 865 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETAREAHEIGHT, yyvsp[-1].pExp); ;
    break;}
case 295:
#line 867 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETHOUR, yyvsp[-1].pExp); ;
    break;}
case 296:
#line 869 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETMINUTE, yyvsp[-1].pExp); ;
    break;}
case 297:
#line 871 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETHOUR, yyvsp[-1].pExp); ;
    break;}
case 298:
#line 873 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETMINUTE, yyvsp[-1].pExp); ;
    break;}
case 299:
#line 875 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETENTITY, yyvsp[-1].pExp); ;
    break;}
case 300:
#line 877 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETPLAYER, yyvsp[-1].pExp); ;
    break;}
case 301:
#line 879 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_ISFLOORVALID, yyvsp[-1].pExp); ;
    break;}
case 302:
#line 881 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETNUMITEMSAT, yyvsp[-1].pExp); ;
    break;}
case 303:
#line 883 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETITEMAT, yyvsp[-1].pExp); ;
    break;}
case 304:
#line 885 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETDISTANCE, yyvsp[-1].pExp); ;
    break;}
case 305:
#line 887 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CALCULEPATHLENGHT, yyvsp[-1].pExp); ;
    break;}
case 306:
#line 889 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_LOADAREA, yyvsp[-1].pExp); ;
    break;}
case 307:
#line 891 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CHANGEENTITYLOCATION, yyvsp[-1].pExp); ;
    break;}
case 308:
#line 893 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_ATTACHCAMERATOENTITY, yyvsp[-1].pExp); ;
    break;}
case 309:
#line 895 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_ATTACHCAMERATOLOCATION, yyvsp[-1].pExp); ;
    break;}
case 310:
#line 897 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_ISCOMBATMODEACTIVE, yyvsp[-1].pExp); ;
    break;}
case 311:
#line 899 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_ENDCOMBAT, yyvsp[-1].pExp); ;
    break;}
case 312:
#line 901 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETCRIATUREINCOMBATTURN, yyvsp[-1].pExp); ;
    break;}
case 313:
#line 903 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETCOMBATANT, yyvsp[-1].pExp); ;
    break;}
case 314:
#line 905 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETNUMBEROFCOMBATANTS, yyvsp[-1].pExp); ;
    break;}
case 315:
#line 907 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETAREALIGHTMODEL, yyvsp[-1].pExp); ;
    break;}
case 316:
#line 909 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETIDLESCRIPTTIME, yyvsp[-1].pExp); ;
    break;}
case 317:
#line 911 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETSCRIPT, yyvsp[-1].pExp); ;
    break;}
case 318:
#line 913 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_DESTROYENTITY, yyvsp[-1].pExp); ;
    break;}
case 319:
#line 915 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CREATECRIATURE, yyvsp[-1].pExp); ;
    break;}
case 320:
#line 917 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CREATEWALL, yyvsp[-1].pExp); ;
    break;}
case 321:
#line 919 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CREATESCENARYOBJECT, yyvsp[-1].pExp); ;
    break;}
case 322:
#line 921 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CREATEITEMABANDONED, yyvsp[-1].pExp); ;
    break;}
case 323:
#line 923 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_CREATEITEMWITHOWNER, yyvsp[-1].pExp); ;
    break;}
case 324:
#line 925 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETWORLDTIMEPAUSE, yyvsp[-1].pExp); ;
    break;}
case 325:
#line 927 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_ISWORLDTIMEINPAUSE, yyvsp[-1].pExp); ;
    break;}
case 326:
#line 929 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETELEVATIONAT, yyvsp[-1].pExp); ;
    break;}
case 327:
#line 931 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETELEVATIONAT, yyvsp[-1].pExp); ;
    break;}
case 328:
#line 933 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_NEXTTURN, yyvsp[-1].pExp); ;
    break;}
case 329:
#line 935 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_GETLIGHTAT, yyvsp[-1].pExp); ;
    break;}
case 330:
#line 937 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_PLAYWAVSOUND, yyvsp[-1].pExp); ;
    break;}
case 331:
#line 939 "crisolscript.y"
{ yyval.pExp = MakeExpWorldObj(WORLDOBJ_SETSCRIPTAT, yyvsp[-1].pExp); ;
    break;}
case 332:
#line 943 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETNAME, yyvsp[-1].pExp); ;
    break;}
case 333:
#line 945 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETNAME, yyvsp[-1].pExp); ;
    break;}
case 334:
#line 947 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETENTITYTYPE, yyvsp[-1].pExp); ;
    break;}
case 335:
#line 949 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETTYPE, yyvsp[-1].pExp); ;
    break;}
case 336:
#line 951 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SAY, yyvsp[-1].pExp); ;
    break;}
case 337:
#line 953 "crisolscript.y"
{ yyval.pExp =  MakeExpEntityObj(ENTITYOBJ_SHUTUP, yyvsp[-1].pExp); ;
    break;}
case 338:
#line 955 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISSAYING, yyvsp[-1].pExp); ;
    break;}
case 339:
#line 957 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ATTACHGFX, yyvsp[-1].pExp); ;
    break;}
case 340:
#line 959 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_RELEASEGFX, yyvsp[-1].pExp); ;
    break;}
case 341:
#line 961 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_RELEASEALLGFX, yyvsp[-1].pExp); ;
    break;}
case 342:
#line 963 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISGFXATTACHED, yyvsp[-1].pExp); ;
    break;}
case 343:
#line 965 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETNUMITEMSINCONTAINER, yyvsp[-1].pExp); ;
    break;}
case 344:
#line 967 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISITEMINCONTAINER, yyvsp[-1].pExp); ;
    break;}
case 345:
#line 969 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETITEMFROMCONTAINER, yyvsp[-1].pExp); ;
    break;}
case 346:
#line 971 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_TRANSFERITEMTOCONTAINER, yyvsp[-1].pExp); ;
    break;}
case 347:
#line 973 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_INSERTITEMINCONTAINER, yyvsp[-1].pExp); ;
    break;}
case 348:
#line 975 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_REMOVEITEMOFCONTAINER, yyvsp[-1].pExp); ;
    break;}
case 349:
#line 977 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETANIMTEMPLATESTATE, yyvsp[-1].pExp); ;
    break;}
case 350:
#line 979 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE, yyvsp[-1].pExp); ;
    break;}
case 351:
#line 981 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETIDLESCRIPTTIME, yyvsp[-1].pExp); ;
    break;}
case 352:
#line 983 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETLIGHT, yyvsp[-1].pExp); ;
    break;}
case 353:
#line 985 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETLIGHT, yyvsp[-1].pExp); ;
    break;}
case 354:
#line 987 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETXPOS, yyvsp[-1].pExp); ;
    break;}
case 355:
#line 989 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETYPOS, yyvsp[-1].pExp); ;
    break;}
case 356:
#line 991 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETELEVATION, yyvsp[-1].pExp); ;
    break;}
case 357:
#line 993 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETELEVATION, yyvsp[-1].pExp); ;
    break;}
case 358:
#line 995 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETLOCALATTRIBUTE, yyvsp[-1].pExp); ;
    break;}
case 359:
#line 997 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETLOCALATTRIBUTE, yyvsp[-1].pExp); ;
    break;}
case 360:
#line 999 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETOWNER, yyvsp[-1].pExp); ;
    break;}
case 361:
#line 1001 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETCLASS, yyvsp[-1].pExp); ;
    break;}
case 362:
#line 1003 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETINCOMBATUSECOST, yyvsp[-1].pExp); ;
    break;}
case 363:
#line 1005 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETGLOBALATTRIBUTE, yyvsp[-1].pExp); ;
    break;}
case 364:
#line 1007 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETGLOBALATTRIBUTE, yyvsp[-1].pExp); ;
    break;}
case 365:
#line 1009 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETWALLORIENTATION, yyvsp[-1].pExp); ;
    break;}
case 366:
#line 1011 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_BLOCKACCESS, yyvsp[-1].pExp); ;
    break;}
case 367:
#line 1013 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_UNBLOCKACCESS, yyvsp[-1].pExp); ;
    break;}
case 368:
#line 1015 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISACCESSBLOCKED, yyvsp[-1].pExp); ;
    break;}
case 369:
#line 1017 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETSYMPTOM, yyvsp[-1].pExp); ;
    break;}
case 370:
#line 1019 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISSYMPTOMACTIVE, yyvsp[-1].pExp); ;
    break;}
case 371:
#line 1021 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETGENRE, yyvsp[-1].pExp); ;
    break;}
case 372:
#line 1023 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETHEALTH, yyvsp[-1].pExp); ;
    break;}
case 373:
#line 1025 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETHEALTH, yyvsp[-1].pExp); ;
    break;}
case 374:
#line 1027 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETEXTENDEDATTRIBUTE, yyvsp[-1].pExp); ;
    break;}
case 375:
#line 1029 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETEXTENDEDATTRIBUTE, yyvsp[-1].pExp); ;
    break;}
case 376:
#line 1031 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETLEVEL, yyvsp[-1].pExp); ;
    break;}
case 377:
#line 1033 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETLEVEL, yyvsp[-1].pExp); ;
    break;}
case 378:
#line 1035 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETEXPERIENCE, yyvsp[-1].pExp); ;
    break;}
case 379:
#line 1037 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETEXPERIENCE, yyvsp[-1].pExp); ;
    break;}
case 380:
#line 1039 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETINCOMBATACTIONPOINTS, yyvsp[-1].pExp); ;
    break;}
case 381:
#line 1041 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETACTIONPOINTS, yyvsp[-1].pExp); ;
    break;}
case 382:
#line 1043 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETACTIONPOINTS, yyvsp[-1].pExp); ;
    break;}
case 383:
#line 1045 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISHABILITYACTIVE, yyvsp[-1].pExp); ;
    break;}
case 384:
#line 1047 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETHABILITY, yyvsp[-1].pExp); ;
    break;}
case 385:
#line 1049 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_USEHABILITY, yyvsp[-1].pExp); ;
    break;}
case 386:
#line 1051 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISRUNMODEACTIVE, yyvsp[-1].pExp); ;
    break;}
case 387:
#line 1053 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETRUNMODE, yyvsp[-1].pExp); ;
    break;}
case 388:
#line 1055 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_MOVETO, yyvsp[-1].pExp); ;
    break;}
case 389:
#line 1057 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISMOVING, yyvsp[-1].pExp); ;
    break;}
case 390:
#line 1059 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_STOPMOVING, yyvsp[-1].pExp); ;
    break;}
case 391:
#line 1061 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_EQUIPITEM, yyvsp[-1].pExp); ;
    break;}
case 392:
#line 1063 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_REMOVEITEMEQUIPPED, yyvsp[-1].pExp); ;
    break;}
case 393:
#line 1065 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETITEMEQUIPPED, yyvsp[-1].pExp); ;
    break;}
case 394:
#line 1067 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISITEMEQUIPPED, yyvsp[-1].pExp); ;
    break;}
case 395:
#line 1069 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_DROPITEM, yyvsp[-1].pExp); ;
    break;}
case 396:
#line 1071 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_USEITEM, yyvsp[-1].pExp); ;
    break;}
case 397:
#line 1073 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_MANIPULATE, yyvsp[-1].pExp); ;
    break;}
case 398:
#line 1075 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETTRANSPARENTMODE, yyvsp[-1].pExp); ;
    break;}
case 399:
#line 1077 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISTRANSPARENTMODEACTIVE, yyvsp[-1].pExp); ;
    break;}
case 400:
#line 1079 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_CHANGEANIMORIENTATION, yyvsp[-1].pExp); ;
    break;}
case 401:
#line 1081 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETANIMORIENTATION, yyvsp[-1].pExp); ;
    break;}
case 402:
#line 1083 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETALINGMENT, yyvsp[-1].pExp); ;
    break;}
case 403:
#line 1085 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETALINGMENTWITH, yyvsp[-1].pExp); ;
    break;}
case 404:
#line 1087 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETALINGMENTAGAINST, yyvsp[-1].pExp); ;
    break;}
case 405:
#line 1089 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETALINGMENT, yyvsp[-1].pExp); ;
    break;}
case 406:
#line 1091 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_HITENTITY, yyvsp[-1].pExp); ;
    break;}
case 407:
#line 1093 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETSCRIPT, yyvsp[-1].pExp); ;
    break;}
case 408:
#line 1095 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISGHOSTMOVEMODEACTIVE, yyvsp[-1].pExp); ;
    break;}
case 409:
#line 1097 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_SETGHOSTMOVEMODE, yyvsp[-1].pExp); ;
    break;}
case 410:
#line 1099 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_GETRANGE, yyvsp[-1].pExp); ;
    break;}
case 411:
#line 1101 "crisolscript.y"
{ yyval.pExp = MakeExpEntityObj(ENTITYOBJ_ISINRANGE, yyvsp[-1].pExp); ;
    break;}
case 412:
#line 1108 "crisolscript.y"
{ yyval.pExp = NULL; ;
    break;}
case 413:
#line 1110 "crisolscript.y"
{ yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 414:
#line 1114 "crisolscript.y"
{ yyval.pExp = yyvsp[0].pExp; ;
    break;}
case 415:
#line 1116 "crisolscript.y"
{ yyvsp[-2].pExp->pSigExp = yyvsp[0].pExp; yyval.pExp = yyvsp[-2].pExp;  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 1120 "crisolscript.y"
