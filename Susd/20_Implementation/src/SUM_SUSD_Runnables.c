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

/*****************************************************************************************************
*                                     Includes
*****************************************************************************************************/
/* PRQA S 0380 1 # header included from RTE */
#include "Rte_SUSD.h"
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

/*
 ***************************************************************************************************
 * Functions
 ***************************************************************************************************
 */

/***********************************************************************************

  Function name :   RE_SUSD_BatteryNotifications
  Description   :   This runnable is responsible for monitoring the vehicles battery voltage and
                    notify the SW-Cs with the correct current state at the rate SUSD_t_Battery_Periodicity.
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 034] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h" /* PRQA S 5087 EOF */
/* include MemMap.h required here to occupy the space in memory for the described macro*/
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(void, SUSD_CODE) RE_SUSD_BatteryNotifications(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
  /* Check the Voltage levels to consider various levels of comparison */
  SUSD_SWC_VehicleBatteryVoltage();
#if(SUSD_VEHICLE_BATTERY_STATE_OF_CHARGE_SUPPORTED == STD_ON)
  SUSD_BswM_VehicleBatteryStateOfCharge();
#endif
	}
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
/***********************************************************************************

  Function name :   RE_SUSD_AppStateManager
  Description   :   This runnable is responsible for managing the startup/shutdown notifications to the SW-Cs based
                    on the input status processed by the runnable at the rate SUSD_t_AppManager_Periodicity.
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 035] [$SDD_SUSD 145]*/
 #define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
 FUNC(void, SUSD_CODE) RE_SUSD_AppStateManager(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
		/* Periodic function to decide the states of SUSD */
		/* Process shutdown refresh timers first to check for timeouts */
		Processing_SUSD_Shutdown_Refresh_Timer();
    /* Process shutdown Force timers */
    Processing_SUSD_Shutdown_GloabalWait_Force_Timer();
    
		SUSD_ApplnStateManager();
	}
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"

#if(SUSD_POWERMODING_SLAVE_SUPPORT == STD_ON)
/***********************************************************************************

  Function name :   RE_SUSD_VehiclePowerMode
  Description   :   This runnable is responsible for managing the Power Mode Slave and has the periodicity of SUSD_T_RUNNABLE_PMSMANAGEMENT_PERIODICITY
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 036] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
 /* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE*/
FUNC(void, SUSD_CODE) RE_SUSD_VehiclePowerMode(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
		SUSD_VehiclePowerModeDetermination();
	}
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
#endif

#if(SUSD_RUNCRANKPLAUSIBILITY_SUPPORT == STD_ON)
/***********************************************************************************

  Function name :   RE_SUSD_RunCrankManagement
  Description   :   This runnable is responsible for managing the Run Crank Plausibility with periodicity SUSD_t_RunCrank_Periodicity
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 037] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(void, SUSD_CODE) RE_SUSD_RunCrankManagement(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
		SUSD_RunCrankPlausibility();
	}
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
#endif

/***********************************************************************************

  Function name :   RE_SUSD_DiagnosticReprogrammingRequest
  Description   :   Runnable to be triggered every time there is RTE write from BswM to SUSD on Reprogramming request
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 039] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(void, SUSD_CODE) RE_SUSD_DiagnosticReprogrammingRequest(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
		SUSD_DiagnosticReprogrammingRequest(SUM_REPREQ_ACTIVE);
	}
  /*Update also the Permit Execution Logic*/
  SUSD_SUM_PermitExecution();
  SUSD_SWC_PermitExecution();
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
/***********************************************************************************

  Function name :   RE_SUSD_DiagnosticResetRequest
  Description   :   Runnable to be triggered every time there is RTE write from BswM to SUSD on Reset request
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 040] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(void, SUSD_CODE) RE_SUSD_DiagnosticResetRequest(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
		SUSD_DiagnosticResetRequest(SUM_RESREQ_ACTIVE);
	}
  /*Update also the Permit Execution Logic*/
  SUSD_SUM_PermitExecution();
  SUSD_SWC_PermitExecution();
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"

#if(SUSD_POWERMODING_SLAVE_SUPPORT == STD_ON)
/***********************************************************************************

  Function name :   RE_SUSD_VPwrSetEnableCond
  Description   :   Function Runnable for Vehicle power mode condition
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 041] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(Std_ReturnType, SUSD_CODE) RE_SUSD_VPwrSetEnableCond(P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Data)//Scheduled in the OS
{
	 Std_ReturnType Status;
	if(SUSD_GetInitState() == E_OK)
	{	
	  *Data = (uint8)SUSD_VehiclePowerMode;
	  Status = E_OK;
	}
	else
	{
		Status = E_NOT_OK;
	}
	return Status;
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
#endif

/***********************************************************************************

  Function name :   SUSD_Dcm_ClearECUWakeupDetectionRecords_Start_41A
  Description   :   Runnable of RID 0x41A Clear ECU Wakeup Detection Records
  Parameter in  :   OpStatus - status of current diagnostic operation
  Parameter out :   ErrorCode - parameter for sending NRCs
  Return value  :   E_OK, E_NOT_OK, E_PENDING
 ************************************************************************************/
/* Satisfies: [$SDD_SUSD 151] [$SDD_SUSD 150] [$SDD_SUSD 150] */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(Std_ReturnType, SUSD_CODE) SUSD_Dcm_ClearECUWakeupDetectionRecords_Start_41A(VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
                                                                                  P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode)
{   //Scheduled in the OS
    Std_ReturnType ret = E_NOT_OK;
    /* Variable to iterate on ECU Wakeup Detection Records */
    uint32  ECUWakeupDetRec;

    /* Clear all fields of Susd_DID_E0D7 */
    /* Satisfies: [$SDD_SUSD 225] */
    Sum_SUSD_ActivityRecord_Clear();    
    /* Clear E0D7 Monitor Disabled flag */
    /* Satisfies: [$SDD_SUSD 227] */
    SUSD_MonitorDisabled = FALSE;

    switch (OpStatus)
    {
        case DCM_INITIAL:
            for(ECUWakeupDetRec = SUSD_ZERO; ECUWakeupDetRec < SUSD_NO_OF_WAKEUPDETRECS; ECUWakeupDetRec++)
            {
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].ECUWakeupDetectionRecordId[0] = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].ECUWakeupDetectionRecordId[1] = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].ECUWakeupDetectionRecordId[2] = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].ECUWakeupDetectionRecordId[3] = SUSD_ZERO;

              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].TimeStampOfOccurence[0]    = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].TimeStampOfOccurence[1]    = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].TimeStampOfOccurence[2]    = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].TimeStampOfOccurence[3]    = SUSD_ZERO;
              SUSD_ECUWakeupDetectionRecordsArray[ECUWakeupDetRec].TimeStampOfOccurence[4]    = SUSD_ZERO;
            }

            /* Update the ECU Wakeup Detection Records NVM Block */
            Susd_NvmWriteBlock(&susd_NVMECUWakeUpDetectionRecordsBlock_FP, &Susd_LastRequestResult_ECUWakeUpDetectionRecords);

            if(Susd_LastRequestResult_ECUWakeUpDetectionRecords == E_PENDING)
            {
                ret = E_PENDING;
            }
            else
            {
                ret = E_NOT_OK;
                *ErrorCode = DCM_E_GENERALPROGRAMMINGFAILURE;
            }
            break;
        case DCM_PENDING:
            /* Get the last request result if still pending. */
            if (E_PENDING == Susd_LastRequestResult_ECUWakeUpDetectionRecords)
            {
                Susd_NvmGetErrorStatus(&susd_NVMECUWakeUpDetectionRecordsBlock_FP,
                                       &Susd_LastRequestResult_ECUWakeUpDetectionRecords);
            }
            ret = Susd_LastRequestResult_ECUWakeUpDetectionRecords;
            if(ret == E_NOT_OK)
            {
                *ErrorCode = DCM_E_GENERALPROGRAMMINGFAILURE;
            }
            break;
        default:
            /* Nothing to do. */
            break;
    }
    return ret;
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"

/***********************************************************************************

  Function name :   RE_Manage_SUM_SUSD_RESETExecution_TIMER
  Description   :   Function Runnable for managing SUM_SUSD_RESETExecution_TIMER
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   None
 ************************************************************************************/
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/* PRQA S 1503 3 # When the SUSD is integrated, this Runnable will be called by RTE */
FUNC(void, SUSD_CODE) RE_Manage_SUM_SUSD_RESETExecution_TIMER(void)//Scheduled in the OS
{
	if(SUSD_GetInitState() == E_OK)
	{
      Manage_SUM_SUSD_RESETExecution_TIMER();
#if(SUSD_POWERMODING_SLAVE_SUPPORT == STD_ON)      
      Manage_SUM_SUSD_RunningResetWait_TIMER();
#endif      
	}
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"

#if (SUSD_RUNNABLE_INIT_SUPPORTED == STD_ON)
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h"
/**
 * @brief			Runnable to execute SUSD init function .
 *
 * @param[in]		None
 * @param[inout]	None
 * @param[out]		None
 * @return			void
 */
/* PRQA S 1503 1 # The function is declared by RTE */
void RE_SUSD_Init(void){
    static uint32 susd_init_val = 0u;

    if( susd_init_val == 0u ) {
         susd_init_val = SUM_INITIALIZED;
         SUSD_Init();
    }
}
#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
#endif
/********************************** END OF FILE ******************************************************/
