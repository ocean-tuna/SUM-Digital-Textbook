@page designoverview_id 3.  Design Overview

@section dd_ar 3.1 Detailed Design Architecture

This section will describe all the RTA-SUM SrvcMgr sub-modules from a code perspective.

@subsection SrvcMgr_ExMan 3.1.1 Sum_SrvcMgr_Execution_Manager

 **Sum_SrvcMgr_Execution_Manager** is the C module that implements the architectural module **Execution Manager**.
 
\subsubsection Runnable_SrvcMgr_Execution_Manager_id Runnable_SrvcMgr_Execution_Manager
 \R{[$DDS_SrvcMgr 001]}

The @ref Runnable_SrvcMgr_ExecutionManager is the entry point, scheduled at a specified period(@ref Sum_SrvcMgr_ExecutionManagerPeriodicity). The user shall map it into a Periodic OS task through the RTE. It is responsible for reading the PermitExecution from the RTA-SUM SUSD and providing back the ShutdownPermission. It also computes the internal state of the module.

\R{[$Satisfies $ADS_SrvcMgr 001]} \R{[$Satisfies $ADS_SrvcMgr 002]} \R{[$Satisfies $ADS_SrvcMgr 004]} \R{[$Satisfies $ADS_SrvcMgr 005]} \R{[$Satisfies $ADS_SrvcMgr 006]}

\startuml
control Scheduler as TE
participant RTE as R
box SUM_SrvcMgr
    participant Sum_SrvcMgr_ExecutionManager as E
end box

TE -> E ++: Runnable_SrvcMgr_ExecutionManager()
note left: Sum_SrvcMgr_ExecutionManagerPeriodic
return 
\enduml

\R{[$DDS_SrvcMgr 002]}

The @ref Runnable_SrvcMgr_ExecutionManager is a function that manages the execution status for the SrvcMgr  module.

It first checks if the SrvcMgr module initialization already occurred.
If so, it reads the port *RP_SUSD_SrvcMgr_SUSD_PermitExecution* to get the Permit Execution value from the RTA-SUM SUSD.

Based on the value read from the port mentioned above, it moves through an FSM having the following states:

 * SUM_SrvcMgr_Invalid_Mode
 * SUM_SrvcMgr_Run_Mode
 * SUM_SrvcMgr_Init_and_Shutdown_Mode 
 * SUM_SrvcMgr_Prepare_to_Shutdown
 
 In particular, the figure below depicts the Finite State Machine  :

\startuml
 if (Sum_SrvcMgr_GetInitialization()==TRUE) then (yes)
   if (Rte_Read_SUM_SrvcMgr_RP_SUSD_SrvcMgr_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus(&srvcmgr_execution_status)) then (yes)
    switch (SrvcMgr_Execution_State)
	case (SUM_SrvcMgr_Prepare_to_Shutdown)
 	  if(srvcmgr_execution_status matches WakeupBits) then (yes)
 	   :SUM_SrvcMgr_Run_Mode;
	  else(no)
	      if(Req from SPM ongoing) then(yes)
          else (no)
          :SUM_SrvcMgr_Init_and_Shutdown_Mode;
          endif
	  endif
 	case (SUM_SrvcMgr_Init_and_Shutdown_Mode)
 	  if(srvcmgr_execution_status matches WakeupBits) then (yes)
 	   :SUM_SrvcMgr_Run_Mode;
 	  endif
 	case (SUM_SrvcMgr_Run_Mode)
 	  if(srvcmgr_execution_status matches ResetBits OR\n srvcmgr_execution_status !matches WakeupBits) then (yes)
 	   	  if(Req from SPM ongoing) then(yes)
          :SUM_SrvcMgr_Prepare_to_Shutdown;
          else (no)
          :SUM_SrvcMgr_Init_and_Shutdown_Mode;
          endif
 	  endif
    case (default)
 	  :SUM_SrvcMgr_Init_and_Shutdown_Mode;
 	endswitch
   else (no)
    :SUM_SrvcMgr_Invalid_Mode;
   endif
 else (no)
   :SUM_SrvcMgr_Init_and_Shutdown_Mode;
 endif
 \enduml
 
 \R{[$Satisfies $ADS_SrvcMgr 001]} \R{[$Satisfies $ADS_SrvcMgr 002]} \R{[$Satisfies $ADS_SrvcMgr 003]} \R{[$Satisfies $ADS_SrvcMgr 004]} \R{[$Satisfies $ADS_SrvcMgr 005]} \R{[$Satisfies $ADS_SrvcMgr 006]}

 \R{[$DDS_SrvcMgr 003]}

 In this requirement, we suppose that the SrvcMgr state is @ref SUM_SrvcMgr_Init_and_Shutdown_Mode. If the result obtained by applying a bitwise OR operation between the PermitExecution and the SUM_SrvcMgr_PERMITEXECUTION_MASK **matches** @ref SUM_SrvcMgr_EXEC_MASKING_PASSED, the next state is @ref SUM_SrvcMgr_Run_Mode. Moreover, the Execution Manager sets the @ref Sum_SrvcMgr_SsmFuncPermitExec to TRUE and resets the ShutdownPermission to 0.

\R{[$Satisfies $ADS_SrvcMgr 002]} \R{[$Satisfies $ADS_SrvcMgr 003]} \R{[$Satisfies $ADS_SrvcMgr 005]} 

\R{[$DDS_SrvcMgr 019]}

When the SrvcMgr module is in SUM_SrvcMgr_Run_Mode, it first checks whether the actions requested for Dtcs U19A6 xx have been performed, taking into account that these actions are performed only once and only when battery voltage is available. (For further information related the Dtcs, see section @ref SrvcMgr_DTC_Management)

\R{[$Satisfies $ADS_SrvcMgr 026]}

\R{[$DDS_SrvcMgr 004]}

In this requirement, we suppose that the SrvcMgr state is @ref SUM_SrvcMgr_Run_Mode.
Until the SrvcMgr state remains unchanged, the Execution Manager implements a rolling counter ranging from 0 to 3 to update the ShutdownPermission. 

If the result obtained by performing a bitwise OR operation between the PermitExecution and the SUM_SrvcMgr_PERMITEXECUTION_MASK **does NOT match** @ref SUM_SrvcMgr_EXEC_MASKING_PASSED and there is no request from SPM ongoing and there is no active learning session,  the next state is @ref SUM_SrvcMgr_Init_and_Shutdown_Mode. The Execution Manager also resets the @ref Sum_SrvcMgr_SsmFuncPermitExec to FALSE and the ShutdownPermission to 0xFF.
 
Similarly, if the result obtained by performing a bitwise OR operation between the PermitExecution and the @ref SUM_SrvcMgr_RESETBIT_MASK **does not match** @ref SUM_SrvcMgr_EXEC_MASKING_PASSED and there is no request from SPM ongoing and there is no active learning session, the next state is @ref SUM_SrvcMgr_Init_and_Shutdown_Mode. The Execution Manager also resets the @ref Sum_SrvcMgr_SsmFuncPermitExec to FALSE and the ShutdownPermission to 0xFF. 
 
If the result obtained by performing a bitwise OR operation between the PermitExecution and the SUM_SrvcMgr_PERMITEXECUTION_MASK **does NOT match** @ref SUM_SrvcMgr_EXEC_MASKING_PASSED and there is request from SPM ongoing or there is an active learning session,  the next state is @ref SUM_SrvcMgr_Prepare_to_Shutdown. The Execution Manager also keeps alive the rolling counter of the ShutdownPermission.
 
Similarly, if the result obtained by performing a bitwise OR operation between the PermitExecution and the @ref SUM_SrvcMgr_RESETBIT_MASK **does not match** @ref SUM_SrvcMgr_EXEC_MASKING_PASSED and there is request from SPM ongoing or there is an active learning session, the next state is @ref SUM_SrvcMgr_Prepare_to_Shutdown. The Execution Manager also keeps alive the rolling counter of the ShutdownPermission. 
 
{\R{[$Satisfies $ADS_SrvcMgr 002]} \R{[$Satisfies $ADS_SrvcMgr 003]} \R{[$Satisfies $ADS_SrvcMgr 005]}} 

\R{[$DDS_SrvcMgr 005]}

If the @ref Rte_Read_SUM_SrvcMgr_RP_SUSD_SrvcMgr_Ssm_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus returns E_NOT_OK, the next state is @ref SUM_SrvcMgr_Invalid_Mode and the Execution Manager also resets the @ref Sum_SrvcMgr_SsmFuncPermitExec to FALSE and the ShutdownPermission to 0xFF.

\R{[$Satisfies $ADS_SrvcMgr 002]} \R{[$Satisfies $ADS_SrvcMgr 003]} \R{[$Satisfies $ADS_SrvcMgr 005]} 

\R{[$DDS_SrvcMgr 006]}

If the SrvcMgr state is @ref SUM_SrvcMgr_Invalid_Mode, the next state is @ref SUM_SrvcMgr_Init_and_Shutdown_Mode, and the Execution Manager also resets the @ref Sum_SrvcMgr_SsmFuncPermitExec to FALSE.

\R{[$Satisfies $ADS_SrvcMgr 002]} \R{[$Satisfies $ADS_SrvcMgr 003]} \R{[$Satisfies $ADS_SrvcMgr 005]} 

@subsection Learning_Procedure 3.1.2 Learning_Procedure_Support
@subsubsection DID_E0CC 3.1.2.0 DID 0xE0CC

\R{[$DDS_SrvcMgr 007]}

Sum_SrvcMgr will read the value of the Learning Table Status using Runable @ref Runnable_SrvcMgr_DID_E0CC_ReadData and @ref Runnable_SrvcMgr_DID_E0CC_CheckRead. The learning status value are one of Cleared ,Updating
,All Services Learned ,Learning Completed ,Learning Incomplete.

\R{[$Satisfies $ADS_SrvcMgr 007]} \R{[$Satisfies $ADS_SrvcMgr 008]} \R{[$Satisfies $ADS_SrvcMgr 001]} \R{[$Satisfies $ADS_SrvcMgr 019]}

@subsubsection DID_E0CD 3.1.2.1 DID 0xE0CD
\R{[$DDS_SrvcMgr 010]}

Sum_SrvcMgr retrieves the list of the required services that are expected to be learned but not found/discovered, using the following Runnables: @ref Runnable_SrvcMgr_DID_E0CD_ReadData, @ref Runnable_SrvcMgr_DID_E0CD_ReadDataLength and @ref Runnable_SrvcMgr_DID_E0CD_CheckRead. 
Firstly, the Runnable @ref Runnable_SrvcMgr_DID_E0CD_CheckRead checks the learning status: LEARNING_INCOMPLETE or LEARNING_COMPLETED; if the learning state is LEARNING_INCOMPLETE the Runnable @ref Runnable_SrvcMgr_DID_E0CD_ReadDataLength is triggered to retrieve the length of the DID data. Thereafter, the Runnable @ref Runnable_SrvcMgr_DID_E0CD_ReadData is triggered to read the current data (DID data including SD list via Sd_RetrieveIncompleteLearningReasons and missing data from serviceIds coming from SSFM in a separate buffer (if these services are not available in the SD list getting by sd api) into a list of first 400 service ids)

\R{[$Satisfies $ADS_SrvcMgr 001]} \R{[$Satisfies $ADS_SrvcMgr 014]} \R{[$Satisfies $ADS_SrvcMgr 016]} \R{[$Satisfies $ADS_SrvcMgr 019]}

@subsubsection DID_E0CE 3.1.2.2 DID 0xE0CE
\R{[$DDS_SrvcMgr 011]}

Sum_SrvcMgr retrieves the list of the required subscriptions that were expected to be sent and acknowledged but not acknowledged, using the following Runnables: @ref Runnable_SrvcMgr_DID_E0CE_ReadData, @ref Runnable_SrvcMgr_DID_E0CE_ReadDataLength and @ref Runnable_SrvcMgr_DID_E0CE_CheckRead. 
Firstly, the Runnable @ref Runnable_SrvcMgr_DID_E0CE_CheckRead checks the learning status: LEARNING_INCOMPLETE or LEARNING_COMPLETED; if the learning state is LEARNING_INCOMPLETE the Runnable @ref Runnable_SrvcMgr_DID_E0CE_ReadDataLength is triggered to retrieve the length of the DID data. Thereafter, the Runnable @ref Runnable_SrvcMgr_DID_E0CE_ReadData is triggered to read the current data. 

\R{[$Satisfies $ADS_SrvcMgr 001]} \R{[$Satisfies $ADS_SrvcMgr 015]} \R{[$Satisfies $ADS_SrvcMgr 017]} \R{[$Satisfies $ADS_SrvcMgr 019]}

@subsubsection RID 3.1.2.3 RID 0x0612
\R{[$DDS_SrvcMgr 008]}

Sum_SrvcMgr will retrieve the content of the Learning Table using the Runnable @ref Runnable_SrvcMgr_RID_0612 that implement the RID 0x0612. The content retrieved is function of the Control Option received:
	*	Number of Client Service Ids or Server Service Ids learned
	*	Range of record of Client Service Ids or Server Service Ids learned


The @ref Runnable_SrvcMgr_RID_0612 runnable implements enable condition checks before executing the learning table retrieval logic:

**Propulsion System Active (PropSysActv) Check:**
The runnable reads the PropSysActv signal from the RTE port **RP_Com_SrvcMgr_VePSAR_b_PropSysActv** via **Rte_Read_RP_Com_SrvcMgr_VePSAR_b_PropSysActv_VePSAR_b_PropSysActvRxSignal()**. The runnable shall only proceed when PropSysActv equals FALSE. If PropSysActv is not FALSE, the runnable returns the negative response code **DCM_E_CONDITIONSNOTCORRECT** and skips the learning table content retrieval.

**Vehicle Speed Average Driven (VehSpdAvgDrvn) Check:**
The runnable reads the VehSpdAvgDrvn signal from the RTE port **RP_Com_SrvcMgr_YeVSPR_v_DrvnVehicle** via **Rte_Read_RP_Com_SrvcMgr_YeVSPR_v_DrvnVehicle_YeVSPR_v_DrvnVehicleRxSignal()**. The vehicle speed must be within acceptable limits before the learning table retrieval is permitted. If the vehicle speed condition is not met, the runnable returns the negative response code **DCM_E_CONDITIONSNOTCORRECT** and skips the learning table content retrieval.

In case of reference to the same service-based or signal-based(group signal) signals, the runnable can alternatively read both the Propulsion System Active signal and Vehicle Speed Average Driven from the RTE port **RP_Com_SrvcMgr_VeActvProp_YeDrvnVeh** via **Rte_Read_RP_Com_SrvcMgr_VeActvProp_YeDrvnVeh_VeActvProp_YeDrvnVehRxSignal()**. This interface is introduced when both signals are provided as service-based signals, allowing unified access to both signals through a single interface for RID 0x0612 enable condition checking.

\R{[$Satisfies $ADS_SrvcMgr 009]} \R{[$Satisfies $ADS_SrvcMgr 010]} \R{[$Satisfies $ADS_SrvcMgr 019]} \R{[$Satisfies $ADS_SrvcMgr 031]} \R{[$Satisfies $ADS_SrvcMgr 032]} \R{[$Satisfies $ADS_SrvcMgr 033]} \R{[$Satisfies $ADS_SrvcMgr 034]}

\subsubsection rid0654_id 3.1.2.4 RID 0x0654: Triggering Learning Session
\R{[$DDS_SrvcMgr 009]}

Sum_SrvcMgr will trigger the Learning Session using Runnable @ref Runnable_SrvcMgr_RID_0654. 

The @ref Runnable_SrvcMgr_RID_0654 runnable implements enable condition checks before executing the learning session. All the following conditions must be satisfied:

**Propulsion System Active (PropSysActv) Check:**
The runnable evaluates the PropSysActv signal from COM inputs.  
If PropSysActv and Vehicle Speed Average Driven share the same signal/service reference, the runnable reads both through **RP_Com_SrvcMgr_VeActvProp_YeDrvnVeh** via **Rte_Read_RP_Com_SrvcMgr_VeActvProp_YeDrvnVeh_VeActvProp_YeDrvnVehRxSignal()**.  
Otherwise, PropSysActv is read through **RP_Com_SrvcMgr_VePSAR_b_PropSysActv** via **Rte_Read_RP_Com_SrvcMgr_VePSAR_b_PropSysActv_VePSAR_b_PropSysActvRxSignal()**.  
The runnable shall only proceed when PropSysActv equals FALSE. If PropSysActv is not FALSE, the runnable returns the negative response code **DCM_E_CONDITIONSNOTCORRECT** and skips the learning session execution.

**Vehicle Speed Average Driven (VehSpdAvgDrvn) Check:**
The runnable evaluates Vehicle Speed Average Driven from COM inputs.  
If PropSysActv and Vehicle Speed Average Driven share the same signal/service reference, the runnable reads both through **RP_Com_SrvcMgr_VeActvProp_YeDrvnVeh** via **Rte_Read_RP_Com_SrvcMgr_VeActvProp_YeDrvnVeh_VeActvProp_YeDrvnVehRxSignal()**.  
Otherwise, Vehicle Speed Average Driven is read through **RP_Com_SrvcMgr_YeVSPR_v_DrvnVehicle** via **Rte_Read_RP_Com_SrvcMgr_YeVSPR_v_DrvnVehicle_YeVSPR_v_DrvnVehicleRxSignal()**.  
The vehicle speed must be less than or equal to 3 km/h before the learning session is permitted. If the vehicle speed condition is not met, the runnable returns the negative response code **DCM_E_CONDITIONSNOTCORRECT** and skips the learning session execution.

**Failed Safety Fault Active (FsFA VSAD/PSA) Check:**
The SSFM fault-status interface depends on configuration:
- If SSFM single-port support is enabled, the runnable uses **FxFA** interfaces:
  - same signal/service reference: **RP_SSFM_SrvcMgr_FxFA_VSAD_PSA** via **Rte_Read_RP_SSFM_SrvcMgr_FxFA_VSAD_PSA_DTC_STATUS_FXFA()**
  - different signal/service references: **RP_SSFM_SrvcMgr_FxFA_VSAD** and **RP_SSFM_SrvcMgr_FxFA_PSA**
- If SSFM single-port support is disabled, the runnable uses **FsFA** interfaces:
  - same signal/service reference: **RP_SSFM_SrvcMgr_FsFA_VSAD_PSA** via **Rte_Read_RP_SSFM_SrvcMgr_FsFA_VSAD_PSA_DTC_STATUS_FSFA()**
  - different signal/service references: **RP_SSFM_SrvcMgr_FsFA_VSAD** and **RP_SSFM_SrvcMgr_FsFA_PSA**

In all cases, the VSAD-related and PSA-related fault statuses must both be FALSE for the routine to proceed. If one of them is TRUE (or the read fails), the runnable returns **DCM_E_CONDITIONSNOTCORRECT** and skips the learning session execution.

Once the enable conditions are met, SUM SrvcMgr follows the following actions:
* Provide the SdLearningStatusIndication "PENDING" to the SPM module via **Rte_Call_RP_SrvcMgr_SPM_SdLearningStatusIndication_SPM_SDLearningStatusIndication**.
* Clear the learning table using Sd_ClearLearningTable.
* For services that belong to a Service Group, start the group using Sd_ServiceGroupStart(). 
* For each Server service that does not belong to a Service Group, set the status to AVAILABLE using Sd_ServerServiceSetState();
* For each Client service that does not belong to a Service Group, set the status as REQUESTED using the Sd_ClientServiceSetState() and set all associated Consumed Event Group as REQUESTED using the Sd_ConsumedEventGroupSetState();
* It continuously monitors the value of the learning table state using the Sd_GetLearningTableState and retrieves the list of services using Sd_RetrieveIncompleteLearningReasons APIs in order to perform the necessary action for the DTCs. (For further information related the Dtcs, see section @ref SrvcMgr_DTC_Management)
* Once the Learning Session finished, the SrvcMgr: 
	* Sum_SrvcMgr provides the SPM module with SdLearningStatusIndication set to either SUCCESSFUL" or "FAILED" via **Rte_Call_RP_SrvcMgr_SPM_SdLearningStatusIndication_SPM_SDLearningStatusIndication**

\R{[$Satisfies $ADS_SrvcMgr 011]} \R{[$Satisfies $ADS_SrvcMgr 012]} \R{[$Satisfies $ADS_SrvcMgr 018]} \R{[$Satisfies $ADS_SrvcMgr 031]} \R{[$Satisfies $ADS_SrvcMgr 032]} \R{[$Satisfies $ADS_SrvcMgr 033]} \R{[$Satisfies $ADS_SrvcMgr 034]}

\R{[$DDS_SrvcMgr 017]}

Moreover, the @ref Runnable_SrvcMgr_RID_0654 runnable will set the Sum_SrvcMgr_LearningSessionReq flag to TRUE when a
learning session request is accepted, to prevent the Execution Manager from voting for shutdown while an NVM operation
is ongoing. The @ref Runnable_SrvcMgr_RID_0654 runnable will also set Sum_SrvcMgr_LearningSessionReq back to FALSE when
the learning session request has been completed in order to let the Execution Manager vote for shutdown.

\R{[$Satisfies $ADS_SrvcMgr 011]}

@subsection Service_Manager 3.1.3 Service Manager
@subsubsection SrvcMgr_ActivateSrvc 3.1.3.0 API SrvcMgr_ActivateService()
\R{[$DDS_SrvcMgr 014]}

This API will be called through the C/S interface SrvcMgr_SPM_ActivateServices by the SPM module.

The purpose of @ref Runnable_SrvcMgr_ActivateServices is to set the state of a list of Service IDs using one of the following SD APIs: Sd_ServiceGroupStart()/Sd_ServiceGroupStop() if the service belongs to a Service Group; otherwise Sd_ClientServiceSetState()/Sd_ServerServiceSetState().
The result of these operations are retrieved by populating an output array Command Status List.
SUM SrvcMgr follows the following operations: 
* For each instance of the input parameter ServiceIdList
	* Check if the actual Service ID match with one Service defined in the SD configuration (@ref SrvcMgr_InstanceSrvcInfo):
		* No Match: set the value of the commandStatusList for that instance E_GENERAL_ERROR and proceed with the other instances;
		* Match: check the Calibration Status of the Service ID :
				* CalibrationOFF: set the value of the commandStatusList for that instance E_DISABLED and proceed with the other instances;
				* CalibrationON: 
                                                * Check if Service belongs to a Service Group. If yes, use Sd_ServiceGroupStart when activationCommandList is TRUE and Sd_ServiceGroupStop when activationCommandList is FALSE. In this case, SrvcMgr does not call Sd_ClientServiceSetState or Sd_ServerServiceSetState for that service.
                                        * If the service does not belong to a Service Group, a helper function @ref Sum_SrvcMgr_HandlerServiceCommand is called that follows the logic:
					* Discrimanates if the actual Service is a Client or Server
                                        * In function of the type of the Service (Client or Service) and w.r.t. the activationCommandList (TRUE means activation, FALSE means disabling) calls the proper SD API (Sd_ClientServiceSetState/Sd_ServerServiceSetState):
                                                * If the API call returns E_OK: set the value of the commandStatusList for that instance E_OK_LOCAL_ENABLED (for Server Service) or E_OK_REMOTE_ENABLED (for Client Service) and proceed with the other instances;
                                                * If the API call returns E_NOT_OK: set the value of the commandStatusList for that instance to E_GENERAL_ERROR and proceed with the other instances;
					
Moreover, if the command for a Client Service is to be requested, it is necessary to call the Sd API Sd_ConsumedEventGroupSetState() for each SdConsumedEventConfiguredEvent associated (only when Sd_ClientServiceSetState is called and returns E_OK).
If one call of Sd_ConsumedEventGroupSetState() returns E_NOT_OK, the commandStatusList for that particular instance is set to E_GENERAL_ERROR, otherwise it remains E_OK_REMOTE_ENABLED.


\R{[$Satisfies $ADS_SrvcMgr 022]} \R{[$Satisfies $ADS_SrvcMgr 023]}

\R{[$DDS_SrvcMgr 015]}

Moreover, this API firstly sets the value to TRUE of the Sum_SrvcMgr_ActivateServiceReq in order to prevent that the Execution Manager votes for shutdown while a request from SPM is ongoing.
Before end its execution, this API sets the value to FALSE of the Sum_SrvcMgr_ActivateServiceReq in order to let that the Execution Manager votes for shutdown while a request from SPM is ongoing

\R{[$Satisfies $ADS_SrvcMgr 024]}

@subsection SrvcMgr_DTC_Management 3.1.4 DTC Management

\R{[$DDS_SrvcMgr 018]}

**DTC U19A6 57**

The Dtc U19A6 57 ({Physical ECU} Invalid/Incompatible SW Component) is managed by SUM SrvcMgr to detect when expected acknowledgments are not received from physical ECUs after subscribing to a service.

SUM SrvcMgr implements a common check function to evaluate Dtc U19A6 57 status. The check is performed during the execution of the Execution Manager, ( only once and when the battery voltage is Available) , after the execution of RID 0x0654 (regardless of success or failure), and after Service 0x14 (Clear Diagnostic Information) is approved. The Dtc status is reported to SSFM through **Rte_Write_PP_SrvcMgr_SSFM_U19A6_57_Status_invalid**.

Test Fail Criteria: Sd_GetLearningTableState() returns LEARNING_INCOMPLETE AND Sd_RetrieveIncompleteLearningReasons().num_Of_Missing_EventGroups > 0.

Test Pass Criteria: Sd_GetLearningTableState() returns LEARNING_COMPLETE OR (LEARNING_INCOMPLETE AND num_Of_Missing_EventGroups == 0).

**DTC U19A6 87**

The Dtc U19A6 87 ( Service Discovery – Missing Message) is managed by the SUM SrvcMgr and executed during the execution of the Execution Manager, ( only once and when the battery voltage is Available), and after RID 0x0654 execution (regardless of success or failure). The module's task is to forward the LearningTableStatus, containig information about the learning table status and the event, to the SSFM module via the  **Rte_Write_PP_SrvcMgr_SSFM_LearningTableStatus_LearningTableStatus** port. This information is necessary for the SSFM module to evaluate conditions for setting this Dtc.

Test Fail Criteria: Sd_GetLearningTableState() returns LEARNING_INCOMPLETE.

Test Pass Criteria: Sd_GetLearningTableState() returns LEARNING_COMPLETE.

**DTC U19A6 54**

The Dtc U19A6 54 ( Service Discovery – Missing Setup/Learn) is managed by the SUM SrvcMgr and executed during the execution of the Execution Manager, ( only once and when the battery voltage is Available), after RID 0x0654 execution (regardless of success or failure) and after Service 0x14 (Clear Diagnostic Information) is approved. The Dtc status is reported to SSFM module through **Rte_Write_PP_SrvcMgr_Errh_U19A6_54_Status_U19A6_54_Status**.

Test Fail Criteria: Sd_GetLearningTableState() returns CLEARED.

Test Pass Criteria: Sd_GetLearningTableState() returns states other than CLEARED.

\R{[$Satisfies $ADS_SrvcMgr 011]} \R{[$Satisfies $ADS_SrvcMgr 030]} \R{[$Satisfies $ADS_SrvcMgr 026]}

@subsection SrvcMgr_CalController 3.1.5 Sum_SrvcMgr_Calibration_Controller

**Sum_SrvcMgr_Calibration_Controller** is the C module that implements the architectural module **Calibration Controller**.

\subsubsection RE_SrvcMgr_DisableSrvc 3.1.5.1 Runnable_SrvcMgr_DisableService
\R{[$DDS_SrvcMgr 012]}

SUM\_SrvcMgr implements the Runnable\_SrvcMgr\_DisableService that will be called through the C/S interface SrvcMgr\_ServiceCalibrationOff (operation SrvcMgr\_DisableService) 
by the SWCs to change the state (disable) of services through SD APIs.

This Runnable will change the status of a Group Service through SD APIs:

* Sd_ServerServiceSetState()/ClientServiceSetState() (depending on whether the service is client or server)
* Sd_CalibrateServicesOff()

This Runnable takes, as an input parameter, the ID of the Service that the user wants to disable. So, in order to retrieve information about the Service ID, this Runnable exploits (by setter and getter function) information of 
@ref SrvcMgr_InstanceSrvcInfo (that contains information retrieved directly from the Sd/SdConfig/SdInstance module).

The Runnable return the following value: 
* E_OK: if the Service is properly disabled.
* E_NOT_OK: if the Service is NOT properly disabled for the following reason:
	* The Service ID is not configured in the SD container; 
	* One of the SD APIs called return E_NOT_OK;

\R{[$Satisfies $ADS_SrvcMgr 020]} 

\subsubsection RE_SrvcMgr_DisableGrpSrvc 3.1.5.2 Runnable_SrvcMgr_DisableServiceGroup

\R{[$DDS_SrvcMgr 013]}

SUM\_SrvcMgr implements the Runnable\_SrvcMgr\_DisableServiceGroup that will be called through the C/S interface SrvcMgr\_ServiceCalibrationOff (operation SrvcMgr\_DisableServiceGroup) 
by the SWCs to change the state (disable) of services group through SD APIs.

This Runnable will change the status of a Group Service through SD APIs:
* Sd_ServiceGroupStop()
* Sd_CalibrateServicesOff()

This Runnable takes, as an input parameter, the ID of the Service Group that the user wants to disable. So, in order to retrieve information about the Service Group ID, this Runnable exploits (by setter and getter function) information of 
@ref SrvcMgr_InstanceSrvcGrp (that contains information retrieved directly from the Sd/SdConfig/SdInstance module).

The Runnable return the following value: 
* E_OK: The Service Group is properly disabled.
* E_NOT_OK: if the Service Group is NOT properly disabled for the following reason:
	* There is no Service Group configured in the SD container; 
	* The Service Group ID is not configured in the SD container; 
	* The Service Group does not have at least one service associated; 
	* The API Sd_CalibrateServicesOff() returns E_NOT_OK;

\R{[$Satisfies $ADS_SrvcMgr 021]} 

@subsection SrvcMgr_Initialization 3.1.6 Sum_SrvcMgr_Initialization

**Sum_SrvcMgr_Initialization** is the C module that implements the architectural module **Initialization**.

\R{[$DDS_SrvcMgr 016]}

There are two ways to initialize the SUM SrvcMgr and this is decided thanks the configuration parameter Sum_SrvcMgr_GenerateInitRunnable.
When the configuration parameter is set to TRUE, the initialization is done by the @ref Runnable_SrvcMgr_Init that calls the static @ref Sum_SrvcMgr_Init, 
otherwise the initialization is done by the global @ref Sum_SrvcMgr_Init, called by the BswM Init rule.

Furthermore, the @ref Sum_SrvcMgr_Init function permits externally to calibrate services off, by calling in sequence the following:
* Sd_InitializeServicesCalibrationState SD API, void function which permits to initialize the calibration state of the services to ON;
* Rte_Call_SUM_SrvcMgr_RP_SrvcMgr_CalibrationController_surveyServiceDisableCalibrations, operation of the external client/server interface that triggers external runnable(s) to calibrate services off. The return value (E_OK / E_NOT_OK) is ignored, according to the specifics.

\R{[$Satisfies $ADS_SrvcMgr 025]} \R{[$Satisfies $ADS_SrvcMgr 027]} \R{[$Satisfies $ADS_SrvcMgr 028]}

@section dd_base 3.2 Detailed Design

@subsection def_id 3.2.1 Definitions

Please refer to [here](./../html/globals_defs.html) for all the definitions of the module.

@subsection en_id 3.2.2 Enumerations

Please refer to [here](./../html/globals_enum.html) for all the enumerations of the module.

@subsection ev_id 3.2.3 Evaluation

Please refer to [here](./../html/globals_eval.html) for all the evaluations of the module.

@subsection ty_id 3.2.4 Types

Please refer to [here](./../html/globals_type.html) for all the types of the module.

@subsection var_id 3.2.5 Variables

Please refer to [here](./../html/globals_vars.html) for all the variables of the module.

@subsection func_id 3.2.6 Functions

Please refer to [here](./../html/globals_func.html) for all the functions of the module.

@section conf 3.3 Module Configuration

Please refer to the Architectural document.

@section ds 3.4 File Structure

Please refer to [here](./../html/files.html) for all the files of the module.


@section ctc 3.5 Critical time constraints

N.D.

@section fhs 3.6 Failure handling strategy

N.D.
