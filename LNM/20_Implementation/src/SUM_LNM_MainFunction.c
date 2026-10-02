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
 * Component   : SUM_LNM
 * Date        : Jun 26, 2026
 * Revision    : 8.0.1_rc
 * Description : LNM Main Function file
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
 */

/******************************************************************************
 * Header Includes *
 ******************************************************************************/
#include "Rte_SUM_LNM.h"
#include "SUM_LNM_Network_Management.h"
#include "SUM_LNM_MainFunction.h"
#include "SUM_LNM_Types.h"
#include "SUM_LNM_Cfg.h"
#include "SUM_LNM_PBCfg.h"
#include "SUM_Common_Types.h"


/*****************************************************************************************
 * Definition of Global Variables *
 *****************************************************************************************/
/* Variable used to store the current status of SUM_LNM */
VAR(uint8,AUTOMATIC)LNM_GlobalStatus = LNM_UNINIT;

/*****************************************************************************************
 * Definition of Global Functions *
 *****************************************************************************************/
#if LNM_RUNNABLE_INIT_ENABLED == STD_ON
static FUNC(void, SUM_LNM_CODE) LNM_Init(void);
#endif
static FUNC(boolean, SUM_LNM_CODE) LNM_CheckTimer_VehiclePowerMode(void);
/***********************************************************
* Function name: LNM_Init
* Description: Function that manages that initializes SUM_LNM
* Parameters in: None
* Return value: None
* Remarks for:
* Satisfies: [$SDD_LNM 001] [$SDD_LNM 093] [$SDD_LNM 515]
***********************************************************/

#if LNM_RUNNABLE_INIT_ENABLED == STD_ON
static FUNC(void, SUM_LNM_CODE) LNM_Init(void)
#else
/*PRQA S 1503 1 # This function is called through a BSW action*/
FUNC(void, SUM_LNM_CODE) LNM_Init(void)
#endif
{
    VAR(uint8,AUTOMATIC) i, j;
    VAR(boolean,AUTOMATIC) rv;
    VAR(uint16,AUTOMATIC) MSCL_WritePointer[LNM_NUMBER_OF_NETWORKS];

    /* Set initial status of the networks */
    for(i = LNM_ZERO; i < LNM_NUMBER_OF_NETWORKS; i++)
    {
        LNM_Network_Status[i].desired_schedule = LNM_NULL_SCHEDULE;
        LNM_Network_Status[i].schedule_request_success = TRUE;
        LNM_Network_Status[i].schedule_enabled = FALSE;
        LNM_Network_Status[i].previous_network_status = LNM_NO_COMMUNICATION;
        LNM_Network_Status[i].current_network_status = LNM_NO_COMMUNICATION;
        LNM_Network_Status[i].current_schedule = LNM_NULL_SCHEDULE;
        LNM_Network_Status[i].current_schedule_state = LNM_START_STATUS;
        LNM_Network_Status[i].commander_timer = LNM_ZERO;
        LNM_Network_Status[i].responder_timer = LNM_ZERO;
		LNM_Network_Status[i].presleep_timer = LNM_ZERO;
        LNM_Network_Status[i].commander_timer_started = FALSE;
        LNM_Network_Status[i].responder_timer_started = FALSE;		LNM_Network_Status[i].presleep_timer_started = FALSE;
        LNM_Network_Status[i].LoC_WithDevice_Status = LNM_ZERO;
        LNM_Network_Status[i].FULL_COMM_requested = FALSE;
        LNM_Network_Status[i].FULL_COMM_retries = LNM_ZERO;

        for(j = 0; j < LNM_Network_Info[i].LNM_ResponderNumber; j++)
        {
            LNM_Network_Info[i].LNM_Responder_Status[j].ief_reset_counter = LNM_ZERO;
            LNM_Network_Info[i].LNM_Responder_Status[j].not_configured_counter = LNM_ZERO;
            LNM_Network_Info[i].LNM_Responder_Status[j].no_comm_counter = LNM_ZERO;
        }
    }

    /* MSCL */
    /* Satisfies: [$SDD_LNM 040] [$SDD_LNM 042] [$SDD_LNM 043] [$SDD_LNM 045]  */
    for(i = LNM_ZERO; i < LNM_NUMBER_OF_NETWORKS; i++)
    {
        /* MSCL initialization to 0x0000u */
        /* Satisfies: [$SDD_LNM 044] */
        LNM_Network_Status[i].MSCL = LNM_MSCL_OFF;

        /* If channel is disabled, MSCL will remain 0x0000u */
        rv = LNM_Get_MSCL_ChannelEnable(i);
        if(rv == TRUE)
        {
            /* ResponderPresent evaluation/calibration for each responder */
            for(j = 0; j < LNM_Network_Info[i].LNM_ResponderNumber; j++)
            {
                rv = LNM_Get_Cal_ResponderPresent(i,j);
                if(rv == TRUE)
                {
                    /* Set to 1 the responder bit */
                    LNM_Network_Status[i].MSCL |= (uint16)((uint16)LNM_RESPONDER_PRESENT << LNM_Network_Info[i].LNM_Responders[j].LNM_MSCL_Index);
                }
            }

            /* Evaluation / Calibration procedure completed. Set the CONFIG bit to 1*/
            LNM_Network_Status[i].MSCL |= (uint16)((uint16)LNM_RESPONDER_PRESENT << LNM_CONFIGSTATUS_BIT);
        }

        /* Notification of the MSCL value, and update the pointer needed for NvM storage */
        (void) LNM_Network_Info[i].LNM_MSCL_SSM_Notification(LNM_Network_Status[i].MSCL);
        MSCL_WritePointer[i] = LNM_Network_Status[i].MSCL;
    }

    /* Store MSCL in NvM */
    /* Satisfies: [$SDD_LNM 041] */
    (void)Rte_Call_RP_NvM_MSCL_WriteBlock(MSCL_WritePointer);

    /* Satisfies: [$SDD_LNM 002] */
#if CYBERFILL_SUPPORT == LNM_ON
    /* Publish the CyberFill value */
    (void)Rte_Write_PP_LNM_Cyberfill_DataElements_VariableDataPrototype(CYBERFILL_VALUE);
#endif

    /* Set internal status to INIT */
    LNM_GlobalStatus = LNM_INIT;

    return;
}


/***********************************************************
* Function name: Runnable_LNM_Init
* Description: Runnable that manages SUM_LNM initialization
* Parameters in: None
* Return value: None
* Remarks for:
* Satisfies: [$SDD_LNM 001] [$SDD_LNM 093]
***********************************************************/
/*PRQA S 1503 3 # Runnable implementation to be called by another module*/
#if LNM_RUNNABLE_INIT_ENABLED == STD_ON
FUNC(void, SUM_LNM_CODE) Runnable_LNM_Init(void)
{
    static uint32 LNM_Inited = 0u;

    if(LNM_Inited == 0u)
    {
        LNM_Init();
    	LNM_Inited = SUM_INITIALIZED;
    }
}
#endif


/******************************************************************************
* Function name: LNM_MainFunction
* Description: Function that manages the execution of SUM_LNM functionalities
* Parameters in: None
* Return value: None
* Remarks for:
* Satisfies: [$SDD_LNM 010] [$SDD_LNM 011]
 ******************************************************************************/
/*PRQA S 1503 1 # Runnable implementation to be called by another module*/
FUNC(void, SUM_LNM_CODE) LNM_MainFunction(void)
{
    VAR(uint8,AUTOMATIC) permit_execution;
    VAR(uint8,AUTOMATIC) i;
    VAR(boolean,AUTOMATIC) enable_shutdown = TRUE;
    VAR(boolean,AUTOMATIC) veh_power_mode;
    static VAR(uint8,AUTOMATIC) rolling_counter = LNM_MIN_ROLLING_COUNTER;
    VAR(Std_ReturnType,AUTOMATIC) ret;

    /* Read the PermitExecution from SUSD */
    ret = Rte_Read_RP_SUSD_LNM_PermitExecution_SUSD_SWC_ExecutionStatus(&permit_execution);
    if(ret == LNM_E_OK)
    {
        /* Reset bit set. Set SUM_LNM to Shutdown status */
        if((permit_execution & LNM_RESET_BIT) != LNM_RESET_BIT)
        {
            LNM_GlobalStatus = LNM_SHUTDOWN;
        }
        /* If permit execution is matching the configured mask, allow SUM_LNM to run its functionalities */
        else if((permit_execution | PERMIT_EXECUTION_MASK) == LNM_EXECUTION_ALLOWED)
        {
            LNM_GlobalStatus = LNM_RUNNING;
        }
        else
        {
            /* Neither PermitExecution, neither Shutdown is requested. Do nothing */
        }
    }

    /* Satisfies: [$SDD_LNM 012] */
    /*Advising the WdgM that the checkpoint is reached */
    (void) Rte_Call_RP_WdgM_LNM_LocalSupervision_CheckpointReached();

    /* If LNM is in shutdown status, set Shutdown Permission to enable*/
    if(LNM_GlobalStatus == LNM_SHUTDOWN)
    {
        (void)Rte_Write_PP_LNM_SUSD_ShutdownPermission_SUSD_UserRequest(LNM_SHUTDOWN_ENABLE);
    }
    else
    {
        /* Check that all the networks are in NO COMMUNICATION */
        /* If all networks are in NO COMMUNICATION allow for shutdown */
        for(i = LNM_ZERO; i < LNM_NUMBER_OF_NETWORKS; i++)
        {
            if((LNM_Network_Status[i].current_network_status == LNM_FULL_COMMUNICATION) || (LNM_Network_Status[i].FULL_COMM_requested == TRUE))
            {
                enable_shutdown = FALSE;
                break;
            }
        }

        if(enable_shutdown == FALSE)
        {
            /* Satisfies: [$SDD_LNM 013] */
            /* Active Rolling Counter mechanism */
            (void)Rte_Write_PP_LNM_SUSD_ShutdownPermission_SUSD_UserRequest(rolling_counter);
            rolling_counter++;

            if(rolling_counter > LNM_MAX_ROLLING_COUNTER)
            {
                rolling_counter = LNM_MIN_ROLLING_COUNTER;
            }
        }
        else
        {
            (void)Rte_Write_PP_LNM_SUSD_ShutdownPermission_SUSD_UserRequest(LNM_SHUTDOWN_ENABLE);
        }
    }

    /* Satisfies: [$SDD_LNM 014] */
    /* Update inter-runnable variable for Vehicle power mode, used in Diagnostic of Loss of Communication with Bus 
       for Bus off test enable criteria. */
    veh_power_mode = LNM_CheckTimer_VehiclePowerMode();
    (void)Rte_IrvWrite_Runnable_MainFunction_LNM_VehiclePowerModeRunOrPropulsion((veh_power_mode));
    
    return;
}

/******************************************************************************
* Function name: LNM_CheckTimer_VehiclePowerMode
* Description: Function that checks the vehicle power mode condition for LIN bus off 
               test enable criteria.
* Parameters in:
* Return value: boolean: TRUE  -> Vehicle in run or propulsion for at least 5 seconds.
*                        FALSE -> ow
* Remarks for:
* Satisfies: [$SDD_LNM 014]
 ******************************************************************************/
static FUNC (boolean, SUM_LNM_CODE)LNM_CheckTimer_VehiclePowerMode(void)
{
    SUM_VehiclePowerMode_Signal_States PowerMode;
    static uint32 LNM_VehiclePowerModeTimer = LNM_VEH_POWER_MODE_MAX_RESET_TIMER * LNM_MS_TO_US_UNIT;
    boolean ret = FALSE;
    #if VPM_USED_IN_ACTIVATION_RULE == STD_ON
    static SUM_VehiclePowerMode_Signal_States PowerModePrev;
    static boolean PowerModeRead = FALSE;
    #endif

    if (E_OK == Rte_Read_RP_SUSD_LNM_VehiclePowerMode_VehiclePowerMode(&(PowerMode)))
    {
        #if (VPM_USED_IN_ACTIVATION_RULE == STD_ON)
        /* Satisfies: [$SDD_LNM 015] */
        // If power mode is changed since last reading, trigger the activation rules that uses it.
        if ( (PowerModePrev != PowerMode) || (PowerModeRead == FALSE))
        {
            LNM_Activation_Rule_VPM();
            PowerModePrev = PowerMode;
            PowerModeRead = TRUE;
        }
        #endif 

        // Update the timer and evaluate return value. 
        if ((PowerMode == RUN) || (PowerMode == PROPULSION))
        {
            if (LNM_VehiclePowerModeTimer >= LNM_MAIN_FUNCTION_PERIODICITY_US)
            {
                LNM_VehiclePowerModeTimer -= LNM_MAIN_FUNCTION_PERIODICITY_US;
            }
            else
            {
                ret = TRUE;
            }
        }
        else
        {
            LNM_VehiclePowerModeTimer = LNM_VEH_POWER_MODE_MAX_RESET_TIMER * LNM_MS_TO_US_UNIT;
        }
    }
    else
    {
        LNM_VehiclePowerModeTimer = LNM_VEH_POWER_MODE_MAX_RESET_TIMER * LNM_MS_TO_US_UNIT;
    }

    return ret;
}



/********************************** END OF FILE ******************************************************/
