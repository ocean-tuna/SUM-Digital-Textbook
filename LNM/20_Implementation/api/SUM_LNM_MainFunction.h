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
 * Description : LNM Main Header file
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
 */

#ifndef SUM_LNM_MAINFUNCTION_H
#define SUM_LNM_MAINFUNCTION_H

/*****************************************************************************************
 * Definition of Global Variables *
 *****************************************************************************************/
/* Variable used to store the current status of SUM_LNM */
extern VAR(uint8,AUTOMATIC)LNM_GlobalStatus;

/*****************************************************************************************
 * Definition of Global Functions *
 *****************************************************************************************/
#if LNM_RUNNABLE_INIT_ENABLED == STD_OFF
extern FUNC(void, SUM_LNM_CODE) LNM_Init(void);
#endif

#endif

/********************************** END OF FILE ******************************************************/
