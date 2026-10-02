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
 * Description : Source for needed integration code
 ***************************************************************************************************
 * END:Header
 ***************************************************************************************************
*/

#ifndef SUM_SPM_ACTIVATIONCRITERIA_H_
#define SUM_SPM_ACTIVATIONCRITERIA_H_

#include "Sum_Spm_Cfg.h"

#define SPM_NUMBER_ITEM_IN_SM_FRAME 98u

void Spm_TxTimeoutException( const NetworkHandleType TxPduId );

#endif

/* EOF */

