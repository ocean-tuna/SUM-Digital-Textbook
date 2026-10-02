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
 * Description : SUM_DIAG header types
 ***************************************************************************************************
 * END:Banner
 ***************************************************************************************************
 */
#ifndef SUM_DIAG_TYPES_H
#define SUM_DIAG_TYPES_H

#include "Std_Types.h"
#include "Rte_SUM_DIAG_Type.h"
#include "SUM_Common_Types.h"

/* SUSD shutdown defines, SUSD_UserRequestType */
#define DIAG_MIN_ROLLING_COUNT        0u
#define DIAG_MAX_ROLLING_COUNT        3u
#define DIAG_ENABLE_SHUTDOWN          0xFF
/* Bit 6 of SUSD PermitExecution byte; SUSD clears this bit when ECUReset ($11/$01) is received */
#define DIAG_ECURESET_BIT_MASK        0x40u

/* internal states for ecu power mode */
#define DIAG_START_OF_CYCLE            0u
#define DIAG_SLEEP                    1u
#define DIAG_NORMAL_OPERATION          2u

/* internal states for MEC Write */
#define DIAG_MEC_DEFAULT           0u
#define DIAG_MEC_PENDING           1u

/* internal states for OTA Write */
#define DIAG_OTA_DEFAULT           0u
#define DIAG_OTA_PENDING           1u

/* Operation status pending */
#define DIAG_E_PENDING 0x0Au

/* [$Satisfies $SDD_DIAG 130] HSM Secure Data Store constants (CYS4008 Section 9) */
#define HSM_MEC_PARAM_ID              ((uint8)0u)
#define HSM_MEC_SECURE_DATA_TYPE      ((uint8)4u)
#define HSM_MEC_INPUT_SIZE            ((uint8)1u)

/* HSM MEC Read status codes (CYS2310 Section 7.5) */
#define HSM_SP_COMMAND_STATE_SUCCESS  ((uint8)0x03u)
#define HSM_SP_READ_STATUS_OK         ((uint8)0x00u)
#define HSM_SP_READ_STATUS_INVALID_ID ((uint8)0x02u)
#define HSM_SP_COMMAND_STATE_PENDING  ((uint8)0x01u)
#define HSM_SP_SET_STATUS_PENDING     ((uint8)0xFFu)

/* HSM MEC Write state machine states (CYS4008 Steps A-L) */
#define DIAG_HSM_MEC_WRITE_IDLE          0u
#define DIAG_HSM_MEC_WRITE_SET_PENDING   1u
#define DIAG_HSM_MEC_WRITE_POLL          2u
#define DIAG_HSM_MEC_WRITE_SUCCESS       3u
#define DIAG_HSM_MEC_WRITE_FAIL          4u

/* HSM MEC Write retry limit (CYS4008 Step A) */
#define DIAG_HSM_MEC_WRITE_RETRY_MAX     ((uint8)2u)

/* NRC for HSM write failure (CYS4008 Step L) */
#define DCM_E_GENERALPROGRAMMINGFAILURE  ((uint8)0x72u)

/* HSM MEC availability status */
#define DIAG_HSM_MEC_AVAILABLE           ((uint8)0u)
#define DIAG_HSM_MEC_UNAVAILABLE         ((uint8)1u)

/* UDS diagnostic service IDs */
/*[$Satisfies $SDD_DIAG 018]*/
typedef enum {
    DiagnosticSessionControl          = 0x10,
    ECUReset                          = 0x11,
    SecurityAccess                    = 0x27,
    CommunicationControl              = 0x28,
    TesterPresent                     = 0x3E,
    AccessTimingParameter             = 0x83,
    SecuredDataTransmission           = 0x84,
    ControlDTCSetting                 = 0x85,
    ResponseOnEvent                   = 0x86,
    LinkControl                       = 0x87,
    ReadDataByIdentifier              = 0x22,
    ReadMemoryByAddress               = 0x23,
    ReadScalingDataByIdentifier       = 0x24,
    ReadDataByPeriodicIdentifier      = 0x2A,
    DynamicallyDefineDataIdentifier   = 0x2C,
    WriteMemoryByAddress              = 0x3D,
    ClearDiagnosticInformation        = 0x14,
    ReadDTCInformation                = 0x19,
    InputOutputControlByIdentifier    = 0x2F,
    RoutineControl                    = 0x31,
    RequestDownload                   = 0x34,
    RequestUpload                     = 0x35,
    TransferData                      = 0x36,
    RequestTransferExit               = 0x37,
    RequestFileTransfer               = 0x38
} UDSDiagnosticServiceIds;

/* UDS sub functions */
/*[$Satisfies $SDD_DIAG 019]*/
typedef enum {
    SubFunction_00                    = 0x00,
    SubFunction_01                    = 0x01,
    SubFunction_02                    = 0x02,
    SubFunction_03                    = 0x03,
    SubFunction_04                    = 0x04,
    SubFunction_05                    = 0x05
} SubFunctions;

/* Diagnostic request types */
#define PHYSICAL_REQUEST            0u
#define FUNCTIONAL_REQUEST          1u

/* Diagnostic RequestData array indexes, note, not as expected, requestData parameter *
 * that is passed in points to the byte AFTER the SID so indexes and data sizes       *
 * (below) have been adjusted, to address OI_126                                      *
 */

#define IDX_SUBFUNCTION              			0u    // UDS spec for SIDs with sub functions
#define IDX_SUBFUNCTION_IO           			2u    // UDS spec for SIDs with sub functions
#define DIAG_SUPPRESSPOSRSPMSGINDICATIONBIT_MASK 	0x7Fu

/* minimum RequestData data size, note, to address OI_126 */
#define DIAG_SESSION_CTRL_MIN_DATASIZE    1u
#define ECU_RESET_MIN_DATASIZE            1u
#define COMM_CTRL_MIN_DATASIZE            1u
#define DYN_DEFINE_DATA_ID_MIN_DATASIZE   3u

/* BswM mode reads and SSM write */
/*[$Satisfies $SDD_DIAG 021] [$Satisfies $SDD_DIAG 084] [$Satisfies $SDD_DIAG 085] [$Satisfies $SDD_DIAG 086] */
typedef struct
{
    SUM_DiagNormalCom_ModeType    CurrentMode;
    SUM_DiagNormalCom_ModeType    (*Rte_ReadFunc_BswMCommMode)(void);        /* Function pointer to Rte_Read_DIAG_BswM_CommunicationMode function call */
    Std_ReturnType                (*Rte_WriteFunc)(SUM_DiagNormalCom_ModeType CommMode);     /* Function pointer to Rte_Write_DIAG_DisableNormalComm function call */
} Diag_NormalCommStatus_Channel_Type;

#endif /* SUM_DIAG_TYPES_H */
