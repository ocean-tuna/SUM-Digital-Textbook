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
 * Description :
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
/* PRQA S 0380 1 # less than 1024 macros are being used in this file verified  */
#include "SUM_SUSD.h"
#include "SUM_SUSD_Ram.h"
/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Variables
 ***************************************************************************************************
 */

#if EXISTS_SWC == STD_ON
/* Variable used to store status(enable/disable) of shut down from SWC and other SUM */

/* PRQA S 1531 11 # Temporary suppression, the arrays can be used in other files. */
VAR(uint16, RTE_CODE) SUSD_GaaShutdownTimer[NO_OF_SWCS];
#endif

/* Variable used to store PermitExecution status for SUM interfaces */
VAR(SUSD_SWC_ExecutionStatusType, RTE_CODE) SUSD_GaaSUMPermitExecution[SUSD_SUM_PERMEXE];

/* Variable used to store status(enable/disable) of shut down from SWC and other SUM */
VAR(uint16, RTE_CODE) SUSD_GaaSUMShutdownTimer[SUSD_SUM_SHUTPERM];

VAR(uint16, RTE_CODE)SUSD_GaaWaitGlobalForceShutdownTimer;

/*$Satisfies: [$SDD_SUSD 137] */
/* Variable used to store the mode of the Application State Manager of SUSD to request BswM */
VAR(CurrentECUOperatingModeType, RTE_CODE) SUSD_GddASM_CurrentMode = SUM_INIT;

/*$Satisfies: [$SDD_SUSD 137] */
/* Variable used to store the mode of the Application State Manager of SUSD to request BswM */
/* PRQA S 1533 1 # This variable can be used in the multiple file and can not be turned into static as well as moved to static code*/
/* PRQA S 1531 1 # This variable can be used in the multiple file and can not be turned into static as well as moved to static code*/
VAR(CurrentECUOperatingModeType, RTE_CODE) SUSD_GddCurrentEcuState = SUM_INIT;

VAR(uint32, RTE_CODE) SUSD_GddValidatedWakeupSrc;

#if EXISTS_SWC == STD_ON
/* Variable used to store status(enable/disable) of shut down from SWC and other SUM */
VAR(SUSD_UserRequestType, RTE_CODE) SUSD_GaaSWCShutdownPermission[NO_OF_SWCS];
#endif

/* Variable used to store status(enable/disable) of shut down from other SUM */
VAR(SUSD_UserRequestType, RTE_CODE) SUSD_GaaSUMShutdownPermission[SUSD_SUM_SHUTPERM];
#if(SUSD_POWERMODING_SLAVE_SUPPORT == STD_ON)
/* Variable used to store Vehicle Power Mode read from the Power Mode Manager */
VAR(SUM_VehiclePowerMode_Signal_States, RTE_CODE) SUSD_VehiclePowerMode;
/* Variable used to store Vehicle Power Mode Invalidity read from the Power Mode Manager */
VAR(boolean, RTE_CODE) SUSD_VehiclePowerModeInv;
#endif

/* Variable used to store status(enable/disable) of shut down from other SUM */
VAR(uint32, RTE_CODE) SUSD_GaaWakeUpSources[NO_OF_WAKEUPSRCS];

/* Variable used to store Vehicle Battery Voltage read from the SW-C */
VAR(uint16, RTE_CODE) SUSD_GucVehicleBattVoltage;

/* Variable used to store Battery voltage status(normal, under, over) for each voltage range */
/* PRQA S 1531 1 # This array can be used in the multiple file and can not be turned into static as well as moved to static code*/
VAR(uint16, RTE_CODE) SUSD_GucBatteryVoltageStatus[NO_OF_VOLTRANGE];

/* Variable used to store notification from BswM for Diagnostic Reprogramming request to support jump to bootloader request received from diagnostic  tester */
VAR(boolean, RTE_CODE) SUSD_GblDiagReprgRequest;

/* Variable used to store notification from BswM for Diagnostic reset request received from diagnostic tester */
VAR(boolean, RTE_CODE) SUSD_GblDiagResetRequest;

/*$Satisfies: [$SDD_SUSD 160] $Satisfies: [$SDD_SUSD 165] */
/* Variable used to store ECU Wakeup Detection Records */
VAR(SUSD_ECUWakeupDetectionRecord, RTE_CODE) SUSD_ECUWakeupDetectionRecordsArray[SUSD_NO_OF_WAKEUPDETRECS];

/*Variable to hold last NVM write request status */
VAR(Std_ReturnType, RTE_CODE) Susd_LastRequestResult_ECUWakeUpDetectionRecords;

/* Variable used to store SUM_SUSD_RESETExecution_TIMER */
VAR(uint16, RTE_CODE) SUM_SUSD_RESETExecution_TIMER;

/*Variable to hold last NVM write request status */
/* PRQA S 1531 1 # This variable can be used in the multiple file and can not be turned into static as well as moved to static code*/
VAR(Std_ReturnType, RTE_CODE) Susd_LastRequestResult_SUM_SUSD_RESETExecution_TIMER;

/* Global variable to track wakeup bits for each SUM module (bit position corresponds to module index) */
VAR(uint32, RTE_CODE) SUSD_ModuleWakeupBits;

/********************************** END OF FILE ******************************************************/
