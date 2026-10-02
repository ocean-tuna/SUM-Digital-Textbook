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
/* PRQA S 0380 1 # header included from RTE */
#include "Rte_SUSD.h"
#include "SUM_SUSD.h"
#include "SUM_SUSD_Ram.h"
#include "SUM_SUSD_Cfg.h"

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
  ******************************************************************************
  * Internal Functions
  ******************************************************************************
  */
 static FUNC(void, SUM_SUSD_CODE) Susd_NvmWrite_ECUWakeUpDetectionRecords(void);
 /* Helper function to determine ECU state transition based on wakeup and shutdown conditions */
 static FUNC(void, SUSD_CODE) SUSD_DetermineEcuStateTransition(boolean Lblshutdownflag, uint8 wakeupstatus, 
                                                               CurrentECUOperatingModeType *SUSD_DesiredEcuState);
 /* Helper function to evaluate shutdown permission status from bit arrays */
 static FUNC(boolean, SUSD_CODE) SUSD_EvaluateShutdownPermission(void);
/*
 ***************************************************************************************************
 * Functions
 ***************************************************************************************************
 */

/*
*  Function name : SUSD_ApplnStateManager
*  Description   : Function to handle the Application State Manager functionality to switch ECU states
*  Parameter in  : None
*  Parameter out : None
*  Return value  : None
*************************************************************************************
*/
/*Satisfies: [$SDD_SUSD 145] [$SDD_SUSD 144]*/
/* PRQA S 1532 1 # Here for modularity */
#define SUSD_START_SEC_CODE
#include "SUSD_MemMap.h" /* PRQA S 5087 EOF */
/* include MemMap.h required here to occupy the space in memory for the described macro*/
 /* PRQA S 1532 1 # When the SUSD is integrated, this function will be called by RTE */
FUNC(void, SUSD_CODE) SUSD_ApplnStateManager(void)
		{
	/* Flag to be set when all SW-Cs are ready for shutdown */
	static boolean Lblshutdownflag;
    static boolean ReqSPMToShoutdown = FALSE;
    boolean LblshutdownflagInternal;
	uint8 wakeupstatus;

	/* Variable to store the desired next ECU operating mode */
	CurrentECUOperatingModeType SUSD_DesiredEcuState = SUM_INIT;
    
	/* Invoke WdgM API to indicate aliveness of SUSD */
	(void)Rte_Call_RP_SUSD_WdgM_AliveSupervision_CheckpointReached();
    
	/* Evaluate shutdown permission status from bit arrays
     this returns true if all SWC and SUM without SSC vote 
     for Shutdown or some timer expired */
	LblshutdownflagInternal = SUSD_EvaluateShutdownPermission();

    if(LblshutdownflagInternal == TRUE)
    {
       /*set the bit 1 of the permit execution of the SPM to zero Shutdown Target bit to OFF */
       SUSD_GaaSUMPermitExecution[SUM_SPM] = (uint8)(SUSD_GaaSUMPermitExecution[SUM_SPM] & (uint8)0xFD);
       /*update the Permit Execution Interface to inform SPM*/
       SUSD_SUM_PermitExecution();
       ReqSPMToShoutdown = TRUE;
    }
    else
    {
        /*Assign False to the Flag*/
        Lblshutdownflag = FALSE;
    }
    
    /*At this point if SPM answare 254 on the shoudownPermission interface give the enable shoudown
      And the request to shoutdown for the SPM is set*/
    if ((SUSD_GaaSUMShutdownPermission[SUM_SPM] == (uint8)0xFE) && (ReqSPMToShoutdown == TRUE))
    {
        Lblshutdownflag = TRUE;
    }
      
    /*Check if the global force shutdown timer has expired*/
    if (SUSD_GaaWaitGlobalForceShutdownTimer == SUSD_ZERO)
    {
        /* Global force shutdown timer has expired */
        wakeupstatus = FALSE;
        Lblshutdownflag = TRUE;
    }
    else
    {
        wakeupstatus=SUSD_No_Active_Wakeup();
    }
    
	SUSD_GddCurrentEcuState = Rte_Mode_RP_SUSD_CurrentECUOperatingModePort_CurrentMode();

	/* Determine ECU state transition based on wakeup and shutdown conditions */
	SUSD_DetermineEcuStateTransition(Lblshutdownflag, wakeupstatus, &SUSD_DesiredEcuState);

	if(SUSD_DesiredEcuState != SUM_INIT)
	{
		if(SUSD_DesiredEcuState != SUSD_GddCurrentEcuState)
		{
			SUSD_GddASM_CurrentMode = SUSD_DesiredEcuState;
			/* Indicate the ECU state change to BswM */
			(void)Rte_Switch_PP_SUSD_ECUOperatingModeReq_ECUOperatingMode(SUSD_GddASM_CurrentMode);
		}
	}

	Susd_NvmWrite_ECUWakeUpDetectionRecords();
}

/******************************************************************************
 * Function name : Susd_NvmWrite_ECUWakeUpDetectionRecords
 * Description   : This function updates the RAM block value and requests the
 *                 NvM_WriteBlock for every ECUWakeUpDetectionRecord if no other
 *                 write operation is ongoing on the ECUWakeUpDetectionRecords NVM block.
 * Parameter in  : none
 * Parameter out : none
 * Return value  : none
 *****************************************************************************/
static FUNC(void, SUM_SUSD_CODE) Susd_NvmWrite_ECUWakeUpDetectionRecords(void)
{
	/* Get the last request result if still pending. */
    if (E_PENDING == Susd_LastRequestResult_ECUWakeUpDetectionRecords)
    {
        Susd_NvmGetErrorStatus(&susd_NVMECUWakeUpDetectionRecordsBlock_FP,
                               &Susd_LastRequestResult_ECUWakeUpDetectionRecords);
    }
}


/******************************************************************************
 * Function name : Susd_NvmGetErrorStatus
 * Description   : This function checks the status of the last write request of
 *                 the given NVM block.
 * Parameter in  : blockNvmConfig - struct with all configuration items of the
 *                 NVM block.
 * Parameter out : lastRequestResult - result of the NVM write request:
 *                 E_PENDING if it's still pending, E_NOT_OK if failed,
 *                 E_OK if completed successfully.
 * Return value  : none
 *****************************************************************************/
/* Satisfies: [$SDD_SUSD 161]*/
FUNC(void, SUM_SUSD_CODE) Susd_NvmGetErrorStatus(const Susd_GenericNvmAccessType * blockNvmConfig,
                                                        Std_ReturnType * lastRequestResult)
{
    NvM_RequestResultType nvmRequestResult = 0x00;

    if (E_PENDING == *lastRequestResult)
    {
        /* Check last request result. */
        if (RTE_E_OK != blockNvmConfig->Susd_RteCallNvmGetErrorStatus_fp(&nvmRequestResult))
        {
            /* An error occurred. */
            *lastRequestResult = E_NOT_OK;
        }
        else
        {
            if (NVM_REQ_OK == nvmRequestResult)
            {
                /* Last request finished successfully. */
                *lastRequestResult = E_OK;
            }
            else if (NVM_REQ_PENDING == nvmRequestResult)
            {
                /* A request is currently pending. Retry on next cycle. */
            }
            else
            {
                /* Last request finished unsuccessfully or other error occurred. */
                *lastRequestResult = E_NOT_OK;
            }
        }
    }
    else
    {
        /* Nothing to do for this block */
    }
}



/******************************************************************************
 * Function name : Susd_NvmWriteBlock
 * Description   : This function handles the request for storing into NVM the
 *                 RAM block value of the given NvM block.
 * Parameter in  : blockNvmConfig - struct with all configuration items of the
 *                 NVM block.
 * Parameter out : lastRequestResult - result of the NVM write request:
 *                 E_PENDING if the NvM_WriteBlock request is accepted,
 *                 E_OK/E_NOT_OK otherwise
 * Return value  : none
 *****************************************************************************/
/* Satisfies: [$SDD_SUSD 161]*/
FUNC(void, SUM_SUSD_CODE) Susd_NvmWriteBlock(const Susd_GenericNvmAccessType * blockNvmConfig,
                                             Std_ReturnType * lastRequestResult)
{
    Std_ReturnType ret;

    /* Check if no writing is pending before requesting new one */
    if (E_PENDING != *lastRequestResult)
    {
        /* Request to store RAM block value into NVM. */
        ret = blockNvmConfig->Susd_RteCallNvmWriteBlock_fp(blockNvmConfig->NvmRamBlock);
        if (RTE_E_OK == ret)
        {
            /* Successfully accepted for write. Result pending. */
            *lastRequestResult = E_PENDING;
        }
    }
}

/******************************************************************************
 * Function name : Susd_NvmGetErrorStatus_uint16
 * Description   : This function checks the status of the last write request of
 *                 the given NVM block.
 * Parameter in  : blockNvmConfig - struct with all configuration items of the
 *                 NVM block.
 * Parameter out : lastRequestResult - result of the NVM write request:
 *                 E_PENDING if it's still pending, E_NOT_OK if failed,
 *                 E_OK if completed successfully.
 * Return value  : none
 *****************************************************************************/
/* PRQA S 1532 1 # This Function can be used in the multiple files and cannot be turned into static as well as moved to static code  */
FUNC(void, SUM_SUSD_CODE) Susd_NvmGetErrorStatus_uint16(const Susd_GenericNvmAccessType_uint16 * blockNvmConfig,
                                                        Std_ReturnType * lastRequestResult)
{
    NvM_RequestResultType nvmRequestResult = 0x00;

    if (E_PENDING == *lastRequestResult)
    {
        /* Check last request result. */
        if (RTE_E_OK != blockNvmConfig->Susd_RteCallNvmGetErrorStatus_fp(&nvmRequestResult))
        {
            /* An error occurred. */
            *lastRequestResult = E_NOT_OK;
        }
        else
        {
            if (NVM_REQ_OK == nvmRequestResult)
            {
                /* Last request finished successfully. */
                *lastRequestResult = E_OK;
            }
            else if (NVM_REQ_PENDING == nvmRequestResult)
            {
                /* A request is currently pending. Retry on next cycle. */
            }
            else
            {
                /* Last request finished unsuccessfully or other error occurred. */
                *lastRequestResult = E_NOT_OK;
            }
        }
    }
    else
    {
        /* Nothing to do for this block */
    }
}



/******************************************************************************
 * Function name : Susd_NvmWriteBlock_uint16
 * Description   : This function handles the request for storing into NVM the
 *                 RAM block value of the given NvM block.
 * Parameter in  : blockNvmConfig - struct with all configuration items of the
 *                 NVM block.
 * Parameter out : lastRequestResult - result of the NVM write request:
 *                 E_PENDING if the NvM_WriteBlock request is accepted,
 *                 E_OK/E_NOT_OK otherwise
 * Return value  : none
 *****************************************************************************/
/* PRQA S 1532 1 # This Function can be used in the multiple files and cannot be turned into static as well as moved to static code  */
FUNC(void, SUM_SUSD_CODE) Susd_NvmWriteBlock_uint16(const Susd_GenericNvmAccessType_uint16 * blockNvmConfig,
                                             Std_ReturnType * lastRequestResult)
{
    Std_ReturnType ret;

    /* Check if no writing is pending before requesting new one */
    if (E_PENDING != *lastRequestResult)
    {
        /* Request to store RAM block value into NVM. */
        ret = blockNvmConfig->Susd_RteCallNvmWriteBlock_fp(blockNvmConfig->NvmRamBlock);
        if (RTE_E_OK == ret)
        {
            /* Successfully accepted for write. Result pending. */
            *lastRequestResult = E_PENDING;
        }
    }
}

/******************************************************************************
 * Function name : SUSD_EvaluateShutdownPermission
 * Description   : Function to evaluate if all configured shutdown permissions
 *                 are enabled based on bit array status.
 *                 Bit = 1: Shutdown enabled OR interface not configured OR unused bit
 *                 Bit = 0: Shutdown disabled
 * 
 * Parameter in  : none
 * Parameter out : none
 * Return value  : TRUE if all shutdown permissions enabled, FALSE otherwise
 *****************************************************************************/
static FUNC(boolean, SUSD_CODE) SUSD_EvaluateShutdownPermission(void)
{
    boolean result = SUSD_TRUE;
    uint32 i;
    uint16 sum_mask;
    uint8 array_index;
    uint8 bit_offset;
    /* Check SUM shutdown enable bits */
    /* Create mask for configured SUM interfaces: bits set to 1 for configured SUMs */
    /* PRQA S 2741 1 # Constant expression is needed for generic scalable code */
#if SUSD_SUM_SHUTPERM < 16u
        /* PRQA S 0499 1 # Shift is protected by if condition above */
        sum_mask = (uint16)((1u << SUSD_SUM_SHUTPERM) - 1u);
#else
        /* PRQA S 2880 1 # Else branch needed for scalability when interfaces >= 16 */
        sum_mask = 0xFFFFu;
#endif
    /* Remove SUM_SSC bit from mask */
    sum_mask = (uint16)(sum_mask & (uint16)(~((uint16)1u << SUM_SSC)));

    /* Check if all configured SUM bits are set (shutdown enabled) */
    if ((GblSUMshutdownenablecnt & sum_mask) != sum_mask)
    {
        result = SUSD_FALSE;
    }

#if SUSD_SWC_SHUTPERM > 0
    /* Check SWC shutdown enable bits */
    if (result == SUSD_TRUE)
    {
        /* PRQA S 2877 25 # Loop designed for scalability with number of configured SWCs */
        for (i = 0u; i < SUSD_SWC_SHUTPERM; i++)
        {
            array_index = (uint8)(i / 64u);
            bit_offset = (uint8)(i % 64u);
 
            /* Check if configured SWC bit is set (shutdown enabled) */
            if ((Gblshutdownenablecnt[array_index] & ((uint64)1ULL << bit_offset)) == 0ULL)
            {
                result = SUSD_FALSE;
                break;
            }
        }
    }
#endif
   
    return result;
}


/******************************************************************************
 * Function name : SUSD_DetermineEcuStateTransition
 * Description   : Function to determine ECU state transition based on
 *                 wakeup and shutdown conditions.
 * 
 * Parameter in  : Lblshutdownflag - flag indicating if shutdown is enabled
 *                 wakeupstatus - current wakeup status
 * Parameter out : SUSD_DesiredEcuState - desired ECU state to transition to
 * Return value  : none
 *****************************************************************************/
static FUNC(void, SUSD_CODE) SUSD_DetermineEcuStateTransition(boolean Lblshutdownflag, uint8 wakeupstatus, 
                                                               CurrentECUOperatingModeType *SUSD_DesiredEcuState)
{
    /* If all BSW modules are initialized, then check other conditions then
	 * requesting BswM to switch ECU state to RUM mode and send Permit execution for other SUMs and SWCs - request RUN state */
    if(((SUSD_ModuleWakeupBits > SUSD_ZERO)))
    {
        /* PRQA S 3469 1 # The function like macros is used. This is necessary to get value via RTE variable */
        if((Rte_IrvRead_Runnable_AppStateManager_Go_to_reset())==FALSE)
        {
            /* Only request RUN if we're not already in RUN state */
            if(SUM_RUN != SUSD_GddCurrentEcuState)
            {
                *SUSD_DesiredEcuState = SUM_RUN;
            }
        }
    }

    /* Check conditions for Sleep/Shutdown */
    /* Diagnostic Reprogramming request */
    if((SUM_REPREQ_ACTIVE == SUSD_GblDiagReprgRequest) &&
            (SUSD_TRUE == Lblshutdownflag))
    {
        /* PRQA S 3469 1 # The function like macros is used. This is necessary to get value via RTE variable */
        if((Rte_IrvRead_Runnable_AppStateManager_Go_to_reset())==FALSE)
        {
            *SUSD_DesiredEcuState = SUM_BOOT;
        }
    }
    /* Diagnostic Reset Request or SOH request */
    /* PRQA S 3469 3 # The function like macros is used. This is necessary to get value via RTE variable */
    else if((SUM_RESREQ_ACTIVE == SUSD_GblDiagResetRequest)&&
            (Rte_IrvRead_Runnable_AppStateManager_NvMWrite_SUM_SUSD_RESETExecution_TIMER_started()==FALSE)&&
            (SUSD_TRUE == Lblshutdownflag))
    {
        /* PRQA S 3469 1 # The function like macros is used. This is necessary to get value via RTE variable */
        if((Rte_IrvRead_Runnable_AppStateManager_Go_to_reset())==FALSE)
        {
            /* Satisfies: [$SDD_SUSD 169] */
            /*Flag to start writing SUM_SUSD_RESETExecution_TIMER into NvM*/
            Rte_IrvWrite_Runnable_AppStateManager_NvMWrite_SUM_SUSD_RESETExecution_TIMER_started((TRUE));
        }
    }
    /* Check for normal shutdown conditions */
    /* Guard against DiagReset active: timer runnable owns SUM_RESET from here, requesting SUM_SLEEP would toggle the mode */
    /* Satisfies: [$SDD_SUSD 169] */
    else if(((wakeupstatus==ALL_WAKEUPS_DISABLED)||(wakeupstatus==INITIAL_WAKEUP_STATUS))
            &&(SUSD_TRUE == Lblshutdownflag)
            &&(SUM_RESREQ_ACTIVE != SUSD_GblDiagResetRequest))
    {
        *SUSD_DesiredEcuState = SUSD_C_DEFAULTSHUTDOWNSTATE;
    }
    else
    {
        /* Do Nothing */
    }
}

#define SUSD_STOP_SEC_CODE
#include "SUSD_MemMap.h"
/********************************** END OF FILE ******************************************************/
