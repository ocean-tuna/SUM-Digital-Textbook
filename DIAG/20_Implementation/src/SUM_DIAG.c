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
 * Description : SUM_DIAG main function
 ***************************************************************************************************
 * END:Banner
 ***************************************************************************************************
 */
 

#include "SUM_Diag.h"
#include "Rte_SUM_DIAG.h"
#include "SUM_DIAG_Ram.h"

#define SUM_DIAG_LEGISLATED_TESTER_SOURCE_ADDRESS    ((uint16)0x0E00u)
/*
 * Local static variables and functions *
 */
static SUM_VehiclePowerMode_Signal_States Off_power_mode;
static FUNC(uint8, RTE_CODE) getEcuAddress (void);
/* Function to write the MEC into NvM */
static FUNC(void, RTE_CODE) setManufacturerEnableCounter(VAR(uint8, AUTOMATIC) mec);
static FUNC(void, RTE_CODE) SetOTARamBlockStatus(VAR(DisableWrite, AUTOMATIC) isDisabledNvmWrite);
static boolean SUM_DIAG_IsProtocolTypeValid(Dcm_ProtocolType protocolType);

#if (MEC_STORAGE_IN_HSM == STD_ON)
/* [$Satisfies $SDD_DIAG 135] Timeout limit in MEC Handler ticks for HSM write polling */
#define DIAG_HSM_MEC_WRITE_TIMEOUT_TICKS  ((uint16)(HSM_MEC_WRITE_TIMEOUT_MS / DIAG_MEC_HANDLER_PERIOD_MS))
#endif
/*
 ********************  Local Functions ********************
 */

/***********************************************************************************

  Function name :   check_offpowermode
  Description   :   read power mode and check for off mode
  Parameter in  :   None
  Parameter out :   None
  Return value  :   TRUE or FALSE
  Remarks       :   [$Satisfies $SDD_DIAG 016][$Satisfies $SDD_DIAG 036] [$Satisfies $SDD_DIAG 098] 
************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"  /* PRQA S 5087 EOF */ /* include SUM_DIAG_MemMap.h required here for memory mapping */
FUNC(boolean, RTE_CODE) check_offpowermode (void) /* PRQA S 1532 EOF # The functions are invoked by runnables or other functions in external file */
{
    boolean  bret = FALSE;

    /* read the vehicle power mode from SUSD */
    Off_power_mode = Rte_DRead_SUSD_Vehicle_Power_Mode_VehiclePowerMode();

    if (Off_power_mode == OFF)
    {
        bret = TRUE;
    }
    else
    {
      /*Do Nothing*/
    }

    return bret;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   SUM_DIAG_IsProtocolTypeValid
  Description   :   Validate if the given protocol type is supported for pre-emption checking
  Parameter in  :   Dcm_ProtocolType protocolType
  Parameter out :   None
  Return value  :   boolean (TRUE if valid, FALSE otherwise)
  Remarks       :   Supported protocols: DCM_OBD_ON_CAN, DCM_OBD_ON_IP, DCM_SUPPLIER_1, DCM_SUPPLIER_2
 ************************************************************************************/
static boolean SUM_DIAG_IsProtocolTypeValid(Dcm_ProtocolType protocolType)
{
    boolean isValid = FALSE;

    switch (protocolType)
    {
        case DCM_OBD_ON_CAN:
        case DCM_OBD_ON_IP:
        case DCM_SUPPLIER_1:
        case DCM_SUPPLIER_2:
            isValid = TRUE;
            break;
        default:
            /* No action */
            break;
    }

    return isValid;
}

/***********************************************************************************

  Function name :   SUM_DIAG_DcmGetRxPermission
  Description   :   Determine if protocol pre-emption is allowed for an incoming request
  Parameter in  :   Dcm_ProtocolType ProtocolType, PduIdType PduId
  Parameter out :   None
  Return value  :   Std_ReturnType (E_OK allow, E_NOT_OK block)
  Remarks       :   Pre-emption is blocked only for legislated tester (0x0E00) on OBD/ZEV protocols
                    when vehicle power mode is OFF and PduId is valid.
                    [$Satisfies $SDD_DIAG 020][$Satisfies $SDD_DIAG 117][$Satisfies $SDD_DIAG 118]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
/* PRQA S 1503 1 # Invoked by DCM configuration externally even if not referenced in local TU */
Std_ReturnType SUM_DIAG_DcmGetRxPermission(
    Dcm_ProtocolType ProtocolType,
    PduIdType PduId)
{
    SUM_VehiclePowerMode_Signal_States vehiclePowerMode;
    uint16 testerSourceAddress = 0u;
    Std_ReturnType retTesterSourceAddr;
    Std_ReturnType retValue = E_OK;
    boolean isProtocolValid = SUM_DIAG_IsProtocolTypeValid(ProtocolType);

    if (isProtocolValid == TRUE)
    {

        retTesterSourceAddr = Dcm_GetTesterSourceAddress(PduId, &testerSourceAddress);

        if ((retTesterSourceAddr == E_OK) &&
            (testerSourceAddress == SUM_DIAG_LEGISLATED_TESTER_SOURCE_ADDRESS))
        {

            vehiclePowerMode = Rte_DRead_SUSD_Vehicle_Power_Mode_VehiclePowerMode();

            if (vehiclePowerMode == OFF)
            {
                retValue = E_NOT_OK;
            }
        }
    }

    return retValue;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   setTesterAddress
  Description   :    store the tester source address and subnetID from the source address
                    in NvM
  Parameter in  :   uint16 sourceAddress
  Parameter out :   None
  Return value  :   None
  Remarks       :   [$Satisfies $SDD_DIAG 008][$Satisfies $SDD_DIAG 039]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(void, RTE_CODE) setTesterAddress (VAR(uint16, AUTOMATIC) sourceAddress)
{
    /* Store full 2-byte address to support both CAN (1 byte) and ETH (2 bytes) */
    /* PRQA S 3432 2 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */
    /* PRQA S 3469 1 # This macro is needed to access the Pim as per RTE definition. */
    *Rte_Pim_TesterSourceAddress() = sourceAddress;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"


/***********************************************************************************

  Function name :   getTesterAddress
  Description   :    get the tester source address from memory
  Parameter in  :   None
  Parameter out :   None
  Return value  :   uint8 address
  Remarks       :   [$Satisfies $SDD_DIAG 009]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
/* PRQA S 1503 1 # This object might be used in the future implementation */
FUNC(uint16, RTE_CODE) getTesterAddress (void)
{
	/* PRQA S 3432 2 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */
    /* PRQA S 3469 1 # Calling functions free from side effects. */
    return *Rte_Pim_TesterSourceAddress();
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"


/***********************************************************************************

  Function name :   setManufacturerEnableCounter 
  Description   :    store the manufacturer enable counter in memory
  Parameter in  :   uint8 mec
  Parameter out :   None
  Return value  :   None
  Remarks       :   [$Satisfies $SDD_DIAG 010]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
static FUNC(void, RTE_CODE) setManufacturerEnableCounter (VAR(uint8, AUTOMATIC) mec)
{
    *MEC_RamBlockPtr = mec;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   getManufacturerEnableCounter 
  Description   :    get the manufacturer enable counter from memory
  Parameter in  :   None
  Parameter out :   None
  Return value  :   uint8 mec
  Remarks       :   [$Satisfies $SDD_DIAG 011]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(uint8, RTE_CODE) getManufacturerEnableCounter (void)
{
    return *MEC_RamBlockPtr;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   getEcuAddress 
  Description   :    get the configured ecu address from memory
  Parameter in  :   None
  Parameter out :   None
  Return value  :   uint8 EcuAddress
  Remarks       : [$Satisfies $SDD_DIAG 012]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
static FUNC(uint8, RTE_CODE) getEcuAddress (void)
{
    return DIAG_ECUADDRESS;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   setOtaPsf
  Description   :   store the OTA PSF in memory
  Parameter in  :   boolean OTA_PSF_Value
  Parameter out :   None
  Return value  :   None
  Remarks       :   [$Satisfies $SDD_DIAG 013]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
/* PRQA S 3432 1 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */
FUNC(void, RTE_CODE) setOtaPsf (VAR(boolean, AUTOMATIC) OTA_PSF_Value)/* PRQA S 1532 EOF # This not part of the DIAG Module. The Owner of this Macro is the RTE. */
{
    *OTA_PSF_RamBlockPtr = OTA_PSF_Value;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   getOtaPsf
  Description   :   get the OTA PSF from memory
  Parameter in  :   None
  Parameter out :   None
  Return value  :   boolean OTA_PSF
  Remarks       :   [$Satisfies $SDD_DIAG 014]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
/* PRQA S 3432 1 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */
FUNC(boolean, RTE_CODE) getOtaPsf (void)
{
    return *OTA_PSF_RamBlockPtr;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   SetOTARamBlockStatus
  Description   :   setsRamBlockStatus 
  Parameter in  :   DisableWrite isNvMWriteDisabled, Std_ReturnType nVMWriteStatus
  Parameter out :   None
  Return value  :   None
  Remarks       :   [$Satisfies $SDD_DIAG 013]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
/* PRQA S 3432 1 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */

static FUNC(void, RTE_CODE) SetOTARamBlockStatus(VAR(DisableWrite, AUTOMATIC) isDisabledNvmWrite)
{
    if (isDisabledNvmWrite == FALSE)
    {
        (void)Rte_Call_RP_NvMService_OTABlock_SetRamBlockStatus(TRUE);
    }
    else
    {
        /*no need to set ram block status because NvMWrite is disabled*/
    }
}

/***********************************************************************************

  Function name :   initializeForRun 
  Description   :   initialize the output ports and variables stored in memory needed
                    for module execution
  Parameter in  :   None
  Parameter out :   None
  Return value  :   None
  Remarks       :   [$Satisfies $SDD_DIAG 007] [$Satisfies $SDD_DIAG 084] 
[$Satisfies $SDD_DIAG 085] [$Satisfies $SDD_DIAG 086] [$Satisfies $SDD_DIAG 087] [$Satisfies $SDD_DIAG 089]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(void, RTE_CODE) initializeForRun (void)
{
    uint8 index;
    Diag_NormalCommStatus_Channel_Type const  *commStatus_channel;
    uint8 ecuAddress;

    /* initialize ecu reset request output port to FALSE, i.e., no reset request has been received */
    (void)Rte_Write_SUM_DIAG_DIAG_ECUResetRequest_DiagResetRequest(FALSE);

    /* initialize any configured DIAG_DisableNormalComm output ports */
    for (index = 0; index < DIAG_NUMBER_OF_CHANNELS; index++)
    {
        /* pointer to port/channel info */
        commStatus_channel = &Diag_NormalCommStatus_Channel[index];

        if (commStatus_channel->Rte_WriteFunc != NULL_PTR)
        {
            /* init the output port for this channel */
            (void)commStatus_channel->Rte_WriteFunc(commStatus_channel->CurrentMode);
        }
    }

    /* initialize the ecuaddress output port with configured value in memory */
    ecuAddress = getEcuAddress();
    (void)Rte_Write_DIAG_GetEcuAddress_EcuAddress(ecuAddress);

    /* initialize the tester source address and subnetID stored in NvM */
    setTesterAddress(0u);

    /* initialize Off and Run power modes */
    Off_power_mode = OFF;    /* power mode is currently off */
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/*
 ********************  Callouts  ********************
 */

/***********************************************************************************

  Function name :   Diag_SetProgConditions
  Description   :   Function to store tester address in NvM and to be included into Dcm_SetProgConditions callout by the integrator.
  Parameter in  :   Pointer to Dcm_ProgConditionsType structure
  Parameter out :   None
  Return value  :   E_OK
  Remarks       :   The function prototype is defined in DcmAppl.h,
                    to enable the function prototype, configuration parameter DcmStoringEnabled
                    is set to TRUE which sets DCM_CFG_STORING_ENABLED to DCM_CFG_ON in the
                    generated DcmCore_DslDsd_Cfg.h file.
                    DcmProgCondtionsType is defined in DcmCore_DslDsd_Pub.h
                    [$Satisfies $SDD_DIAG 017][$Satisfies $SDD_DIAG 040][$Satisfies $SDD_DIAG 088] [$Satisfies $SDD_DIAG 094]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
/* PRQA S 1503 EOF */ /* This function is called by DCM module at the integration level. */
FUNC(Std_ReturnType,DCM_APPL_CODE) Diag_SetProgConditions (VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
														   P2CONST(Dcm_ProgConditionsType, AUTOMATIC, DCM_INTERN_DATA) ProgConditions)
{
	Std_ReturnType Ret = E_NOT_OK;
	boolean Initreflashflag;

	Std_ReturnType status;

	/* Set the value of the InitialReflashFlag to True and update it in the NVM through SUM_SSC */
	Initreflashflag = TRUE;

	if(DCM_INITIAL == OpStatus)
	{
		setTesterAddress(ProgConditions->TesterSourceAddr);
		(void)Rte_Write_DIAG_SetInitialReflashFlag_DE_InitialReflashFlag(Initreflashflag);

		/* Let DCM call Diag_SetProgConditions() in next DCM cycle to check storage status in application */
		Ret = DIAG_E_PENDING;
	}
	else if(DCM_PENDING == OpStatus)
	{
		(void)Rte_Call_RP_DIAG_SSC_InitialReflashFlag_WrtStatus_WrtStatus(&status);
		Ret = status;
	}
	else
	{
		/* DCM_CANCEL */
	}

	return Ret;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************

  Function name :   ProcessMECWrite
  Description   :   FSM to write the MEC in the NvM
  Parameter in  :   uint8 * mecValue
  Parameter out :   None
  Return value  :   Std_ReturnType status
  Remarks       :   [$Satisfies $SDD_DIAG 072] [$Satisfies $SDD_DIAG 090][$Satisfies $SDD_DIAG 092]
  [$Satisfies $SDD_DIAG 104] [$Satisfies $SDD_DIAG 105] [$Satisfies $SDD_DIAG 106]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) ProcessMECWrite (uint8 const * const mecValue)
{
    Std_ReturnType status = E_OK;
    DisableWrite  isDisabledNvmWrite;
    

    switch (SUM_DIAG_MEC_Write_State)
    {
        case DIAG_MEC_DEFAULT:
            if (mecValue != NULL_PTR)
            {
                /* writes MEC value to mirror RAM block */
                setManufacturerEnableCounter(*mecValue);
                /* Writes the MEC value to output port */
                (void)Rte_Write_PP_DIAG_SSC_GetMEC_GetMec(*mecValue);
                (void)Rte_Write_PP_DIAG_SOH_GetMEC_GetMec(*mecValue);
                (void)Rte_Write_PP_DIAG_SSFM_GetMEC_GetMec(*mecValue);
                /*Check if the battery voltage level is on range from SUSD (FALSE = voltage level ok, TRUE otherwise)*/
                (void) Rte_Read_RP_SUSD_DIAG_BatteryVoltage_DisableNVMWrite_DisableWriteData(&isDisabledNvmWrite);

                /* PRQA S 3415 1 # side effects can be ignored as if the isDisabledNvmWrite is TRUE, no need to write to the NVM*/
                if ((isDisabledNvmWrite == FALSE) && (Rte_Call_RP_NvMService_MECBlock_WriteBlock(mecValue) == RTE_E_OK))
                {
                    SUM_DIAG_MEC_Write_State = DIAG_MEC_PENDING;
                    status = DIAG_E_PENDING;
                }
                else
                {
                    if(isDisabledNvmWrite == FALSE)
                    {
                        (void)Rte_Call_RP_NvMService_MECBlock_SetRamBlockStatus(TRUE);
                    }
                    SUM_DIAG_MEC_Write_State = DIAG_MEC_DEFAULT;
                    status = E_NOT_OK;
                }
            }
            else
            {
                status = E_NOT_OK;
            }
            break;
        case DIAG_MEC_PENDING:
            /* check that write to RAM block is successful */
            status = CheckMECWrite();

            if (mecValue != NULL_PTR)
            {
                /* writes MEC value to mirror RAM block */
                setManufacturerEnableCounter(*mecValue);
                /* Writes the MEC value to output port */
                (void)Rte_Write_PP_DIAG_SSC_GetMEC_GetMec(*mecValue);
                (void)Rte_Write_PP_DIAG_SOH_GetMEC_GetMec(*mecValue);
                (void)Rte_Write_PP_DIAG_SSFM_GetMEC_GetMec(*mecValue);
                /*Check if the battery voltage level is on range from SUSD (FALSE = voltage level ok, TRUE otherwise)*/
                (void) Rte_Read_RP_SUSD_DIAG_BatteryVoltage_DisableNVMWrite_DisableWriteData(&isDisabledNvmWrite);

                /* Writes the MEC value on the NvM, if the CheckMECWrite returns a status different than Pending. */
                if (status != DIAG_E_PENDING)
                {   
                    /* PRQA S 3415 1 # side effects can be ignored as if the isDisabledNvmWrite is TRUE, no need to write to the NVM*/
                    if ((isDisabledNvmWrite == FALSE) && (Rte_Call_RP_NvMService_MECBlock_WriteBlock(mecValue) == RTE_E_OK))
                    {
                        SUM_DIAG_MEC_Write_State = DIAG_MEC_PENDING;
                        status = DIAG_E_PENDING;
                    }
                    else
                    {
                        if(isDisabledNvmWrite == FALSE)
                        {
                            (void)Rte_Call_RP_NvMService_MECBlock_SetRamBlockStatus(TRUE);
                        }
                        SUM_DIAG_MEC_Write_State = DIAG_MEC_DEFAULT;
                        status = E_NOT_OK;
                    }
                }
            }
            break;
        default:
            /* Do nothing */
            break;

    }

    return status;
}/* PRQA S 6004 1 # Reducing the Cyclomatic Complexity would impact on function readability. */


/***********************************************************************************
  Function name :   CheckMECWrite
  Description   :   Check the write of the MEC in the NvM
  Parameter in  :   None
  Parameter out :   None
  Return value  :   Std_ReturnType status
  Remarks       :   [$Satisfies $SDD_DIAG 072]
 ************************************************************************************/
FUNC(Std_ReturnType, RTE_CODE) CheckMECWrite (void)
{
    Std_ReturnType status;
    Std_ReturnType ReturnStatus;
    NvM_RequestResultType NvmErrorStatus;

    /* check that write to RAM block is successful */
    ReturnStatus = Rte_Call_RP_NvMService_MECBlock_GetErrorStatus(&NvmErrorStatus);

    if (ReturnStatus == E_OK)
    {
        switch (NvmErrorStatus)
        {
            case NVM_REQ_OK:
                SUM_DIAG_MEC_Write_State = DIAG_MEC_DEFAULT;
                status = E_OK;
                break;
            case NVM_REQ_PENDING:
                status = DIAG_E_PENDING;
                break;
            default:
                SUM_DIAG_MEC_Write_State = DIAG_MEC_DEFAULT;
                status = E_NOT_OK;
                break;
        }
    }
    else
    {
        SUM_DIAG_MEC_Write_State = DIAG_MEC_DEFAULT;
        status = E_NOT_OK;
    }

    return status;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"


/***********************************************************************************

  Function name :   ProcessOTAWrite
  Description   :   FSM to write the OTA_PSF in the NvM
  Parameter in  :   None
  Parameter out :   None
  Return value  :   Std_ReturnType status
  Remarks       :   [$Satisfies $SDD_DIAG 029]  [$Satisfies $SDD_DIAG 103]
 ************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) ProcessOTAWrite (void)
{
    Std_ReturnType status = E_OK;
    Std_ReturnType ReturnStatus;
    NvM_RequestResultType NvmErrorStatus;
    DisableWrite  isDisabledNvmWrite;
    /*Check if the battery voltage level is on range from SUSD (FALSE = voltage level ok, TRUE otherwise)*/
    (void) Rte_Read_RP_SUSD_DIAG_BatteryVoltage_DisableNVMWrite_DisableWriteData(&isDisabledNvmWrite);
    
    switch (SUM_DIAG_OTA_Write_State)
    {
        case DIAG_OTA_DEFAULT:
        	/* PRQA S 3432 3 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */
            /* PRQA S 3415 1 # side effects can be ignored as if the isDisabledNvmWrite is TRUE, no need to write to the NVM*/
            if ((isDisabledNvmWrite == FALSE) && (Rte_Call_RP_NvMService_OTABlock_WriteBlock((NvM_Rb_ConstVoidPtr) OTA_PSF_RamBlockPtr) == RTE_E_OK))
            {
                SUM_DIAG_OTA_Write_State = DIAG_OTA_PENDING;
                status = DIAG_E_PENDING;
            }
            else
            {
                SetOTARamBlockStatus(isDisabledNvmWrite);
                SUM_DIAG_OTA_Write_State = DIAG_OTA_DEFAULT;
                status = E_NOT_OK;
            }
            break;
        case DIAG_OTA_PENDING:
            /* check that write to RAM block is successful */
            ReturnStatus = Rte_Call_RP_NvMService_OTABlock_GetErrorStatus(&NvmErrorStatus);

            if (ReturnStatus == E_OK) 
            {
                switch (NvmErrorStatus)
                {
                    case NVM_REQ_OK:
                        SUM_DIAG_OTA_Write_State = DIAG_OTA_DEFAULT;
                        status = E_OK;
                        break;
                    case NVM_REQ_PENDING:
                        status = DIAG_E_PENDING;
                        break;
                    default:
                        SUM_DIAG_OTA_Write_State = DIAG_OTA_DEFAULT;
                        status = E_NOT_OK;
                        break;
                }
            }
            else 
            {
                SUM_DIAG_OTA_Write_State = DIAG_OTA_DEFAULT;
                status = E_NOT_OK;
            }

            if (status != DIAG_E_PENDING)
            {
            	/* PRQA S 3432 3 # Suppress violation warnings for MR12 20.7, see Compiler.h for details */
                /* PRQA S 3415 1 # side effects can be ignored as if the isDisabledNvmWrite is TRUE, no need to write to the NVM*/
                if ((isDisabledNvmWrite == FALSE) && (Rte_Call_RP_NvMService_OTABlock_WriteBlock((NvM_Rb_ConstVoidPtr) OTA_PSF_RamBlockPtr) == RTE_E_OK))
                {
                    SUM_DIAG_OTA_Write_State = DIAG_OTA_PENDING;
                    status = DIAG_E_PENDING;
                }
                else
                {
                    SetOTARamBlockStatus(isDisabledNvmWrite);
                    SUM_DIAG_OTA_Write_State = DIAG_OTA_DEFAULT;
                    status = E_NOT_OK;
                }
            }
            break;
        default:
            /* Do nothing */
            break;
    }

    return status;
}

#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"


#if (MEC_STORAGE_IN_HSM == STD_ON)

/***********************************************************************************
  Function name :   Diag_HSM_MEC_Read
  Description   :   Read MEC value from HSM Secure Data Store via LLSI
  Parameter in  :   None
  Parameter out :   mecValue - pointer to store MEC value
                    mecStatus - pointer to store SP_SECUREDATA_READSTORE_STATUS
  Return value  :   E_OK if read successful, E_NOT_OK otherwise
  Remarks       :   [$Satisfies $SDD_DIAG 132]
                    Implements CYS4008 read validation chain:
                    1. Check SP_COMMAND_STATE == 0x03
                    2. Check SP_SECUREDATA_READSTORE_STATUS == 0x00
                    3. Check byte count == 1
                    4. All OK -> use returned MEC value
************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_Read(P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) mecValue,
                                                  P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) mecStatus) /* PRQA S 3432 # Suppress violation warnings for MR12 20.7 */
{
    Std_ReturnType retValue = E_NOT_OK;
    uint8 commandState;
    uint8 readStatus;
    uint8 readValue;
    uint8 byteCount;

    /* Call HSM LLSI to read MEC from secure data store */
    (void)Rte_Call_RP_HSM_SecureDataStore_ReadHWIO_e_SP_SecureDataStore(
        HSM_MEC_PARAM_ID, &commandState, &readStatus, &readValue, &byteCount);

    /* CYS4008 Read Validation Chain */
    if (commandState == HSM_SP_COMMAND_STATE_SUCCESS)
    {
        /* Return SP_SECUREDATA_READSTORE_STATUS to caller (per SUMDIAG_79 / CYS2310) */
        *mecStatus = readStatus;

        if (readStatus == HSM_SP_READ_STATUS_OK)
        {
            if (byteCount == HSM_MEC_INPUT_SIZE)
            {
                *mecValue = readValue;
                retValue = E_OK;
            }
            else
            {
                *mecValue = 0u;
            }
        }
        else
        {
            *mecValue = 0u;
        }
    }
    else
    {
        /* SP_COMMAND_STATE not complete — return it so caller can distinguish from readStatus */
        *mecStatus = commandState;
        *mecValue = 0u;
    }

    return retValue;
}
#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************
  Function name :   Diag_HSM_MEC_WriteStart
  Description   :   Start HSM MEC Write state machine (CYS4008 Steps A-C)
  Parameter in  :   mecValue - value to write to HSM
  Parameter out :   None
  Return value  :   DIAG_E_PENDING if write started, E_NOT_OK on immediate failure
  Remarks       :   [$Satisfies $SDD_DIAG 133]
************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_WriteStart(VAR(uint8, AUTOMATIC) mecValue)
{
    Std_ReturnType retValue;
    uint8 commandState;
    uint8 setStatus;

    /* Step A: Set retry counter */
    SUM_DIAG_HSM_MEC_Write_RetryCounter = DIAG_HSM_MEC_WRITE_RETRY_MAX;

    /* Step B: Set write value */
    SUM_DIAG_HSM_MEC_Write_Value = mecValue;

    /* Step C: Call SetHWIO */
    (void)Rte_Call_RP_HSM_SecureDataStore_SetHWIO_e_SP_SecureDataStore(
        &SUM_DIAG_HSM_MEC_Write_Value, HSM_MEC_PARAM_ID,
        HSM_MEC_SECURE_DATA_TYPE, HSM_MEC_INPUT_SIZE);

    /* Step D: Check initial response */
    (void)Rte_Call_RP_HSM_SecureDataStore_GetHWIO_e_SP_SecureDataStoreSetStatus(
        &commandState, &setStatus);

    if ((commandState == HSM_SP_COMMAND_STATE_PENDING) && (setStatus == HSM_SP_SET_STATUS_PENDING))
    {
        /* Step E: Start timeout */
        SUM_DIAG_HSM_MEC_Write_TimeoutCounter = 0u;
        SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_POLL;
        retValue = DIAG_E_PENDING;
    }
    else
    {
        /* Step I: Check retry */
        if (SUM_DIAG_HSM_MEC_Write_RetryCounter > 0u)
        {
            SUM_DIAG_HSM_MEC_Write_RetryCounter--;
            /* Step J->C: Retry SetHWIO */
            (void)Rte_Call_RP_HSM_SecureDataStore_SetHWIO_e_SP_SecureDataStore(
                &SUM_DIAG_HSM_MEC_Write_Value, HSM_MEC_PARAM_ID,
                HSM_MEC_SECURE_DATA_TYPE, HSM_MEC_INPUT_SIZE);

            SUM_DIAG_HSM_MEC_Write_TimeoutCounter = 0u;
            SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_POLL;
            retValue = DIAG_E_PENDING;
        }
        else
        {
            /* Step L: Failure */
            SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_FAIL;
            retValue = E_NOT_OK;
        }
    }

    return retValue;
}
#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************
  Function name :   Diag_HSM_MEC_WritePoll
  Description   :   Poll HSM MEC Write state machine (CYS4008 Steps F-K)
  Parameter in  :   None
  Parameter out :   None
  Return value  :   E_OK on success, DIAG_E_PENDING if still in progress, E_NOT_OK on failure
  Remarks       :   [$Satisfies $SDD_DIAG 133]
                    Called periodically from Runnable_MEC_Handler when write is in progress
************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_WritePoll(void)
{
    Std_ReturnType retValue = DIAG_E_PENDING;
    uint8 commandState;
    uint8 setStatus;

    if (SUM_DIAG_HSM_MEC_Write_State != DIAG_HSM_MEC_WRITE_POLL)
    {
        if (SUM_DIAG_HSM_MEC_Write_State == DIAG_HSM_MEC_WRITE_SUCCESS)
        {
            SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_IDLE;
            retValue = E_OK;
        }
        else if (SUM_DIAG_HSM_MEC_Write_State == DIAG_HSM_MEC_WRITE_FAIL)
        {
            SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_IDLE;
            retValue = E_NOT_OK;
        }
        else
        {
            retValue = E_OK;
        }
    }
    else
    {
        /* Step F: Poll status */
        (void)Rte_Call_RP_HSM_SecureDataStore_GetHWIO_e_SP_SecureDataStoreSetStatus(
            &commandState, &setStatus);

        /* Step G: Check command state */
        if (commandState == HSM_SP_COMMAND_STATE_SUCCESS)
        {
            /* Step H: Check status */
            if (setStatus == HSM_SP_READ_STATUS_OK)
            {
                SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_IDLE;
                retValue = E_OK;
            }
            else
            {
                /* Step L: Failure */
                SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_IDLE;
                retValue = E_NOT_OK;
            }
        }
        else
        {
            /* Step K: Check timeout */
            SUM_DIAG_HSM_MEC_Write_TimeoutCounter++;
            if (SUM_DIAG_HSM_MEC_Write_TimeoutCounter >= DIAG_HSM_MEC_WRITE_TIMEOUT_TICKS)
            {
                /* Step I: Check retry counter */
                if (SUM_DIAG_HSM_MEC_Write_RetryCounter > 0u)
                {
                    /* Step J: Decrement and retry */
                    SUM_DIAG_HSM_MEC_Write_RetryCounter--;
                    (void)Rte_Call_RP_HSM_SecureDataStore_SetHWIO_e_SP_SecureDataStore(
                        &SUM_DIAG_HSM_MEC_Write_Value, HSM_MEC_PARAM_ID,
                        HSM_MEC_SECURE_DATA_TYPE, HSM_MEC_INPUT_SIZE);
                    SUM_DIAG_HSM_MEC_Write_TimeoutCounter = 0u;
                }
                else
                {
                    /* Step L: Failure — retries exhausted */
                    SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_IDLE;
                    retValue = E_NOT_OK;
                }
            }
            /* else: still waiting, remain DIAG_E_PENDING */
        }
    }

    return retValue;
}
#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

/***********************************************************************************
  Function name :   Diag_HSM_MEC_Init
  Description   :   Initialize HSM MEC — double-read retry + first-time set (SUMDIAG_79)
  Parameter in  :   None
  Parameter out :   None
  Return value  :   E_OK if MEC is available, E_NOT_OK if unavailable
  Remarks       :   [$Satisfies $SDD_DIAG 134]
                    If read returns status 0x02 twice, initialize MEC to MecInitialValue.
************************************************************************************/
#define SUM_DIAG_START_SEC_CODE
#include "SUM_DIAG_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) Diag_HSM_MEC_Init(void)
{
    Std_ReturnType retValue;
    uint8 mecValue;
    uint8 mecStatus;

    /* Initialize HSM write state */
    SUM_DIAG_HSM_MEC_Write_State = DIAG_HSM_MEC_WRITE_IDLE;
    SUM_DIAG_HSM_MEC_Write_RetryCounter = 0u;
    SUM_DIAG_HSM_MEC_Write_TimeoutCounter = 0u;
    SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_UNAVAILABLE;

    /* First read attempt */
    retValue = Diag_HSM_MEC_Read(&mecValue, &mecStatus);

    if (retValue == E_OK)
    {
        SUM_DIAG_HSM_MEC_Read_Value = mecValue;
        SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_AVAILABLE;
    }
    else if (mecStatus == HSM_SP_READ_STATUS_INVALID_ID)
    {
        /* First read returned 0x02 — try again (fault injection mitigation) */
        retValue = Diag_HSM_MEC_Read(&mecValue, &mecStatus);

        if (retValue == E_OK)
        {
            SUM_DIAG_HSM_MEC_Read_Value = mecValue;
            SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_AVAILABLE;
        }
        else if (mecStatus == HSM_SP_READ_STATUS_INVALID_ID)
        {
            /* Two consecutive 0x02 — MEC never set in HSM, initialize with configured value */
            retValue = Diag_HSM_MEC_WriteStart(MEC_INITIAL_VALUE);
            if (retValue == DIAG_E_PENDING)
            {
                SUM_DIAG_HSM_MEC_Read_Value = MEC_INITIAL_VALUE;
                SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_AVAILABLE;
                retValue = E_OK;
            }
            else
            {
                /* Write failed — set DTC U19A3.92 per CYS4008 Step L[3] */
                (void)Rte_Call_RP_DemService_U19A392_SetEventStatus(DEM_EVENT_STATUS_FAILED);
                SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_UNAVAILABLE;
            }
        }
        else
        {
            /* Read failed with non-0x02 status — set DTC */
            (void)Rte_Call_RP_DemService_U19A392_SetEventStatus(DEM_EVENT_STATUS_FAILED);
            SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_UNAVAILABLE;
            retValue = E_NOT_OK;
        }
    }
    else
    {
        /* Read failed — set DTC U19A3.92 */
        (void)Rte_Call_RP_DemService_U19A392_SetEventStatus(DEM_EVENT_STATUS_FAILED);
        SUM_DIAG_HSM_MEC_Availability = DIAG_HSM_MEC_UNAVAILABLE;
    }

    return retValue;
}
#define SUM_DIAG_STOP_SEC_CODE
#include "SUM_DIAG_MemMap.h"

#endif /* MEC_STORAGE_IN_HSM == STD_ON */


