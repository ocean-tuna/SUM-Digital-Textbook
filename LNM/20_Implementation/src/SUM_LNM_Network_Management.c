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
 * Description : LNM NetworkManagement file
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
#include "SUM_LNM_PBCfg.h"

/*****************************************************************************************
 * Definition of Global Data *
 *****************************************************************************************/
/* Data structure containing current states of the networks */
LNM_Network_Status_Type LNM_Network_Status[LNM_NUMBER_OF_NETWORKS];

/*****************************************************************************************
 * Implementation of Runnables *
 *****************************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_InternalElectronicFailure(void);
static FUNC(void, SUM_LNM_CODE) LNM_LossOfCommunication_Bus(void);
static FUNC(void, SUM_LNM_CODE) LNM_LossOfCommunication_Device(void);
static FUNC(void, SUM_LNM_CODE) LNM_Activation_Timer_Management(VAR(uint8,AUTOMATIC) network_index);
/*****************************************************************************************
 * Implementation of Private functions *
 *****************************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_ActivationManager(VAR(uint8,AUTOMATIC) network_index);
static FUNC(boolean, SUM_LNM_CODE) LNM_Get_CalibPar_LNM_C_DTC_LostCommWithLinResp(void);

/******************************************************************************
* Function name: LNM_ScheduleManagement
* Description: Function that periodically manages the networks schedules
* Parameters in: None
* Return value: None
* Remarks for:
* Satisfies: [$SDD_LNM 030]
 ******************************************************************************/
/*PRQA S 1503 1 # Runnable implementation to be called by another module*/
FUNC(void, SUM_LNM_CODE) LNM_ScheduleManagement(void)
{
    VAR(uint8,AUTOMATIC) i;

    if(LNM_GlobalStatus == LNM_RUNNING)
    {
        for(i = 0; i < LNM_NUMBER_OF_NETWORKS; i++)
        {
            /* Manages the scheduling of the function if schedule is enabled */
            if(LNM_Network_Status[i].schedule_enabled == TRUE)
            {
                LNM_Network_Info[i].LNM_ScheduleManager();
            }
        }
    }
}

/******************************************************************************
* Function name: LNM_NetworkStatus
* Description: Function that manages that performs the communication management
* of all the configured networks
* Parameters in: None
* Return value: None
 ******************************************************************************/
/*PRQA S 1503 1 # Runnable implementation to be called by another module*/
FUNC(void, SUM_LNM_CODE) LNM_NetworkStatus(void)
{
    VAR(uint8,AUTOMATIC) i;

    if(LNM_GlobalStatus == LNM_RUNNING)
    {
        /* Execute the network management for each network */
        for(i = LNM_ZERO; i < LNM_NUMBER_OF_NETWORKS; i++)
        {
            /* Load the current status of the communication channel */
            LNM_Network_Status[i].current_network_status = LNM_Network_Info[i].LNM_GetNetworkStatus();

            /* Channel is in FULL COMMUNICATION */
            if(LNM_Network_Status[i].current_network_status == LNM_FULL_COMMUNICATION)
            {
                /* Timers management: if timers expires the schedule is enabled by the function */
                LNM_Activation_Timer_Management(i);

                /* Satisfies: [$SDD_LNM 026] */
                /* Responder wakeup detection conditions:
                 * 1) Channel just transitioned to FULL COMMUNICATION
                 * 2) Activation was not requested from the commander */
                if((LNM_Network_Status[i].current_network_status != LNM_Network_Status[i].previous_network_status)
                        && (LNM_Network_Status[i].commander_timer_started == FALSE))
                {
                    /* Responder wakeup is recognized. Timer is started */
                    LNM_Network_Status[i].responder_timer_started = TRUE;
                    /* PRQA S 3469 1 # Configuration value, generated, in future can be used by other modules */
                    LNM_Network_Status[i].responder_timer = Rte_CData_LNM_TWuDelay_Responder();
		            LNM_Network_Status[i].responder_timer*=1000u; /*Convert to us*/

                    /* Send request for FULL COMMUNICATION to allow transmission of wakeup signal */
                    (void)LNM_Network_Info[i].LNM_RequestComMode(LNM_FULL_COMMUNICATION);
                    LNM_Network_Status[i].FULL_COMM_requested = TRUE;
                    /* Set the initial status of the schedule state machine */
                    LNM_Network_Status[i].current_schedule_state = LNM_START_STATUS;
                }
            }
            else if(LNM_Network_Status[i].current_network_status == LNM_NO_COMMUNICATION)
            {
                /* The channel transitioned from FULL COMMUNICATION to NO COMMUNICATION
                 * Timers are reset and schedule disabled */
                if(LNM_Network_Status[i].current_network_status != LNM_Network_Status[i].previous_network_status)
                {
                    LNM_Network_Status[i].commander_timer_started = FALSE;
                    LNM_Network_Status[i].responder_timer_started = FALSE;
                    LNM_Network_Status[i].commander_timer = LNM_ZERO;
                    LNM_Network_Status[i].responder_timer = LNM_ZERO;
                    LNM_Network_Status[i].schedule_enabled = FALSE;
                }
            }
            else
            {
                /* Error: Invalid state */
            }

            /* Update the previous channel state to the current */
            LNM_Network_Status[i].previous_network_status = LNM_Network_Status[i].current_network_status;

            /* Check for Networks activation / deactivation */
            LNM_ActivationManager(i);
        }
    }
    else if(LNM_GlobalStatus == LNM_SHUTDOWN)
    {
        /* Satisfies: [$SDD_LNM 025] */
        /* Reset bit has been set. Request for NO COMMUNICATION of all channels */
        for(i = LNM_ZERO; i < LNM_NUMBER_OF_NETWORKS; i++)
        {
            (void)LNM_Network_Info[i].LNM_RequestComMode(LNM_NO_COMMUNICATION);
        }
    }
    else
    {
        /* SUM_LNM is not allowed to execute its functions */
    }
}

/******************************************************************************
* Function name: LNM_NetworkFaultManager
* Description: Function that periodically manages the errors in the LIN network
* Parameters in: None
* Return value: None
 ******************************************************************************/
/*PRQA S 1503 1 # Runnable implementation to be called by another module*/
FUNC(void, SUM_LNM_CODE) LNM_NetworkFaultManager(void)
{
    if(LNM_GlobalStatus == LNM_RUNNING)
    {
        /* Management of Loss of communication with Device */
        LNM_LossOfCommunication_Device();

        /* Management of Loss of communication with Bus */
        LNM_LossOfCommunication_Bus();

        /* Management of Internal Electronic Failure */
        LNM_InternalElectronicFailure();
    }
}

/*****************************************************************************************
 * Implementation of Private Functions *
 *****************************************************************************************/

/******************************************************************************
* Function name: LNM_ActivationManager
* Description: Function that process all the activation / deactivation requests
* for the communication channels
* Parameters in:
* network_index            --- network identifier
* Return value: None
 ******************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_ActivationManager(VAR(uint8,AUTOMATIC) network_index)
{
#if (LNM_CHANNEL_SCHEDULE_USRFUNCS_ENABLED == STD_ON)
	LNM_ComM_ModeType ComMode_ret;

	/* Satisfies: [$SDD_LNM 513] [$SDD_LNM 514] */
	ComMode_ret = LNM_GetRequiredChannelState(LNM_Network_Info[network_index].LNM_ComMChannel_ID);

	if((LNM_FULL_COMMUNICATION == ComMode_ret) && (LNM_Network_Status[network_index].current_network_status == LNM_NO_COMMUNICATION))
		{
			/* Satisfies: [$SDD_LNM 024] */
			/* Commander wakeup is sent by requesting the FULL COMMUNICATION, commander timer starts */
			LNM_Network_Status[network_index].commander_timer_started = TRUE;
			/* PRQA S 3469 1 # The function like macros is used. This is necessary to get value via RTE variable */
			LNM_Network_Status[network_index].commander_timer = Rte_CData_LNM_TWuDelayCommander();
			LNM_Network_Status[network_index].commander_timer *= 1000u; /*Convert to us*/

			/* Request for FULL COMMUNICATION */
			(void)LNM_Network_Info[network_index].LNM_RequestComMode(LNM_FULL_COMMUNICATION);
			LNM_Network_Status[network_index].FULL_COMM_requested = TRUE;
		}
		/* Satisfies: [$SDD_LNM 022] */
		/* Check for Network activation criteria request */
		else if ((LNM_NO_COMMUNICATION == ComMode_ret) && (LNM_Network_Status[network_index].current_network_status == LNM_FULL_COMMUNICATION))
		{
			/* Satisfies: [$SDD_LNM 025] */
			/* Request for NO COMMUNICATION of the channel */
			(void)LNM_Network_Info[network_index].LNM_RequestComMode(LNM_NO_COMMUNICATION);
		}
		else
		{
			/* Invalid request */
		}
#else
		uint8 lnm_activationstatus = LNM_Network_Status[network_index].activation_signal | LNM_Network_Info[network_index].LNM_ActivationMask;
		uint8 lnm_deactivationstatus = LNM_Network_Status[network_index].activation_signal | LNM_Network_Info[network_index].LNM_DeactivationMask;

        boolean activate = FALSE;
        boolean deactivate = FALSE;
        /* Satisfies: [$SDD_LNM 022] [$SDD_LNM 023] [$SDD_LNM 027] */
        /* Check for Network activation/deactivation criteria request */
        // If the network activation is decided by Rule, check activation_rule. 
        // If the activation is decided by activation signal, evaluate by checking activation/deactivation masks.
        if(LNM_Network_Status[network_index].activation_requested == TRUE)
        {
            if (LNM_Network_Status[network_index].current_network_status == LNM_NO_COMMUNICATION)
            { 
                // Check if activation is required.
                if(LNM_Network_Info[network_index].LNM_ActivationTypeIsRule == TRUE)
                {
                    /* Satisfies: [$SDD_LNM 027] */
                    activate = LNM_Network_Status[network_index].activation_rule;
                }
                else
                {
                    /* Satisfies: [$SDD_LNM 022] */
                    activate = (lnm_activationstatus == LNM_MASK_FF) ? TRUE : FALSE;
                }
            }
            else if (LNM_Network_Status[network_index].current_network_status == LNM_FULL_COMMUNICATION)
            {
                // Check if deactivation is required.
                if(LNM_Network_Info[network_index].LNM_ActivationTypeIsRule == TRUE)
                {
                    /* Satisfies: [$SDD_LNM 027] */
                    deactivate = (LNM_Network_Status[network_index].activation_rule == FALSE) ? TRUE : FALSE;
                }
                else
                {
                    /* Satisfies: [$SDD_LNM 023] */
                    deactivate = (lnm_deactivationstatus == LNM_Network_Info[network_index].LNM_DeactivationMask) ? TRUE : FALSE;
                }
            }
            else
            {
                /* Invalid network status.*/
            }
        }
        else
        {
        	/* No activation requested. Do nothing. */
        }

        /* Satisfies: [$SDD_LNM 022] */
        /* Check for Network activation criteria request */
		if(activate == TRUE)
		{
            /* Satisfies: [$SDD_LNM 024] */
            /* Commander wakeup is sent by requesting the FULL COMMUNICATION, commander timer starts */
            LNM_Network_Status[network_index].commander_timer_started = TRUE;
            /* PRQA S 3469 1 # Configuration value, generated, in future can be used by other modules */
            LNM_Network_Status[network_index].commander_timer = Rte_CData_LNM_TWuDelayCommander();
            LNM_Network_Status[network_index].commander_timer *= 1000u; /*Convert to us*/

            /* Request for FULL COMMUNICATION */
            (void)LNM_Network_Info[network_index].LNM_RequestComMode(LNM_FULL_COMMUNICATION);
            LNM_Network_Status[network_index].FULL_COMM_requested = TRUE;
            /* Set the initial status of the schedule state machine */
            LNM_Network_Status[network_index].current_schedule_state = LNM_START_STATUS;
            LNM_Network_Status[network_index].activation_requested = FALSE;
			/* Satisfies: [$SDD_LNM 023] */
			/* Check for Network deactivation criteria request */
        }
        else if(deactivate == TRUE)
        {
            /* Satisfies: [$SDD_LNM 025] */
            /* Request for NO COMMUNICATION of the channel */
            (void)LNM_Network_Info[network_index].LNM_RequestComMode(LNM_NO_COMMUNICATION);
            LNM_Network_Status[network_index].activation_requested = FALSE;
        }
        else
        {
            /* No valid request */
        }
#endif
}

/******************************************************************************
* Function name: LNM_Activation_Timer_Management
* Description: Function that process timers of channel activation.
* When timer expires, it allows scheduling for the network
* Parameters in:
* network_index   ---   network identifier
* Return value: None
 ******************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_Activation_Timer_Management(VAR(uint8,AUTOMATIC) network_index)
{
    /* Responder timer management */
    if(LNM_Network_Status[network_index].responder_timer_started == TRUE)
    {
        /* Timer not expired */
        if(LNM_Network_Status[network_index].responder_timer >= LNM_NETWORK_STATUS_PERIOD_US)
        {
            LNM_Network_Status[network_index].responder_timer -= LNM_NETWORK_STATUS_PERIOD_US;
        }
        /* Timer expired */
        else
        {
            /* Enable the schedule */
            LNM_Network_Status[network_index].schedule_enabled = TRUE;
            /* Set the initial status of the schedule state machine */
            LNM_Network_Status[network_index].current_schedule_state = LNM_START_STATUS;
            /* Stop the responder commander timer */
            LNM_Network_Status[network_index].responder_timer_started = FALSE;
        }
    }

    /* Commander timer management */
    if(LNM_Network_Status[network_index].commander_timer_started == TRUE)
    {
        /* Timer not expired */
        if(LNM_Network_Status[network_index].commander_timer >= LNM_NETWORK_STATUS_PERIOD_US)
        {
            LNM_Network_Status[network_index].commander_timer -= LNM_NETWORK_STATUS_PERIOD_US;
        }
        /* Timer expired */
        else
        {
            /* Enable the schedule */
            LNM_Network_Status[network_index].schedule_enabled = TRUE;
        }
    }
}

/******************************************************************************
* Function name: LNM_ScheduleTable_Request
* Description: Function that periodically checks the current schedule and 
*              perform a request if it is not the expected one
* Parameters in:
* network_index   ---   network identifier
* Return value:
* Remarks for:
* Satisfies: [$SDD_LNM 033]
 ******************************************************************************/
/*PRQA S 1532 1 # implementation to be called by another module*/
FUNC(void, SUM_LNM_CODE) LNM_ScheduleTable_Request(VAR(uint8,AUTOMATIC) network_index)
{
    /* Get the current schedule of the network */
    LNM_Network_Status[network_index].current_schedule = LNM_Network_Info[network_index].LNM_GetCurrentSchedule();

    /* The algorithm prevents that RUN_ONCE schedules are requested multiple times */
    /* Check if the desired schedule has been successfully activated */
    if(LNM_Network_Status[network_index].desired_schedule == LNM_Network_Status[network_index].current_schedule)
    {
        LNM_Network_Status[network_index].schedule_request_success = TRUE;
    }
    /* If the desired schedule has not been activated, request the schedule */
    else if(LNM_Network_Status[network_index].schedule_request_success == ((boolean)FALSE))
    {
        (void)LNM_Network_Info[network_index].LNM_ScheduleRequest(LNM_Network_Status[network_index].desired_schedule);
    }
    else
    {
        /* Do nothing */
    }
}
#if (LNM_CHANNEL_SCHEDULE_USRFUNCS_ENABLED == STD_OFF)
/******************************************************************************
* Function name: LNM_PreSleep_Timer_Management
* Description: Functions that manage pre sleep delay
* Parameters in:network_index
* Return value:
* Remarks for:
* Satisfies: [$SDD_LNM 515]
 ******************************************************************************/
 /*PRQA S 1532 1 # implementation to be called by another module*/
FUNC(void, SUM_LNM_CODE) LNM_PreSleep_Timer_Management(VAR(uint8,AUTOMATIC) network_index) {
    /* Pre-sleep timer management */
    if (LNM_Network_Status[network_index].presleep_timer_started == TRUE) {
        /* Timer not expired */
        if (LNM_Network_Status[network_index].presleep_timer >= LNM_SCHEDULE_PERIOD_US) {
            LNM_Network_Status[network_index].presleep_timer -= LNM_SCHEDULE_PERIOD_US;
        } else {
            /* Timer expired - Transition to sleep */
            LNM_Network_Status[network_index].current_schedule_state = LNM_SLEEP_SCHEDULE_STATE;
            LNM_Network_Status[network_index].presleep_timer_started = FALSE;
			
			/* Request NO_COMMUNICATION of the channel */
			LNM_Network_Info[network_index].LNM_RequestComMode(LNM_NO_COMMUNICATION);
            /* Any additional actions upon transitioning to sleep can be added here */
        }
    }
}	
#endif
/******************************************************************************
* Function name: LNM_LossOfCommunication_Device
* Description: Function that handles the Loss of Communication with Device
* Parameters in:
* Return value:
* Remarks for:
* Satisfies: [$SDD_LNM 060] [$SDD_LNM 061] [$SDD_LNM 062]
 ******************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_LossOfCommunication_Device(void)
{
    VAR(uint8,AUTOMATIC) i, j;
    VAR(Std_ReturnType,AUTOMATIC) rv;
    VAR(uint8,AUTOMATIC) data;
    /* Satisfies: [$SDD_LNM 060] */
    /* Error detection iif the DTC setting is enabled */
    if (LNM_Get_CalibPar_LNM_C_DTC_LostCommWithLinResp() == TRUE)
    {
        /* Error detection for all responders of every network */
        for(i = 0; i < LNM_NUMBER_OF_NETWORKS; i++)
        {
            for(j = 0; j < LNM_Network_Info[i].LNM_ResponderNumber; j++)
            {
                /* Checks for LoC iif
                 * 1) the LIN responder is present
                 * 2) the channel is in full communication
                 */
                if (
                    ((LNM_Network_Status[i].MSCL & ((uint16)LNM_RESPONDER_PRESENT << LNM_Network_Info[i].LNM_Responders[j].LNM_MSCL_Index)) != LNM_ZERO) &&
                    (LNM_Network_Status[i].current_network_status == LNM_FULL_COMMUNICATION)
                )
                {
                    /* Satisfies: [$SDD_LNM 061] */
                    /* Read timeout status from SSM */
                    rv = LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_SSM_LNM_NodeTimeout(&data);

                    /* Satisfies: [$SDD_LNM 062] */
                    /* Check that the timeout status read completed successfully */
                    if(rv == LNM_E_OK)
                    {
                        /* If no timeout has been detected for this node by SSM  */
                        if(data == ERROR_EVENT_INACTIVE)
                        {
                            /* Update notification to SUM_ERRH */
                            (void)LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_LNM_ERRH_NodeId(LNM_DTC_INACTIVE);
                            (void)LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_LNM_ERRH_NodeTimer(LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_Timer);

                            /* Update internal status of active LoC with device */
                            LNM_Network_Status[i].LoC_WithDevice_Status &= ~((uint16)LNM_RESPONDER_PRESENT << LNM_Network_Info[i].LNM_Responders[j].LNM_MSCL_Index);                        
                        }
                        else
                        {
                            /* Update notification to SUM_ERRH */
                            (void)LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_LNM_ERRH_NodeId(LNM_DTC_ACTIVE);
                            (void)LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_LNM_ERRH_NodeTimer(LNM_Network_Info[i].LNM_Responders[j].LNM_LoCWithDevice_Timer);

                            /* Update internal status of active LoC with device */
                            LNM_Network_Status[i].LoC_WithDevice_Status |= ((uint16)LNM_RESPONDER_PRESENT << LNM_Network_Info[i].LNM_Responders[j].LNM_MSCL_Index);
                        }   
                    }
                }
            }
        }
    }
}

/******************************************************************************
* Function name: LNM_LossOfCommunication_Bus
* Description: Function that handles the Loss of Communication with Bus
* Parameters in:
* Return value:
* Remarks for:
* Satisfies: [$SDD_LNM 070] [$SDD_LNM 071] [$SDD_LNM 072] [$SDD_LNM 073]
 ******************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_LossOfCommunication_Bus(void)
{
    VAR(uint8,AUTOMATIC) loc_status;
    VAR(uint16,AUTOMATIC) loc_mask;
    VAR(uint8,AUTOMATIC) i;
    VAR(uint8,AUTOMATIC) max_retries;
    VAR(boolean,AUTOMATIC) rv_ch_enabled;
    VAR(boolean,AUTOMATIC) rv_ch_DTC_enabled;
    VAR(boolean,AUTOMATIC) rv_veh_power_mode_ok;

    /* Diagnostic of Loss of Communication with Bus executed only Vehicle Power Mode is in run or in 
       propulsion for at least  */
    /* PRQA S 3469 1 # The function like macros is used. This is necessary to get value via RTE variable */
    rv_veh_power_mode_ok = Rte_IrvRead_Runnable_NetworkFaultManager_LNM_VehiclePowerModeRunOrPropulsion();
    /* Check LoC with Bus for each LIN channel */
    for(i = 0; i < LNM_NUMBER_OF_NETWORKS; i++)
    {
        /* Diagnostic of Loss of Communication with Bus executed only if channel is enabled */
        rv_ch_enabled = LNM_Get_MSCL_ChannelEnable(i);
        rv_ch_DTC_enabled = LNM_Network_Info[i].LNM_Get_LINBusOff_DTCEnable();
        if((rv_veh_power_mode_ok == TRUE) && (rv_ch_DTC_enabled == TRUE) && (rv_ch_enabled == TRUE))
        {
            /* Initialize status */
            loc_status = (uint8)LNM_ERROR_EVENT_INACTIVE;

            /* TEST FAILED: Set event active if ALL the PRESENT LIN Responders of the network are lost */
            /* Retrieve all the present responders removing Config bit from MSCL */
            loc_mask = LNM_Network_Status[i].MSCL & ((uint16)LNM_RESPONDERS_MASK);

            /* Compare the computed mask with the current LoC with device status */
            if((loc_mask & LNM_Network_Status[i].LoC_WithDevice_Status) == loc_mask)
            {
                /* All the responders have been lost. Set the event active */
                loc_status = (uint8)LNM_ERROR_EVENT_ACTIVE;
            }

            /* TEST FAILED: Set event active if channel unable to transition to FULL COMMUNICATION */
            if(LNM_Network_Status[i].FULL_COMM_requested == TRUE)
            {
                if(LNM_Network_Status[i].current_network_status == LNM_FULL_COMMUNICATION)
                {
                    /* Channel successfully transitioned to FULL COMMUNICATION. Reset number of retries and clear the flag */
                    LNM_Network_Status[i].FULL_COMM_retries = LNM_ZERO;
                    LNM_Network_Status[i].FULL_COMM_requested = FALSE;
                }
                else
                {
                    /* Channel is in NO COMMUNICATION, even if FULL COMMUNICATION has been requested.
                     * Set event active after maximum number of retries reached */
                	/* PRQA S 3469 1 # Configuration value, generated, in future can be used by other modules */
                    max_retries = Rte_CData_LNM_WupTxRetries_Commander();
                    if(LNM_Network_Status[i].FULL_COMM_retries == max_retries)
                    {
                        loc_status = (uint8)ERROR_EVENT_ACTIVE;
                    }
                    else
                    {
                        LNM_Network_Status[i].FULL_COMM_retries++;
                    }
                }
            }

            /* Report the current error status to ERRH */
            (void)LNM_Network_Info[i].LNM_ERRH_LossCommBus(loc_status);
        }
    }
}

/******************************************************************************
* Function name: LNM_InternalElectronicFailure
* Description: Function that handles the Internal Electronic Fault (APINFO4 detection)
* Parameters in:
* Return value:
* Remarks for:
* Satisfies: [$SDD_LNM 080] [$SDD_LNM 081] [$SDD_LNM 082] 
 ******************************************************************************/
static FUNC(void, SUM_LNM_CODE) LNM_InternalElectronicFailure(void)
{
    VAR(uint8,AUTOMATIC) i,j;
    VAR(uint8,AUTOMATIC) loc_status;
    VAR(boolean,AUTOMATIC) apinfo4 = LNM_ERROR_EVENT_INACTIVE;
    VAR(Std_ReturnType,AUTOMATIC) rv;

    /* Check IEF for each responder of each LIN channel*/
    for(i = 0; i < LNM_NUMBER_OF_NETWORKS; i++)
    {
        for(j = 0; j < LNM_Network_Info[i].LNM_ResponderNumber; j++)
        {
            /* Check if the selected responder supports IEF */
            if(LNM_Network_Info[i].LNM_Responders[j].LNM_IEF_Supported() == TRUE)
            {
                /* Check if the responder is currently active in the network */
                if((LNM_Network_Status[i].MSCL & ((uint16)LNM_RESPONDER_PRESENT << LNM_Network_Info[i].LNM_Responders[j].LNM_MSCL_Index)) != LNM_ZERO)
                {
                    /* Initialize status */
                    loc_status = (uint8)LNM_ERROR_EVENT_INACTIVE;

                    /* Check if APINFO4 detection supported */
                    if(LNM_Network_Info[i].LNM_Responders[j].LNM_IEF_APINFO4 != NULL_PTR)
                    {
                        /* If the bit Application Status bit is set to 1, error is present */
                        rv = LNM_Network_Info[i].LNM_Responders[j].LNM_IEF_APINFO4(&apinfo4);
                        if((rv == LNM_E_OK) && (apinfo4 == LNM_ERROR_EVENT_ACTIVE))
                        {
                                loc_status = (uint8)LNM_ERROR_EVENT_ACTIVE;
                        }
                    }
					
                    /* Check if APINFO2 detection supported */
                    if( (LNM_Network_Info[i].LNM_Responders[j].LNM_IEF_APINFO2 != NULL_PTR) &&
                        (LNM_Network_Info[i].LNM_Responder_Status[j].ief_reset_counter == LNM_Network_Info[i].LNM_Responders[j].LNM_APINFO2_Threshold)	)
                    {
                        /* If the number of consecutive reset reached the threshold, error is present */
                        loc_status = (uint8)LNM_ERROR_EVENT_ACTIVE;
                    }

                    /* Report test result to ERRH */
                    (void)LNM_Network_Info[i].LNM_Responders[j].LNM_IEF_LNM_ERRH_IntElectFault(loc_status);

                }
            }
        }
    }
}

/******************************************************************************
* Function name: LNM_Get_CalibPar_LNM_C_DTC_LostCommWithLinResp
* Description: Getter function to obtain the calibration value that controls the aggregation of DTC Lost Communication with LIN Responder
* Parameters in:
* Return value: boolean
* Remarks for:
* Satisfies: [$SDD_LNM 080] [$SDD_LNM 081] [$SDD_LNM 082] 
 ******************************************************************************/
static FUNC(boolean, SUM_LNM_CODE) LNM_Get_CalibPar_LNM_C_DTC_LostCommWithLinResp(void)
{
    /* PRQA S 3469 1 # The function like macros is used. This is necessary to get the calibration value via RTE variable */
    return Rte_CData_LNM_C_DTC_LostCommWithLinResp();
}

/********************************** END OF FILE ******************************************************/
