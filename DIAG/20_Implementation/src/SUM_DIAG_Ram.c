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
 * Description : SUM_DIAG ram variables
 ***************************************************************************************************
 * END:Banner
 ***************************************************************************************************

 */

#include "SUM_Diag.h"
#include "SUM_DIAG_Ram.h"


/* Variable used to store status(enable/disable) of permit execution from other SUM */
VAR(boolean, RTE_DATA) SUM_DIAG_permit_execution;
/* Variable used to store ECU reset requested state, set when SUSD clears bit 6 of PermitExecution */
VAR(boolean, RTE_DATA) SUM_DIAG_ecu_reset_requested; /* PRQA S 1531 # Based on the architecture design it is decided to keep the object in the corresponding component */
/* Variable used to store the current value of manufacturer enable counter */
VAR(uint8, RTE_DATA) SUM_DIAG_current_mec;
/* Variable to store the cycle of states for decrementing MEC, that is SLEEP->RUN->SLEEP */
VAR(uint8, RTE_DATA) SUM_DIAG_MEC_Cycle_States; /* PRQA S 1531 # Based on the architecture design it is decided to keep the object in the corresponding component */
/* Variable to store the cycle of states for write MEC */
VAR(uint8, RTE_DATA) SUM_DIAG_MEC_Write_State;
/* Variable to store the cycle of states for write OTA */
VAR(uint8, RTE_DATA) SUM_DIAG_OTA_Write_State;

#if (MEC_STORAGE_IN_HSM == STD_ON)
/* [$Satisfies $SDD_DIAG 131] HSM MEC variables (CYS4008) */
VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Write_Value;
VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Write_RetryCounter;
VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Read_Value;
VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Read_ByteCount;
VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Write_State;
VAR(uint8, RTE_DATA) SUM_DIAG_HSM_MEC_Availability;
VAR(uint16, RTE_DATA) SUM_DIAG_HSM_MEC_Write_TimeoutCounter;
#endif
