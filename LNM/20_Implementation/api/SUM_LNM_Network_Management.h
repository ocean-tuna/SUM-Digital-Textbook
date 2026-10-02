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
 * Description : LNM Network Management Header file
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
 */

#ifndef SUM_LNM_NETWORK_MANAGEMENT_H
#define SUM_LNM_NETWORK_MANAGEMENT_H


/*****************************************************************************************
 * Header Includes *
 *****************************************************************************************/
#include "Rte_SUM_LNM.h"
#include "SUM_LNM_Cfg.h"
#include "SUM_LNM_Types.h"

/*****************************************************************************************
 * Definition of Global Variables *
 *****************************************************************************************/
/* Data structure containing current states of the networks */
extern LNM_Network_Status_Type LNM_Network_Status[LNM_NUMBER_OF_NETWORKS];

/*****************************************************************************************
 * Definition of Global Functions *
 *****************************************************************************************/
#if (LNM_CHANNEL_SCHEDULE_USRFUNCS_ENABLED == STD_ON)
extern FUNC(LNM_ComM_ModeType, SUM_LNM_CODE) LNM_GetRequiredChannelState(VAR(NetworkHandleType, AUTOMATIC) ID);
extern FUNC(LNM_SchHandleType, SUM_LNM_CODE) LNM_GetRequiredSchedule(VAR(NetworkHandleType, AUTOMATIC) ID);
#endif

/*****************************************************************************************
 * Definition of Private Functions *
 *****************************************************************************************/
extern FUNC(void, SUM_LNM_CODE) LNM_ScheduleTable_Request(VAR(uint8,AUTOMATIC) network_index);
extern FUNC(void, SUM_LNM_CODE) LNM_PreSleep_Timer_Management(VAR(uint8,AUTOMATIC) network_index);
#endif

/********************************** END OF FILE ******************************************************/
