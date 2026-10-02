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
 * Component   : SUM_TCS
 * Date        : Mar 31, 2026
 * Revision    : 8.0.1_Pr10
 * Description : TCS Private Header
 ***************************************************************************************************
 * END:Header
 ***************************************************************************************************
*/

#ifndef SUM_TCS_PRV_H
#define SUM_TCS_PRV_H

#include "Std_Types.h"
#include "Rte_SUM_TCS_Type.h"

#define SUM_TCS_START_SEC_CODE
#include "SUM_TCS_MemMap.h"  /* PRQA S 5087 EOF */
/* include MemMap.h required here to occupy the space in memory for the described macro*/
/* extern is required here since this function has been used globally*/
/* Initialization function of TCS to be invoked by BswM */

/* NvM physical buffer size for the 5-byte big-endian timestamp */
#define SUM_TCS_TIMESYNCMESSAGE_NVM_SIZE    5u
/* Variable used to store TCS Init Value - NvM physical buffer (5 bytes big-endian).
 * Kept as uint8 array so NvM_Cfg.c can reference it as uint8[].
 * Conversion to/from uint64 is done by TCS_Prv_getInitTime / TCS_Prv_setInitTime. */
extern uint8 SumTCSInitValue[SUM_TCS_TIMESYNCMESSAGE_NVM_SIZE];
/* Mask for the 36-bit effective counter range of the 5-byte big-endian NvM timestamp.
 * NOTE: confgen NvM block size must remain 5 bytes even though TimeSyncMessageType is uint64. */
#define SUM_TCS_TIMESYNCMESSAGE_MASK            0x0FFFFFFFFFuLL
/* Mask applied to the high byte of the timestamp only on 32-bit low-word overflow */
#define SUM_TCS_TIMESYNCMESSAGE_HIGH_BITMASK    0x0Fu
#define SUM_TCS_STOP_SEC_CODE
#include "SUM_TCS_MemMap.h"

/* Named numerical constants */
#define TCS_ZERO 	0u
#define TCS_ONE 	1u
#define TCS_TWO 	2u



#define UNUSED_POINTER(PointerName) {if((PointerName) != NULL_PTR)\
                                    {/* Do Nothing*/ }}

#define UNUSED_PARAMETER(VariableName) {if((VariableName) != 0U)\
                                       {/* Do Nothing */}}

/* ResetRequest mask
 * Satisfies: [$SDD_TCS 028] */
#define TCS_RESETREQUEST_MASK   0xBFu  /* bit 6 is Ecu Reset Requested flag */

/* result of PermitExecution or'd with TCS_PermitExecution_Mask */
#define TCS_PERMITEXECUTION     0xFFu

/* flag for shut down enable
 * Satisfies: [$SDD_TCS 029] */
#define TCS_ENABLE_SHUTDOWN 255u

/* Limit value for alive counter mechanism
 * Satisfies: [$SDD_TCS 030] */
#define TCS_SHUTDOWN_CNT_LIMIT 4u

typedef enum TCS_EXECUTION_STATUS
{
	TCS_INIT_MODE,
	TCS_RUN_MODE,
	TCS_SHUTDOWN
} TCS_Execution_Status_Type;

#ifndef TCS_E_PENDING
#define TCS_E_PENDING (10u)
#endif
#endif
