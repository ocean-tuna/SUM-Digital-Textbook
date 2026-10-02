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
 * Description : LNM Types Header file
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
 */

#ifndef SUM_LNM_TYPES_H
#define SUM_LNM_TYPES_H

/****************************************************************************/
/* Environment Definitions                                                  */
/****************************************************************************/

/* Satisfies: [$SDD_LNM 501] */
/* LNM Global status */
#define LNM_UNINIT		 	         0x00u
#define LNM_INIT		 		     0x01u
#define LNM_RUNNING			     	 0x02u
#define LNM_SHUTDOWN			     0x03u

/* Satisfies: [$SDD_LNM 502] */
/* Permit execution */
#define LNM_EXECUTION_ALLOWED        0xFFu
#define LNM_RESET_BIT                0x40u

/* Satisfies: [$SDD_LNM 503] */
/* Shutdown Permission */
#define LNM_SHUTDOWN_ENABLE          0xFFu
#define LNM_MIN_ROLLING_COUNTER      0x00u
#define LNM_MAX_ROLLING_COUNTER      0x03u

/* Satisfies: [$SDD_LNM 504] */
/* Network Status */
#define LNM_NO_COMMUNICATION         0x00u
#define LNM_FULL_COMMUNICATION       0x01u

/* Satisfies: [$SDD_LNM 505] */
/* Schedule definitions */
#define LNM_NULL_SCHEDULE            0x00u
#define LNM_START_STATUS             0x01u

/* Satisfies: [$SDD_LNM 506] */
/* MSCL definitions */
#define LNM_MSCL_OFF                 0x0000u
#define LNM_RESPONDERS_MASK              0x7FFFu
#define LNM_RESPONDER_PRESENT            0x01u
#define LNM_CONFIGSTATUS_BIT         0x0Fu

/* Satisfies: [$SDD_LNM 507] */
/* DTCs definitions */
#define LNM_DTC_INACTIVE             0x00u
#define LNM_DTC_ACTIVE               0x01u

/* Satisfies: [$SDD_LNM 508] */
/* Error events definitions */
#define LNM_ERROR_EVENT_INACTIVE     (boolean)0x00u
#define LNM_ERROR_EVENT_ACTIVE       (boolean)0x01u

/* Signal availability */
#define LNM_SIGNAL_AVAILABLE         0x03u

/* Bit Mask FF */
#define LNM_MASK_FF                  0xFFu

/* Internal macros for RTE return values */
#define LNM_E_OK                     0x00u

/* Internal macros for enable / disable of features */
#define LNM_OFF                      0x00u
#define LNM_ON                       0x01u

/* Macros for for DID F1BE */
#define LNM_DID_F1BE_LENGTH          0x80u
#define LNM_BYTE_SHIFT               0x08u

/* Macros for constants */
#define LNM_ZERO                     0x00u

/* Macros for LIN bus off enable criteria */
#define LNM_VEH_POWER_MODE_MAX_RESET_TIMER 5000u

/* Macros for timing unit conversion */
#define LNM_MS_TO_US_UNIT                  1000u

/*
 * AUTOSAR Version support
 */
#define LNM_AR_4_0_x_SUPPORT	0u
#define LNM_AR_4_2_x_SUPPORT	1u
#define LNM_AR_4_3_x_SUPPORT	2u

/****************************************************************************/
/* Data structures definition                                               */
/****************************************************************************/
typedef struct lnm_responder_status_type
{
    VAR(uint8,AUTOMATIC)ief_reset_counter;
    VAR(uint8,AUTOMATIC)not_configured_counter;
    VAR(uint8,AUTOMATIC)no_comm_counter;
}LNM_Responder_Status_Type;

typedef struct lnm_responder_info_type
{
    VAR(uint8,AUTOMATIC)LNM_MSCL_Index;
    VAR(uint16,AUTOMATIC)LNM_LoCWithDevice_Timer;
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_LoCWithDevice_LNM_ERRH_NodeId)) (VAR(uint8, AUTOMATIC) data);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_LoCWithDevice_LNM_ERRH_NodeTimer)) (VAR(uint32, AUTOMATIC) data);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_LoCWithDevice_SSM_LNM_NodeTimeout)) (CONSTP2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) data);
    P2FUNC(boolean, TYPEDEF, (LNM_IEF_Supported)) (void);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_IEF_LNM_ERRH_IntElectFault)) (VAR(uint8, AUTOMATIC) data);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_IEF_APINFO2))(CONSTP2VAR(boolean, AUTOMATIC, RTE_APPL_DATA) data);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_IEF_APINFO4)) (CONSTP2VAR(boolean, AUTOMATIC, RTE_APPL_DATA) data);
    VAR(uint8,AUTOMATIC)LNM_APINFO2_Threshold;
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_NotConfigured_APINFO3)) (CONSTP2VAR(boolean, AUTOMATIC, RTE_APPL_DATA) data);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_ERRH_NotConfigured)) (VAR(uint8, AUTOMATIC) data);
}LNM_Responder_Info_Type;

typedef struct lnm_network_info_type
{
    P2VAR(LNM_Responder_Info_Type, AUTOMATIC, LNM_VAR_FAST) LNM_Responders;
    P2FUNC(void, TYPEDEF, (LNM_ScheduleManager)) (void);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_ScheduleRequest)) (VAR(uint8, AUTOMATIC) data);
    P2FUNC(uint8, TYPEDEF, (LNM_GetCurrentSchedule)) (void);
    P2FUNC(uint8, TYPEDEF, (LNM_GetNetworkStatus)) (void);
#if (LNM_CHANNEL_SCHEDULE_USRFUNCS_ENABLED == STD_OFF)
    VAR(boolean,AUTOMATIC)LNM_ActivationTypeIsRule;
    P2FUNC(boolean, TYPEDEF, (LNM_GetActivationRule)) (void);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_GetActivationSignal)) (CONSTP2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) data);
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_GetActivationSignal_Av)) (CONSTP2VAR(SignalStatusType, AUTOMATIC, RTE_APPL_DATA));
#endif
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_RequestComMode)) (VAR(uint8, AUTOMATIC) data);
#if (LNM_CHANNEL_SCHEDULE_USRFUNCS_ENABLED == STD_OFF)
    VAR(boolean,AUTOMATIC)LNM_isComActivationSignal;
    VAR(uint8,AUTOMATIC)LNM_ActivationMask;
    VAR(uint8,AUTOMATIC)LNM_DeactivationMask;
#endif
    VAR(uint8,AUTOMATIC)LNM_ResponderNumber;
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_MSCL_SSM_Notification)) (VAR(uint16, AUTOMATIC) data);
    VAR(uint8,AUTOMATIC)LNM_Network_ID;
    P2FUNC(Std_ReturnType, TYPEDEF, (LNM_ERRH_LossCommBus)) (VAR(uint8, AUTOMATIC) data);
    VAR(LNM_Responder_Status_Type*,AUTOMATIC)LNM_Responder_Status;
    VAR(uint8,AUTOMATIC)LNM_LinSMNetwork_ID;
    VAR(uint8,AUTOMATIC)LNM_ComMChannel_ID;
    P2FUNC(boolean, TYPEDEF, (LNM_Get_LINBusOff_DTCEnable)) (void);
}LNM_Network_Info_Type;

typedef struct lnm_network_status_type
{
    VAR(uint8,AUTOMATIC)current_schedule_state;
    VAR(uint8,AUTOMATIC)current_schedule;
    VAR(uint8,AUTOMATIC)desired_schedule;
    VAR(boolean,AUTOMATIC)schedule_request_success;
    VAR(boolean,AUTOMATIC)schedule_enabled;
    VAR(uint8,AUTOMATIC)current_network_status;
    VAR(uint8,AUTOMATIC)previous_network_status;
    VAR(boolean,AUTOMATIC)commander_timer_started;
    VAR(boolean,AUTOMATIC)responder_timer_started;
    VAR(boolean,AUTOMATIC)presleep_timer_started;
	VAR(uint32,AUTOMATIC)commander_timer;
    VAR(uint32,AUTOMATIC)responder_timer;	VAR(uint32,AUTOMATIC)presleep_timer;
    VAR(uint16,AUTOMATIC)MSCL;
    VAR(uint16,AUTOMATIC)LoC_WithDevice_Status;
    VAR(boolean,AUTOMATIC)FULL_COMM_requested;
    VAR(uint8,AUTOMATIC)FULL_COMM_retries;
    VAR(boolean,AUTOMATIC)activation_requested;
    VAR(uint8,AUTOMATIC)activation_signal;
    VAR(boolean,AUTOMATIC)activation_rule;
}LNM_Network_Status_Type;

#endif

/********************************** END OF FILE ******************************************************/
