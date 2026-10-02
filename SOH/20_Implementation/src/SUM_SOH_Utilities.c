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
 * Description : Utilities function for SOH
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************
*/

/*! @defgroup SUM_SOH_Utilities_c The SUM_SOH_Utilities.c file
 *  This is the SUM_SOH_Utilities.c file
 *  @{
 */

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "SUM_SOH_Utilities.h"
#include "Rte_SUM_SOH.h"

/*!
 *	\file
 *	This file defines additional variables needed by the SOH.
 *
 */


/* Unsigned 16 Bit */
#define SUM_SOH_START_SEC_VAR_CLEARED_16 /* PRQA S 3214 */ /* For memory mapping */
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/

#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
/* [$Satisfies $DDS_SOH 005] */ /* [$Satisfies $DDS_SOH 007] */
uint16			 SUM_SOH_ShutDown_Timer;
#endif

#define SUM_SOH_STOP_SEC_VAR_CLEARED_16 /* PRQA S 3214 */ /* For memory mapping */
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/


/* START OF CODE */

#define SUM_SOH_START_SEC_CODE /* PRQA S 3214 */ /* For memory mapping */
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/

/*!
 *Reads the values stored in the NvM.
 *@param[in]  void
 *@return void
 */
/* PRQA S 1532 3*//* Here for modularity */
 FUNC(void, SUM_SOH_CODE) SOH_NvMReadall(void)
{
    uint8 i;

    for(i=0u;i<NUM_OF_DETECTORS;i++)
    {
    	/*If the detector is not the one related to DID F094*/
    	if(SOH_DetectorItems[i]->NvMDID.NvmRamBlock != NULL_PTR)
    	{
			*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_DIDCntr) = *(SOH_DetectorItems[i]->NvMDID.NvmRamBlock);

			if((SOH_DetectorItems[i]->SOH_Storage == SOH_AdaptiveNvM) || (SOH_DetectorItems[i]->SOH_Storage == SOH_SecureCumulativeNvM))
			{
				*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_Cntr) = *(SOH_DetectorItems[i]->NvMCtr.NvmRamBlock);
			}
			else if(SOH_DetectorItems[i]->SOH_Storage == SOH_RAM)
			{
				*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_Cntr)=0x0000;
				if(SOH_DetectorItems[i]->SOH_Filter == SOH_TWFiltering)
				{
					*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_TW) = 0x00000000;
				}
			}
			else
			{
				/*No Need to anything*/
			}
    	}
    }

}

 /*!
  *Checks the NvM Write status.
  *@param[in]  list of detector to check
  *@param[in]  length of list of detector to check
  *@return if any NvM Writes is pending
  */
 FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_CheckNvMWritesList(SOH_DetectorType* const items[], uint8 length)
{
	Std_ReturnType NvmStatus;
	uint8 i;
    boolean AllOkFlag = TRUE;

	for(i = 0u; i < length; i++)
	{
		/*Check the Nvm write status for each detector*/
		
		NvmStatus = SOH_CheckNvMWrite(items[i]);
		if(NvmStatus != E_OK)
		{
			/* Not all NvM calls returned NVM_REQ_OK*/
			AllOkFlag =FALSE;
		}
		if(NvmStatus == SOH_E_PENDING)
		{
			/*At least one NvM writing is pending*/
			break;
		}
			
	}

	if(AllOkFlag == TRUE)
	{
		/*If all the NvM calls were OK, return E_OK*/
		NvmStatus = E_OK;
	}
	else if(NvmStatus != SOH_E_PENDING)
	{
		/*If at least one NvM call was not ok and there are not
		 * pending NvM writing return SOH_E_PENDING
		 */
		NvmStatus = E_NOT_OK;
	}
	else
	{
      /*Do Nothing*/
	}

	return NvmStatus;
}

 /*!
  *Checks the NvM Write status.
  *@param[in]  specific detector to check
  *@return if any NvM Writes is pending
  */
 /* PRQA S 1505 3*//* Here for modularity [$Satisfies $SDD_SOH 114]*/
 FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_CheckNvMWrite(SOH_DetectorType const* item)
 {

 	 Std_ReturnType ReturnStatus;
 	 NvM_RequestResultType NvmErrorStatus;
 	 Std_ReturnType NvmStatus = E_NOT_OK;
#if (NUM_OF_SOH_DETECTORS != 0u)
 	if(item->NvMDID.SOH_RteCallNvmGetErrorStatus_fp != NULL_PTR)
 	{ 
     
 		 ReturnStatus = item->NvMDID.SOH_RteCallNvmGetErrorStatus_fp(&NvmErrorStatus);
         /*If the return Status is E_OK and the NvmErrorStatus is OK:
          * -if the SOH_Storage is not AdaptiveNvM or SecureCumulativeNvM return E_OK
          * -otherwise, check if there is any NvM writing pending related to the Ctr
          *  and:
          *      -if the return status is OK and NvMErrorStatus is OK, return E_OK
          *      -if the NvM writing is pending return SOH_E_PENDING
          *      -otherwise return E_NOT_OK
          *
          */
		 if ((ReturnStatus == E_OK) && (NvmErrorStatus == NVM_REQ_OK))
		 {
			if((item->SOH_Storage == SOH_AdaptiveNvM) || (item->SOH_Storage == SOH_SecureCumulativeNvM))
			{
				 ReturnStatus = item->NvMCtr.SOH_RteCallNvmGetErrorStatus_fp(&NvmErrorStatus);

				 if((ReturnStatus == E_OK) && (NvmErrorStatus == NVM_REQ_OK))
				 {
					 NvmStatus = E_OK;
				 }
				 else if ((ReturnStatus == E_OK) && (NvmErrorStatus == NVM_REQ_PENDING))
				 {
					 NvmStatus = SOH_E_PENDING;
				 }
				 else
				 {
					 /*Do Nothing*/
				 }
			}
			else
			{
				NvmStatus = E_OK;
			}
		 }
 		 else if((ReturnStatus == E_OK) && (NvmErrorStatus == NVM_REQ_PENDING))
		 {
 			 /*If the NvM writing is pending, return SOH_E_PENDING*/
			 NvmStatus = SOH_E_PENDING;
		 }
         else if((item->SOH_Storage == SOH_AdaptiveNvM) || (item->SOH_Storage == SOH_SecureCumulativeNvM))
		 {
        	 /*If the SOH_Storage is AdaptiveNvM or SecureCumulativeNvM, check if there
        	  * is any NvM writing pending related to the Cntr
        	  */
			 ReturnStatus = item->NvMCtr.SOH_RteCallNvmGetErrorStatus_fp(&NvmErrorStatus);

			 /*If yes, return SOH_E_PENDING, otherwise return E_NOT_OK*/
			 if((ReturnStatus == E_OK) && (NvmErrorStatus == NVM_REQ_PENDING))
			 {
				 NvmStatus = SOH_E_PENDING;
			 }
		 }
         else
         {
           /*Do nothing*/
         }
	}
	else
	{
		#if (ERRH_SOH_LOCAL_SUP_ERRORS_SUPPORT_ENABLED == 0x01)
		/* Case DID 0xF94. No way to be informed from the ERRH about the status of its Nvm ram block.
		Assuming that the last nvm write was successful */
		if (item->SOH_Filter == SOH_ERRHImplementsFilter)
		{
			NvmStatus = E_OK;	
		}
		#else 
		/* Do nothing. NvmStatus = E_NOT_OK*/
		#endif
		
	}

#endif
 	return NvmStatus;
 }

/*!
 *Sets the variables to their initial values at power on.
 *@param[in]  void
 *@return void
 */
 /* PRQA S 1532 3*//* Here for modularity */
 FUNC(void, SUM_SOH_CODE) SOH_InitVariables(void)
{

    uint8 i;

    for(i=0u;i<NUM_OF_DETECTORS;i++)
    {
        /*Initialize SOH_Cntr and SOH_TW to zero*/
        if(SOH_DetectorItems[i]->SOH_Storage == SOH_RAM)
    	{
    		*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_Cntr)=0x0000;
    		if(SOH_DetectorItems[i]->SOH_Filter == SOH_TWFiltering)
    		{
    			*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_TW) = 0x00000000;
    			*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_PrevCounterValue) = 0x00u;
    		}
    	}
    }

}



/*!
 *Write the variable in the NvM.
 *@param[in]  item to write
 *@return void
 */
 /* PRQA S 1505 3*//* Here for modularity [$Satisfies $SDD_SOH 114]*/
 FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_Write(SOH_DetectorType const* item)
{
	Std_ReturnType retValue;

	if(item->NvMDID.NvmRamBlock==NULL_PTR)
	{
		/*If the detector is the one related to DID F094
		 * ask the ERRH to write the NvM to zero
		 * */
#if ERRH_SOH_LOCAL_SUP_ERRORS_SUPPORT_ENABLED 
		/* Clear operation (by setting the the detectors to their default values) done in the SSFM module*/
		retValue = Rte_Call_SUM_SOH_RP_SOH_ERRH_ClearRID0x02C0_Start_Clear();
#endif
	}
	else
	{
		*(item->NvMDID.NvmRamBlock) = *(item->SOH_TresholdItem.SOH_DIDCntr);
		item->NvMDID.SOH_RteCallNvmSetRamBlockStatus_fp(TRUE);
		retValue = item->NvMDID.SOH_RteCallNvmWriteBlock_fp(NULL_PTR);
		if((retValue == E_OK) &&((item->SOH_Storage == SOH_AdaptiveNvM) || (item->SOH_Storage == SOH_SecureCumulativeNvM)))
		{
			*(item->NvMCtr.NvmRamBlock) = *(item->SOH_TresholdItem.SOH_Cntr);
			item->NvMCtr.SOH_RteCallNvmSetRamBlockStatus_fp(TRUE);
			retValue = item->NvMCtr.SOH_RteCallNvmWriteBlock_fp(NULL_PTR);
		}
	}

	return retValue;
}

/*!
 *Sets the the detectors to their default values.
 *@param[in]  list of detector to check
 *@param[in]  length of list of detector to check
 *@return void
 */
 /* [$Satisfies $SDD_SOH 114] */
 FUNC(Std_ReturnType, SUM_SOH_CODE) SOH_ClearItems(SOH_DetectorType* const items[], uint8 length)
{
	Std_ReturnType retVal=E_NOT_OK;
    uint8 i;
 	DisableWrite DisableWriteData;

	/* Read Voltage operation range from SUM_SUSD to determine whether Voltage in range*/
	(void)Rte_Read_SUSD_SOH_BatteryVoltage_DisableNVMWrite_DisableWriteData(&DisableWriteData);

	/* If Battery Voltage Operating in range */
	if(DisableWriteData == FALSE)
	{
		for(i=0u;i<length;i++)
		{
			if(items[i]->SOH_TresholdItem.SOH_DIDCntr!=NULL_PTR)
			{
				*(items[i]->SOH_TresholdItem.SOH_Cntr)=0x0000;
				*(items[i]->SOH_TresholdItem.SOH_DIDCntr)=0x0000;
			}
			if(items[i]->SOH_Filter == SOH_TWFiltering)
			{
				*(items[i]->SOH_TresholdItem.SOH_TW) = 0x00000000;
				*(items[i]->SOH_TresholdItem.SOH_PrevCounterValue) = 0x00u;
			}

			retVal = SOH_Write(items[i]);

			if(retVal!=E_OK)
			{
				break;
			}
		}
	}

	return retVal;
}


/*!
 *Increments of 1s the Time Windows.
 *@param[in]  void
 *@return void
 */
#if(NUM_OF_SOH_DETECTORS!=0u)
 /* PRQA S 1532 3*//* Here for modularity */
 FUNC(void, SUM_SOH_CODE) SOH_UpdateTimeWindows(uint32 counterValue)
{
	uint8 i;
	uint32 *previousCounterValue;

	for(i=0u;i<NUM_OF_DETECTORS;i++)
	{
		/* [$Satisfies $DDS_SOH 018] */
		if((SOH_DetectorItems[i]->SOH_Filter == SOH_TWFiltering) &&
		   ((*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_Cntr)) != 0x00u))
		{
			previousCounterValue = SOH_DetectorItems[i]->SOH_TresholdItem.SOH_PrevCounterValue;

			/* At init and for detectors with data in NvM */
			if( (*previousCounterValue==0x00u) || (*previousCounterValue>counterValue) )
			{
				/* [$Satisfies $DDS_SOH 034] */
				*previousCounterValue = counterValue;
			}

			/* Update Time Windows if 1s elapsed */
			/* PRQA S 3469 3 *//* It's an OS limit */
			while( OS_TICKS2MS_SOH_Counter(counterValue-*previousCounterValue) >= 1000u )
			{
				 /* Increase *previousCounterValue by the number of ticks in 1s */
				*previousCounterValue = *previousCounterValue + (1000000000u/OSTICKDURATION_SOH_Counter);

				/* Increment the time window of 1s */
				/* [$Satisfies $DDS_SOH 019] */
				(*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_TW))++;

				/*If the detection counter does not reach its threshold count calibration within the
				 * calibration time window, the SUM_SOH resets its time window to 0x000000 to start
				 * a new detection cycle and  resets its detection counter to 0x0000*/
				if((*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_TW)) >= SOH_DetectorItems[i]->SOH_TresholdItem.SOH_TWThreshold())
				{
					/* [$Satisfies $DDS_SOH 030] */
					*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_TW) = 0x00000000;
					/* [$Satisfies $DDS_SOH 029] */
					*(SOH_DetectorItems[i]->SOH_TresholdItem.SOH_Cntr) = 0x00u;
					*previousCounterValue = 0x00u;

					break;
				}
			}
		}
	}
}
#endif
#if (SOH_SHUTDOWN_DELAY_SUPPORT_ENABLED == STD_ON)
 /*!
  * Evaluates if SUM_SOH_ShutDown_Timer needs to be updated.
  *@param[in]  uint32 counterValue
  *@return void
*/
 /* PRQA S 1532 3 *//* Here for modularity */
FUNC(void, SUM_SOH_CODE) SOH_MonitorShutDownTimer(uint32 counterValue)
{
	boolean ShutdownTimerFlag;
	static boolean prev_ShutdownTimerFlag = FALSE;
	static uint32 previousCounterValueShutdown = 0x00u;

	/* Update Shutdown timer */
	/* PRQA S 3469 3 *//* It's an RTE problem */
	ShutdownTimerFlag = Rte_IrvRead_Runnable_SOH_Timing_SOH_ShutdownTimerFlag();
	/* [$Satisfies $DDS_SOH 031] */
	if(ShutdownTimerFlag)
	{
		if(!prev_ShutdownTimerFlag)
		{
			/* [$Satisfies $DDS_SOH 035] */
			previousCounterValueShutdown = counterValue;
			prev_ShutdownTimerFlag = ShutdownTimerFlag;
		}
		/* PRQA S 3469 3 *//* It's an OS limit */
		else
		{
			while( OS_TICKS2MS_SOH_Counter(counterValue-previousCounterValueShutdown) >= 1000u )
			{
				/* [$Satisfies $DDS_SOH 036] */
				 /* Increase previousCounterValueShutdown by the number of ticks in 1s */
				previousCounterValueShutdown = previousCounterValueShutdown + (1000000000u/OSTICKDURATION_SOH_Counter);

				SUM_SOH_ShutDown_Timer += 1u;

				 /* PRQA S 3469 3 *//* RTE problem */
				if(SUM_SOH_ShutDown_Timer>=Rte_CData_SUM_SOH_Shutdown_Timer_Thresh())
				{
					/* [$Satisfies $DDS_SOH 032] */
					/* Pass event to the corresponding filter */
					(void)SOH_Event_Detector(SOH_DetectorItems[SOH_ShutdownDelay]);
					/* Check condition then Write DID counter and detection counter into NvM */
					SOH_CheckConditionAndWrite(SOH_DetectorItems[SOH_ShutdownDelay]);

					/* Reset variables */
					Rte_IrvWrite_Runnable_SOH_Timing_SOH_ShutdownTimerFlag((FALSE));
					SUM_SOH_ShutDown_Timer = 0;
					previousCounterValueShutdown = counterValue;
				}

			}
		}
	}
	else
	{
		prev_ShutdownTimerFlag = FALSE;
	}
}
#endif

/*!
 *
 *	@param[in]	SOH_DetectorType, a struct with all the data related to the detector
 *	@return		boolean, TRUE if the detector's counter has reached its threshold
 */
/*[$Satisfies $SDD_SOH 020]*/
#if(NUM_OF_SOH_DETECTORS!=0u)
/* PRQA S 1505 3*//* Here for modularity */
 FUNC(boolean, SUM_SOH_CODE) SOH_Filter(SOH_DetectorType const* item)
{
	boolean ret = TRUE;

	/*In the case where the detection counter reaches its threshold count calibration, the following steps SUM_SOH:
	  - Increments the DID counter by one if it has not reached the MAX value.
	  - Resets the detection counter to 0x0000
	*/
	if((*(item->SOH_TresholdItem.SOH_Cntr))>= item->SOH_TresholdItem.SOH_CntrThreshold())
	{
		/* [$Satisfies $DDS_SOH 021] */
		*(item->SOH_TresholdItem.SOH_Cntr) = 0x0000;

		if(item->SOH_Filter == SOH_TWFiltering)
		{
			/* [$Satisfies $DDS_SOH 022] */
			*(item->SOH_TresholdItem.SOH_TW) = 0x00000000;
			*(item->SOH_TresholdItem.SOH_PrevCounterValue) = 0x00u;
		}

		if(*(item->SOH_TresholdItem.SOH_DIDCntr)<0xFFFEu)
		{
			/* [$Satisfies $DDS_SOH 020] */
			(*(item->SOH_TresholdItem.SOH_DIDCntr))++;
		}
	}
	else
	{
		ret = FALSE;
	}

	return ret;
}

 /*!
  *Check conditions to decide to write counter into corresponding memory type.
  *@param[in]  specific detector to check
  *@return void
  */
 /* [$Satisfies $SDD_SOH 029] [$Satisfies $SDD_SOH 030] [$Satisfies $SDD_SOH 114]*/
 FUNC(void, SUM_SOH_CODE) SOH_CheckConditionAndWrite(SOH_DetectorType const* item)
 {
 	boolean DIDCnt_Condition_b = FALSE;
 	boolean DetectionCnt_Condition_b = FALSE;

 	DisableWrite DisableWriteData;
 	Std_ReturnType NvmStatus;

 	static uint16 prev_DIDCnt_u16 = 0;
 	static uint16 prev_DetectionCnt_u16 = 0;
 	uint16 curr_DIDCnt_u16;
 	uint16 curr_DetectionCnt_u16;

 	/* Get previous DID Counter value */
 	prev_DIDCnt_u16 = *(item->NvMDID.NvmRamBlock);

 	/* Get current DID Counter value after processed by corresponding filter */
 	curr_DIDCnt_u16 = *(item->SOH_TresholdItem.SOH_DIDCntr);

 	/* Check whether DID counter is changed */
 	if((prev_DIDCnt_u16 != curr_DIDCnt_u16))
 	{
 		DIDCnt_Condition_b = TRUE;
 	}

    /*[$Satisfies $SDD_SOH 058]*/
 	if((item->SOH_Storage == SOH_AdaptiveNvM) || (item->SOH_Storage == SOH_SecureCumulativeNvM))
 	{
 		/* Get previous Detection Counter value */
 		prev_DetectionCnt_u16 = *(item->NvMCtr.NvmRamBlock);

 		/* Get current Detection Counter value after processed by corresponding filter */
 		curr_DetectionCnt_u16 = *(item->SOH_TresholdItem.SOH_Cntr);

 		/* Check whether Detection counter is changed */
 		if(prev_DetectionCnt_u16 != curr_DetectionCnt_u16)
 		{
 			DetectionCnt_Condition_b = TRUE;
 		}
 	}

 	/* If at least one filed must be updated */
 	if((TRUE == DIDCnt_Condition_b) || (TRUE == DetectionCnt_Condition_b))
 	{
		/* get NvM Writes status if any write process is pending*/
		NvmStatus = SOH_CheckNvMWrite(item);

		/* If no NvM writes pending*/
		if(SOH_E_PENDING != NvmStatus)
		{
			/* Read Voltage operation range from SUM_SUSD to determine whether Voltage in range*/
			(void)Rte_Read_SUSD_SOH_BatteryVoltage_DisableNVMWrite_DisableWriteData(&DisableWriteData);

			/* If Battery Voltage Operating in range */
			if(DisableWriteData == FALSE)
			{
				/* Perform write counters into memory */
				(void)SOH_Write(item);

			}
		}
 	}
 }


 /*!
  *Function: SOH_Event_Detector implementation.
  *@param[in]  void
  *@return void
  */
FUNC(void, SUM_SOH_CODE) SOH_Event_Detector(SOH_DetectorType const* item)
{
 	/* Increase detection counter */
 	(*(item->SOH_TresholdItem.SOH_Cntr))++;

 	(void)SOH_Filter(item);
}
#endif


#define SUM_SOH_STOP_SEC_CODE /* PRQA S 3214 */ /* For memory mapping */
#include "SUM_SOH_MemMap.h" /* PRQA S 5087 */ /* include SUM_SOH_MemMap.h required here to occupy the space in memory for the described macro*/

/* END OF CODE */

/********************************** END OF FILE ******************************************************/

/*! @} */

