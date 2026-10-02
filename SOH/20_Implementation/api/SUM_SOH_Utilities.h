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
 * Component   : SUM_SOH
 * Date        : Jun 25, 2026
 * Revision    : 8.0.1_rc
 * Description : Utilities header for SOH
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

/*! @defgroup SUM_SOH_Utilities_h The SUM_SOH_Utilities.h file
 *  This is the SUM_SOH_Utilities.h file
 *  @{
 */

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#ifndef SUM_SOH_UTILITIES_H_
#define SUM_SOH_UTILITIES_H_

#include "SUM_SOH.h"
#include "SUM_SOH_Cfg.h"

/* START OF PROTOTYPES */

#define SUM_SOH_START_SEC_CODE
#include "SUM_SOH_MemMap.h"


FUNC(void, SUM_SOH_CODE) SOH_NvMReadall(void);
FUNC(void, SUM_SOH_CODE) SOH_InitVariables(void);

/*!
 *	This function is called only by SOH_Event_Detector(), after having increase the field SOH_Cntr in a detector's SOH_DetectorType instance.
 *	Here, that detector goes under the operations of its related filter using that new value of SOH_Cntr.
 *
 *	If field SOH_Cntr reaches its threshold count calibration in field SOH_CntrThreshold():
 *	\Requirement{020}
 *		- increments field SOH_DIDCntr, if SOH_DIDCntr has not reached 0xFFFE yet
 *	\Satisfies{015} \Satisfies{018}
 *	\Requirement{021}
 *		- resets field SOH_Cntr to 0
 *	\Satisfies{015}
 *	\Requirement{022}
 *		- resets field SOH_TW to 0.
 *	\Satisfies{015}
 */
#if (NUM_OF_SOH_DETECTORS != 0u)
FUNC(boolean, SUM_SOH_CODE) SOH_Filter(SOH_DetectorType const* item);
FUNC(void, SUM_SOH_CODE) SOH_CheckConditionAndWrite(SOH_DetectorType const* item);
FUNC(void, SUM_SOH_CODE) SOH_Event_Detector(SOH_DetectorType const* item);
#endif

FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_Write(SOH_DetectorType const* item);
FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_ClearItems(SOH_DetectorType* const items[], uint8 length);
FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_CheckNvMWritesList(SOH_DetectorType* const items[], uint8 length);
FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_CheckNvMWrite(SOH_DetectorType const* item);


/*!
 *	This function is called only by Runnable_SOH_Timing(), after having read the OsCounter.
 *	Here, that counter value is used to update the time windows of the detectors that adopt the Time Window Filter.
 *
 *	\Requirement{034}
 *		If the previous counter value has not been initialized (is 0 or lower than the current counter value), it assumes the current
 *		counter value, so the time elapsed will result lower than 1s and the time window will not be updated.
 *	\Satisfies{025}
 *
 *	\Requirement{018}
 *		If a detector adopts the Time Window filter, the field SOH_Filter in its SOH_DetectorType instance is equal to SOH_TWFiltering.
 *	\Satisfies{014}
 *	\Requirement{019}
 *		If field SOH_Cntr in the detector's SOH_DetectorType instance is different from 0, it means (at least) the first error occurrence
 *		is detected.
 *		For a detector that adopts the Time Window filter, if the first error occurrence is detected, the field SOH_TW in the detector's
 *		SOH_DetectorType instance can be incremented (if time elapsed is greater than 1s). The field SOH_TW implements the time window for
 *		the relative detector.
 *	\Satisfies{014} \Satisfies{017}
 *
 *	If field SOH_TW reaches the relative threshold in field SOH_TWThreshold:
 *	\Requirement{029}
 *		- SOH_Cntr goes to 0
 *	\Satisfies{016}
 *	\Requirement{030}
 *		- SOH_TW goes to 0.
 *	\Satisfies{016}
 */
#if(NUM_OF_SOH_DETECTORS != 0u)
FUNC(void, SUM_SOH_CODE) SOH_UpdateTimeWindows(uint32 counterValue);
#endif

#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
/*!
 *	This function is called only by Runnable_SOH_Timing() and ShutdownNotificationToSoh(), after having read the OsCounter.
 *	Here, that counter value is used to update SUM_SOH_ShutDown_Timer of the Shutdown Delay Detector.
 *
 *	\Requirement{035}
 *		If the previous counter value has not been initialized yet, it assumes the current counter value.
 *	\Satisfies{027}
 *	\Requirement{036}
 *		Otherwise, the difference between the two values is converted in seconds and the previous counter value is incremented by a number
 *		of ticks equal to 1s till the difference stops being greater than 1s.
 *	\Satisfies{027}
 *
 *	\Requirement{031}
 *		If a shutdown has been detected by the Main Runnable, the IRV ShutdownTimerFlag is TRUE.
 *	\Satisfies{005}
 *
 *	\Requirement{032}
 *		If SUM_SOH_ShutDown_Timer reaches SUM_SOH_ShutDown_Timer_Thresh, SOH_Event_Detector() is called to increment SOH_Event_Detector and
 *		to call the respective filter.
 *	\Satisfies{024} \Satisfies{026}
 *
 */
FUNC(void, SUM_SOH_CODE) SOH_MonitorShutDownTimer(uint32 counterValue);
extern uint16			 SUM_SOH_ShutDown_Timer;

#endif //SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED

extern CONST(Os_CounterType, OS_CONST_FAST) Os_const_counters[];


#define SUM_SOH_STOP_SEC_CODE
#include "SUM_SOH_MemMap.h"

/* END OF PROTOTYPES */

#endif
/********************************** END OF FILE ******************************************************/

/*! @} */
