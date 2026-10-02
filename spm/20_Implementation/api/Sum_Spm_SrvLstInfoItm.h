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
 * Description : Header for SPM Power Service List Info Type Item
 ***************************************************************************************************
 * END:Header
 ***************************************************************************************************
*/

#ifndef SPM_SRVC_LST_INFO_ITM_H_
#define SPM_SRVC_LST_INFO_ITM_H_

#include "Sum_Spm_Cfg.h"


/**
 * @brief DEFINITION OF RECORD STRUCTURE SpmSrvLstInfoItm
 */
struct SpmSrvLstInfoItm {
   uint16 pp_sID;
   boolean is_active;
};


/* Following DEFINE signalizes the completion of definition                   */
/* of data structs for component: ....SpmSrvLstInfoItm */
#define _SpmSrvLstInfoItm_TYPE_DEF_


#endif /* SPM_SRVC_LST_INFO_ITM_H_ */

/* EOF */
