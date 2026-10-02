/*
 ***************************************************************************************************
 * BEGIN:Banner
 ***************************************************************************************************
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 24
 *--------------------------------------------------------------------------------------------------
 *
 * COPYRIGHT RESERVED, 2020 ETAS GmbH. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as
 * the communication of its contents to others without explicit authorization
 * is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or 
 * design.
 *--------------------------------------------------------------------------------------------------
 *    Administrative Information (automatically filled in)
 *--------------------------------------------------------------------------------------------------
 * Project     : GM SUM
 * Component   : SUM_DIAG
 * Date        : Jun 13, 2026
 * Revision    : 8.0.1_Pr24
 * Description : SUM_DIAG header main
 ***************************************************************************************************
 * END:Banner
 ***************************************************************************************************
 */
#ifndef SUM_DIAG_H
#define SUM_DIAG_H

#include "SUM_DIAG_Types.h"
#include "SUM_Diag_Cfg.h"
#include "Dcm.h"

#if (DIAG_GENERATE_INIT_RUNNABLE_STATUS == STD_OFF)
/* PRQA S 3447 EOF */ 
/*extern is required here since this function is used globally*/  
/* Extern Function declaration for DIAG Initialization function */
extern FUNC (void, SUM_DIAG_CODE) DIAG_Init(void);
#endif
/*
 * Indication and Confirmation prototypes *
 */
extern FUNC(Std_ReturnType, RTE_CODE) DiagnosticSessionControl_Indication(VAR(uint8, AUTOMATIC) SID,
                                                                          P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) RequestData,
                                                                          VAR(uint32, AUTOMATIC) DataSize,
                                                                          VAR(uint8, AUTOMATIC) ReqType,
                                                                          VAR(uint16, AUTOMATIC) SourceAddress,
                                                                          CONSTP2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode);

extern FUNC(Std_ReturnType, RTE_CODE) DiagnosticSessionControl_Confirmation(VAR(uint8, AUTOMATIC) SID,
                                                                            VAR(uint8, AUTOMATIC) ReqType,
                                                                            VAR(uint16, AUTOMATIC) SourceAddress,
                                                                            VAR(Dcm_ConfirmationStatusType, AUTOMATIC) ConfirmationStatus);

extern FUNC(Std_ReturnType, RTE_CODE) EcuReset_Indication(VAR(uint8, AUTOMATIC) SID,
                                                          P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) RequestData,
                                                          VAR(uint32, AUTOMATIC) DataSize,
                                                          VAR(uint8, AUTOMATIC) ReqType,
                                                          VAR(uint16, AUTOMATIC) SourceAddress,
                                                          CONSTP2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode);

#if (RMBA_SERVICE_SUPPORT == STD_ON)
extern FUNC(Std_ReturnType, RTE_CODE) Diag_RMBA_Indication(P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) RequestData,
                                                           VAR(uint32, AUTOMATIC) DataSize,
                                                           VAR(uint8, AUTOMATIC) ReqType,
                                                           CONSTP2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode);
#endif

extern FUNC(Std_ReturnType, RTE_CODE) CommunicationControl_Indication(VAR(uint8, AUTOMATIC) SID,
                                                                      P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) RequestData,
                                                                      VAR(uint32, AUTOMATIC) DataSize,
                                                                      VAR(uint8, AUTOMATIC) ReqType,
                                                                      VAR(uint16, AUTOMATIC) SourceAddress,
                                                                      CONSTP2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode);

extern FUNC(Std_ReturnType, RTE_CODE) CommunicationControl_Confirmation(VAR(uint8, AUTOMATIC) SID,
                                                                        VAR(uint8, AUTOMATIC) ReqType,
                                                                        VAR(uint16, AUTOMATIC) SourceAddress,
                                                                        VAR(Dcm_ConfirmationStatusType, AUTOMATIC) ConfirmationStatus);

#if (DDDI_SERVICE_SUPPORT == STD_ON)
extern FUNC(Std_ReturnType, RTE_CODE) Diag_DDDI_Indication(P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) RequestData,
                                                           VAR(uint32, AUTOMATIC) DataSize,
                                                           CONSTP2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode);
#endif
/* To initialize ports and variables after Permit execution */
extern FUNC(void, RTE_CODE) initializeForRun(void);
/* Function to check for vehicle power mode from SUSD */
extern FUNC(boolean, RTE_CODE) check_offpowermode(void);
/* Function to set the Tester source address */
extern FUNC(void, RTE_CODE) setTesterAddress(VAR(uint16, AUTOMATIC) sourceAddress);
/* Function to read the MEC from NvM */
extern FUNC(uint8, RTE_CODE) getManufacturerEnableCounter(void);
/* Function to get the Tester source address */
extern FUNC(uint16, RTE_CODE) getTesterAddress(void);
/* Function to read the OTA PSF flag from NvM */
extern FUNC(boolean, RTE_CODE) getOtaPsf(void);
/* Function to write the OTA PSF flag into NvM */
extern FUNC(void, RTE_CODE) setOtaPsf(VAR(boolean, AUTOMATIC) OTA_PSF_Value);
/* Function to process the write of  MEC */
extern FUNC(Std_ReturnType, RTE_CODE) ProcessMECWrite (uint8 const * const mecValue);

/* Function to check the write of  MEC */
extern FUNC(Std_ReturnType, RTE_CODE) CheckMECWrite (void);

/* Function to process the write of  MEC */
extern FUNC(Std_ReturnType, RTE_CODE) ProcessOTAWrite (void);

extern FUNC(Std_ReturnType,DCM_APPL_CODE) Diag_SetProgConditions (VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
																  P2CONST(Dcm_ProgConditionsType, AUTOMATIC, DCM_INTERN_DATA) ProgConditions);

/* DCM GetRxPermission callout for protocol pre-emption control */
extern FUNC(Std_ReturnType, DCM_APPL_CODE) SUM_DIAG_DcmGetRxPermission(Dcm_ProtocolType ProtocolType, PduIdType PduId);

#if (MEC_STORAGE_IN_HSM == STD_ON)
/* [$Satisfies $SDD_DIAG 132] HSM MEC Read — calls ReadHWIO_e_SP_SecureDataStore */
extern FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_Read(P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) mecValue,
                                                        P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) mecStatus);

/* [$Satisfies $SDD_DIAG 133] HSM MEC Write state machine — Steps A-L (CYS4008) */
extern FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_WriteStart(VAR(uint8, AUTOMATIC) mecValue);
extern FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_WritePoll(void);

/* [$Satisfies $SDD_DIAG 134] HSM MEC Init — double-read retry + first-time set (SUMDIAG_79) */
extern FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_Init(void);
#endif

#endif /* SUM_DIAG_H */
