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
 * Description : SUM_SUSD header ram variables
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

#ifndef SUSD_RAM_H
#define SUSD_RAM_H

#include "SUM_SUSD.h"
/***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************
 */

#if EXISTS_SWC == STD_ON
/* Timer Array used to refresh the time, when the DRE of SWC is called and interface is read */
extern VAR(uint16, RTE_CODE) SUSD_GaaShutdownTimer[NO_OF_SWCS];
#endif
/* Variable used to store PermitExecution status for SUM interfaces */
extern VAR(SUSD_SWC_ExecutionStatusType, RTE_CODE) SUSD_GaaSUMPermitExecution[SUSD_SUM_PERMEXE];
/* Timer Array used to refresh the time, when the DRE of SUM is called and interface is read */
extern VAR(uint16, RTE_CODE) SUSD_GaaSUMShutdownTimer[SUSD_SUM_SHUTPERM];

/* Global Wait force Timer Used when no permit Execution On*/
extern VAR(uint16, RTE_CODE) SUSD_GaaWaitGlobalForceShutdownTimer;

/* Variable used to store the mode of the Application State Manager of SUSD to request BswM */
extern VAR(CurrentECUOperatingModeType, RTE_CODE) SUSD_GddASM_CurrentMode;

/* Variable used to store the mode of the Application State Manager of SUSD to request BswM */
extern VAR(CurrentECUOperatingModeType, RTE_CODE) SUSD_GddCurrentEcuState;

/* Variable used to store the mode of the Application State Manager of SUSD to request BswM */
extern VAR(uint32, RTE_CODE) SUSD_GddValidatedWakeupSrc;

#if EXISTS_SWC == STD_ON
/* Variable used to store status(enable/disable) of shut down from SWC and other SUM */
extern VAR(SUSD_UserRequestType, RTE_CODE) SUSD_GaaSWCShutdownPermission[NO_OF_SWCS];
#endif

/* Variable used to store status(enable/disable) of shut down from other SUM */
extern VAR(SUSD_UserRequestType, RTE_CODE) SUSD_GaaSUMShutdownPermission[SUSD_SUM_SHUTPERM];

#if(SUSD_POWERMODING_SLAVE_SUPPORT == STD_ON)
/* Variable used to store Vehicle Power Mode read from the Power Mode Manager */
extern VAR(SUM_VehiclePowerMode_Signal_States, RTE_CODE) SUSD_VehiclePowerMode;
/* Variable used to store Vehicle Power Mode Invalidity read from the Power Mode Manager */
extern VAR(boolean, RTE_CODE) SUSD_VehiclePowerModeInv;
#endif

/* Variable used to store status(enable/disable) of shut down from other SUM */
extern VAR( uint32, RTE_CODE) SUSD_GaaWakeUpSources[NO_OF_WAKEUPSRCS];

/* Variable used to store Vehicle Battery Voltage read from the SW-C */
extern VAR(uint16, RTE_CODE) SUSD_GucVehicleBattVoltage;

/* Variable used to store Battery voltage status(normal, under, over) for each voltage range */
extern VAR(uint16, RTE_CODE) SUSD_GucBatteryVoltageStatus[NO_OF_VOLTRANGE];

/* Variable used to store notification from BswM for Diagnostic Reprogramming request to support jump to bootloader request received from diagnostic  tester */
extern VAR(boolean, RTE_CODE) SUSD_GblDiagReprgRequest;

/* Variable used to store notification from BswM for Diagnostic reset request received from diagnostic tester */
extern VAR(boolean, RTE_CODE) SUSD_GblDiagResetRequest;


/* Variable used to store ECU Wakeup Detection Records */
extern VAR(SUSD_ECUWakeupDetectionRecord, RTE_CODE) SUSD_ECUWakeupDetectionRecordsArray[SUSD_NO_OF_WAKEUPDETRECS];

/*Variable to hold last NVM write request status */
extern VAR(Std_ReturnType, RTE_CODE) Susd_LastRequestResult_ECUWakeUpDetectionRecords;

/* Variable used to store SUM_SUSD_RESETExecution_TIMER */
extern VAR(uint16, RTE_CODE) SUM_SUSD_RESETExecution_TIMER;

/*Variable to hold last NVM write request status */
extern VAR(Std_ReturnType, RTE_CODE) Susd_LastRequestResult_SUM_SUSD_RESETExecution_TIMER;

/* Global variable to track wakeup bits for each SUM module (bit position corresponds to module index) */
extern uint32 SUSD_ModuleWakeupBits;

#endif /* SUSD_RAM_H */
/********************************** END OF FILE ******************************************************/
