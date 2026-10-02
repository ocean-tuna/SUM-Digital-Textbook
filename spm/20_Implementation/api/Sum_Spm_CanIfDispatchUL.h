/*
 ***************************************************************************************************
 * BEGIN:Header
 ***************************************************************************************************
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 24
 ***************************************************************************************************
 * COPYRIGHT RESERVED, 2020 ETAS GmbH. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as
 * the communication of its contents to others without explicit authorization
 * is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or
 * design.
 ***************************************************************************************************
 * Project     : GM SUM
 * Component   : SUM_SPM
 * Date        : Jun 15, 2026
 * Revision    : 8.1.0_Pr33
 * Description : header for user defined (SPM) function to replace the Standard AUTOSAR CanSM
 ***************************************************************************************************
 * END:Header
 ***************************************************************************************************
*/

#ifndef SPM_CANIF_DISPATCH_UL_H_
#define SPM_CANIF_DISPATCH_UL_H_

#include "Sum_Spm_Cfg.h"
# if (SPM_WKUP_ON_CAN_SUPPORT == STD_ON)

#include "Platform_Types.h"
#include "CanIf_Types.h"


extern void Spm_CtrlBusOff( 
    const uint8 ControllerId 
);

extern void Spm_CtrlModeIndication(
    const uint8 ControllerId,
    Can_ControllerStateType ControllerMode
);

/* 
 * The SPM attachment point with the Can Driver is the Can Interface.
 *
 * The configuration of the Transceiver must be present in the CanIf configuration.
 */
#ifdef SPM_TRCV_CONF_IN_CANIF
#if (SPM_TRCV_CONF_IN_CANIF == STD_ON)

extern void Spm_TrcvModeIndication(
    const uint8 TransceiverId, 
    const CanTrcv_TrcvModeType TransceiverMode
);

/*
 * Not all Transceivers can be configured to detect the wake-up.
 *
 * If the Transceiver has been configured to detect the wake-up, The following macro is defined 
 * at SPM scope.
 * 
 * If this parameter is configured, then the Partial Networking is implicitly enabled
 * by definition.
 */
#if (SPM_TRCV_WAKEUP_CONF_SUPP == STD_ON)

extern void Spm_CheckTrcvWakeFlagIndication(
    const uint8 Transceiver
);

extern void Spm_ClearTrcvWufFlagIndication(
    const uint8 Transceiver
);

#endif
#endif /* ENDIF DEF SPM_TRCV_WAKEUP_CONF */

#endif /* ENDIF DEF SPM_TRCV_CONF_IN_CANIF */

#endif /* END SPM_HAS_CAN_WUF_SUPPORT */

#endif /* END MULTIPLE-INCLUSION GUARD */

/* EOF */
