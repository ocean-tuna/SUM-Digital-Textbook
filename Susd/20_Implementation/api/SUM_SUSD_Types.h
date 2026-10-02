/*
 *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 24
 *-----------------------------------------------------------------------------
 *
 * COPYRIGHT RESERVED, 2020 ETAS GmbH. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as
 * the communication of its contents to others without explicit authorization
 * is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or
 * design.
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in)
 *-----------------------------------------------------------------------------
 * Project     : GM SUM
 * Component   : SUM_SUSD
 * Date        : Jun 25, 2026
 * Revision    : 8.0.1_rc
 * Description : SUM_SUSD header types
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

#ifndef SUM_SUSD_TYPES_H
#define SUM_SUSD_TYPES_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "Std_Types.h"
/* PRQA S 0380 1 # header included from RTE */
#include "Rte_SUSD_Type.h"
#include "SUM_SUSD_Cfg.h"

/***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/* SUM SSM Signal Status values */
#define SUM_SSM_SIGNAL_STATUS_DISABLED 0u
#define SUM_SSM_SIGNAL_STATUS_UNINITIALIZED 1u
#define SUM_SSM_SIGNAL_STATUS_UNAVAILABLE 2u
#define SUM_SSM_SIGNAL_STATUS_AVAILABLE 3u

#if(SUSD_POWERMODING_SLAVE_SUPPORT == STD_ON)
/* Power Mode Output states */
/* Satisfies: [$SDD_SUSD 132] */
#define SUM_PMS_OFF           (SUM_VehiclePowerMode_Signal_States)0
#define SUM_PMS_ACCESSORY     (SUM_VehiclePowerMode_Signal_States)1
#define SUM_PMS_RUN           (SUM_VehiclePowerMode_Signal_States)2
#define SUM_PMS_START         (SUM_VehiclePowerMode_Signal_States)3
#define SUM_PMS_PROPULSION    (SUM_VehiclePowerMode_Signal_States)4
#endif


#if(SUSD_RUNCRANKPLAUSIBILITY_SUPPORT == STD_ON)
/* Satisfies: [$SDD_SUSD 105] */
typedef enum
{
    SUSD_DIAGNOSTIC_NO_STATUS_TO_REPORT = 0x00, /* No Status to Report */
    SUSD_DIAGNOSTIC_PASSED              = 0x01, /* Diagnostic Passed */
    SUSD_DIAGNOSTIC_FAILED              = 0x02  /* Diagnostic Failed */
} SUSD_DiagnosticStatusType;
#endif

/* Enable from individual SW-C to SUSD to shut down. SUSD will request shut down to BswM when all SW-C
   are sending EnableShutdown.
   DisableShutdown - Value 0 to 3
   Reserved - 4 to 254
   EnableShutdown  - 255
*/
/* Satisfies: [$SDD_SUSD 102] */
#define ENABLESHUTDOWN      (SUSD_UserRequestType)255
#define READYFORSHUTDOWN    (SUSD_UserRequestType)254

/* Satisfies: [$SDD_SUSD 001] [$SDD_SUSD 135]*/
/* Pointer to function for SUM Permit Execution Rte Interfaces */
typedef P2FUNC(Std_ReturnType, TYPEDEF, PermitExecutionInterface) (SUSD_SWC_ExecutionStatusType data); /* PRQA S 3448 EOF */
/*declaration is in header file as this has been used globally*/

/* Satisfies: [$SDD_SUSD 136] */
/* Pointer to function for SUM Shutdown Permission Rte Interfaces */
typedef P2FUNC(Std_ReturnType, TYPEDEF, SUMShutdownPermInterface) (P2VAR(SUSD_UserRequestType, AUTOMATIC, AUTOMATIC)data);

/* Satisfies: [$SDD_SUSD 001],[$SDD_SUSD 016], [$SDD_SUSD 017], [$SDD_SUSD 018] */
typedef struct SUSD_RteInterfaces
{
  /* Satisfies: [$SDD_SUSD 135] */
  /* Mapping of SWC with the wakeup to indicate the Permit Execution */
  P2FUNC(Std_ReturnType, TYPEDEF, SWCPermitExecutionInterface) (VAR(SUSD_SWC_ExecutionStatusType, AUTOMATIC)data);

  /* Satisfies: [$SDD_SUSD 136] */
  /* Mapping of SWC with the wakeup to read the Shutdown Permission */
  P2FUNC(Std_ReturnType, TYPEDEF, SWCShutdownPermInterface) (P2VAR(SUSD_UserRequestType, AUTOMATIC, AUTOMATIC)data);

  /* A boolean calibration parameter for each shutdown permission interface to control the processing of its permissions */
  P2FUNC(boolean, TYPEDEF, ShutdownPermControl) ();
}SUSD_SWCRteInterfaces;


/* Init values */
#define SUSD_ZERO 0u
#define SUSD_ONE 1u
#define SUSD_TWO 2u
#define SUSD_THREE 3u
#define SUSD_FOUR 4u
#define SUSD_FIVE 5u
#define SUSD_SIX 6u
#define SUSD_SEVEN 7u
#define SUSD_EIGHT 8u
#define SUSD_NINE 9u
#define SUSD_TEN 10u
#define SUSD_INIT_VOLTAGE 120u
#define SUSD_MAX_VALUE 255u
#define SUSD_TRUE (boolean)1
#define SUSD_FALSE (boolean)0
#define SUSD_HIGH (boolean)1
#define SUSD_LOW (boolean)0
#if(SUSD_RUNCRANKPLAUSIBILITY_SUPPORT == STD_ON)
#define SUSD_ACTIVE (RunCrankPlausibility)1
#define SUSD_INACTIVE (RunCrankPlausibility)0
#endif
#define SUSD_IN_RANGE 0
#define SUSD_OUT_OF_RANGE_LOW 1
#define SUSD_OUT_OF_RANGE_HIGH 2
/* Satisfies: [$SDD_SUSD 103] */
#define SUSD_CONTROLLER_VOLTAGE_THRESHOLD 95u
/* Satisfies: [$SDD_SUSD 104] */
#define SUSD_CONTROLLER_VOLTAGE_HYSTERESIS_LOW 5u

#define SUSD_MASKBITS_8TO15 (uint16)0xFF00u
#define SUSD_MASKBITS_16TO23 (uint32)0xFF0000u
#define SUSD_MASKBITS_24TO31 (uint32)0xFF000000u


/* Diagnostic Reprogramming Request */
#define SUM_REPREQ_ACTIVE RTE_MODE_DiagnosticReprogramming_ModeGroup_SUM_REPREQ_ACTIVE
#define SUM_REPREQ_INACTIVE RTE_MODE_DiagnosticReprogramming_ModeGroup_SUM_REPREQ_INACTIVE

/* Diagnostic Reset Request */
#define SUM_RESREQ_ACTIVE RTE_MODE_DiagnosticRequest_ModeGroup_SUM_RESREQ_ACTIVE
#define SUM_RESREQ_INACTIVE RTE_MODE_DiagnosticRequest_ModeGroup_SUM_RESREQ_INACTIVE

/* Number of ECU Wakeup Detection Records*/
#define SUSD_NO_OF_WAKEUPDETRECS 10u

/* DID E0D7 max lenght*/
#define SUSD_E0D7_MAX_LENGHT (182u)

/*Macro to set the shutdown target bit of the PermitExecution interface*/
#define NOT_OFF 0x2u

/*Macro to indicate that the last reset was an invalid reset*/
#define LAST_RESET_INVALID 0x3u

/*Define Security levels */
#define NOT_Applicable 0u
#define With_OBD 1u
#define With_Cybersecurity 2u
#define Without_Cybersecurity 3u

/*
 ***************************************************************************************************
 * Configuration Constant arrays
 ***************************************************************************************************
 */

/* Satisfies: [$SDD_SUSD 001] */
/* Constant array used to store the Permit Execution values for SUSD to invoke for other SUM and SWC connected */
extern CONST (PermitExecutionInterface, RTE_CONST) SUSD_GaaSUM_PermitExec_Interface[SUSD_SUM_PERMEXE];/* PRQA S 3447 EOF */
/* extern is required here since this array has been used globally*/

/* Constant array used to store the Permit Execution values for SUSD to invoke for other SUM and SWC connected */
extern CONST (uint32, RTE_CONST) SUSD_GaaWakeupSourcesArray[NO_OF_WAKEUPSRCS];

/* Satisfies: [$SDD_SUSD 001] */
/* Constant array used to store the Permit Execution values for SUSD to invoke for other SUM and SWC connected */
extern CONST (SUMShutdownPermInterface, RTE_CONST) SUSD_GaaSUM_ShutdwnPer_Interface[SUSD_SUM_SHUTPERM];

#endif /* SUM_SUSD_TYPES_H*/
/********************************** END OF FILE ******************************************************/
