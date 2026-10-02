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
 * Description : LNM DcmServices
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
 */

/******************************************************************************
 * Header Includes *
 ******************************************************************************/
#include "Rte_SUM_LNM.h"
#include "SUM_LNM_Network_Management.h"
#include "SUM_LNM_Cfg.h"
#include "SUM_LNM_Types.h"

/*****************************************************************************************
 * Definition of Global Functions *
 *****************************************************************************************/

/***********************************************************
* Function name: LNM_DID_MSCL_ConditionCheckRead
* Description: Function that checks the conditions for DID F1BE
* Parameters in :
*   OpStatus -> Current status
*   ErrorCode -> Data Pointer where to store an error code
* Return value: RTE_E_OK
* Remarks for:
* Satisfies: [$SDD_LNM 050]
***********************************************************/
/*PRQA S 3432 3 # This is due to the testing project, not part of the module itself*/
/*PRQA S 1503 1 # Runnable implementation to be called by another module*/
FUNC(Std_ReturnType, SUM_LNM_CODE) LNM_DID_MSCL_ConditionCheckRead(VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
                                                                   P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode)
{
    VAR(Std_ReturnType,AUTOMATIC) ret = LNM_E_OK;

    switch (OpStatus)
    {
        case DCM_INITIAL:
        case DCM_PENDING:
            *ErrorCode = 0;
            break;

        default:
        	/*Nothing to do */
            break;
    }

    return ret;
}

/***********************************************************
* Function name: LNM_DID_MSCL_ReadData
* Description: Function that sends data for DID F1BE
* Parameters in:
*   OpStatus -> Current status
*   Data -> Data Pointer where to store DIF F1BE content
* Return value: RTE_E_OK
* Remarks for:
* Satisfies: [$SDD_LNM 050]
***********************************************************/
/*PRQA S 3432 3 # This is due to the testing project, not part of the module itself*/
/*PRQA S 1503 1 # Runnable implementation to be called by another module*/
FUNC(Std_ReturnType, SUM_LNM_CODE) LNM_DID_MSCL_ReadData(VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
                                                         P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Data)
{
    VAR(Std_ReturnType,AUTOMATIC) ret = LNM_E_OK;
    VAR(uint8,AUTOMATIC) i;
    VAR(uint8,AUTOMATIC) DID_index;

    switch (OpStatus)
    {
        case DCM_INITIAL:
        case DCM_PENDING:
            /* Initialize all LIN channels to 0 */
            for(i = 0; i < LNM_DID_F1BE_LENGTH; i++)
            {
                Data[i] = LNM_ZERO;
            }

            /* Set the MSCL for configured channels */
            for(i = 0; i < LNM_NUMBER_OF_NETWORKS; i++)
            {
                /* Compute DID Index. Index conversion from 1-16 to 0-15. Takes into account MSCL is uint16 */
                DID_index = (LNM_Network_Info[i].LNM_Network_ID - 1u)*2u;

                /* Populate data for write. Takes into account conversion from uint16 to uint8 */
                Data[DID_index] = (uint8) (LNM_Network_Status[i].MSCL >> LNM_BYTE_SHIFT);
                Data[DID_index+1u] = (uint8) LNM_Network_Status[i].MSCL;
            }
            break;

        default:
        	/*Nothing to do */
            break;
    }

    return ret;
}

