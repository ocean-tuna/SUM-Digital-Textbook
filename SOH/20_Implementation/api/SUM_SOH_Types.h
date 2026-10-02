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
 * Description : Types Header for SOH
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

/*! @defgroup SUM_SOH_Types_h The SUM_SOH_Types.h file
 *  This is the SUM_SOH_Types.h file
 *  @{
 */

#ifndef SUM_SOH_API_SUM_SOH_TYPES_H_
#define SUM_SOH_API_SUM_SOH_TYPES_H_

#include "Std_Types.h"
#include "Rte_SUM_SOH_Type.h"
#include "SUM_SOH_PBCfg.h"

#define SOH_PERMITEXECUTION     0xFFu
#define SOH_PERMITSHUTDOWN      0xFFu
#define SOH_SHUTDOWN_CNT_LIMIT 4u

#ifndef SOH_E_PENDING
#define SOH_E_PENDING (10u)
#endif

typedef struct
{
    /* Pointer to the RAM block */
    P2VAR(uint16, TYPEDEF, RTE_APPL_DATA) NvmRamBlock;
    /* Function pointer to the NvM_GetErrorStatus RTE API */
    P2FUNC(Std_ReturnType, TYPEDEF, SOH_RteCallNvmGetErrorStatus_fp) (CONSTP2VAR(NvM_RequestResultType, AUTOMATIC, RTE_APPL_DATA) RequestResultPtr);
    /* Function pointer to the NvM_SetRamBlockStatus RTE API */
    P2FUNC(Std_ReturnType, TYPEDEF, SOH_RteCallNvmSetRamBlockStatus_fp) (VAR(boolean, AUTOMATIC) BlockChanged);
    /* Function pointer to the NvM_WriteBlock RTE API */
    P2FUNC(Std_ReturnType, TYPEDEF, SOH_RteCallNvmWriteBlock_fp) (VAR(NvM_Rb_ConstVoidPtr, AUTOMATIC) SrcPtr);

} SOH_GenericNvmAccessType;

typedef enum{

	SOH_ThresholdFiltering,
	SOH_TWFiltering,
	SOH_ERRHImplementsFilter

}SOH_FilterType;

/*[$Satisfies $SDD_SOH 057]*/
typedef enum{

	SOH_RAM,
	SOH_NonClearedRAM,
	SOH_AdaptiveNvM,
	SOH_SecureCumulativeNvM,
	SOH_ERRHDefinesNvM

}SOH_StorageType;

/*!
 *	To make each Detector work with the Time Window Filter, this struct has the following fields:
 *	\Requirement{009}
 *		a 2-byte unsigned integer used as detection counter to maintain a raw count of the detected events
 *	\Satisfies{008}
 *	\Requirement{010}
 *		a 4-byte unsigned integer used as time window in seconds
 *	\Satisfies{008}	\Satisfies{010}
 *	\Requirement{011}
 *		a 2-byte unsigned integer used as DID counter to maintain a filtered count of the detected events
 *	\Satisfies{008}
 *	\Requirement{012}
 *		a counter threshold calibration value associated with the detection counter
 *	\Satisfies{009}
 *	\Requirement{014}
 *		a time window threshold calibration value associated with the time window.
 *	\Satisfies{010}
 *
 *	Moreover, the variables related to the Time Window Filter respect the following memory requirements:
 *	\Requirement{013}
 *		- each detection counter is stored in a specific memory type according to the table in the ADS, for the 16-bit variables
 *	\Satisfies{009} \Satisfies{012}
 *	\Requirement{015}
 *		- each time window is stored in the same memory type of the related detection counter, but for the 32-bit variables
 *	\Satisfies{010}
 *	\Requirement{016}
 *		- each DID counter is stored in secure cumulative NVM, for the 16-bit variables.
 *	\Satisfies{011}
 *
 *	\Requirement{017}
 *		At ECU production, only the detection counters (and the relative time windows) to be stored in RAM are guaranteed to be 0:
 *		So, all the DID counters and the other counters (with the relative time windows) require specific integration operations.
 *	\Satisfies{013}
 */
/* [$Satisfies $SDD_SOH 011]*/
typedef struct{
	/* [$Satisfies $DDS_SOH 009] */
	uint16   *SOH_Cntr;
	/* [$Satisfies $DDS_SOH 010] */
	uint32   *SOH_TW;
	/* [$Satisfies $DDS_SOH 011] */
	uint16   *SOH_DIDCntr;
	/* [$Satisfies $DDS_SOH 012] */
	P2FUNC(uint16, TYPEDEF, SOH_CntrThreshold) (void);
	/* [$Satisfies $DDS_SOH 014] */
	P2FUNC(uint32, TYPEDEF, SOH_TWThreshold) (void);
	uint32   *SOH_PrevCounterValue;
}SOH_ThresholdType;

typedef struct{

	SOH_FilterType  SOH_Filter;
	SOH_StorageType SOH_Storage;
	SOH_ThresholdType  SOH_TresholdItem;
	SOH_GenericNvmAccessType NvMCtr;
	SOH_GenericNvmAccessType NvMDID;

}SOH_DetectorType;

typedef struct{

	SOH_DetectorType** SOH_Detectors;
	uint8 length;

}SOH_MonitorType;

#endif /* SUM_SOH_API_SUM_SOH_TYPES_H_ */

/*! @} */

