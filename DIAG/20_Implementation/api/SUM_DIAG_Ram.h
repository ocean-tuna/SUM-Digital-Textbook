/*
 ***************************************************************************************************
 * BEGIN:Banner
 ***************************************************************************************************
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 24
 *--------------------------------------------------------------------------------------------------
 *
 * COPYRIGHT RESERVED, 2020 ETAS GmbH. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as
 * the communication of its contents to others without explicit authorization
 * is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or 
 * design.
 *--------------------------------------------------------------------------------------------------
 *    Administrative Information (automatically filled in)
 *--------------------------------------------------------------------------------------------------
 * Project     : GM SUM
 * Component   : SUM_DIAG
 * Date        : Jun 13, 2026
 * Revision    : 8.0.1_Pr24
 * Description : SUM_DIAG header ram variables
 ***************************************************************************************************
 * END:Banner
 ***************************************************************************************************
 */
#ifndef SUM_DIAG_RAM_H
#define SUM_DIAG_RAM_H

#include "SUM_DIAG_Types.h"

/* Variable used to store status(enable/disable) of permit execution from other SUM */
extern VAR(boolean, RTE_DATA) SUM_DIAG_permit_execution; /* PRQA S 3447 EOF */ /*extern is required here since this has been used globally*/
/* Variable used to store ECU reset requested state, set when SUSD clears bit 6 of PermitExecution */
extern VAR(boolean, RTE_DATA) SUM_DIAG_ecu_reset_requested;
/* Variable to hold the current MEC read from NvM */
extern VAR(uint8, RTE_DATA) SUM_DIAG_current_mec; 
/* Variable to store the cycle of states for decrementing MEC, that is SLEEP->RUN->SLEEP */
extern VAR(uint8, RTE_DATA) SUM_DIAG_MEC_Cycle_States;
/* Variable to store the cycle of states for write MEC */
extern VAR(uint8, RTE_DATA) SUM_DIAG_MEC_Write_State;
/* Variable to store the cycle of states for write OTA */
extern VAR(uint8, RTE_DATA) SUM_DIAG_OTA_Write_State;

#if (MEC_STORAGE_IN_HSM == STD_ON)
/* [$Satisfies $SDD_DIAG 131] HSM MEC variables (CYS4008) */
extern VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Write_Value;
extern VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Write_RetryCounter;
extern VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Read_Value;
extern VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Read_ByteCount;
extern VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Write_State;
extern VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Availability;
extern VAR(uint16, RTE_DATA) SUM_DIAG_HSM_MEC_Write_TimeoutCounter;
#endif


#endif /*SUM_DIAG_RAM_H */
