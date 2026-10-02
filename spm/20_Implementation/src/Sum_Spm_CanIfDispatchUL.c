#include "Sum_Spm_CanIfDispatchUL.h"

#if (SPM_WKUP_ON_CAN_SUPPORT == STD_ON)

/* PRQA S 1503 EOF # Usage of function in a complete integrated project due to project specific conditions */
/* PRQA S 3214 EOF # Memory Mapping for security reason */


/**
 * @brief 
 * 
 * @param ControllerId 
 */
void Spm_CtrlBusOff( const uint8 ControllerId )
{
	/* Please remove this when you defining the current function */
    (void)( ControllerId );

}

/**
 * @brief 
 * 
 * @param ControllerId 
 * @param ControllerMode 
 */
void Spm_CtrlModeIndication( const uint8 ControllerId, Can_ControllerStateType ControllerMode )
{
	/* Please remove these when you defining the current function */
    (void)( ControllerId );
    (void)( ControllerMode );

}

#ifdef SPM_TRCV_CONF_IN_CANIF
#if (SPM_TRCV_CONF_IN_CANIF == STD_ON)

/**
 * @brief 
 * 
 * @param TransceiverId 
 * @param TransceiverMode 
 */
void Spm_TrcvModeIndication( const uint8 TransceiverId, const CanTrcv_TrcvModeType TransceiverMode )
{
	/* Please remove these when you defining the current function */
	(void)( TransceiverId );
	(void)( TransceiverMode );
}

/* Please define this if the CAN Transceiver supports the Wake-Uo mode */
#ifdef SPM_TRCV_WAKEUP_CONF_SUPP

/**
 * @brief 
 * 
 * @param Transceiver 
 */
void Spm_CheckTrcvWakeFlagIndication( const uint8 Transceiver )
{
	/* Please remove this when you defining the current function */
	(void)( Transceiver );
}

/**
 * @brief 
 * 
 * @param Transceiver 
 */
void Spm_ClearTrcvWufFlagIndication( const uint8 Transceiver )
{
	/* Please remove this when you defining the current function */
	(void)( Transceiver );
}
#endif
#endif /* SPM_TRCV_WAKEUP_CONF_SUPP  */

#endif /* SPM_TRCV_CONF_IN_CANIF */

#endif /* SPM_HAS_CAN_WUF_SUPPORT */


/* EOF */
