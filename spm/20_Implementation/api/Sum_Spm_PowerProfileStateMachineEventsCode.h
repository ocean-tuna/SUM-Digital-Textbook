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
 * Description : Header for SPM Power Profile State Machine events codes
 ***************************************************************************************************
 * END:Header
 ***************************************************************************************************
*/

#ifndef SPM_POWERPROFILESM_EVENTCODE_H_
#define SPM_POWERPROFILESM_EVENTCODE_H_


/*-----------------------------------------------------------------------------
 *    Include files
 *----------------------------------------------------------------------------*/


enum SpmPowerProfileStateMachineEventsCode {
   Spm_PrfMgr_evDeactivationRequest = 0U,
   Spm_PrfMgr_evActivationComplete = 1U,
   Spm_PrfMgr_evNoRequestActive = 2U,
   Spm_PrfMgr_evActivationRequest = 3U,
   Spm_PrfMgr_evDeactivationComplete = 4U,
   Spm_PrfMgr_evActivationFailed = 5U,
   Spm_PrfMgr_evDeactivationFailed = 6U
};

#endif /* SPM_POWERPROFILESM_EVENTCODE_H_ */

/* EOF */
