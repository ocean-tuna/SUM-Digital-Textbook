/* 
 ******************************************************************************
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
 * Component   : SUM_SOH
 * Date        : Jun 25, 2026
 * Revision    : 8.0.1_rc
 * Description : Source for SOH
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

/*! @defgroup SUM_SOH_c The SUM_SOH.c file
 *  This is the SUM_SOH.c file
 *  @{
 */

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "Rte_SUM_SOH.h"
#include "SUM_SOH_Utilities.h"

/* START OF PROTOTYPES */

#define SUM_SOH_START_SEC_CODE
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/

static FUNC(void, SUM_SOH_CODE) SOH_CheckTransitionToNormal(SUSD_SWC_ExecutionStatusType SUSDPermitExecution);

#if (SOH_MAC_FAILURES_SUPPORT_ENABLED == STD_ON)
static uint8 MACFailuresType3_DelayTime;
#endif


#define SUM_SOH_STOP_SEC_CODE
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/

/*!
 *	This function is called only by Runnable_SOH_Main() in order to perform the initial checks/activities of the Shutdown Detector.
 *
 *	A shutdown is detected if both conditions are present:
 *	\Requirement{023}
 *		- port PNC_SUSD_ShutdownPermission says all the Partial Networks are deactivated, so its value is 0xFF
 *	\Satisfies{005}
 *	\Requirement{024}
 *		- port SUSD_SOH_PermitExecution has the ECU shutdown target bit (bit 1) equals to 0.
 *	\Satisfies{005}
 *
 *	\Requirement{027}
 *		If a shutdown is detected, the flag ShutdownTimerFlag becomes TRUE, so SUM_SOH_ShutDown_Timer can be incremented by the Time
 *		Manager Runnable.
 *	\Satisfies{005}
 *
 *	\Requirement{025}
 *		A shutdown is aborted if a wakeup event occurs, so if port SUSD_SOH_PermitExecution has the LSB bit equals to 1.
 *	\Satisfies{007}
 *
 *	\Requirement{026}
 *		If a shutdown is aborted, SUM_SOH_ShutDown_Timer is cleared.
 *	\Satisfies{023}
 */
#if SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED
static FUNC(void, SUM_SOH_CODE) SOH_ShutdownDelayDetectorLogic(SUSD_SWC_ExecutionStatusType SUSDPermitExecution);
#endif

static FUNC(boolean, SUM_SOH_CODE) SOH_CheckMEC(void);

/* END OF PROTOTYPES */


/* START OF CODE */
#define SUM_SOH_START_SEC_CODE
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/

/*!
 * The SOH_Init function loads the values stored in the NvM and initializes the variables to their default value.
 *
 *@param[in]  void
 *@return void
 */
#if (SOH_GENERATE_INIT_RUNNABLE_STATUS == STD_ON)
static FUNC(void, SUM_SOH_CODE) SOH_Init(void)
#else
/* PRQA S 1503 1 *//* The function is called through a BSW Action*/
FUNC(void, SUM_SOH_CODE) SOH_Init(void)
#endif
{
	/*[$Satisfies $SDD_SOH 002]*/
	SOH_NvMReadall();

	/*[$Satisfies $SDD_SOH 003]*/
	Rte_IrvWrite_Runnable_SOH_Main_SOH_ExecutionState((SOH_INIT_MODE));

	SOH_InitVariables();


#if (SOH_MAC_FAILURES_SUPPORT_ENABLED == STD_ON)
	/*[$Satisfies $SDD_SOH 071]*/
	MACFailuresType3_DelayTime = Rte_CData_SUM_SOH_ReplayCount_Start_Thresh(); /* PRQA S 3469 */ /* The owner of Rte_CData_SUM_SOH_ReplayCount_Start_Thresh is the RTE, not this module */
#endif

#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
	SUM_SOH_ShutDown_Timer = 0x00u;
#endif
}
/**
 * \defgroup Runnables
 * @{
 * \addtogroup TimePeriodic
 * @{
 */
/*!
 *
 *@param[in]  void
 *@return void
 */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_Main(void)
{
	static SUSD_UserRequestType SOHShutdownPermission = SOH_PERMITEXECUTION;
	static Std_ReturnType NvMstatus;
	SUSD_SWC_ExecutionStatusType SUSDPermitExecution;
	SOH_ExecutionStateType ExecutionStatus = Rte_IrvRead_Runnable_SOH_Main_SOH_ExecutionState(); /* PRQA S 3469 */ /* The owner of Rte_CData_SUM_SOH_ReplayCount_Start_Thresh is the RTE, not this module */

	(void) Rte_Read_SUSD_SOH_PermitExecution_SUSD_SWC_ExecutionStatus(&SUSDPermitExecution);

	/* Notify the reaching of Checkpoint to WdgM */
	/* [$Satisfies $SDD_SOH 093] [$Satisfies $SDD_SOH 101]*/
	(void) Rte_Call_RP_SOH_WdgM_AliveSupervision_CheckpointReached();

	/*[$Satisfies $SDD_SOH 004]*/
    switch (ExecutionStatus)
    {
        /* The variable is set to SOH_INIT_MODE by RTE and  SOH_Init function*/
        case SOH_INIT_MODE:

        	/*[$Satisfies $SDD_SOH 006] [$Satisfies $SDD_SOH 007]*/
        	SOH_CheckTransitionToNormal(SUSDPermitExecution);
        	SOHShutdownPermission = SOH_PERMITSHUTDOWN;

            break;

        case SOH_NORMAL_MODE:

        	/*[$Satisfies $SDD_SOH 008] [$Satisfies $SDD_SOH 009]*/
        	/* PRQA S 3469 1 *//* The Rte_IrvRead_Runnable_SOH_Main_SOH_SOHRIDIsWriting owner is the RTE, not this module*/
        	if((SOH_CheckMEC()) && ((Rte_IrvRead_Runnable_SOH_Main_SOH_SOHRIDIsWriting())==FALSE))
        	{
        		(void)SOH_ClearItems(SOH_DetectorItems,NUM_OF_DETECTORS);
        	}

         	if((SUSDPermitExecution | SOH_PERMITEXECUTION_MASK) != SOH_PERMITEXECUTION)
         	{
            	Rte_IrvWrite_Runnable_SOH_Main_SOH_PermitExecution((FALSE));

                NvMstatus = SOH_CheckNvMWritesList(SOH_DetectorItems,NUM_OF_DETECTORS);

            	if(SOH_E_PENDING == NvMstatus)
            	{
                	SOHShutdownPermission = 0x0;
             	}
            	else
              	{
                    SOHShutdownPermission = SOH_PERMITEXECUTION;
            	}
            	Rte_IrvWrite_Runnable_SOH_Main_SOH_ExecutionState((SOH_SHUTDOWN_MODE));
        	}
        	else
         	{
            	Rte_IrvWrite_Runnable_SOH_Main_SOH_PermitExecution((TRUE));
            	SOHShutdownPermission = SOH_PERMITSHUTDOWN;
        	}

            break;

        case SOH_SHUTDOWN_MODE:
        	/*[$Satisfies $SDD_SOH 010] [$Satisfies $SDD_SOH 012]*/
        	if(SOH_E_PENDING == NvMstatus)
        	{
                NvMstatus = SOH_CheckNvMWritesList(SOH_DetectorItems,NUM_OF_DETECTORS);
            	SOHShutdownPermission = (SOHShutdownPermission + 1u) %  SOH_SHUTDOWN_CNT_LIMIT;
        	}
        	else
        	{
        		SOHShutdownPermission = SOH_PERMITSHUTDOWN;
                SOH_CheckTransitionToNormal(SUSDPermitExecution);
        	}
        	break;

        default:
        	/* No possible action, error */
            break;
    }

    (void)Rte_Write_SOH_SUSD_ShutdownPermission_SUSD_UserRequest((SOHShutdownPermission));

#if SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED
	/* Shutdown Delay Detector logic */
    SOH_ShutdownDelayDetectorLogic(SUSDPermitExecution);
#endif
}
/**
 * @}
 * @}
 */

/**
 * \defgroup Runnables
 * @{
 * \addtogroup TimePeriodic
 * @{
 */
/*!
 *Runnable_SOH_Timing implementation.
 *It reads the current Time from OS and if 1s is elapsed it updates:
 *- the Time Window of the detectors that are using a Time Windows filter
 *- the Shutdown timer.
 *
 *
 *@param[in]  void
 *@return void
 */
/* PRQA S 1503 3 *//* It's scheduled in the OS */
#if(NUM_OF_SOH_DETECTORS != 0u)
FUNC(void, SUM_SOH_CODE) Runnable_SOH_Timing(void)
{
	uint32 counterValue;

	/* Read counter */
	/* Counter OsCounterSOHName must be ticked at 200 ms. */
	if( GetCounterValue(OsCounterSOHName, &counterValue)==E_OK )
	{
		/* Manage the Time Windows of the Time Window Filter */
		SOH_UpdateTimeWindows(counterValue);

#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
		/* Manage the Shutdown Timer of the Shutdown Delay Monitor */
		SOH_MonitorShutDownTimer(counterValue);
#endif
	}
}
#endif
/**
 * @}
 * @}
 */


static FUNC(void, SUM_SOH_CODE) SOH_CheckTransitionToNormal(SUSD_SWC_ExecutionStatusType SUSDPermitExecution)
{
    if ((SUSDPermitExecution | SOH_PERMITEXECUTION_MASK) == SOH_PERMITEXECUTION)
    {
    	Rte_IrvWrite_Runnable_SOH_Main_SOH_PermitExecution((TRUE));
    	Rte_IrvWrite_Runnable_SOH_Main_SOH_ExecutionState((SOH_NORMAL_MODE));
    }
    else
    {
    	/* SOH functionality cannot be executed */
    	Rte_IrvWrite_Runnable_SOH_Main_SOH_PermitExecution((FALSE));
    }
}

#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
/**
 * SOH_ShutdownDelayDetectorLogic implementation.
 *
 *@param[in]  SUSD_SWC_ExecutionStatusType
 *@return void
 */
/* PRQA S 1505 3 *//* Helps reducing STCYC */
static FUNC(void, SUM_SOH_CODE) SOH_ShutdownDelayDetectorLogic(SUSD_SWC_ExecutionStatusType SUSDPermitExecution)
{
	boolean ShutdownTimerFlag;
	SUSD_UserRequestType PartialNetworksDeactivated;
	static boolean shutdown_detected = FALSE;

    /* PRQA S 3469 3 *//* It's an RTE problem */
	ShutdownTimerFlag = Rte_IrvRead_Runnable_SOH_Main_SOH_ShutdownTimerFlag();

	/* [$Satisfies $DDS_SOH 025] */
 	/* Check if the Wakeup bit in SUSD_SWC_ExecutionStatus is ACTIVE: shutdown aborted */
 	if(((SUSDPermitExecution & 0x01u) == 0x01u) && ShutdownTimerFlag)
 	{
 		/* [$Satisfies $DDS_SOH 026] */
		SUM_SOH_ShutDown_Timer = 0x00u;
		Rte_IrvWrite_Runnable_SOH_Main_SOH_ShutdownTimerFlag((FALSE));
		shutdown_detected = FALSE;

 	}
	/* [$Satisfies $DDS_SOH 024] */
 	else if(((SUSDPermitExecution & 0x01u) == 0x00u) && !ShutdownTimerFlag && !shutdown_detected)
 	{
	    /* Check if the Shutdown Target bit in SUSD_SWC_ExecutionStatus is OFF */
	 	if((SUSDPermitExecution & 0x02u) == 0x00u)
	 	{
	 		/* Then, check if PNC notified that all partial networks are deactivated */
	 		(void)Rte_Read_PNC_SUSD_ShutdownPermission_SUSD_UserRequest(&PartialNetworksDeactivated);
	 		/* [$Satisfies $DDS_SOH 023] */
	 		if(PartialNetworksDeactivated == 0xFFu)
	 		{
		 		/* [$Satisfies $DDS_SOH 027] */
	 			/* Allow Runnable_SOH_Timing() to monitor ShutDown Timer. */
	 			Rte_IrvWrite_Runnable_SOH_Main_SOH_ShutdownTimerFlag((TRUE));
	 			shutdown_detected = TRUE;
	 		}
	 	}
 	}
 	else
 	{
 		/* Nothing to do */
 	}
}
#endif

static FUNC(boolean, SUM_SOH_CODE) SOH_CheckMEC(void)
{
	static uint8 InitMEC = 0u;
	uint8 LocalMEC;
	boolean ret = FALSE;

	(void)Rte_Read_DIAG_SOH_GetMEC_GetMec(&LocalMEC);

	if((InitMEC == 0u) && (LocalMEC != 0u)) {
			InitMEC = LocalMEC;
	}

	if((InitMEC != 0u) && (LocalMEC == 0u)){
		ret = TRUE;
		InitMEC = 0u;
	}
	else {
		//do nothing
	}

	return ret;
}


#if (SOH_GENERATE_INIT_RUNNABLE_STATUS == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup TimePeriodic
 * @{
 */
/*!
 * Runnable_SOH_Init implementation.
 * The Runnable calls SOH_init function once to init all the needed metrics of all the detectors
 * and the internal states of SUM_SOH
 *  *
 *@param[in]  void
 *@return void
 */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_Init(void)
{
	static uint32 Soh_init_count = 0x0;

	/* Check to make sure that SOH_init function is called only once */
	if(Soh_init_count != SUM_INITIALIZED)
	{
		SOH_Init();
		Soh_init_count = SUM_INITIALIZED;
	}

}
/**
 * @}
 * @}
 */

#endif

#if (SOH_IMMEDIATE_DELAYED_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOHResetReliabilityMonitor_ImmeDelayedReset implementation.
 *It handles the notification sent by the SUM_SUSD through the port SUSD_SOH_ResetOccured
 *to identify if the reset is immediate or delayed and update the proper event detector (counter + filter + memory handling).
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 025] [$Satisfies $SDD_SOH 026] [$Satisfies $SDD_SOH 027] [$Satisfies $SDD_SOH 029]*/
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOHResetReliabilityMonitor_ImmeDelayedReset(void)
{
	SUSD_SOH_ResetOccurredType SUSD_ResetType;

	(void)Rte_Read_SUSD_SOH_ResetOccurred_SUSDResetOccurred(&SUSD_ResetType);

	if(SUSDResetOccurred_ImmediateResetOccurred == SUSD_ResetType)
	{
		/* Pass event into corresponding to filter */
		SOH_Event_Detector(SOH_DetectorItems[SOH_ImmeReset]);

		/* Check condition to Write DID counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_ImmeReset]);
	}
	else if(SUSDResetOccurred_DelayedResetOccurred == SUSD_ResetType)
	{
		/* Pass event into corresponding to filter */
		SOH_Event_Detector(SOH_DetectorItems[SOH_DelayedReset]);

		/* Check condition to Write DID counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_DelayedReset]);
	}
	else
	{
		/* Invalid or default value */
	}
}
/**
 * @}
 * @}
 */
#endif

#if (SOH_DIAG_RESET_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOHResetReliabilityMonitor_DiagReset implementation.
 *It handles the notification sent by the SUM_DIAG through the port DIAG_SOH_ECUResetRequest
 *to identify if the reset is immediate or delayed and update the proper event detector (counter + filter + memory handling).
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 025] [$Satisfies $SDD_SOH 028] [$Satisfies $SDD_SOH 030] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOHResetReliabilityMonitor_DiagReset(void)
{
	boolean DIAG_ECUResetRequest = FALSE;

	(void)Rte_Read_DIAG_SOH_ResetRequest_DiagResetRequest(&DIAG_ECUResetRequest);

	if(TRUE == DIAG_ECUResetRequest)
	{
		/* Pass event into corresponding to filter */
		(void)SOH_Event_Detector(SOH_DetectorItems[SOH_DiagReset]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_DiagReset]);
	}
	else
	{
		/* No Diagnostic Reset Request */
	}
}
/**
 * @}
 * @}
 */
#endif

#if ((SOH_STATUS_NVM_SUPPORT_ENABLED == STD_ON) || (SOH_ACTION_NVM_SUPPORT_ENABLED == STD_ON) || (SOH_PRIVILEGE_NVM_SUPPORT_ENABLED == STD_ON) || (SOH_WdgMDEM_SUP_ERRORS_SUPPORT_ENABLED == STD_ON))
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */ 

/**
 * Runnable_SOH_ERRH_BswEvent implementation.
 * It handles the notification sent by the SUM_ERRH through the port ERRH_E_BswEvent
 * to identify the NVM Error type and update the proper event detector (counter + filter + memory handling).
 * @param[in]  void
 * @return void
 */
/*[$Satisfies $SDD_SOH 044] [$Satisfies $SDD_SOH 045] [$Satisfies $SDD_SOH 047] [$Satisfies $SDD_SOH 099]*/
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_ERRH_BswEvent(void)
{
	static SUM_ERRH_BswEvent NVMErrorOld;
	SUM_ERRH_BswEvent NVMError, counter, NVMErrorSupport;
	SOH_DetectorsId DetectorID;
	boolean errorPresent;

	/*[$Satisfies $SDD_SOH 046]*/
	(void)Rte_Read_RP_ERRH_SOH_ERRH_E_BswEvent_BswEvent(&NVMErrorSupport);
	/*Check only the ones that changed from 0 to 1 (negate the old value to enable the check of old zero value)*/
	NVMError = ((~NVMErrorOld) & NVMErrorSupport);
	/*Save the latest error status processed*/
	NVMErrorOld=NVMErrorSupport;

	/*For each possible bit*/
	counter = ERRH_BSWEVENT_NVM_E_HARDWARE;
	while(NVMError>0u)
	{
		/*Check if is set*/
		errorPresent =  ((NVMError &  0x1u) == 0x1u ) ? TRUE:FALSE ;

		if(errorPresent)
		{
			/*Switch on the set error bit*/
			switch(counter)
			{
		#if SOH_STATUS_NVM_SUPPORT_ENABLED
				/*SOH Status Failure Detector */
				case ERRH_BSWEVENT_NVM_E_HARDWARE:
				case ERRH_BSWEVENT_NVM_E_QUEUE_OVERFLOW:
				case ERRH_BSWEVENT_NVM_E_INTEGRITY_FAILED:

					DetectorID = SOH_StatNVM;

				break;
		#endif
		#if SOH_ACTION_NVM_SUPPORT_ENABLED
				/*SOH Action Failure Detector */
				case ERRH_BSWEVENT_NVM_E_REQ_FAILED:
				case ERRH_BSWEVENT_NVM_E_WRONG_BLOCK_ID:
				case ERRH_BSWEVENT_NVM_E_VERIFY_FAILED:
				case ERRH_BSWEVENT_NVM_E_LOSS_OF_REDUNDANCY:

					DetectorID = SOH_ActNVM;

				break;
		#endif
		#if SOH_PRIVILEGE_NVM_SUPPORT_ENABLED
				/*SOH Privilege Failure Detector */
				case ERRH_BSWEVENT_NVM_E_WRITE_PROTECTED:

					DetectorID = SOH_PrivNVM;

				break;
		#endif
        #if (SOH_WdgMDEM_SUP_ERRORS_SUPPORT_ENABLED == STD_ON)
				/* SOH WDGM_E_SUPERVISION and WDGM_E_SET_MODE Error Detector */
				case ERRH_BSWEVENT_WDGM_E_SUPERVISION:
				case ERRH_BSWEVENT_WDGM_E_SET_MODE:
					DetectorID = SOH_WdgMDEMErr;
				break;
		#endif

				default:
				 /*Nothing to do */
					DetectorID = SOH_ALL;
				break;
			}

			if (DetectorID!= SOH_ALL)
			{
				/* Pass event into corresponding to filter */
				(void)SOH_Event_Detector(SOH_DetectorItems[DetectorID]);

				/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
				SOH_CheckConditionAndWrite(SOH_DetectorItems[DetectorID]);
			}
		}

		counter++;
		NVMError = NVMError>>1u;
	}
}

/**
 * @}
 * @}
 */ 
#endif

/**
 * \defgroup Functions
 * @{
 * \addtogroup CallBacks
 * @{
 */

/**
 * 
 * @param[in] void
 * @return void
 */ 
/* [$Satisfies $SDD_SOH 044] [$Satisfies $SDD_SOH 068] [$Satisfies $SDD_SOH 069] [$Satisfies $SDD_SOH 073] */
/* PRQA S 1503 3 *//* It's called in the OS ErrorHook*/
#if (SOH_EXCEP_OS_SUPPORT_ENABLED == STD_ON) || (SOH_EXCEP_OS_SUPPORT_ENABLED == STD_ON) || (SOH_SERV_OS_SUPPORT_ENABLED == STD_ON) || (SOH_STACK_OS_SUPPORT_ENABLED == STD_ON) ||(SOH_MEMPROT_OS_SUPPORT_ENABLED == STD_ON) || (SOH_TIMEPROT_OS_SUPPORT_ENABLED == STD_ON) || (SOH_ERR_OS_SUPPORT_ENABLED == STD_ON)
FUNC(void, SUM_SOH_CODE) SOH_OS_Protection_ErrorHook_CallBack(StatusType OS_Error) {

	SOH_DetectorsId DetectorID;

	/*[$Satisfies $SDD_SOH 067]*/
	switch (OS_Error) {

		case E_OK:
			/*Do Nothing, No Error Detected, no Detector set*/
		break;


	#if SOH_EXCEP_OS_SUPPORT_ENABLED
		/*SOH Hardware Protection Detector */
		case E_OS_PROTECTION_EXCEPTION:

			DetectorID = SOH_ExcepOS;

		break;
	#endif

	#if SOH_SERV_OS_SUPPORT_ENABLED
		/*SOH Service Protection / Trusted Function Detector */
		case E_OS_SERVICEID:
		case E_OS_ILLEGAL_ADDRESS:
		case E_OS_MISSINGEND:
		case E_OS_DISABLEDINT:

			DetectorID = SOH_ServOS;

			break;
	#endif

	#if SOH_STACK_OS_SUPPORT_ENABLED
		/*SOH Stack Fault Detector */
		case E_OS_STACKFAULT:

			DetectorID = SOH_StackOS;

			break;
	#endif

	#if SOH_MEMPROT_OS_SUPPORT_ENABLED
		/*SOH Memory Protection Detector */
		case E_OS_PROTECTION_MEMORY:

			DetectorID = SOH_MemProtOS;

			break;
	#endif

	#if SOH_TIMEPROT_OS_SUPPORT_ENABLED
		/*SOH Time protection Detector */
		case E_OS_PROTECTION_TIME:
		case E_OS_PROTECTION_ARRIVAL:
		case E_OS_PROTECTION_LOCKED:

			DetectorID = SOH_TimeProtOS;

			break;
	#endif

	#if SOH_ERR_OS_SUPPORT_ENABLED
		/*SOH Other OS Errors Detector */
		default:

			DetectorID = SOH_ErrOS;

		break;
	#endif
	}

	if (OS_Error != E_OK) {
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[DetectorID]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[DetectorID]);
	}

}
#endif
/**
 * @}
 * @}
 */


/*!
 * 	This function is a callback used only once: in the BswM shutdown action list.
 * 	Its scope is only to call SOH_MonitorShutDownTimer(), that updates SUM_SOH_ShutDown_Timer if SOH is monitoring a shutdown delay.
 *
 * 	The Time Manager Runnable is the only other function that calls SOH_MonitorShutDownTimer(), so the only other one that could ask to
 * 	update SUM_SOH_ShutDown_Timer.
 * 	But, once the shutdown action list starts, all the runnables will not be scheduled anymore.
 *
 * 	\Requirement{028}
 *		So, during this callback, SUM_SOH_ShutDown_Timer will be updated for the last time.
 * 	\Satisfies{006}
 */
/**
 * Function implementation.
 *
 * @param[in]	void
 * @return		void
 */
/* [$Satisfies $DDS_SOH 028] : to be scheduled in the BswM action list */
/* PRQA S 1503 3 *//* It's called as a BSW Action */
FUNC(void, SUM_SOH_CODE) ShutdownNotificationToSoh(void)
{
	uint32 counterValue;

	/* Read counter */
	/* Counter OsCounter must be ticked at 200 ms. In our test project, 1 tick is 1s. */
	if( GetCounterValue(OsCounterSOHName, &counterValue)==E_OK )
	{
#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
		SOH_MonitorShutDownTimer(counterValue);
#endif
	}
}

#if (SOH_CLEAR_DIAG_INFO_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_ClearDiag implementation.
 *It handles the notification sent by the SUM_SSFM through the port SSFM_ClearDiagReq
 *to identify the state of clear diagnostic information requests and update proper detectors.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 062] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_ClearDiag(void)
{
	SUM_SSFM_ClearDiagReqType SSFM_ClearDiagType = SUM_SSFM_ClearDiagReq_Default;
	SOH_DetectorsId DetectorID = SOH_ALL;

	(void)Rte_Read_SSFM_SOH_ClearDiagReq_SSFMClearDiagReq(&SSFM_ClearDiagType);

	switch(SSFM_ClearDiagType){

	case SUM_SSFM_ClearDiagReq_Approved:
		DetectorID = SOH_ClearDiagInfo;
		break;

	case SUM_SSFM_ClearDiagReq_Rejected:
		DetectorID = SOH_RejectClearDiag;
		break;

	default:
		/* invalid or default values */
		break;
	}

	if(SOH_ALL != DetectorID){
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[DetectorID]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[DetectorID]);
	}
}

/**
 * @}
 * @}
 */
#endif

#if (SOH_DIAG_SESSION_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_DiagSession implementation.
 *It handles the notification sent by the SUM_DIAG through the port DIAG_SOH_DiagSessionRequest
 *(service 0x10 subfunction 0x02) and update the Diagnostic Session Control Request Detector.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 063] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_DiagSession(void)
{
	boolean DiagSessionRequested;

	DiagSessionRequested = Rte_DRead_DIAG_SOH_DiagSessionRequest_DiagSessionRequest();
	if(TRUE == DiagSessionRequested){
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[SOH_DiagSession]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_DiagSession]);
	}

}

/**
 * @}
 * @}
 */

/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_AcceptDiagSession implementation.
 *It handles the notification sent by the BSWM through the port DiagnosticReprogrammingRequest
 *When BSWM requests diagnostic session for reprogramming, this runnable will be triggered and update Accepted Diagnostic Session Control Request Detector.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 064] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_AcceptDiagSession(void)
{
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[SOH_AcceptDiagSession]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_AcceptDiagSession]);
}

/**
 * @}
 * @}
 */
#endif

#if (SOH_SEC_ACCESS_FAILURES_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_SecurityAccess implementation.
 *It handles the notification sent by the SUM_SSC through the port SSC_SOH_SecurityAccessError
 *about a SecurityAccess CompareKey (servcie 0x27) Failure and update SecurityAccess Failures Detector.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 065] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_SecurityAccess(void)
{
	boolean SecurityAccessError;

	SecurityAccessError = Rte_DRead_SSC_SOH_SecurityAccessError_SecurityAccessError();

	if(TRUE == SecurityAccessError){
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[SOH_SecurityAccess]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_SecurityAccess]);
	}
}

/**
 * @}
 * @}
 */
#endif

#if (SOH_MAC_FAILURES_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup TimePeriodic
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_MacFailType3Delay implementation.
 *Respect to the SOH initialization, MAC Failures Type 3 logs are delayed a number of seconds.
 *This runnable runs periodically to check the elapsing time and - if the delay expired, it will notify
 *the corresponding runnable entity to start MAC Failures Type 3 logging.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 071] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_MacFailType3Delay(void)
{
	static boolean SUM_SOH_ReplayCount_Start_Status = FALSE;

	if(TRUE != SUM_SOH_ReplayCount_Start_Status){
		/* decrease time counter */
		MACFailuresType3_DelayTime--;

		/* MAC Failures Type 3 logs are delayed */
		(void)Rte_IrvWrite_Runnable_SOH_CommunicationReliabilityMonitor_MacFailType3Delay_SOH_MACType3Execution((FALSE));

		/* check if time expired */
		if(MACFailuresType3_DelayTime == SOH_UNSIGNED_ZERO){
			SUM_SOH_ReplayCount_Start_Status = TRUE;

			/* MAC Failures Type 3 logs are started */
			(void)Rte_IrvWrite_Runnable_SOH_CommunicationReliabilityMonitor_MacFailType3Delay_SOH_MACType3Execution((TRUE));
		}
	}

}
/**
 * @}
 * @}
 */

/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_MacFailType123 implementation.
 *It handles the notification sent by the SUM_SSC through the port SSC_E_SecureMessageMonitoringEvents
 *about MAC Failures and update MAC Failures Types 1 and 2 Detector or MAC Failures Type 3 Detector.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 072] */
/* PRQA S 1503 3 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_MacFailType123(void)
{
	SOHMonitor_E_ErrorEvent_Type ErrorEvent_Type = SSC_ERR_NO_Event;
	SOH_DetectorsId DetectorID;
	boolean MACType3Execution;

	(void)Rte_Read_SSC_E_SecureMessageMonitoringEvents_SOHMonitor_E_ErrorEvent(&ErrorEvent_Type);
	MACType3Execution = Rte_IrvRead_Runnable_SOH_CommunicationReliabilityMonitor_MacFailType123_SOH_MACType3Execution(); /* PRQA S 3469 */ /* The owner of Rte_CData_SUM_SOH_ReplayCount_Start_Thresh is the RTE, not this module */

	switch(ErrorEvent_Type){

	case SSC_ERR_MAC_Active:
		DetectorID = SOH_MACfailTenAct;
		break;

	case SSC_ERR_Tentative_Active:
		DetectorID = SOH_MACfailTenAct;
		break;

	case SSC_ERR_Low_Tentative:

		if(TRUE == MACType3Execution){
			DetectorID = SOH_ReplayCount;
		}
		else{
			DetectorID = SOH_ALL;
		}
		break;

	default:
		/* invalid or default values */
		DetectorID = SOH_ALL;
		break;

	}

	if(SOH_ALL != DetectorID){
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[DetectorID]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[DetectorID]);
	}
}
/**
 * @}
 * @}
 */
#endif

#if (SOH_SECP_ERRORS_SUPPORT_ENABLED == STD_ON)
/**
 * \defgroup Runnables
 * @{
 * \addtogroup DataReceiveEvent
 * @{
 */
/*!
 *Runnable_SOH_CommunicationReliabilityMonitor_SecP implementation.
 *It handles the notification sent by the BSWM through the port RP_ErrorStatus
 *about about a SecP Errors and update SecP Errors Detector.
 *
 *@param[in]  void
 *@return void
 */
/*[$Satisfies $SDD_SOH 054] [$Satisfies $SDD_SOH 052]*/
/* PRQA S 1503 1 *//* PRQA S 3408 2 *//* It's scheduled in the OS*/
FUNC(void, SUM_SOH_CODE) Runnable_SOH_CommunicationReliabilityMonitor_SecP(void) /* PRQA S 0794 7 *//*The ID name of this runnable is unique*/
{
	uint16 SecP_OldMode, SecP_NextMode;

	/* The return value, the current Transition Mode, could be ignored, we are interested in the next mode value*/
	(void)Rte_Mode_RP_ErrorStatus_SUM_ERC(&SecP_OldMode, &SecP_NextMode);

	if(RTE_MODE_SECP_Error_Status_SUM_ERC_NO_ERROR != SecP_NextMode){
		/* Pass event into corresponding to filter */
		(void) SOH_Event_Detector(SOH_DetectorItems[SOH_SecP]);

		/* Check condition then Write DID counter and detection counter changed into NvM if satisfied*/
		SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_SecP]);
	}
}

/**
 * @}
 * @}
 */
#endif

#define SUM_SOH_STOP_SEC_CODE
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/


/* END OF CODE */

/********************************** END OF FILE ******************************************************/

/*! @} */

