@page designoverview_id 3.  Design Overview

@section dd_ar 3.1 Detailed Design Architecture

This section will explain in detail all che modules of RTA-SUM SSFM from a code ralated point of view.

@subsection Ssfm_ExMan 3.1.1 Sum_Ssfm_ExecutionManager

 **Sum_Ssfm_ExecutionManager** is the C module that implements the architectural module **Execution Manager**.

\subsubsection Runnable_SSFM_ExecutionManager_id Runnable_SSFM_ExecutionManager

\R{[$DDS_SSFM 001]}

The @ref Runnable_SSFM_ExecutionManager is the entry point, it's scheduled at a specified periodicity ( @ref SSM_MAIN_FUNCTION_PERIODICITY_US ) and shall be mapped to a Periodic OS task through the RTE.

It executes the two main functions of this module, @ref Sum_Ssfm_prv_ErrhExecutionManager and @ref Sum_Ssfm_prv_SsmExecutionManager. They are responsible for reading the Permit Execution form RTA-SUM SUSD and provide back the Shutdown Permission taking in account the internal status of the module.

\R{[$Satisfies $ADS_SSFM 002]} \R{[$Satisfies $ADS_SSFM 003]} \R{[$Satisfies $ADS_SSFM 004]} \R{[$Satisfies $ADS_SSFM 006]} \R{[$Satisfies $ADS_SSFM 007]} \R{[$Satisfies $ADS_SSFM 017]}

\startuml
control Scheduler as TE
participant RTE as R
box SUM_SSFM
    participant Sum_Ssfm_ExecutionManager as E
end box

TE -> E ++: Runnable_SSFM_ExecutionManager()
note left: SSM_MAIN_FUNCTION_PERIODICITY_US

E -> E: Sum_Ssfm_prv_ErrhExecutionManager()
E -> E: Sum_Ssfm_prv_SsmExecutionManager()
return 
\enduml

\subsubsection Sum_Ssfm_prv_ErrhExecutionManager_id Sum_Ssfm_prv_ErrhExecutionManager

The @ref Sum_Ssfm_prv_ErrhExecutionManager is a private function that manages the execution status for the SSFM-Errh submodule.

\R{[$DDS_SSFM 016]}

It firstly check if SSFM-Errh submodule has been already initialized.
If true, it reads the port *Rte_Read_SUM_SSFM_RP_SUSD_SSFM_Errh_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus* to get
from the RTA-SUM Susd the Permit Execution value.

\R{[$Satisfies $ADS_SSFM 001]} \R{[$Satisfies $ADS_SSFM 022]} \R{[$Satisfies $ADS_SSFM 018]}

Based on the read value it will move through a FSM with states :

 * SUM_SSFM_Invalid_Mode
 * SUM_SSFM_Run_Mode
 * SUM_SSFM_Shutdown_Mode

It implements the following finite state machine :

 \startuml
 if (Sum_Ssfm_Errh_GetInitialization()==TRUE) then (yes)
   if (Rte_Read_SUM_SSFM_RP_SUSD_SSFM_Errh_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus(&errh_execution_status)) then (yes)
    switch (Errh_Execution_State)
 	case (SUM_SSFM_Shutdown_Mode)
 	  if(errh_execution_status matchs WakeupBits) then (yes)
 	   :SUM_SSFM_Run_Mode;
 	  endif
 	case (default)
 	  :SUM_SSFM_Shutdown_Mode;
 	case (SUM_SSFM_Run_Mode)
 	  if(errh_execution_status matchs ResetBits OR\n errh_execution_status !matchs WakeupBits) then (yes)
 	   :SUM_SSFM_Shutdown_Mode;
 	  endif
 	endswitch
   else (no)
    :SUM_SSFM_Invalid_Mode;
   endif
 else (no)
   :SUM_SSFM_Shutdown_Mode;
 endif
 \enduml
 
  The main transitions are :

\R{[$DDS_SSFM 017]}

 If it's in @ref SUM_SSFM_Shutdown_Mode and the Permit Execution value plus @ref SUM_SSFM_ERRH_PERMITEXECUTION_MASK **match** @ref SUM_SSFM_EXEC_MASKING_PASSED the next state is @ref SUM_SSFM_Run_Mode  and the @ref Sum_Ssfm_ErrhFuncPermitExec is granted to the SSFM-Errh submodule.

\R{[$Satisfies $ADS_SSFM 018]} \R{[$Satisfies $ADS_SSFM 022]} 

\R{[$DDS_SSFM 018]}

 If it's in @ref SUM_SSFM_Run_Mode and the Permit Execution value plus @ref SUM_SSFM_ERRH_PERMITEXECUTION_MASK **not match** @ref SUM_SSFM_EXEC_MASKING_PASSED or plus @ref SUM_SSFM_RESETBIT_MASK **not match** @ref SUM_SSFM_EXEC_MASKING_PASSED the next state is @ref SUM_SSFM_Shutdown_Mode and the @ref Sum_Ssfm_ErrhFuncPermitExec is revoked to the SSFM-Errh submodule.

 \R{[$Satisfies $ADS_SSFM 018]} \R{[$Satisfies $ADS_SSFM 022]} 

\R{[$DDS_SSFM 019]}

 If we can't read the @ref Rte_Read_SUM_SSFM_RP_SUSD_SSFM_Errh_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus then the next state is @ref SUM_SSFM_Invalid_Mode and the @ref Sum_Ssfm_ErrhFuncPermitExec is revoked to the SSFM-Errh submodule.

 \R{[$Satisfies $ADS_SSFM 018]} \R{[$Satisfies $ADS_SSFM 022]} 

\R{[$DDS_SSFM 020]}

 If it's in @ref SUM_SSFM_Invalid_Mode the next state is @ref SUM_SSFM_Shutdown_Mode and the @ref Sum_Ssfm_ErrhFuncPermitExec is revoked to the SSFM-Errh submodule.

 \R{[$Satisfies $ADS_SSFM 018]} \R{[$Satisfies $ADS_SSFM 022]} 
 
 \R{[$DDS_SSFM 121]}

 The function before ending calls the two following functions: 
  * Rte_Write_SUM_SSFM_PP_SSFM_SUSD_ERRH_ShutdownPermission_SUSD_UserRequest to notify the SUSD about the Shutdown Permission.
  * Rte_Call_RP_SSFM_ERRH_WdgM_WdgM_LocalSupervision_CheckpointReached to notify the WdgM that Errh is alive.

 \R{[$Satisfies $ADS_SSFM 018]} \R{[$Satisfies $ADS_SSFM 022]} \R{[$Satisfies $ADS_SSFM 002]} 


\subsubsection Sum_Ssfm_prv_SsmExecutionManager_id Sum_Ssfm_prv_SsmExecutionManager

The @ref Sum_Ssfm_prv_SsmExecutionManager is a private function that manages the execution status for the SSFM-Ssm submodule.

\R{[$DDS_SSFM 002]}

It firstly check that the SSFM-Errh submodule is running and that the SSFM-Ssm submodule has been already initialized.
If true, it reads the port *Rte_Read_SUM_SSFM_RP_SUSD_SSFM_Ssm_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus* to get
from the RTA-SUM Susd the Permit Execution value.

\R{[$Satisfies $ADS_SSFM 001]} \R{[$Satisfies $ADS_SSFM 005]} \R{[$Satisfies $ADS_SSFM 023]} \R{[$Satisfies $ADS_SSFM 018]}

Based on the read value it will move through a FSM with states :

 * SUM_SSFM_Invalid_Mode
 * SUM_SSFM_Run_Mode
 * SUM_SSFM_Shutdown_Mode

It implements the following finite state machine :

 \startuml
 if (Sum_Ssfm_Ssm_GetInitialization()==TRUE AND\nSum_Ssfm_ErrhFuncPermitExec == TRUE ) then (yes)
   if (Rte_Read_SUM_SSFM_RP_SUSD_SSFM_Ssm_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus(&ssm_execution_status)) then (yes)
    switch (Ssm_Execution_State)
 	case (SUM_SSFM_Shutdown_Mode)
 	  if(ssm_execution_status matchs WakeupBits) then (yes)
 	   :SUM_SSFM_Run_Mode;
 	  endif
 	case (default)
 	  :SUM_SSFM_Shutdown_Mode;
 	case (SUM_SSFM_Run_Mode)
 	  if(ssm_execution_status matchs ResetBits OR\n ssm_execution_status !matchs WakeupBits) then (yes)
 	   :SUM_SSFM_Shutdown_Mode;
 	  endif
 	endswitch
   else (no)
    :SUM_SSFM_Invalid_Mode;
   endif
 else (no)
   :SUM_SSFM_Shutdown_Mode;
 endif
 \enduml

 The main transitions are :

\R{[$DDS_SSFM 003]}

 If it's in @ref SUM_SSFM_Shutdown_Mode and the Permit Execution value plus @ref SUM_SSFM_SSM_PERMITEXECUTION_MASK **match** @ref SUM_SSFM_EXEC_MASKING_PASSED the next state is @ref SUM_SSFM_Run_Mode  and the @ref Sum_Ssfm_SsmFuncPermitExec is granted to the SSFM-Ssm submodule.

\R{[$Satisfies $ADS_SSFM 005]} \R{[$Satisfies $ADS_SSFM 023]} 

\R{[$DDS_SSFM 004]}

 If it's in @ref SUM_SSFM_Run_Mode and the Permit Execution value plus @ref SUM_SSFM_SSM_PERMITEXECUTION_MASK **not match** @ref SUM_SSFM_EXEC_MASKING_PASSED or plus @ref SUM_SSFM_RESETBIT_MASK **not match** @ref SUM_SSFM_EXEC_MASKING_PASSED the next state is @ref SUM_SSFM_Shutdown_Mode and the @ref Sum_Ssfm_SsmFuncPermitExec is revoked to the SSFM-Ssm submodule.

 \R{[$Satisfies $ADS_SSFM 005]} \R{[$Satisfies $ADS_SSFM 023]} 

\R{[$DDS_SSFM 005]}

 If we can't read the @ref Rte_Read_SUM_SSFM_RP_SUSD_SSFM_Ssm_SUSD_PermitExecution_SUSD_SWC_ExecutionStatus then the next state is @ref SUM_SSFM_Invalid_Mode and the @ref Sum_Ssfm_SsmFuncPermitExec is revoked to the SSFM-Ssm submodule.

 \R{[$Satisfies $ADS_SSFM 005]} \R{[$Satisfies $ADS_SSFM 023]} 

\R{[$DDS_SSFM 006]}

 If If it's in @ref SUM_SSFM_Invalid_Mode the next state is @ref SUM_SSFM_Shutdown_Mode and the @ref Sum_Ssfm_SsmFuncPermitExec is revoked to the SSFM-Ssm submodule.

 \R{[$Satisfies $ADS_SSFM 005]} \R{[$Satisfies $ADS_SSFM 023]} 
 

\subsubsection Sum_Ssfm_prv_ErrhCheckStartOperationCycle_id Sum_Ssfm_prv_ErrhCheckStartOperationCycle

\R{[$DDS_SSFM 025]}

The @ref Sum_Ssfm_prv_ErrhCheckStartOperationCycle is a private function that manages the starting of operation cycle for the SSFM-Errh submodule.

If the state is in @ref SUM_SSFM_Run_Mode, it firstly checks the operation started flag if it is **FALSE**. Then, it reads the port *Read_SUM_SSFM_RP_SWC_SSFM_Errh_SWC_OperationCycle_EnableConditions_SWC_SSFM_OpCycle_EnableConditions* to get
from the SW-C the project specific criteria and operation cycle enable condtion. 
If there is one of below conditions happened:

1. The port is returned *RTE_E_UNCONNECTED* which means there is no Project Specific requirement. 
2. SSFM-Errh will be notified from the application SWC (set the value **TRUE**)

The operation cycle will start, SSFM-Errh will invoke *Rte_Call_SUM_SSFM_RP_Dem_SSFM_Errh_Dem_OperationCycle_RestartOperationCycle* to notify to Dem and then set the operation cycle started flag to **TRUE**.

\R{[$Satisfies $ADS_SSFM 028]}

\subsubsection Sum_Ssfm_Errh_prv_CheckTimer_BatteryVoltage_id Sum_Ssfm_Errh_prv_CheckTimer_BatteryVoltage

\R{[$DDS_SSFM 029]}

The @ref Sum_Ssfm_Errh_prv_CheckTimer_BatteryVoltage is the function that reads the value of the battery voltage level from SUSD port *Read_RP_SUSD_SSFM_Errh_BatteryVoltage_BatteryVoltageRangeMinToMax_OperatingRange* . The levels of the battery should be Battery_OverVoltage, Battery_UnderVoltage, Battery_NormalVoltage 
and check the timer of the battery voltage level recovery from over voltage or under voltage within the first 5 second to return a @ref FALSE flag otherwise the flag should be @ref TRUE.


\R{[$Satisfies $ADS_SSFM 035]}

\subsubsection Sum_Ssfm_Errh_prv_CheckTimer_ResetPowerUp_id Sum_Ssfm_Errh_prv_CheckTimer_ResetPowerUp

\R{[$DDS_SSFM 030]}

The @ref Sum_Ssfm_Errh_prv_CheckTimer_ResetPowerUp is the function that initialize the timer @ref Sum_Ssfm_ResetPowerUpRecoveryTimer each power up or reset, within the first 5 second return a @ref FALSE flag otherwise the flag should be @ref TRUE.

\R{[$Satisfies $ADS_SSFM 002]}



 @subsection Ssfm_FaultIndMan 3.1.2 Sum_Ssfm_FaultIndicationsManager
 
 **Sum_Ssfm_FaultIndicationsManager** is the C module that implements the architectural module **Fault Indications Manager**.
The following assumptions are made based on the requirements and the RTE code:

1. The error codes starting with E_SER are definitely related to serializer/deserializer transformers. Therefore, if the received error code is one starting with E_SER, it is not needed to check the transformerClass to get the transformer type; it can be inferred that it is a serializer/deserializer.
2. The error code itself shows that if it is either a hard error or a soft one. A single error code cannot represent both a hard error and a soft error. Therefore, whenever it is asked to check a condition of having a hard error with code equals to E_<error_code>, it is enough to check for the "error code equals to E_<error_code>".


\subsubsection ReceivedSerialDataHandler_Id  Received SerialData Handler

 \R{[$DDS_SSFM 007]}
 
 The Received SerialData Handler is responsible to implement the callback function for each SerialData defined in SSFM with a given configured name.
 
 \R{[$Satisfies $ADS_SSFM 008]} \R{[$Satisfies $ADS_SSFM 009]} \R{[$Satisfies $ADS_SSFM 010]} 
 
 \R{[$DDS_SSFM 008]}
 
The callback function will be called directly by the other SWCs, providing the E2E TransformerError if necessary.
 
 \R{[$Satisfies $ADS_SSFM 008]} \R{[$Satisfies $ADS_SSFM 009]} \R{[$Satisfies $ADS_SSFM 010]} 
 
 \R{[$DDS_SSFM 009]}
 
 It also asks the Security Status Manager to reset the corresponding FcI interface.
 
 \R{[$Satisfies $ADS_SSFM 008]} \R{[$Satisfies $ADS_SSFM 009]} \R{[$Satisfies $ADS_SSFM 010]} 
 
 \R{[$DDS_SSFM 013]}
 
 In case of safety protected SerialDatas, it asks the Continuous Operation Status Manager to set the corresponding FoI interface based on the transformer error.
 
\R{[$Satisfies $ADS_SSFM 014]} \R{[$Satisfies $ADS_SSFM 015]} \R{[$Satisfies $ADS_SSFM 016]} 

\R{[$DDS_SSFM 021]}
 
If the SerialData is protected (either is authenticated or the E2E is configured), it asks the Protection Status Manager to set the corresponding FpI interface.
In this case, if the SerialData is safety protected (E2E is configured), the FpI will be set according the transformer error. Otherwise, the FpI will be directly set to false because the received SerialData handler being call can imply the successful MAC verification.
 
\R{[$Satisfies $ADS_SSFM 019]} \R{[$Satisfies $ADS_SSFM 020]} \R{[$Satisfies $ADS_SSFM 021]} 

\R{[$DDS_SSFM 026]}

If the SerialData is either protected or safety protected (FoI or FpI is available) the Received SerialData Handler asks Safety Status Manager to update the state of the corresponding FsI interface.

\R{[$Satisfies $ADS_SSFM 029]} \R{[$Satisfies $ADS_SSFM 030]} \R{[$Satisfies $ADS_SSFM 031]} 

\R{[$DDS_SSFM 106]}

Upon the reception of callback, if the serial data is LcFA enabled, it first calls @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions to verify the test enable conditions are met. Only if the test is enabled, it calls the @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm to update the node status at serial data level.
If the serial data is service-based, before updating the node status, the provider controller and the corresponding DTC will be evaluated through @ref Sum_Ssfm_ResolveServiceDemEvent and it is checked if setting the DTC is allowed for the given service.

\R{[$Satisfies $ADS_SSFM 105]} 

\R{[$DDS_SSFM 116]}

During the initialization of SSM, the @ref Sum_Ssfm_SetSerialDatasProviders implemented by Fault Indications Data Manager is responsible for monitoring calibration parameters providing the information
about the provider of multiple provider SerialDatas (SSM_C_\<PDU name\>_IsReceivedFrom_\<ECU name\>).
The @ref Sum_Ssfm_SetSerialDatasProviders checks the status of each calibration parameter
and records the current provider of each multiple provider SerialData. In particular, it directly assigns the corresponding Dem Event to the FxI Dem events and sets the current provider of the Serial Data which will be used to set the DTC while processing the Loss of Comm.

\R{[$Satisfies $ADS_SSFM 111]} \R{[$Satisfies $ADS_SSFM 112]} 



\subsubsection SecurityStatusManager_Id Security Status Manager


\R{[$DDS_SSFM 010]}
 
The callout function @ref SSFM_Verification_Status_Callout and the public function @ref Sum_Ssfm_SecStatus_Manager_Set_FcI implement the architectural module Security Status Manager.

\R{[$Satisfies $ADS_SSFM 011]} \R{[$Satisfies $ADS_SSFM 012]} \R{[$Satisfies $ADS_SSFM 013]} 


\R{[$DDS_SSFM 011]}

In particular, the callout function  @ref SSFM_Verification_Status_Callout first verifies if the SUSD has permitted the SSFM-SSM execution and then it checks if the secOCDataID, received as input, matches one of the configured SerialDatas (@ref Sum_Ssfm_prv_GetSerialData_GivenSecOCDataID).

If both conditions are verified, it checks if a MAC verification failure occurred.
In particular, it verifies one of the following errors:
 * SECOC_VERIFICATIONFAILURE;
 * SECOC_FRESHNESSFAILURE;
 * SECOC_AUTHENTICATIONBUILDFAILURE;
 * SECOC_VERIFICATIONFAILURE_OVERWRITTEN;
 * SECOC_NO_VERIFICATION (only if the MAC Authentication for the signal group is enabled).

If one of the above occurred, it sets the corresponding FcI state to true by calling the function @ref Sum_Ssfm_SecStatus_Manager_Set_FcI.

\R{[$Satisfies $ADS_SSFM 001]} \R{[$Satisfies $ADS_SSFM 011]} \R{[$Satisfies $ADS_SSFM 012]} \R{[$Satisfies $ADS_SSFM 013]} \R{[$Satisfies $ADS_SSFM 019]} 


\R{[$DDS_SSFM 075]} 

If the observed MAC verification failure is SECOC_NO_VERIFICATION but the MAC Authentication is not
enabled, the FcI is reset by calling @ref Sum_Ssfm_SecStatus_Manager_Set_FcI.

\R{[$Satisfies $ADS_SSFM 011]} \R{[$Satisfies $ADS_SSFM 012]} 


\R{[$DDS_SSFM 027]}

Then, the callout function, asks:
 * the safety status manager to update the signal group's FsI interface through the @ref Sum_Ssfm_SafetyStatus_Manager_Populate_FsI.
 * the protection status manager to update the group's FpI interface through the @ref Sum_Ssfm_ProtStatus_Manager_Set_FpI_Given_FcI.
 * the CAN/ETH availability manager to update the group's availability status interface through the @ref Sum_Ssfm_SigStsAvailability_Manager_CANETH.


\R{[$Satisfies $ADS_SSFM 029]} \R{[$Satisfies $ADS_SSFM 030]} \R{[$Satisfies $ADS_SSFM 031]} 

\R{[$DDS_SSFM 012]}

The function @ref Sum_Ssfm_SecStatus_Manager_Set_FcI can also be called by other modules to reset the FcI interfaces.

\R{[$Satisfies $ADS_SSFM 001]} \R{[$Satisfies $ADS_SSFM 011]} \R{[$Satisfies $ADS_SSFM 012]} \R{[$Satisfies $ADS_SSFM 013]} 

\R{[$DDS_SSFM 081]}

The function @ref Sum_Ssfm_SecStatus_Manager_Set_FcI calls @ref Sum_Ssfm_FxI_Debouncing_Handler, to provide the latest FcI status and handle the debouncing logic. 

\R{[$Satisfies $ADS_SSFM 084]} \R{[$Satisfies $ADS_SSFM 090]} 

\subsubsection ContinuousOperationStatusManager_Id Continuous Operation Status Manager

\R{[$DDS_SSFM 014]}
 
The public function @ref Sum_Ssfm_ContOpStatus_Manager_Set_FoI_Given_Transformer_Error implement the architectural module Continuous Operation Status Manager.
This function is used to set the FoI interface of a safety protected SerialData according the transformer error returned upon the signal reception.

\R{[$Satisfies $ADS_SSFM 014]} \R{[$Satisfies $ADS_SSFM 015]} \R{[$Satisfies $ADS_SSFM 016]} 


\R{[$DDS_SSFM 015]}

The function @ref Sum_Ssfm_ContOpStatus_Manager_Set_FoI_Given_Transformer_Error can be called by other modules to reset the FoI interfaces based on the given transformer error.

\R{[$Satisfies $ADS_SSFM 014]} \R{[$Satisfies $ADS_SSFM 015]} \R{[$Satisfies $ADS_SSFM 016]} 

\R{[$DDS_SSFM 082]}

The function @ref Sum_Ssfm_ContOpStatus_Manager_Set_FoI_Given_Transformer_Error calls @ref Sum_Ssfm_FxI_Debouncing_Handler, to provide the latest FoI status and handle the debouncing logic. 

\R{[$Satisfies $ADS_SSFM 086]} \R{[$Satisfies $ADS_SSFM 091]} 

\subsubsection ProtectionStatusManager_Id Protection Status Manager

\R{[$DDS_SSFM 022]}

The public function @ref Sum_Ssfm_ProtStatus_Manager_Set_FpI enables the other modules with the ability to set or reset the FpI interface state of a given SerialData, directly, if the SerialData is safety protected.

\R{[$Satisfies $ADS_SSFM 019]} \R{[$Satisfies $ADS_SSFM 020]} \R{[$Satisfies $ADS_SSFM 021]} 

\R{[$DDS_SSFM 023]}

The public function @ref Sum_Ssfm_ProtStatus_Manager_Set_FpI_Given_Transformer_Error enables the other modules with the ability to set or reset the FpI interface state of a given SerialData, based on a transformer error state, if the SerialData is safety protected.

\R{[$Satisfies $ADS_SSFM 019]} \R{[$Satisfies $ADS_SSFM 020]} \R{[$Satisfies $ADS_SSFM 021]} 

\R{[$DDS_SSFM 083]}

The function @ref Sum_Ssfm_ProtStatus_Manager_Set_FpI_Given_Transformer_Error calls @ref Sum_Ssfm_FxI_Debouncing_Handler, to provide the latest FoI status and handle the debouncing logic. 

\R{[$Satisfies $ADS_SSFM 085]} \R{[$Satisfies $ADS_SSFM 092]} 

\subsubsection SafetyStatusManager_Id Safety Status Manager

\R{[$DDS_SSFM 028]}

The public function @ref Sum_Ssfm_SafetyStatus_Manager_Populate_FsI enables the other modules with the ability to request the update of the FsI interface state of a given SerialData, if the SerialData is safety protected or protected. Upon the reception of the request, Safety Status Manager sets the corresponding FsI interface to True (failed status) if there is an error notification reported by FoI and/or FpI interface. Otherwise, the FsI will be set to False.

\R{[$Satisfies $ADS_SSFM 029]} \R{[$Satisfies $ADS_SSFM 030]} \R{[$Satisfies $ADS_SSFM 031]} 


\R{[$DDS_SSFM 084]}

The function @ref Sum_Ssfm_SafetyStatus_Manager_Populate_FsI calls @ref Sum_Ssfm_FxI_Debouncing_Handler, to provide the latest FoI status and handle the debouncing logic. 

\R{[$Satisfies $ADS_SSFM 087]} \R{[$Satisfies $ADS_SSFM 093]} 

\subsubsection SingleFxIManager_Id Single FxI Manager

\R{[$DDS_SSFM 078]}

The public function @ref Sum_Ssfm_SingleFxI_Manager_Set_FxI enables the other modules with the ability to request the update of the FxI interface state of a given SerialData, if the single port FxI is enabled through the parameters. In this case the Single FxI Manager will take the corresponding FxI as a source to populate the FxI interface.

\R{[$Satisfies $ADS_SSFM 078]} \R{[$Satisfies $ADS_SSFM 079]} 

\subsubsection FaultIndicationDataManager_Id Fault Indication Data Manager


\R{[$DDS_SSFM 150]}

The functions responsible for calculating the instantaneous FxI will call @ref Sum_Ssfm_FxI_Debouncing_Handler to handle the debouncing logic.
The @ref Sum_Ssfm_FxI_Debouncing_Handler calls @ref Sum_Ssfm_ErrhDebouncerExecution, with the corresponding Event Debouncer instance, to provide the latest FxI status and get the result of the maturation. 

If the result of maturation is a failed event, the @ref Sum_Ssfm_FxI_Debouncing_Handler resolves the DTC through @ref Sum_Ssfm_ResolveServiceDemEvent if the serial data is service based, keeps the internal state of FxFA (Fault Active Status) and sets the corresponding Dem Event. 
If the result of the maturation is a passed event, it asks the @ref Sum_Ssfm_FIM_Debouncer_MUX to apply the multiplexing logic.
If the result of the maturation is unknown or InDebouncing, no actions are performed.

Any of these actions is only valid if the following checks are TRUE
1. Rte_Read_RP_SUSD_SSFM_Errh_BatteryVoltage_BatteryVoltageRangeMinToMax_OperatingRange returns a normal voltage range
2. @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive returns TRUE
3. @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_StabilityCheck returns TRUE
4. @ref Sum_Ssfm_Errh_GetBatteryVoltageRecoveryTimerFlag returns TRUE
5. @ref Sum_Ssfm_Errh_GetResetPowerUpRecoveryTimerFlag returns TRUE
6. Parameter Sum_Ssfm_Errh_C_DTC_UXXXX_83_SignalProtectionIncorrect is enabled.
\R{[$Satisfies $ADS_SSFM 148]} \R{[$Satisfies $ADS_SSFM 149]}  

\R{[$DDS_SSFM 085]}

At the event of a DTC Confirm, Dem is responsible to call the general call back defined as @ref Ssfm_Dem_CB_DTCStatusChanged.
Then, @ref Ssfm_Dem_CB_DTCStatusChanged calls the following callbacks:
 - @ref Sum_Ssfm_FIM_Dem_CB_DTCStatusChanged
 - @ref Sum_Ssfm_LoC_Dem_CB_DTCStatusChanged

The @ref Sum_Ssfm_FIM_Dem_CB_DTCStatusChanged compares the input DTC Value with the configured Dem Events and fires the
Callback function @ref Sum_Ssfm_FIM_DTCStatusChanged with the given Dem Event as an argument.

\R{[$Satisfies $ADS_SSFM 088]} \R{[$Satisfies $ADS_SSFM 094]} \R{[$Satisfies $ADS_SSFM 096]} 

\R{[$DDS_SSFM 086]}

The @ref Sum_Ssfm_FIM_DTCStatusChanged applies the demultiplexing logic. Whenever the new Dem Status is TestFailed, the Fault Active Status of the corresponding FxFAs will be propagated. Otherwise, all the corresponding FxFAs will be set to False as the demultiplexing "AND" logic is not satisfied.

\R{[$Satisfies $ADS_SSFM 089]} \R{[$Satisfies $ADS_SSFM 095]} \R{[$Satisfies $ADS_SSFM 096]}

\R{[$DDS_SSFM 101]}

The @ref Sum_Ssfm_UnassignFxIDemFromService is a utility function to remove the current Dem assignment to a service based serial data. It will be used whenever the DTC Value or the IP address of a given service cannot be resolved.

\R{[$Satisfies $ADS_SSFM 099]} \R{[$Satisfies $ADS_SSFM 101]}

\R{[$DDS_SSFM 102]}

The @ref Sum_Ssfm_AssignFxIDemToService is a utility function to assign a Dem event instance to a given service based serial data, providing the dtc value.

\R{[$Satisfies $ADS_SSFM 099]} \R{[$Satisfies $ADS_SSFM 101]}

\R{[$DDS_SSFM 103]}

Whenever the SSFM wants to apply the multiplexing logic, it is necessary to resolve the DTC Values of the services. The @ref Sum_Ssfm_ResolveServiceDemEvent interacts with Sd API and SPM_GetDTCIdentifier interface to get the two high bytes of the DTC value of a given service. Concatenating the high bytes with the predefined low byte results in a complete dtc value. The calculated dtc value will be searched among the Sum_Ssfm_FxI_DemEvents and upon the match, the Dem event will be assigned to the service using @ref Sum_Ssfm_AssignFxIDemToService. Any failure in the above process results in an unmatched Dem event hence calling the @ref Sum_Ssfm_UnassignFxIDemFromService.

\R{[$Satisfies $ADS_SSFM 099]} \R{[$Satisfies $ADS_SSFM 101]}

\R{[$DDS_SSFM 156]}

Moreover, the @ref Sum_Ssfm_ResolveServiceDemEvent gets the latest state of loss of comm, updates the Serial Data LcFA interface and availability accordingly.
For signal-based SerialDatas, the Serial Data LcFA is set when the provider has `LossOfComm` set or `NodeStsECU` set.
For service-based SerialDatas, the Serial Data LcFA is set only when the provider has `LossOfComm` set **AND** the SerialData's own `LostCommSts` is `ECU_NODE_STATUS_MISSINGMSG`. This ensures that only the specific missing service triggers LcFA, while other services from the same provider remain unaffected.

\R{[$Satisfies $ADS_SSFM 042]} \R{[$Satisfies $ADS_SSFM 064]}

\R{[$DDS_SSFM 104]}

The @ref Sum_Ssfm_FIM_Debouncer_MUX is responsible for applying the multiplexing logic to set the Fault Active DTCs. It is also the callback function of the Fault Active time-based debouncers.
Before applying the multiplexing logic, it resolves the Dem Events of the service based SerialDatas using @ref Sum_Ssfm_ResolveServiceDemEvent.

\R{[$Satisfies $ADS_SSFM 098]} \R{[$Satisfies $ADS_SSFM 100]} \R{[$Satisfies $ADS_SSFM 102]}

\R{[$DDS_SSFM 107]}

The @ref Sum_Ssfm_UnassignLoFDemFromService is a utility function to remove the current Dem assignment to a service based serial data provider ECU. It will be used whenever the DTC Value or the IP address of a given service cannot be resolved. It also resets the current service provider to the unknown provider.

\R{[$Satisfies $ADS_SSFM 106]}

\R{[$DDS_SSFM 108]}

The @ref Sum_Ssfm_AssignLoFDemToService is a utility function to assign a Dem event instance to a given service based serial data provider, given the dtc value.

\R{[$Satisfies $ADS_SSFM 106]}


@subsection Sum_Ssfm_MessageInfo_Handler 3.1.3 Sum_Ssfm_MessageInfo_Handler

 **Sum_Ssfm_MessageInfo_Handler** is the C module that implements the architectural module **Handler**.

\subsubsection Sum_Ssfm_SsmDisablePdu_Tx_id Sum_Ssfm_SsmDisablePdu_Tx


\R{[$DDS_SSFM 024]}

The @ref Runnable_SSFM_Init is the entry point and will be called by BswM at initialization.

The public function @ref Sum_Ssfm_SsmDisablePdu_Tx implement the architectural module message information handler.

The function @ref Sum_Ssfm_SsmDisablePdu_Tx is called by a private function @ref Sum_Ssfm_SsmInit which executes the initialization sequence for SSFM_SSM when @ref Runnable_SSFM_Init is invoked or by BswM rule init(in this case @ref Sum_Ssfm_SsmInit will be a global function).

**For transmitter:**
1. The function @ref Sum_Ssfm_SsmDisablePdu_Tx is used to check the configured Sum_Ssfm_C_SsmDisable_Tx_PDU_<Pdu name> for a particular transmitter PDU if it is TRUE to invoke SUM_BswM_DisablePdu to BswM. 

2. The function @ref Sum_Ssfm_SsmDisablePdu_Tx shall publishes the value of configured Sum_Ssfm_C_SsmDisable_Tx_PDU_<Pdu name> of the Tx PDUs for TRUE/FALSE to SUM SSC and SUM_PNC by their value.

3. BswM shall invoke Com for disabling the PDU.

\R{[$Satisfies $ADS_SSFM 026]} \R{[$Satisfies $ADS_SSFM 027]} \R{[$Satisfies $ADS_SSFM 032]} 
\R{[$Satisfies $ADS_SSFM 033]} \R{[$Satisfies $ADS_SSFM 034]}

\subsubsection Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_id Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive

\R{[$DDS_SSFM 031]}

The public functions @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive, @ref Sum_Ssfm_Is_DisableNormalComm, @ref Sum_Ssfm_Is_DisablePdu_Rx, @ref Sum_Ssfm_Is_SecOC_VerificationSts_Failure implement the architectural module message information handler.

**Purpose:**

The function @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive checks if Activation Profile (AP) or any Partial Network (PN) is active right now.

**Behavior:**

 * **AP-based (service-based):** Returns AP active status from *ServiceInfo->IsActive*
 * **PN-based (OCM-based):** Checks all configured PNs by reading *Rte_Read_SUM_SSFM_RP_SSFM_PNC_RxStatus_Sig_<Id>_RxPDUGroups* from SUM_PNC. Returns TRUE if at least one PN is active (OR logic)
 * **No PNs configured:** Returns TRUE (default permissive behavior)

**Important side effect - Timer Management:**

This function also manages activity timers for each PN:

 * When PN becomes active: Starts timer by setting *AvailableSince* timestamp (via *GetCounterValue(SSFM_1ms_Counter)*)
 * When PN becomes inactive: Resets timer to 0
 * These timers track how long each PN has been continuously active

**Return values:**

 * **TRUE**: AP is active OR at least one PN is active
 * **FALSE**: AP is inactive AND all PNs are inactive

**Usage context:**

This function is used by:

 * @ref Sum_Ssfm_CheckDisableCondition for instant active status check
 * @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm for timer updates and Loss of Communication monitoring

\R{[$Satisfies $ADS_SSFM 039]} \R{[$Satisfies $ADS_SSFM 040]}  \R{[$Satisfies $ADS_SSFM 041]} 
\R{[$Satisfies $ADS_SSFM 042]}

\subsubsection Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_StabilityCheck_id Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_StabilityCheck

\R{[$DDS_SSFM 048]}

**Purpose:**

The function @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_StabilityCheck checks if AP or any PN has been continuously active for 5 seconds or longer. This implements the Test Enable Criteria for Loss of Communication monitoring.

**Behavior:**

The function performs stability check in two steps:

1. **Check AP first (early exit optimization):**
   * If *ServiceInfo* is configured AND active AND timer ?5000ms: Returns TRUE immediately
   
2. **Check PNs (if AP not stable):**
   * If *pSigRxStatus* is NULL (no PNs configured): Returns TRUE (default)
   * Otherwise: Loops through all mapped PNs
   * Reads PN active status via *Rte_Read_SUM_SSFM_RP_SSFM_PNC_RxStatus_Sig_<Id>_RxPDUGroups*
   * If any PN is active AND its timer ?5000ms: Returns TRUE
   * Timer calculation: *(current_time_ms - AvailableSince) >= 5000u*

**Timer behavior:**

 * Timers are maintained by @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive
 * Timer starts when PN/AP activates
 * Timer resets to 0 only when PN/AP deactivates
 * This function **reads** these timers (does not modify them)

**Return values:**

 * **TRUE**: AP or at least one PN has been stable for ?5 seconds (test enabled)
 * **FALSE**: Nothing is stable yet (test disabled)

**Dependency:**

The function @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive must be called periodically (from @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions) to update timers, otherwise this function always returns FALSE.

**Usage context:**

 * Used by @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions for Loss of Communication test enable criteria
 * Ensures transient activation does not trigger false positives
 * Only enables diagnostic tests when connection is truly stable

**Comparison with @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive:**

 * @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive: Checks instant active status and updates timers
 * @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_StabilityCheck: Checks 5-second stable status (reads timers only)

\R{[$Satisfies $ADS_SSFM 039]} \R{[$Satisfies $ADS_SSFM 040]}  \R{[$Satisfies $ADS_SSFM 041]} 
\R{[$Satisfies $ADS_SSFM 060]}

\subsubsection Sum_Ssfm_Is_DisableNormalComm_id Sum_Ssfm_Is_DisableNormalComm

\R{[$DDS_SSFM 032]}

The function @ref Sum_Ssfm_Is_DisableNormalComm will check the occurrence of Disable Normal communication request from SUM_DIAG. The function shall check the status by reading *Rte_Read_SUM_SSFM_RP_DIAG_SSFM_DIAG_DisableNormalComm_<channel>_NormalComMode* and check whether the SerialData configured. 
If the channel is not configured for a specific SerialData, the return value will be **FALSE**. If the return value is **TRUE** indicated normal communications mode is disable and vice versa. 

\R{[$Satisfies $ADS_SSFM 036]} \R{[$Satisfies $ADS_SSFM 037]} \R{[$Satisfies $ADS_SSFM 040]} 
\R{[$Satisfies $ADS_SSFM 041]} \R{[$Satisfies $ADS_SSFM 042]}

\subsubsection Sum_Ssfm_Is_DisablePdu_Rx_id Sum_Ssfm_Is_DisablePdu_Rx

\R{[$DDS_SSFM 033]}

**For receiver:**
The function @ref Sum_Ssfm_Is_DisablePdu_Rx is used to check the configured calibration parameter SSM_C_Disable_<Pduname> for a particular receiver PDU of the SerialData. 
If Rx PDU is not configured for a specific SerialData, the return value will be **FALSE**. If the return value is **TRUE** indicated Rx PDU disabled and vice versa.

\R{[$Satisfies $ADS_SSFM 038]} \R{[$Satisfies $ADS_SSFM 040]}  \R{[$Satisfies $ADS_SSFM 041]} 
\R{[$Satisfies $ADS_SSFM 042]}

\subsubsection Sum_Ssfm_Is_SecOC_VerificationSts_Failure_id Sum_Ssfm_Is_SecOC_VerificationSts_Failure

\R{[$DDS_SSFM 034]}

The function @ref Sum_Ssfm_Is_SecOC_VerificationSts_Failure will check if the SecOC verification status of the SerialData is failed. The function will read the FcI status from @ref Sum_Ssfm_Authenticated_SerialDataType. 
If the return value is **TRUE** which means the verification status is failed and vice versa.

\R{[$Satisfies $ADS_SSFM 040]} \R{[$Satisfies $ADS_SSFM 041]} \R{[$Satisfies $ADS_SSFM 042]}

\subsubsection Callback_SerialDataid Callback_SerialData

\R{[$DDS_SSFM 035]}

 The Callback function of <SerialData> implements the **Message Info Handler** architectural module for the SerialData named <SerialData>. The list of the implemented callback functions and their associated SerialData are given in the following table:
 
| # | Callback | SerialData Name |
|--------|:-----------:|:-----:|
| 1 | @ref Rte_COMCbkTOut_SG_SrlDat250_Prtctd_SecOC_Interdomain_PhysicalChannel_ETH_Rx | SG_SrlDat250_Prtctd_SecOC_Interdomain_PhysicalChannel_ETH_Rx |
| 2 | @ref Rte_COMCbkTOut_SG_SrlDat250_Prtctd_E2E_SecOC_Interdomain_PhysicalChannel_ETH_Rx | SG_SrlDat250_Prtctd_E2E_SecOC_Interdomain_PhysicalChannel_ETH_Rx |
| 3 | @ref Rte_COMCbkTOut_SG_SrlDat250_Prtctd_E2E_Interdomain_PhysicalChannel_ETH_Rx | SG_SrlDat250_Prtctd_E2E_Interdomain_PhysicalChannel_ETH_Rx |
| 4 | @ref RE_CbkRxAck_SG_SrlDat250_Prtctd_SecOC_Interdomain_PhysicalChannel_ETH_Rx | SG_SrlDat250_Prtctd_SecOC_Interdomain_PhysicalChannel_ETH_Rx |
| 5 | @ref RE_CbkRxAck_SG_SrlDat250_Prtctd_E2E_SecOC_Interdomain_PhysicalChannel_ETH_Rx | SG_SrlDat250_Prtctd_E2E_SecOC_Interdomain_PhysicalChannel_ETH_Rx |
| 6 | @ref RE_CbkRxAck_SG_SrlDat250_Prtctd_E2E_Interdomain_PhysicalChannel_ETH_Rx | SG_SrlDat250_Prtctd_E2E_Interdomain_PhysicalChannel_ETH_Rx |

The Callback function of <SerialData> shall set the signal status supervision for the relevant SerialData:
1. The callback *Rte_COMCbkTOut_<SerialData>* is called when Com timeout and shall set the signal status supervision to @ref SUM_SSFM_SigSts_Supervision_Timeout.
2. The callback *RE_CbkRxAck_<SerialData>* is called when data received event of runnable and shall set the signal status supervision to @ref SUM_SSFM_SigSts_Supervision_Received.

\R{[$Satisfies $ADS_SSFM 040]} \R{[$Satisfies $ADS_SSFM 041]} \R{[$Satisfies $ADS_SSFM 042]}

\R{[$DDS_SSFM 105]}

Upon the reception of COMCbkTOut, if the serial data is LcFA enabled, it first calls @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions to verify the test enable conditions are met. Only if the test is enabled, it calls the @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm to update the node status at serial data level.
If the serial data is service-based, before updating the node status, the provider controller and the corresponding DTC will be evaluated through @ref Sum_Ssfm_ResolveServiceDemEvent and it is checked if setting the DTC is allowed for the given service.

\R{[$Satisfies $ADS_SSFM 104]} \R{[$Satisfies $ADS_SSFM 107]} 

\subsubsection Runnable_SSFM_PnDtcAllowedStatusChanged_PN_ID
\R{[$DDS_SSFM 158]}

The @ref Runnable_SSFM_PnDtcAllowedStatusChanged_PN_ID is triggered anytime there is a PN_DTC_Allowed notification from the PNC for a configured partial network. Its aim is to update the PN_DTC_Enabled flag for serial data participating in partial networks to allow the setting of DTCs.
For this purpose, whenever there is an update event from partial network PN_ID, the runnable is invoked and it in turn calls, for every serial data affected by the notification, the private update function @ref Sum_Ssfm_prv_UpdatePnDtcEnabledFlag.

\R{[$Satisfies $ADS_SSFM 157]}

\subsubsection Sum_Ssfm_prv_UpdatePnDtcEnabledFlag

\R{[$DDS_SSFM 159]}

The private function @ref Sum_Ssfm_prv_UpdatePnDtcEnabledFlag performs, on a serial data, the update of the PN_DTC_Enabled flag when there is status change in one of the partial networks the serial data participates in. More specifically:

-if all the participated PNs are in DTC_Not_Allowed state, then the PN_DTC_Enabled flag is set to Not Allowed;

-if any of the PNs is in DTC_Allowed state, then the PN_DTC_Enabled flag is set to Allowed;

-if the PN_DTC_Enabled flag is set to Allowed and not all the PNs are set to DTC_Not_Allowed, then the flag remains set at Allowed.

\R{[$Satisfies $ADS_SSFM 157]}

@subsection Sum_Ssfm_CANETH_Av_Manager 3.1.4 Sum_Ssfm_CANETH_Av_Manager

 **Sum_Ssfm_CANETH_Av_Manager** is the C module that implements the architectural module **Manager**.

\subsubsection Sum_Ssfm_CheckDisableCondition_id Sum_Ssfm_CheckDisableCondition

\R{[$DDS_SSFM 036]}

The function @ref Sum_Ssfm_CheckDisableCondition shall check if the disabled conditions for the SerialData. 
The function shall read the status of the SerialData by following functions:
 * Reading the partial network active status @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive. Meaningful if and only if the SerialData belongs to com-based traffic.
 * Reading the disable normal communication status @ref Sum_Ssfm_Is_DisableNormalComm.
 * Reading the disable Rx PDU status by calling @ref Sum_Ssfm_Is_DisablePdu_Rx.
 * Checking the activation status of the corresponding service. Meaningful if and only if the SerialData belongs to service-based traffic.

The function shall check if @ref Sum_Ssfm_Is_DisablePdu_Rx returns **TRUE** then:

Check if the function @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive returns **FALSE** **OR** @ref Sum_Ssfm_Is_DisableNormalComm returns **TRUE**, the disable condition is fulfilled or else it is not satisfied.
The signal status availability of the SerialData shall set to state @ref SUM_SSFM_SigSts_Disabled if there is one of those above conditions is fulfilled.
In the case of service-based SerialDatas the service activation status will be used instead of the PN Activation status.

\R{[$Satisfies $ADS_SSFM 041]} \R{[$Satisfies $ADS_SSFM 042]} \R{[$Satisfies $ADS_SSFM 074]} \R{[$Satisfies $ADS_SSFM 075]}

\subsubsection Sum_Ssfm_SigSts_Disabled_State_Handler_id Sum_Ssfm_SigSts_Disabled_State_Handler

\R{[$DDS_SSFM 037]}

The function @ref Sum_Ssfm_SigSts_Disabled_State_Handler shall handle the **Disabled** state of signal status availability for the SerialData. 
This function shall check if the disable conditions is **FALSE** by checking @ref Sum_Ssfm_CheckDisableCondition, the signal status availability of the SerialData shall set to @ref SUM_SSFM_SigSts_Uninitialized state.

\R{[$Satisfies $ADS_SSFM 042]}

\subsubsection Sum_Ssfm_SigSts_Uninitialized_State_Handler_id Sum_Ssfm_SigSts_Uninitialized_State_Handler

\R{[$DDS_SSFM 038]}

The function @ref Sum_Ssfm_SigSts_Uninitialized_State_Handler shall handle the **Uninitialized** state of signal status availability for the SerialData. 
This function shall check:
 * If the disable conditions is **TRUE** by checking @ref Sum_Ssfm_CheckDisableCondition, the signal status availability of the SerialData shall set to state @ref SUM_SSFM_SigSts_Disabled.
 * Else if the signal status supervision is @ref SUM_SSFM_SigSts_Supervision_Timeout **AND** if there serial data is configured as service based, this function shall also check if the ServiceActive_Status is **TRUE** by serialData ServiceInfo is activated or not via @ref Runnable_SSFM_ServiceActivationStatusReceived, the signal status availability of the SerialData shall set to state @ref SUM_SSFM_SigSts_Unavailable. Also, if the Serial Data is in loss of comm provided by @ref Sum_Ssfm_IsSerialDataNodeStsForAvailability, the status will be set to @ref SUM_SSFM_SigSts_Unavailable.
 * Else if the signal status supervision is @ref SUM_SSFM_SigSts_Supervision_Received **AND** SecOC verification status is **FALSE**, the signal status availability of the SerialData shall set to @ref SUM_SSFM_SigSts_Available state.

\R{[$Satisfies $ADS_SSFM 042]} \R{[$Satisfies $ADS_SSFM 075]}

\subsubsection Sum_Ssfm_SigSts_Available_State_Handler_id Sum_Ssfm_SigSts_Available_State_Handler

\R{[$DDS_SSFM 039]}

The function @ref Sum_Ssfm_SigSts_Available_State_Handler shall handle the **Available** state of signal status availability for the SerialData. 
This function shall check:
 * If the disable conditions is **TRUE** by checking @ref Sum_Ssfm_CheckDisableCondition, the signal status availability of the SerialData shall set to state @ref SUM_SSFM_SigSts_Disabled.
 * Else if the signal status supervision is @ref SUM_SSFM_SigSts_Supervision_Timeout **AND** if there serial data is configured as service based, this function shall also check if the ServiceActive_Status is **TRUE** by serialData ServiceInfo is activated or not via @ref Runnable_SSFM_ServiceActivationStatusReceived. **OR** SecOC verification status is **TRUE**, the signal status availability of the SerialData shall set to @ref SUM_SSFM_SigSts_Unavailable state. Also, if the Serial Data is in loss of comm provided by @ref Sum_Ssfm_IsSerialDataNodeStsForAvailability, the status will be set to @ref SUM_SSFM_SigSts_Unavailable.

\R{[$Satisfies $ADS_SSFM 042]} \R{[$Satisfies $ADS_SSFM 075]}

\subsubsection Sum_Ssfm_SigSts_Unavailable_State_Handler_id Sum_Ssfm_SigSts_Unavailable_State_Handler

\R{[$DDS_SSFM 040]}

The function @ref Sum_Ssfm_SigSts_Unavailable_State_Handler shall handle the **Unavailable** state of signal status availability for the SerialData. 
This function shall check:
 * If the disable conditions is **TRUE** by checking @ref Sum_Ssfm_CheckDisableCondition, the signal status availability of the SerialData shall set to state @ref SUM_SSFM_SigSts_Disabled.
 * Else if the signal status supervision is @ref SUM_SSFM_SigSts_Supervision_Received, **AND** SecOC verification status is **FALSE**, **AND** the Serial Data is not in loss of comm provided by @ref Sum_Ssfm_IsSerialDataNodeStsForAvailability, the signal status availability of the SerialData shall set to @ref SUM_SSFM_SigSts_Available state.

\R{[$Satisfies $ADS_SSFM 042]}

\subsubsection Sum_Ssfm_SigStsAvailability_Manager_CANETH_id Sum_Ssfm_SigStsAvailability_Manager_CANETH

\R{[$DDS_SSFM 042]}

The function @ref Sum_Ssfm_SigStsAvailability_Manager_CANETH shall check the signal status availability for the particular SerialDatas provided by parameter input.
This function shall get the SecOC verification status from FcI by calling function @ref Sum_Ssfm_Is_SecOC_VerificationSts_Failure.
The function Sum_Ssfm_SigStsAvailability_Manager_CANETH function shall implement the following finite state machine:

 \startuml
 start
 	if (serial_data->IsNonDiscoverable == TRUE) then (yes)
 		:Return immediately (bypass state machine);;
 		stop
 	endif
 	:SecOCVerification_Status = Sum_Ssfm_Is_SecOC_VerificationSts_Failure(serial_data);
    switch (serial_data->SigSts)
 	case (SUM_SSFM_SigSts_Disabled)
		:Sum_Ssfm_SigSts_Disabled_State_Handler(serial_data);
 	case (SUM_SSFM_SigSts_Uninitialized)
 	  	:Sum_Ssfm_SigSts_Uninitialized_State_Handler(serial_data);
 	case (SUM_SSFM_SigSts_Available)
		:Sum_Ssfm_SigSts_Available_State_Handler(serial_data);
 	case (SUM_SSFM_SigSts_Unavailable)
		:Sum_Ssfm_SigSts_Unavailable_State_Handler(serial_data);
	case (default)
		:do nothing;
 	endswitch
 stop	
 \enduml

This function will be called at the event of any change in the conditions of the signal status availability of a given SerialData. These possible changes will be handled through the following runnables:
1. `Runnable_SSFM_PNStatusChanged_<PN_ID>`: Will be called upon any change in the status of one of the related Partial Networks.
2. `Runnable_SSFM_DIAG_DisableNormalCommChanged_Ch<Channel_ID>`: Will be called upon any change in the status of the DIAG Disable Normal Communication of the related channel.
3. `Sum_Ssfm_COMCbkTOut_<SerialData>`: Will be called upon Com timeout which result in a change in signal supervision status.
4. `Runnable_SSFM_Received_SerialData_<SerialData>`: Will be called upon reception of a SerialData.
5. `SSFM_Verification_Status_Callout`: Will be called upon SecOC verification status.
6. `Runnable_SSFM_ServiceActivationStatusReceived`: Will be called upon a change in services' activation status

\R{[$Satisfies $ADS_SSFM 041]} \R{[$Satisfies $ADS_SSFM 042]} \R{[$Satisfies $ADS_SSFM 043]} \R{[$Satisfies $ADS_SSFM 045]} \R{[$Satisfies $ADS_SSFM 046]} \R{[$Satisfies $ADS_SSFM 075]} \R{[$Satisfies $ADS_SSFM 074]}

\R{[$DDS_SSFM 365]}

For non-discoverable SOME/IP PDUs (identified by the IsNonDiscoverable flag set to TRUE), this function shall immediately return without executing the state machine, as these PDUs are always available and do not require availability monitoring. Non-discoverable PDUs are initialized to Available status during module initialization and remain in that state throughout the ECU lifecycle.

\R{[$Satisfies $ADS_SSFM 209]}

@subsection Sum_Ssfm_SigStat_Av_Manager 3.1.5 Sum_Ssfm_SigStat_Av_Manager

\R{[$DDS_SSFM 043]}

**Sum_Ssfm_SigStat_Av_Manager** is the C module that implements the architectural module **Manager**.

This module implements two of the above mentioned runnables to check the signal status availability:
1. `Runnable_SSFM_DIAG_DisableNormalCommChanged_Ch<Channel_ID>`
2. `Runnable_SSFM_PNStatusChanged_<PN_ID>`
3. `Runnable_SSFM_ServiceActivationStatusReceived`

\R{[$Satisfies $ADS_SSFM 044]}

\subsubsection Runnable_SSFM_ServiceActivationStatusReceived_id Runnable_SSFM_ServiceActivationStatusReceived

\R{[$DDS_SSFM 076]}

The runnable @ref Runnable_SSFM_ServiceActivationStatusReceived is responsible for monitor the service activation status. The SPM will notify the changes in
service activation status to SSFM using the sender-receiver interface SPM_ServiceActivationStatusNotification which triggers the above runnable.
The runnable reads the latest activation status and updates the corresponding service-based SerialDatas. If the activation status of a SerialData is changed, the runnable will:
1. Inform the Signal Availability Status Manager to update the availability status of the given SerialData via @ref Sum_Ssfm_SigStsAvailability_Manager_CANETH.
2. Check @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions and, only if the test is enabled, invoke @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm to re-evaluate the loss of communication status of the given SerialData.

\R{[$Satisfies $ADS_SSFM 074]} \R{[$Satisfies $ADS_SSFM 075]} \R{[$Satisfies $ADS_SSFM 064]}


@subsection Sum_Ssfm_ErrhEventDebouncer_Manager 3.1.6 Sum_Ssfm_ErrhEventDebouncer_Manager

\subsubsection Sum_Ssfm_ErrhDebouncerExecution_id Sum_Ssfm_ErrhDebouncerExecution

The @ref Sum_Ssfm_ErrhDebouncerExecution is a function that manages the debouncer execution for the SSFM-Errh submodule.

\R{[$DDS_SSFM 047]}

**Sum_Ssfm_ErrhDebouncerExecution** is the C module that implements the architectural module **Manager**.

The function @ref Sum_Ssfm_ErrhDebouncerExecution checks the error filter type provided by parameter input then evaluate the debouncer depending on input of its configuration and new event notification.
The error filter type shall have the following value:
 * @ref SUM_SSFM_ErrorFilter_None
 * @ref SUM_SSFM_ErrorFilter_Block
 * @ref SUM_SSFM_ErrorFilter_MovingWindow_TimeBased
 * @ref SUM_SSFM_ErrorFilter_MovingWindow_EventBased

The function Sum_Ssfm_ErrhDebouncerExecution function implements the following logic:

 \startuml
 start
    switch (self->error_filter_type)
 	case (SUM_SSFM_ErrorFilter_None)
		:ret_val = (new_event_notification == SUM_SSFM_EventSts_Passed) ? SUM_SSFM_EvaluationSts_Passed : SUM_SSFM_EvaluationSts_Failed;
 	case (SUM_SSFM_ErrorFilter_Block)
 	  	:ret_val = SUM_SSFM_EvaluationSts_Passed;
 	case (SUM_SSFM_ErrorFilter_MovingWindow_TimeBased)
		:ret_val = Sum_Ssfm_ErrhDebouncerMovingWindow(self, new_event_notification);
 	case (SUM_SSFM_ErrorFilter_MovingWindow_EventBased)
		:ret_val = Sum_Ssfm_ErrhDebouncerMovingWindow(self, new_event_notification);
	case (default)
		:ret_val = SUM_SSFM_EvaluationSts_Unknown;
 	endswitch
 stop	
 \enduml

The return value may have the following values after executing debouncer for the new event notification:
 * @ref SUM_SSFM_EvaluationSts_Failed
 * @ref SUM_SSFM_EvaluationSts_Passed
 * @ref SUM_SSFM_EvaluationSts_InDebouncing
 * @ref SUM_SSFM_EvaluationSts_Unknown

\R{[$Satisfies $ADS_SSFM 054]} \R{[$Satisfies $ADS_SSFM 055]}

\subsubsection Sum_Ssfm_ErrhDebouncerMovingWindow_id Sum_Ssfm_ErrhDebouncerMovingWindow

\R{[$DDS_SSFM 054]}

The @ref Sum_Ssfm_ErrhDebouncerMovingWindow is a function that evaluates debouncing logic for error filters with moving window X of Y for the error configuration instance of the SSFM-Errh submodule.

The function @ref Sum_Ssfm_ErrhDebouncerMovingWindow checks the current event status provided by parameter input then evaluates the debouncer depending on the input of its configuration.
 
The return value may have the following values after evaluating debouncing for the new event notification:
 * @ref SUM_SSFM_EvaluationSts_Failed
 * @ref SUM_SSFM_EvaluationSts_Passed
 * @ref SUM_SSFM_EvaluationSts_InDebouncing

\R{[$Satisfies $ADS_SSFM 053]} \R{[$Satisfies $ADS_SSFM 054]} \R{[$Satisfies $ADS_SSFM 055]}

\subsubsection Runnable_SSFM_Debouncer_MovingWindow_TimeBased_id Runnable_SSFM_Debouncer_MovingWindow_TimeBased

\R{[$DDS_SSFM 056]}

The @ref Runnable_SSFM_Debouncer_MovingWindow_TimeBased is a function for moving window X of Y Time Based that run periodically with configuration time to count the filter counter for time based event filters and check if the interval exceeds the Y threshold.

\R{[$Satisfies $ADS_SSFM 054]} \R{[$Satisfies $ADS_SSFM 055]}


@subsection Sum_Ssfm_Errh_U19A3_57_Handler 3.1.7 Sum_Ssfm_Errh_U19A3_57_Handler

The Sum_Ssfm_Errh_U19A3_57_Handler is responsible for setting/resetting the failure status of U19A3 57 DTCs. 
For the code generation of Sum_Ssfm_Errh_U19A3_57_Handler, two parameters must be configured to true:
	* Sum_Ssfm_Errh_CodeGen_Enabled, present in Sum_Ssfm_Errh_U19A3_57_InvIncCfg in SSFM module;
	* inVehicleMessageAuthentictnSupprted in SSC module, configured to a non-zero value.

\subsubsection Runnable_SSFM_SSC_DTC_U19A3_57_Status_Changed 
\R{[$DDS_SSFM 057]}

The DTC U19A3 57 corresponds to the failure of control module because of invalid/incompatible configuration. Whenever a change happens in the status of the U19A3 57 DTC, the SUM_SSC will notify the SSFM through the designated port. The SSFM triggers the @ref Runnable_SSFM_SSC_DTC_U19A3_57_Status_Changed to handle the logic of the given DTC. 

\R{[$Satisfies $ADS_SSFM 056]} \R{[$Satisfies $ADS_SSFM 057]} \R{[$Satisfies $ADS_SSFM 058]}

\R{[$DDS_SSFM 058]}

The @ref Runnable_SSFM_SSC_DTC_U19A3_57_Status_Changed, first, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_ControlModule_InvalidIncompatibleConfiguration` is true or not. If it is set to false, the SUM_SSC notification will be ignored.

\R{[$Satisfies $ADS_SSFM 059]}

\R{[$DDS_SSFM 059]}

Then, the battery voltage range will be retrieved through SUM_SUSD. If the OBD is supported it is enough for the voltage to be not under voltage. Otherwise, the voltage should be strictly in normal range. If the battery voltage requirement is not satisfied, the notification from SUM_SSC will be ignored.  

\R{[$Satisfies $ADS_SSFM 056]} \R{[$Satisfies $ADS_SSFM 057]} \R{[$Satisfies $ADS_SSFM 058]}


\R{[$DDS_SSFM 060]}

Finally, the runnable will inform the Dem with the current status of DTC  U19A3 57.

\R{[$Satisfies $ADS_SSFM 056]} \R{[$Satisfies $ADS_SSFM 057]} \R{[$Satisfies $ADS_SSFM 058]}


@subsection Sum_Ssfm_NodeSts_Manager 3.1.8 Sum_Ssfm_NodeSts_Manager

**Sum_Ssfm_NodeSts_Manager** is the C module that implements the architectural module **Manager**.

This module mainly implements the following functions to manage the lost communication status of SerialDatas and ECUs:
1. `Sum_Ssfm_Node_MissingMsg_TestEnableConditions`
2. `Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm`
3. `Sum_Ssfm_prv_UpdateECULostComm`
4. `Sum_Ssfm_prv_UpdateTrgDvcLostComm`
5. `Runnable_SSFM_NodeStsECU_<X>_StatusChanged`
6. `Runnable_SSFM_LossCommWithNode`
7. `Sum_Ssfm_LoC_Dem_CB_DTCStatusChanged`


\R{[$DDS_SSFM 368]}

The @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions function checks Test Enable Conditions for Missing Message detection. It is called before @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm to determine whether the test should be performed. All of the following conditions must be satisfied for the test to be enabled:
1. The MissingMessage DTC is enabled: the calibration parameter (`U0XXX87_ENABLE`) must be set (via @ref Sum_Ssfm_Errh_Get_C_DTC_UXXXX_87_MissingMessage) AND DTCs must not be globally disabled by PNC RX disable (via @ref Sum_Ssfm_Errh_GetDTCsDisabled). **This condition is evaluated first; if not met, the remaining conditions are not evaluated.**
2. Vehicle Supply voltage is within calibratable range (via @ref Sum_Ssfm_Errh_GetBatteryVoltageRecoveryTimerFlag). **OBD ECU exception:** if the `Rte_Read` of the operating range returns `E_OK` and the battery voltage is in overvoltage (`OUT_OF_RANGE_HIGH`), this condition is still considered satisfied — OBD ECUs shall not inhibit the setting of OBD relevant lost communication DTCs during an overvoltage condition. If `Rte_Read` does not return `E_OK`, the voltage is conservatively treated as out of range.
3. Not within first 5 seconds of reset/recovery (via @ref Sum_Ssfm_Errh_GetResetPowerUpRecoveryTimerFlag)
4. Monitored PDU/Signal is part of active PN or AP AND has been active for 5 seconds or longer (via @ref Sum_Ssfm_Is_ServiceOrAtLeastOnePNActive_StabilityCheck)
5. Bus-off condition is not current — only for signal-based SerialDatas: if `sum_Ssfm_Ssm_C_UsageOfCanBusOffDTC` is enabled and bus-off status is `SUM_SSFM_EvaluationSts_Failed`, the test is disabled

If `LostCommInfo` is `NULL_PTR` (non-discoverable SOME/IP PDUs), the function returns `FALSE` immediately.

\R{[$Satisfies $ADS_SSFM 211]}

\R{[$DDS_SSFM 044]}

The @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm is responsible to handle the state machine of a given SerialData regarding the loss of communication status.

\R{[$Satisfies $ADS_SSFM 050]}

\R{[$DDS_SSFM 366]}

Non-discoverable SOME/IP PDUs (with LostCommInfo set to NULL_PTR) are excluded from loss of communication monitoring. These PDUs do not participate in timeout-based detection as they are always present on the Ethernet network and do not require wake-up conditions.

Whenever a change happens in the status of the signal, the @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions function is called first to verify the test enable conditions. Only if the test is enabled, this function will be called to assess the loss of communication status. The following runnables and callbacks are responsible for
firing the request to assess the status:

1. `Runnable_SSFM_Received_SerialData_<SerialData>`
2. `Sum_Ssfm_COMCbkTOut_<SerialData>`
3. `Runnable_SSFM_PNStatusChanged_<PN_ID>`
4. `Runnable_SSFM_DIAG_DisableNormalCommChanged_Ch<Channel_ID>`
5. `Runnable_SSFM_ServiceActivationStatusReceived`

\R{[$Satisfies $ADS_SSFM 052]} \R{[$Satisfies $ADS_SSFM 049]} \R{[$Satisfies $ADS_SSFM 210]}

\R{[$DDS_SSFM 045]}

Note: The @ref Sum_Ssfm_Node_MissingMsg_TestEnableConditions function is always called before @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm. The PN/AP stability check, battery voltage, reset recovery, DTC enable, and bus-off conditions are now evaluated in the test enable and are no longer part of the FSM transition logic below.

Whenever the @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm is called, the following conditions have to be met to move from ECU_NODE_STATUS_NO_MISSINGMSG to ECU_NODE_STATUS_MISSINGMSG:
1. The signal supervision status should be equal to SUM_SSFM_SigSts_Supervision_Timeout
2. The "disable normal comm" flag should not be true
3. The calibration parameter for disable RX PDU should be false
4. The CAN_BUS_OFF_DTC flag should NOT be set (Sum_Ssfm_Errh_Publish_CAN_<n> = SUM_SSFM_EvaluationSts_Passed). This condition should be considered only if sum_Ssfm_Ssm_C_UsageOfCanBusOffDTC is true
5. The PN_DTC_Enabled_Flag should be in the Allowed state (or the SerialData has no PN mappings)

Whenever the @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm is called, the state will move from ECU_NODE_STATUS_MISSINGMSG to ECU_NODE_STATUS_NO_MISSINGMSG when both of the following conditions are met:
1. The signal supervision status being equal to SUM_SSFM_SigSts_Supervision_Received
2. The calibration parameter for disable RX PDU being true

\R{[$Satisfies $ADS_SSFM 050]} \R{[$Satisfies $ADS_SSFM 052]} \R{[$Satisfies $ADS_SSFM 049]}

\R{[$DDS_SSFM 109]}

If the given serial data is service based, the only condition to consider from the lists above will be the supervision status.
Specifically, for service-based SerialDatas, the supervision status (`SUM_SSFM_SigSts_Supervision_Timeout` or `SUM_SSFM_SigSts_Supervision_Received`) combined with the SPM service activation status (`IsActive`) drives the state transitions. The CAN_BUS_OFF_DTC, disable normal comm, and disable PDU Rx conditions do not apply for service-based SerialDatas.

\R{[$Satisfies $ADS_SSFM 103]} 


\R{[$DDS_SSFM 046]}

If calling the @ref Sum_Ssfm_NodeSts_Manager_UpdateSerialDataLostComm causes a change in a given SerialData lost communication status, the function @ref Sum_Ssfm_prv_UpdateECULostComm will be called for the current provider ECU of the SerialData, only if the SerialData is signal-based.

If the current Node Status of a given ECU is ECU_NODE_STATUS_NO_MISSINGMSG, the @ref Sum_Ssfm_prv_UpdateECULostComm iterates over all the signals configured as FaultAndRecoveryList. If and only if one or more of these SerialDatas are in the ECU_NODE_STATUS_MISSINGMSG state, the ECU Node Status will be changed to ECU_NODE_STATUS_MISSINGMSG. On the other hand whenever the @ref Sum_Ssfm_prv_UpdateECULostComm is called and the state of the ECU is in ECU_NODE_STATUS_MISSINGMSG, the function iterates over all the FaultAndRecoveryList as well as the RecoveryOnlyList. If all of the above mentioned SerialDatas are in ECU_NODE_STATUS_NO_MISSINGMSG state, the state of ECU will be changed to ECU_NODE_STATUS_NO_MISSINGMSG.

\R{[$Satisfies $ADS_SSFM 051]} \R{[$Satisfies $ADS_SSFM 047]} \R{[$Satisfies $ADS_SSFM 049]}

\R{[$DDS_SSFM 183]}

Instead, if the SerialData is service-based the @ref Sum_Ssfm_prv_UpdateTrgDvcLostComm will be responsible to check the loss of comm at target device level. Given a Target Device,
all the serial datas that are provided by it at the moment will be assumed as fault and recovery serial datas. This means that whenever @ref Sum_Ssfm_prv_UpdateTrgDvcLostComm is called,
all the provided serial datas will be iterated on, and if there is a serial data in ECU_NODE_STATUS_MISSINGMSG status the target device will be in ECU_NODE_STATUS_MISSINGMSG status.
If there is no such serial data, then the target device status will be ECU_NODE_STATUS_NO_MISSINGMSG.

\R{[$Satisfies $ADS_SSFM 175]} \R{[$Satisfies $ADS_SSFM 047]} \R{[$Satisfies $ADS_SSFM 049]}

\R{[$DDS_SSFM 063]}

The @ref Sum_Ssfm_LoC_Dem_CB_DTCStatusChanged callback compares the input DTC Value with the DTC values linked to the
configured ECUs and update the Signal<n>\_LcFA indications for the SerialDatas linked to the target ECU.
If the current ECU is the Gateway module, the NodeStatusECU_<X>\[\_CAN\_<n>\] will be set as well.

\R{[$Satisfies $ADS_SSFM 063]} \R{[$Satisfies $ADS_SSFM 064]}

\R{[$DDS_SSFM 157]}

Moreover, the @ref Sum_Ssfm_LoC_Dem_CB_DTCStatusChanged gets the latest state of loss of comm, updates the Serial Data LcFA interface and availability accordingly. In the presence of Loss Of Comm with Provider (using bit 0 (TestFailed) of the Dem_UdsStatusByte) or reception of NodeSts Failure, the Serial Data is assumed to be in loss of comm.

\R{[$Satisfies $ADS_SSFM 042]} \R{[$Satisfies $ADS_SSFM 076]} \R{[$Satisfies $ADS_SSFM 064]}

\R{[$DDS_SSFM 064]}

If the current ECU is not the Gateway module, it monitor the interface NodeStatusECU_<X>\[\_CAN\_<n>\] provided by the
CGM (if applicable) using the function Runnable_SSFM_NodeStsECU_<X>\[\_CAN\_<n>\]_StatusChanged.
In this case, upon the reception of the notification the status will be recorded and used to calculate the signal
status availability.

When the notification is received and the new status is different than the old one, all the serial datas that are currently provided by the given controller, will be assessed for availability status. Moreover, the LcFA status of the mentioned serial datas will be updated accordingly.

\R{[$Satisfies $ADS_SSFM 063]} \R{[$Satisfies $ADS_SSFM 065]}


\R{[$DDS_SSFM 077]}

The @ref Sum_Ssfm_NodeLossError_Manager is responsible to check the disable DTC or system conditions to evaluate the maturation of node loss error. Whenever a change happens in the lost communication status of a signal group,
the function @ref Sum_Ssfm_NodeLossError_Manager will be called via the function @ref Sum_Ssfm_ErrhDebouncerExecution for all the controllers related to that signal group.

The @ref Sum_Ssfm_Process_SSM_NodeLossErrors_Handler is responsible to handle the state machine of ECU Node Loss Error maturation. 

The @ref Sum_Ssfm_Process_SSM_NodeLossErrors_Normal is responsible to handle the Normal state of ECU Node Loss Error maturation. Upon any transition from ECU_NODE_STATUS_MISSINGMSG to ECU_NODE_STATUS_NO_MISSINGMSG received from ECU node loss status shall change the state to SUM_SSFM_EcuNodeState_LostComm.

The @ref Sum_Ssfm_Process_SSM_NodeLossErrors_LossComm is responsible to handle the Loss of Communication state of ECU Node Loss Error maturation. Upon any transition from ECU_NODE_STATUS_NO_MISSINGMSG to ECU_NODE_STATUS_MISSINGMSG received from ECU node loss status shall change the state to SUM_SSFM_EcuNodeState_Normal.

The @ref Runnable_SSFM_NodeLossError_Time_Management is responsible for handling the timer evaluation that runs periodically at a fixed interval 1ms. For each ECU, whenever the DTCs status is Enabled and the elapsed time is equal to or greater than the configured calibration parameter Sum_Ssfm_LC_ECU_Timer_Ref, the temporary debouncing evaluation status is set to SUM_SSFM_EvaluationSts_Failed and the time elapsed is set to zero. Otherwise if the DTCs status is Enabled and the elapsed time condition is not satisfied, the temporary debouncing evaluation status is set to SUM_SSFM_EvaluationSts_InDebouncing and the elapsed time will increase with the fixed interval 1ms. On the other hand, if the DTCs status is Disabled the temporary debouncing evaluation status is set to SUM_SSFM_EvaluationSts_Unknown and the elapsed time is set to zero.

\R{[$Satisfies $ADS_SSFM 048]} \R{[$Satisfies $ADS_SSFM 076]} \R{[$Satisfies $ADS_SSFM 077]} 

\R{[$DDS_SSFM 100]}

If the current ECU configures target devices in the SPM module, it monitors the interface SPM_SSFM_LossOfNodeIndication provided by the SPM
using the function Runnable_SSFM_LossCommWithNode().
In this case, upon the reception of an event indication, the runnable will check for which target devices the SPM is reporting a new event
and it will update the relative LossOfCommunication DTCs' status accordingly.
The DTC status for each node is set through the DiagnosticMonitor.SetEventStatus interface from the DEM module.

\R{[$Satisfies $ADS_SSFM 049]} \R{[$Satisfies $ADS_SSFM 097]}


@subsection Sum_Ssfm_Errh_U19A3_92_Handler 3.1.9 Sum_Ssfm_Errh_U19A3_92_Handler

The Sum_Ssfm_Errh_U19A3_92_Handler is responsible for setting/resetting the failure status of U19A3_92 DTC. 
For the code generation of Sum_Ssfm_Errh_U19A3_92_Handler, two parameters must be configured to true:
	* Sum_Ssfm_Errh_CodeGen_Enabled, present in Sum_Ssfm_Errh_U19A3_92_SecPer_PerIncOp in SSFM module;
	* intgrtedSecrtyPeriphrlKeyStrgPresent present in SSC module.

@subsubsection Description_of_Runnable_SSFM_BSWM_DTC_U19A3_92_Status_Changed Description of Runnable_SSFM_BSWM_DTC_U19A3_92_Status_Changed
\R{[$DDS_SSFM 071]}

The DTC U19A3 92 corresponds to the failure of control module security peripheral because of performance or incorrect operation. Whenever a change happens in the status of the SecPError, the BSWM will notify the SSFM through the designated port. The SSFM triggers the @ref Runnable_SSFM_BSWM_DTC_U19A3_92_Status_Changed to handle the logic of the given DTC. 

\R{[$Satisfies $ADS_SSFM 067]} \R{[$Satisfies $ADS_SSFM 070]} \R{[$Satisfies $ADS_SSFM 071]} \R{[$Satisfies $ADS_SSFM 128]}

\R{[$DDS_SSFM 072]}

The @ref Runnable_SSFM_BSWM_DTC_U19A3_92_Status_Changed, first, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_SecurityPeripheral_PerformanceIncorrectOperation` is true or not. If it is set to false, the BSWM notification will be ignored.

\R{[$Satisfies $ADS_SSFM 066]} \R{[$Satisfies $ADS_SSFM 128]}

\R{[$DDS_SSFM 073]}

Then, the battery voltage range will be retrieved through SUM_SUSD. If the OBD is supported it is enough for the voltage to be not under voltage. Otherwise, the voltage should be strictly in normal range. If the battery voltage requirement is not satisfied, the notification from BSWM will be ignored.  

\R{[$Satisfies $ADS_SSFM 067]} \R{[$Satisfies $ADS_SSFM 070]} \R{[$Satisfies $ADS_SSFM 071]} \R{[$Satisfies $ADS_SSFM 128]}


\R{[$DDS_SSFM 074]}

Finally, the runnable will inform the Dem with the current status of DTC U19A3 92, setting it if the notification value from the BSWM is one of the following:
 * RTE_MODE_SECP_Error_Status_SUM_ERC_MEMORY_FAILURE
 * RTE_MODE_SECP_Error_Status_SUM_ERC_SEQUENCE_ERROR
 * RTE_MODE_SECP_Error_Status_SUM_ERC_GENERAL_FAILURE

otherwise, the DTC status will be reset.

\R{[$Satisfies $ADS_SSFM 067]} \R{[$Satisfies $ADS_SSFM 070]} \R{[$Satisfies $ADS_SSFM 071]}

@subsection Sum_Ssfm_RunCrankFault_Manager 3.1.10 Sum_Ssfm_RunCrankFault_Manager

The ERRH Run Crank Input Fault Indication Manager shall perform the below tasks if required by project specific deployment:

1. Receive the notifications from SUM_SUSD through the `Rte_Read_RP_SUSD_SSFM_RunCrankInputStuckHighDiagnosticStatus_HighStatus` and `Rte_Read_RP_SUSD_SSFM_RunCrankInputStuckLowDiagnosticStatus_LowStatus` to request setting clearing the corresponding DTC from Dem.
2. Notify Dem of the debouncing result via `Rte_Call_RP_Dem_SSFM_DiagnosticMonitor_RunCrankInputStuckHigh_SetEventStatus`, `Rte_Call_RP_Dem_SSFM_DiagnosticMonitor_RunCrankInputStuckLow_SetEventStatus` and public function @ref Sum_Ssfm_ErrhDebouncerExecution.
3. Receive the DTC status and value from Dem via callback  @ref Sum_Ssfm_RunCrank_DTCStatusChanged.
4. Compare the value of DTC with that sent by the Dem and, if condition is met, notify SUM_SUSD module of the run crank input stuck diagnostic DTC by using `Rte_Write_PP_SSFM_SUSD_RunCrankInputStuckHigh_FA_StatusRequest_FA_HighStatus_DTC_Confirm` and `Rte_Write_PP_SSFM_SUSD_RunCrankInputStuckLow_FA_StatusRequest_FA_LowStatus_DTC_Confirm`

\subsubsection Sum_Ssfm_Errh_Process_RunCrankInputStuckDiagnosticStatus 
\R{[$DDS_SSFM 065]}

The function @ref Sum_Ssfm_Errh_Process_RunCrankInputStuckDiagnosticStatus will be called by  runnables @ref Runnable_SSFM_RunCrankHighInputStuckDiagnosticStatus_Changed and @ref Runnable_SSFM_RunCrankLowInputStuckDiagnosticStatus_Changed to process Run Crank input stuck diagnostic status.

This function shall check the configured calibration parameter via getter function @ref Sum_Ssfm_Get_CalibPar_Sum_Ssfm_Errh_C_DTC_Process_RunCrankHighDiagSts and @ref Sum_Ssfm_Get_CalibPar_Sum_Ssfm_Errh_C_DTC_Process_RunCrankLowDiagSts whether to process the DTC.

\R{[$Satisfies $ADS_SSFM 072]} \R{[$Satisfies $ADS_SSFM 073]} \R{[$Satisfies $ADS_SSFM 069]}

\subsubsection Sum_Ssfm_Errh_Process_DTC_RunCrankInputStuckFault 
\R{[$DDS_SSFM 066]}

The function @ref Sum_Ssfm_Errh_Process_DTC_RunCrankInputStuckFault will be called by @ref Sum_Ssfm_Errh_Process_RunCrankInputStuckDiagnosticStatus to process DTC for Run Crank input stuck fault indication.

This function shall read the run crank input stuck diagnostic status via `Rte_Read_RP_SUSD_SSFM_RunCrankInputStuckHighDiagnosticStatus_HighStatus` and `Rte_Read_RP_SUSD_SSFM_RunCrankInputStuckLowDiagnosticStatus_LowStatus` from SUSD and perform checking condition then process the debouncing evaluation with @ref Sum_Ssfm_ErrhDebouncerExecution. Finally, the function shall set the event status to Dem by
`Rte_Call_RP_Dem_SSFM_DiagnosticMonitor_RunCrankInputStuckHigh_SetEventStatus` and `Rte_Call_RP_Dem_SSFM_DiagnosticMonitor_RunCrankInputStuckLow_SetEventStatus`.

\R{[$Satisfies $ADS_SSFM 072]} \R{[$Satisfies $ADS_SSFM 073]} \R{[$Satisfies $ADS_SSFM 069]}

\subsubsection Sum_Ssfm_Errh_Process_DTC_Confirmation 
\R{[$DDS_SSFM 067]}

The function @ref Sum_Ssfm_Errh_Process_DTC_Confirmation will be called by the callback @ref Sum_Ssfm_RunCrank_DTCStatusChanged to process DTC confirmation for Run Crank input stuck fault diagnostic.

Once the event status is received from the Dem via the previously mentioned callback, this function performs the DTC test failed bitmask and debouncer result, and then notifies SUSD by using `Rte_Write_PP_SSFM_SUSD_RunCrankInputStuckHigh_FA_StatusRequest_FA_HighStatus_DTC_Confirm` and `Rte_Write_PP_SSFM_SUSD_RunCrankInputStuckLow_FA_StatusRequest_FA_LowStatus_DTC_Confirm`.

\R{[$Satisfies $ADS_SSFM 072]} \R{[$Satisfies $ADS_SSFM 073]} \R{[$Satisfies $ADS_SSFM 068]} \R{[$Satisfies $ADS_SSFM 069]}

\subsubsection Runnable_SSFM_RunCrankHighInputStuckDiagnosticStatus_Changed 
\R{[$DDS_SSFM 068]}

Whenever a change happens to the status of run crank input stuck high diagnostic status from SUSD, @ref Runnable_SSFM_RunCrankHighInputStuckDiagnosticStatus_Changed will be called to process Run Crank input stuck high diagnostic status.

\R{[$Satisfies $ADS_SSFM 072]} \R{[$Satisfies $ADS_SSFM 073]} \R{[$Satisfies $ADS_SSFM 069]}

\subsubsection Runnable_SSFM_RunCrankLowInputStuckDiagnosticStatus_Changed 
\R{[$DDS_SSFM 069]}

Whenever a change happens to the status of run crank input stuck low diagnostic status from SUSD, @ref Runnable_SSFM_RunCrankLowInputStuckDiagnosticStatus_Changed will be called to process Run Crank input stuck low diagnostic status.

\R{[$Satisfies $ADS_SSFM 072]} \R{[$Satisfies $ADS_SSFM 073]} \R{[$Satisfies $ADS_SSFM 069]}

\subsubsection Sum_Ssfm_RunCrank_DTCStatusChanged 
\R{[$DDS_SSFM 070]}

The @ref Sum_Ssfm_RunCrank_DTCStatusChanged is a callback that is triggered when the DEM notifies a change in the status of a DTC and it evaluates the DTC confirmation for the Run Crank input stuck fault diagnostic.

\R{[$Satisfies $ADS_SSFM 072]} \R{[$Satisfies $ADS_SSFM 073]} \R{[$Satisfies $ADS_SSFM 068]} \R{[$Satisfies $ADS_SSFM 069]}

@subsection Sum_Ssfm_Errh_U19A3_8A_Handler 3.1.11 Sum_Ssfm_Errh_U19A3_8A_Handler

The Sum_Ssfm_Errh_U19A3_8A_Handler is responsible for setting/resetting the failure status of U19A3_8A DTC.
For the code generation of Sum_Ssfm_Errh_U19A3_8A_Handler, two parameters must be configured to true:
	* Sum_Ssfm_Errh_CodeGen_Enabled, present in Sum_Ssfm_Errh_U19A3_8A_CntrlModSecPer_AuthFailure in SSFM module;
	* inVehicleMessageAuthentictnSupprted in SSC module, configured to a non-zero value.
	
@subsubsection Runnable_SSFM_SSC_KeySlotVerifyErrors_keyslot_number
\R{[$DDS_SSFM 079]}

The DTC U19A3 8A corresponds to the failure of control module security peripheral because of authentication failure. 
Whenever SUM_SSFM-ERRH receives data of individual MAC verify failure events from SUM_SSC. The SSFM triggers the Runnable_SSFM_SSC_KeySlotVerifyErrors_<keyslot_number> to handle the logic of the given DTC. 

The Runnable_SSFM_SSC_KeySlotVerifyErrors_<keyslot_number> for each keyslot shall read the SSC data via `Rte_Read_SSC_SSFM_E_KeySlotVerify`. If there is any data then it will 
call @ref Sum_Ssfm_Errh_Process_KeySlotVerifyErrors to process the KeySlotVerifyErrors and reset the per-keyslot elapsed timer (`last_received_pdu_timer`) each time receiving a frame from SSC

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsubsection Runnable_SSFM_KeySlotVerifyErrors_Time_Management_id Runnable_SSFM_KeySlotVerifyErrors_Time_Management
\R{[$DDS_SSFM 091]}

The runnable @ref Runnable_SSFM_KeySlotVerifyErrors_Time_Management is responsible for monitoring the timing for keyslotverify that runs periodically at fixed interval 1ms. It shall execute @ref Sum_Ssfm_LastReceivedPDU_Timer_Update @ref Sum_Ssfm_Authentication_Timer_Manager to manage the timing following the specification.

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsubsection Sum_Ssfm_LastReceivedPDU_Timer_Update_id Sum_Ssfm_LastReceivedPDU_Timer_Update
\R{[$DDS_SSFM 092]}

The function @ref Sum_Ssfm_LastReceivedPDU_Timer_Update is responsible for tracking how long ago the last PDU was received on each key slot. For each key slot, it shall increment the per-keyslot elapsed timer (`last_received_pdu_timer`) unless it would overflow, in which case the timer is held at its current value.

\R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsubsection Sum_Ssfm_Authentication_Timer_Manager_id Sum_Ssfm_Authentication_Timer_Manager
\R{[$DDS_SSFM 093]}

The function @ref Sum_Ssfm_Authentication_Timer_Manager is responsible for managing passed/failed authentication timer of KeySlotVerifyError.
For each keyslot, it checks if KeySlotState equals $2 (Failed) AND FailedAuthenticationTimer exceeds k_ERRH_FailedAuthenticationTimer (Test Fail condition), or all keyslots are not equal to $2 with at least one equal to $1 (Passed) (Test Pass condition).
If Test Fail is detected, the function sets DEM_EVENT_STATUS_FAILED via `Rte_Call_RP_Dem_SSFM_DiagnosticMonitor_SSC_E_KeySlotVerifyError_SetEventStatus`. If Test Pass is detected, it sets DEM_EVENT_STATUS_PASSED.

At the end, this function calls @ref Sum_Ssfm_StartStopAuthenticationTimer_Handler to start/stop authentication timer handler.

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}


@subsubsection Sum_Ssfm_StartStopAuthenticationTimer_Handler_id Sum_Ssfm_StartStopAuthenticationTimer_Handler
\R{[$DDS_SSFM 094]}

The function @ref Sum_Ssfm_StartStopAuthenticationTimer_Handler is responsible for managing the authentication timers on a per-keyslot basis.
For each key slot, it shall check if the time since the last received PDU (`last_received_pdu_timer`) is less than three times the per-keyslot calibration `Sum_Ssfm_Errh_C_SlowestPDU_RateTimer`; if so, it shall decrement the `AuthenticationWindowTimer` and increment or reset the `FailedAuthenticationTimer`; otherwise both timers shall stop for that key slot.
Finally: @ref Sum_Ssfm_Update_KeySlotState shall be called for each keyslot to update KeySlotState based on updated timer values.

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsubsection Sum_Ssfm_Errh_KeySlotVerifyErrors_Handler_id  Sum_Ssfm_Errh_KeySlotVerifyErrors_Handler
\R{[$DDS_SSFM 097]}

The function @ref Sum_Ssfm_Errh_KeySlotVerifyErrors_Handler is responsible for handling the KeySlotVerifyErrors event for the input keyslot. It increases the num_of_failure for the input keyslot when eventStatus data from SSC is DEM_EVENT_STATUS_FAILED and sets it to zero when the data is DEM_EVENT_STATUS_PASSED.

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsubsection Sum_Ssfm_Errh_SecPError_Handler_id  Sum_Ssfm_Errh_SecPError_Handler
\R{[$DDS_SSFM 098]}

The function @ref Sum_Ssfm_Errh_SecPError_Handler is responsible for handling SecPError. It shall read SecPError status via `Rte_Mode_RP_BswM_SSFM_SecPError_SUM_ERC`. If there is no error, it shall convert the event status to DEM_EVENT_STATUS_PASSED or DEM_EVENT_STATUS_FAILED depending on the input from SSC data and execute @ref Sum_Ssfm_Errh_KeySlotVerifyErrors_Handler.

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsubsection Sum_Ssfm_Errh_Process_KeySlotVerifyErrors_id  Sum_Ssfm_Errh_Process_KeySlotVerifyErrors
\R{[$DDS_SSFM 099]}

The function @ref Sum_Ssfm_Errh_Process_KeySlotVerifyErrors is responsible for processing KeySlotVerifyErrors. It shall evaluate if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTCProcess_KeySlotVerifyError` is true or not. If it is set to true, the battery voltage range will be retrieved through SUM_SUSD. If the OBD is supported it is enough for the voltage to be not under voltage. Otherwise, the voltage should be strictly in normal range. If the battery voltage requirement is satisfied, SSFM shall check for SECP_Error via @ref Sum_Ssfm_Errh_SecPError_Handler and if its notification value from the BSWM has no error, SSFM shall process keyslotverify error.

\R{[$Satisfies $ADS_SSFM 080]} \R{[$Satisfies $ADS_SSFM 081]} \R{[$Satisfies $ADS_SSFM 082]} \R{[$Satisfies $ADS_SSFM 083]}

@subsection Sum_Ssfm_Service0x14_Handler 3.1.12 Sum_Ssfm_Service0x14_Handler

The Sum_Ssfm_Service0x14_Handler is responsible for supporting reception of clear diagnostic request and interpreting the request type per diagnostic definition. 

@subsubsection Sum_Ssfm_Errh_Reset_Internal_Statuses
\R{[$DDS_SSFM 110]}

The function @ref Sum_Ssfm_Errh_Reset_Internal_Statuses is responsible for reseting the internal variable statuses. This function shall go through all the configred @ref Sum_Ssfm_ErrhDebouncer_Event and reset all the variable statuses depending on theirs @ref DebouncerOwnerType.

\R{[$Satisfies $ADS_SSFM 108]} \R{[$Satisfies $ADS_SSFM 109]} \R{[$Satisfies $ADS_SSFM 110]}

@subsubsection Sum_Ssfm_Errh_Notification_SUSD
\R{[$DDS_SSFM 111]}

The function @ref Sum_Ssfm_Errh_Notification_SUSD is responsible for sending notification to SUSD. This function shall go through the configred `SSFM_ERRH_NUM_OF_CFG_ERROR_SUSD` and set the notification to **FALSE**.

\R{[$Satisfies $ADS_SSFM 108]} \R{[$Satisfies $ADS_SSFM 109]} \R{[$Satisfies $ADS_SSFM 110]}


@subsubsection Sum_Ssfm_Errh_ClearDiagnosticRequest_Confirmation
\R{[$DDS_SSFM 113]}

The function @ref Sum_Ssfm_Errh_ClearDiagnosticRequest_Confirmation is responsible for handling clear diagnostic request of ServiceRequest confirmation. 
If the input argument ConfirmationStatus is `DCM_RES_POS_OK`, this function shall clear application related DTC flags by calling the function @ref Sum_Ssfm_Errh_Reset_Internal_Statuses and @ref Sum_Ssfm_Errh_Notification_SUSD (if Run Crank feature is configured) and notifiy to the other modules as approval of the clear diagnostic request `Rte_Write_SUM_SSFM_PP_SSFM_ClearDiagReq_SSFMClearDiagReq`

Or else this function shall only notifiy to the other modules as rejection of the clear diagnostic request via `Rte_Write_SUM_SSFM_PP_SSFM_ClearDiagReq_SSFMClearDiagReq`

\R{[$Satisfies $ADS_SSFM 108]} \R{[$Satisfies $ADS_SSFM 109]} \R{[$Satisfies $ADS_SSFM 110]}

@subsubsection Runnable_SSFM_ServiceRequest_Confirmation
\R{[$DDS_SSFM 115]}

The runnable @ref Runnable_SSFM_ServiceRequest_Confirmation is responsible for handling the ServiceRequest upon a ClearDiagnosticInformation confirmation. This runnable is invoked when a ClearDiagnosticRequest is received from the Dcm. Depending on the input argument service ID, this runnable shall call the function @ref Sum_Ssfm_Errh_ClearDiagnosticRequest_Confirmation if the service ID is `SSFM_ERRH_DCM_CLEARDIAGNOSTICREQUEST` (0x14) to clear diagnostic request of ServiceRequest confirmation.

\R{[$Satisfies $ADS_SSFM 108]} \R{[$Satisfies $ADS_SSFM 109]} \R{[$Satisfies $ADS_SSFM 110]}

@subsection Sum_Ssfm_Errh_U19A3_54_Handler 3.1.13 Sum_Ssfm_Errh_U19A3_54_Handler

The Sum_Ssfm_Errh_U19A3_54_Handler is responsible for setting/resetting the failure status of U19A3_54 DTC.

@subsubsection Description_of_Runnable_SSFM_SSC_DTC_U19A3_54_Status_Changed Description of Runnable_SSFM_SSC_DTC_U19A3_54_Status_Changed
\R{[$DDS_SSFM 117]}

The DTC U19A3 54 corresponds to the failure of control module security peripheral because of missing setup. Whenever a change happens in the status of the DTC 19A3, the SSC will notify the SSFM through the designated port. The SSFM triggers the @ref Runnable_SSFM_SSC_DTC_U19A3_54_Status_Changed to handle the logic of the given DTC. 

\R{[$Satisfies $ADS_SSFM 113]} \R{[$Satisfies $ADS_SSFM 114]} \R{[$Satisfies $ADS_SSFM 115]}

\R{[$DDS_SSFM 118]}

The @ref Runnable_SSFM_SSC_DTC_U19A3_54_Status_Changed, first, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_SecurityPeripheral_MissingSetup` is true or not. If it is set to false, the SSC notification will be ignored.

\R{[$Satisfies $ADS_SSFM 116]}

\R{[$DDS_SSFM 119]}

Then, the battery voltage range will be retrieved through SUM_SUSD. The IVMAS_Status will be retrieved through SSC. If the IVMAS_Status indicates Key Provisioning is supported (value 1 or 3) and OBD operating voltage is strictly in normal range, further DTC status will be monitored. If the battery voltage requirement and IVMAS_Status condition are not satisfied, the notification from SUM_SUSD and SSC will be ignored.  

\R{[$Satisfies $ADS_SSFM 113]} \R{[$Satisfies $ADS_SSFM 114]} \R{[$Satisfies $ADS_SSFM 115]}


\R{[$DDS_SSFM 120]}

Finally, the runnable will inform the Dem with the current status of DTC U19A3 54, setting it if the notification value from the DTC_U19A3_54 is True.

otherwise, the DTC status will be reset.

\R{[$Satisfies $ADS_SSFM 113]} \R{[$Satisfies $ADS_SSFM 114]} \R{[$Satisfies $ADS_SSFM 115]}

@subsection Sum_Ssfm_Errh_U19A6_54_Handler 3.1.14 Sum_Ssfm_Errh_U19A6_54_Handler

The Sum_Ssfm_Errh_U19A6_54_Handler is responsible for setting/resetting the failure status of U19A6_54 DTC.

@subsubsection Description_of_Runnable_SSFM_SrvcMgr_DTC_U19A6_54_Status_Changed Description of Runnable_SSFM_SrvcMgr_DTC_U19A6_54_Status_Changed
\R{[$DDS_SSFM 160]}

The DTC U19A6 54 corresponds to the failure of Service Discovery module because of missing setup. Whenever a change happens in the status of the DTC 19A6 54, the SrvcMgr will notify the SSFM through the designated port. The SSFM triggers the @ref Runnable_SSFM_SrvcMgr_DTC_U19A6_54_Status_Changed to handle the logic of the given DTC. 

\R{[$Satisfies $ADS_SSFM 158]} \R{[$Satisfies $ADS_SSFM 159]} \R{[$Satisfies $ADS_SSFM 160]}

\R{[$DDS_SSFM 161]}

The @ref Runnable_SSFM_SrvcMgr_DTC_U19A6_54_Status_Changed, first, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_U19A6_54_MissingSetup` is true or not. If it is set to false, the SrvcMgr notification will be ignored.

\R{[$Satisfies $ADS_SSFM 158]} \R{[$Satisfies $ADS_SSFM 159]} \R{[$Satisfies $ADS_SSFM 160]} \R{[$Satisfies $ADS_SSFM 183]}

\R{[$DDS_SSFM 162]}

Then, the battery voltage range will be retrieved through SUM_SUSD. If OBD operating voltage is strictly in normal range, further DTC status will be monitored. If the battery voltage requirement is not satisfied, the notification from SrvcMgr will be ignored.  

\R{[$Satisfies $ADS_SSFM 158]} \R{[$Satisfies $ADS_SSFM 159]} \R{[$Satisfies $ADS_SSFM 160]}

\R{[$DDS_SSFM 163]}

Then, the MEC value will be retrieved through SUM_DIAG. According to the test enable criteria requirement, the condition of "MEC = 0" has to be met to enable the DTC. 
So, if the MEC value is not equal to zero, the notification from SrvcMgr will be ignored.  

\R{[$Satisfies $ADS_SSFM 158]} \R{[$Satisfies $ADS_SSFM 159]} \R{[$Satisfies $ADS_SSFM 160]}

\R{[$DDS_SSFM 164]}

Finally, the runnable will inform the Dem with the current status of DTC U19A6 54, setting it if the notification value from the DTC_19A6_54 is True.
otherwise, the DTC status will be reset.

\R{[$Satisfies $ADS_SSFM 158]} \R{[$Satisfies $ADS_SSFM 159]} \R{[$Satisfies $ADS_SSFM 160]}


@subsection Sum_Ssfm_Errh_U19A6_87_Handler 3.1.15 Sum_Ssfm_Errh_U19A6_87_Handler

The Sum_Ssfm_Errh_U19A6_87_Handler is responsible for setting/resetting the failure status of U19A6_87 DTC.

@subsubsection Description_of_Sum_Ssfm_U19A6_87_Update Description of Sum_Ssfm_U19A6_87_Update

\R{[$DDS_SSFM 166]}

The DTC U19A6 87 corresponds to the failure of Service Discovery module because of missing message. Whenever SSFM tries to resolve the provider ip address of a service the @ref Sum_Ssfm_U19A6_87_Update will be called to assess the status of the DTC U19A6 87.

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

\R{[$DDS_SSFM 167]}

The @ref Sum_Ssfm_U19A6_87_Update takes a boolean value interpreted as the direct failure.
If an ip resolution is failed, then the @ref Sum_Ssfm_U19A6_87_Update will be called with direct_failure equal to true, meaning that no further assessment is required and the DTC value should be set to true directly, if the enable criteria is met.

If the function is called with a false input, the @ref Sum_Ssfm_U19A6_87_Update iterates over the services and see if there is an unresolved ip address (ip == 0.0.0.0). If there is such service, the DTC is assessed as failed. If there is no such service, the DTC is passed.

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

\R{[$DDS_SSFM 168]}

The @ref Sum_Ssfm_U19A6_87_Update, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_U19A6_87_MissingMessage` is true or not. If it is set to false, the DTC handling will be ignored.

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} \R{[$Satisfies $ADS_SSFM 183]} 

\R{[$DDS_SSFM 169]}

Then, the battery voltage range will be retrieved through SUM_SUSD. If OBD operating voltage is strictly in normal range, further DTC status will be monitored. If the battery voltage requirement is not satisfied, the DTC handling will be ignored.  

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

\R{[$DDS_SSFM 170]}

Then, the LearningTableState is retrieved from SUM_SrvcMgr. According to the test enable criteria requirement, the condition of "LearningTableState = Complete || LearningTableState = Incomplete" has to be met to enable the DTC.
So, if the LearningTableState value is not equal to Complete and Incomplete, the DTC handling will be ignored.

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

\R{[$DDS_SSFM 171]}

Finally, the function will inform the Dem with the current status of DTC U19A6 87.

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

\R{[$DDS_SSFM 364]}

@subsubsection Runnable_SSFM_LearningTableStatus

The @ref Runnable_SSFM_LearningTableStatus is a runnable that shall be triggered by an RTE data received event whenever the Learning Table Status changes from the Service Manager (SUM_SrvcMgr).

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

\R{[$DDS_SSFM 216]}

@subsubsection Missing Service Notification
The missing-service notification is performed inside `Sum_Ssfm_ResolveServiceDemEvent` when service resolution fails (e.g., SD API error or resolved IP equals 0).
The behavior is to notify SrvcMgr via the `SSFM_SrvcMgr_SDMissingMessage` port with the unresolved `ServiceID`.

\R{[$Satisfies $ADS_SSFM 163]} \R{[$Satisfies $ADS_SSFM 164]} \R{[$Satisfies $ADS_SSFM 165]} 

@subsection Sum_Ssfm_Errh_U198C_8A_Handler 3.1.16 Sum_Ssfm_Errh_U198C_8A_Handler

The Sum_Ssfm_Errh_U198C_8A_Handler is responsible for setting/resetting the failure status of U198C_8A DTC.

@subsubsection Description_of_Runnable_SSFM_U198C_8A_Status_Changed Description of Runnable_SSFM_U198C_8A_Status_Changed
\R{[$DDS_SSFM 174]}

The DTC U19A6 54 corresponds to the Primary Security Information Authentication Failure. Whenever a change happens in the status of the DTC U198C_8A, the SSC will notify the SSFM through the designated port. The SSFM triggers the @ref Runnable_SSFM_U198C_8A_Status_Changed to handle the logic of the given DTC. 

\R{[$Satisfies $ADS_SSFM 169]} \R{[$Satisfies $ADS_SSFM 170]} \R{[$Satisfies $ADS_SSFM 171]}

\R{[$DDS_SSFM 175]}

The @ref Runnable_SSFM_U198C_8A_Status_Changed, first, evaluates if vehicle region signal availability is checked along with its FsFA. If signal is not available or FsFA is set then the SSC notification will be ignored.

\R{[$Satisfies $ADS_SSFM 169]} \R{[$Satisfies $ADS_SSFM 170]} \R{[$Satisfies $ADS_SSFM 171]}

\R{[$DDS_SSFM 177]}

The X/Y debouncing method is used to set or clear the DTC code U198C 8A. The debouncer status is evaluated, and the event status is updated in DEM accordingly.

\R{[$Satisfies $ADS_SSFM 169]} \R{[$Satisfies $ADS_SSFM 170]} \R{[$Satisfies $ADS_SSFM 171]}


\R{[$Satisfies $ADS_SSFM 169]} \R{[$Satisfies $ADS_SSFM 170]} \R{[$Satisfies $ADS_SSFM 171]}

\R{[$DDS_SSFM 178]}

Additionally, to enable the test, 5 seconds should be passed from the latest recovery from low or high voltage, as well as from the latest reset or power up. This information is provided by @ref Sum_Ssfm_Errh_GetResetPowerUpRecoveryTimerFlag and @ref Sum_Ssfm_Errh_GetBatteryVoltageRecoveryTimerFlag.
If any of the mentioned flags are not set, the SSC notification will be ignored.

\R{[$Satisfies $ADS_SSFM 169]} \R{[$Satisfies $ADS_SSFM 170]} \R{[$Satisfies $ADS_SSFM 171]}


\R{[$DDS_SSFM 179]}

Finally, the runnable will inform the Dem with the current status of DTC U198C_8A, setting it if the notification value from the DTC_U198C_8A is True.
Otherwise, the DTC status will be reset.

\R{[$Satisfies $ADS_SSFM 169]} \R{[$Satisfies $ADS_SSFM 170]} \R{[$Satisfies $ADS_SSFM 171]}

@subsection Sum_Ssfm_Errh_U198C_54_Handler 3.1.17 Sum_Ssfm_Errh_U198C_54_Handler

The Sum_Ssfm_Errh_U198C_54_Handler is responsible for setting/resetting the failure status of U198C_54 DTC.

@subsubsection Description_of_Runnable_SSFM_U198C_54_Status_Changed Description of Runnable_SSFM_U198C_54_Status_Changed
\R{[$DDS_SSFM 184]}

The DTC U198C 54 corresponds to the Missing Setup/Learn Failure. Whenever a change happens in the status of the DTC U198C_54, the SSC will notify the SSFM through the designated port. 
The SSFM triggers the @ref Runnable_SSFM_U198C_54_Status_Changed to handle the logic of the given DTC. 

\R{[$Satisfies $ADS_SSFM 177]} \R{[$Satisfies $ADS_SSFM 178]} \R{[$Satisfies $ADS_SSFM 179]}

\R{[$DDS_SSFM 185]}

The @ref Runnable_SSFM_U198C_54_Status_Changed, first, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_U198C_54_MissSetup` is true or not. If it is set to false, the SSC notification will be ignored.

\R{[$Satisfies $ADS_SSFM 177]} \R{[$Satisfies $ADS_SSFM 178]} \R{[$Satisfies $ADS_SSFM 179]}

\R{[$DDS_SSFM 186]}

Then, to enable the test, 5 seconds should be passed from the latest recovery from low or high voltage, as well as from the latest reset or power up. This information is provided by @ref Sum_Ssfm_Errh_GetResetPowerUpRecoveryTimerFlag and @ref Sum_Ssfm_Errh_GetBatteryVoltageRecoveryTimerFlag.
If any of the mentioned flags are not set, the SSC notification will be ignored.

\R{[$Satisfies $ADS_SSFM 177]} \R{[$Satisfies $ADS_SSFM 178]} \R{[$Satisfies $ADS_SSFM 179]}

\R{[$DDS_SSFM 187]}


The status of the DTC U198C_54 is read from SSC and a filter (debouncing) mechanism is applied.

The X/Y debouncing method is used to set or clear the DTC code U198C 54. The debouncer status is evaluated, and the event status is updated in DEM accordingly.

\R{[$Satisfies $ADS_SSFM 177]} \R{[$Satisfies $ADS_SSFM 178]} \R{[$Satisfies $ADS_SSFM 179]}

\R{[$DDS_SSFM 194]}

The U198C_54 status is informed to Dem through the `DiagnosticMonitor` interface.

\R{[$Satisfies $ADS_SSFM 177]}

\R{[$DDS_SSFM 213]}

The @ref Sum_Ssfm_Clear_KeySlot_Authentication_Failure_Timers is called by @ref Sum_Ssfm_Errh_Reset_Internal_Statuses in order to reset AuthenticationWindowTimer and FailedAuthenticationTimer for each KeySlot configured. 

\R{[$Satisfies $ADS_SSFM 108]}

@subsection Sum_Ssfm_Errh_U198C_49_Handler 3.1.18 Sum_Ssfm_Errh_U198C_49_Handler

The Sum_Ssfm_Errh_U198C_49_Handler is responsible for setting/resetting the failure status of U198C_49 DTC, which corresponds to the Primary Security Information - Key Provisioning.

@subsubsection Description_of_Runnable_SSFM_U198C_49_Status_Changed Description of Runnable_SSFM_U198C_49_Status_Changed

Whenever a change happens in the status of the DTC U198C_49, the SSC will notify the SSFM through the designated port. The SSFM triggers the Runnable_SSFM_U198C_49_Status_Changed to handle the logic of the given DTC.

\R{[$DDS_SSFM 191]}

* The Runnable_SSFM_U198C_49_Status_Changed, first, evaluates if the required calibration parameter Sum_Ssfm_Errh_U198C_49_DTCProcess is true or not. If it is set to false, the SSC notification will be ignored.
* It verifies that both `ResetPowerUpRecoveryTimerFlag` and `BatteryVoltageRecoveryTimerFlag` are set to TRUE, confirming that sufficient time has passed since the last power-up or battery recovery.
* The KP Keys Provisioned flag, calculated in the SSC module, is checked. 
		If keys are not provisioned, the DTC is set as failed. 
		If keys are provisioned, the filter (debouncing) mechanism is applied.

\R{[$Satisfies $ADS_SSFM 187]}  \R{[$Satisfies $ADS_SSFM 188]} 
\R{[$DDS_SSFM 192]}

* The X/Y debouncing method is used to set or clear the DTC code U198C_49. The debouncer status is evaluated, and the event status is updated in DEM accordingly.

\R{[$Satisfies $ADS_SSFM 188]}

\R{[$DDS_SSFM 193]}

The U198C_49 status is informed to Dem through the `DiagnosticMonitor` interface.

\R{[$Satisfies $ADS_SSFM 187]}  \R{[$Satisfies $ADS_SSFM 189]}


@subsection Sum_Ssfm_Errh_U19A6_57_Handler 3.1.19 Sum_Ssfm_Errh_U19A6_57_Handler

The Sum_Ssfm_Errh_U19A6_57_Handler is responsible for setting/resetting the failure status of U19A6_57 DTC.

@subsubsection Description_of_Runnable_SSFM_SrvcMgr_DTC_U19A6_57_Status_Changed Description of Runnable_SSFM_SrvcMgr_DTC_U19A6_57_Status_Changed
\R{[$DDS_SSFM 195]}

The primary purpose of this DTC is to serve as an indicator to know when DID E0CE should be read for diagnostic information about failed service acknowledgments.

The SrvcMgr will notify the SSFM through the boolean interface `SrvcMgr_SSFM_SrvcDscvryFlr_U19A6_57` to indicate if the test has failed or not (True means failure). The SSFM triggers the @ref Runnable_SSFM_SrvcMgr_DTC_U19A6_57_Status_Changed with a Data Receive Event to handle the logic of the given DTC.

\R{[$Satisfies $ADS_SSFM 190]} \R{[$Satisfies $ADS_SSFM 191]} \R{[$Satisfies $ADS_SSFM 192]}

\R{[$DDS_SSFM 196]}

The @ref Runnable_SSFM_SrvcMgr_DTC_U19A6_57_Status_Changed, first, evaluates if the required calibration parameter `Rte_CData_Sum_Ssfm_Errh_C_DTC_U19A6_57_PhyECUInvIncComp` is true or not. If it is set to false, the SrvcMgr notification will be ignored.

\R{[$Satisfies $ADS_SSFM 190]} \R{[$Satisfies $ADS_SSFM 191]} \R{[$Satisfies $ADS_SSFM 192]}

\R{[$DDS_SSFM 197]}

Then, the battery voltage range will be retrieved through SUM_SUSD. If OBD operating voltage is strictly in normal range, further DTC status will be monitored. If the battery voltage requirement is not satisfied, the notification from SrvcMgr will be ignored.

\R{[$Satisfies $ADS_SSFM 190]} \R{[$Satisfies $ADS_SSFM 191]} \R{[$Satisfies $ADS_SSFM 193]}

\R{[$DDS_SSFM 198]}

The status of DTC U19A6_57 is read from SrvcMgr via the boolean interface `SrvcMgr_SSFM_SrvcDscvryFlr_U19A6_57` and a filter (debouncing) mechanism is applied.

A configuration container has been created for U19A6_57 with a linked debouncer type. The DebouncerOwner has been configured with appropriate start/check debouncing functions and callback mechanisms.

The X/Y debouncing method is used to set or clear the DTC code U19A6 57. The debouncer status is evaluated, and the event status is updated in DEM accordingly.

\R{[$Satisfies $ADS_SSFM 190]} \R{[$Satisfies $ADS_SSFM 191]} \R{[$Satisfies $ADS_SSFM 193]}

\R{[$DDS_SSFM 199]}

Finally, the runnable will inform the Dem with the current status of DTC U19A6 57, setting it if the notification value from the DTC_U19A6_57 is True.
Otherwise, the DTC status will be reset.

\R{[$Satisfies $ADS_SSFM 190]} \R{[$Satisfies $ADS_SSFM 191]} \R{[$Satisfies $ADS_SSFM 193]}

@subsection Sum_Ssfm_errh_Bsw_errors_manager 3.1.20 Sum_Ssfm_errh_Bsw_errors_manager

BSW Errors Manager shall implement the logic to filter all BSW related events which are sent to DEM. The DEM module will notify ERRH when a BSW event status has changed, and ERRH will filter this event according to the configuration.

\R{[$DDS_SSFM 122]}
BSW Errors Manager shall debounce all BSW errors as per the configuration. It is assumed, each of the events are configured in the DEM module for the specific ECU type. ERRH shall be responsible for setting the DEM event status based on the outcome of filtering.

\R{[$Satisfies $ADS_SSFM 117]} \R{[$Satisfies $ADS_SSFM 118]}

\R{[$DDS_SSFM 123]}
Each BSW DEM event shall have the ability to have a corresponding DEM event which triggers a DTC. This specific implementation is project specific as not all projects will require every BSW module or every DEM event. If the filtered DEM event fails, the corresponding DTC shall be set, likewise if the DEM event passes, the corresponding DTC shall be cleared.

\R{[$Satisfies $ADS_SSFM 117]} \R{[$Satisfies $ADS_SSFM 118]}

\R{[$DDS_SSFM 124]}
ERRH shall be responsible for Notifying specific BSW error to SOH, and storing confirmed CAN BusOff Dtc's.

\R{[$Satisfies $ADS_SSFM 119]}


@subsection DID_Handler 3.1.21 DID_Handler
@subsubsection Sum_Ssfm_DID_E0B9_Handler

\R{[$DDS_SSFM 125]}

The DID 0xE0B9 is responsible for providing the last 16 SerialDatas that went to Unavailable status. Upon any change in the signal status, the @ref Sum_Ssfm_DID_E0B9_Update_Message_Av will be informed and the list will be updated. 
When a message becomes Unavailable the following logic will be applied:
* If the message is already in the list, the position of the message will not be changed. Only the status, and the duration of unavailability will be changed.
* If the message is not in the list, if there is an empty space in the list (the list contains less than 16 messages), the new message will be added to the first empty space.
* If the message is not in the list and there is no empty space, the oldest available/disabled/uninitialized message in the list will be replaced by the new message.

\R{[$Satisfies $ADS_SSFM 120]} \R{[$Satisfies $ADS_SSFM 121]} 

\R{[$DDS_SSFM 128]}

The content of the DID shall be stored in the NvM. 

A proper configuration of one NvM block is needed in order to guarantee: 
* RAM buffer associated to it (which name is retrieved by the NvMRamBlockDataAddress parameter of a NvMBlocKDescriptor) is populated directly by NvM at startup of the ECU.  
* The content of the RAM buffer is saved at the shutdown. At this end, the RAM buffer is updated during the transition of the ERRH Execution Manager from SUM_SSFM_Run_Mode state to SUM_SSFM_Shutdown_Mode one. 

\R{[$Satisfies $ADS_SSFM 126]} 

\R{[$DDS_SSFM 129]}

During the SSFM init phase, data stored in the RAM buffer are used to set Availabality status and timer offset of previous session w.r.t. the corresponding
Serial Data message.

\R{[$Satisfies $ADS_SSFM 126]} 

\R{[$DDS_SSFM 131]}

The value of the timer stored in the NvM alongside the message ID shall stop incrementing when the ECU goes to sleep and resume incrementing when the ECU
wakes up. So, at run time this value is added to the actual unavailability time if the message associated is set as unavailable since startup.

\R{[$Satisfies $ADS_SSFM 126]} 

\R{[$DDS_SSFM 132]}

When the Clear DTC (UDS 0x14) is performed, the RAM buffer and the internal buffer Sum_Ssfm_DID_E0B9_SerialDatas_InList are reset.

\R{[$Satisfies $ADS_SSFM 109]} 


\R{[$DDS_SSFM 126]}

Upon the request of the DID 0xE0B9 the @ref Runnable_SSFM_DID_E0B9 will be called and the buffer will be filled as described in the SDV-6001.

\R{[$Satisfies $ADS_SSFM 120]} \R{[$Satisfies $ADS_SSFM 121]} 

\R{[$DDS_SSFM 190]}

The @ref Runnable_SSFM_E0B9_FreezeFrame will be called by the Dem to store the current state of the DID 0xE0B9 data in the Freeze Frame buffer.

\R{[$Satisfies $ADS_SSFM 185]} \R{[$Satisfies $ADS_SSFM 186]} \R{[$Satisfies $ADS_SSFM 120]} 

@subsubsection Sum_Ssfm_DID_E0B8_Handler

\R{[$DDS_SSFM 137]}

Upon the report of an unavailability status, a continuous operation error, or a protection error, @ref Sum_Ssfm_DID_E0B8_MetricUpdated will be called to inform the E0B8 Handler about the error. 
The @ref Sum_Ssfm_DID_E0B8_MetricUpdated increments the corresponding counter and adds the serial data to the list if 
there is enough space and the serial data is not already on the list.

\R{[$Satisfies $ADS_SSFM 133]} \R{[$Satisfies $ADS_SSFM 134]}

\R{[$DDS_SSFM 138]}

The @ref Runnable_SSFM_DID_E0B8 is responsible for filling the Dcm buffer given the internal structure of the E0B8.
Whenever the DID is requested @ref Runnable_SSFM_DID_E0B8 iterates over the list of messages in E0B8 list, and fills out the 
buffer with correct data.

\R{[$Satisfies $ADS_SSFM 133]} \R{[$Satisfies $ADS_SSFM 134]}

\R{[$DDS_SSFM 139]}

The @ref Runnable_SSFM_DID_E0B8_RDL provides the number of messages in the E0B8 list whenever asked by RTE.

\R{[$Satisfies $ADS_SSFM 133]} \R{[$Satisfies $ADS_SSFM 134]}

\R{[$DDS_SSFM 140]}

The @ref Sum_Ssfm_DID_E0B8_ResetData will be called upon the reception of Clear Diagnostic. It resets all the internal lists and counters of E0B8.

\R{[$Satisfies $ADS_SSFM 133]} \R{[$Satisfies $ADS_SSFM 134]}

\R{[$DDS_SSFM 141]}

The @ref Sum_Ssfm_DID_E0B8_SaveData is reponsible for updating the buffer during the shutdown, so that the NvM can store the most updated state of E0B8.

\R{[$Satisfies $ADS_SSFM 133]} \R{[$Satisfies $ADS_SSFM 134]}

\R{[$DDS_SSFM 142]}

The @ref Sum_Ssfm_DID_E0B8_RestoreData is responsible for reading the E0B8 buffer from NvM at the time of initialization and initializing the list and counters of E0B8 Handler and Serial Datas.

\R{[$Satisfies $ADS_SSFM 133]} \R{[$Satisfies $ADS_SSFM 134]}

@subsubsection Sum_Ssfm_DID_E0BB_Handler

\R{[$DDS_SSFM 210]}

The @ref Runnable_SSFM_DID_E0BB is responsible for filling the DCM buffer given the internal structure of the E0BB. 
Whenever it is called, the @ref Runnable_SSFM_DID_E0BB : 

* compute the number of KeySlot supported. (Round the configured KeySlot count up to the next supported value. The allowed supported values are exactly: 0, 10, 20, 50, 100, 150, 200, 255. If the configured count is already one of these, use it as-is; otherwise return the next higher value in that list);
* iterate over all the Sum_Ssfm_Errh_KeySlotVerifyFilter[] buffer in order to retrieve the status slot (values $0, $1 or $2), the DTC ID and the failure timer;
* if the configured KeySlot count is less than the computed supported value, create filler entries for the difference and mark them with status $3 (Unusued);

\R{[$Satisfies $ADS_SSFM 202]} 

\R{[$DDS_SSFM 211]}

The @ref Runnable_SSFM_DID_E0BB_ReadDataLength is responsible to provide the length of the payload that will be retrieved. 
The number of key slots supported determines the length of the DID:
- 0 slots = 1 byte
- 10 slots = 28 bytes
- 20 slots = 58 bytes
- 50 slots = 148 bytes
- 100 slots = 298 bytes
- 150 slots = 448 bytes
- 200 slots = 598 bytes
- 255 slots = 763 bytes

Its computation is: 1 byte + 3 bytes * (numberOfSupportedKeySlot - 1), where numberOfSupportedKeySlot is the result of rounding up the configured key slot count (minus 1 for excluding Key Slot 1 (ECU Unlock Key) from reporting in the DID E0BB) to the next supported value. When 0 slots are configured, the length is 1 byte.

\R{[$Satisfies $ADS_SSFM 202]} 

\R{[$DDS_SSFM 212]}

The @ref Runnable_SSFM_DID_E0BB_CheckRead populate the ErrorCode as E_OK if the OpStatus is DCM_INITIAL or DCM_PENDING.

\R{[$Satisfies $ADS_SSFM 202]} 

\R{[$DDS_SSFM 214]}

The content of this DID is stored in the NvM block referenced by Sum_Ssfm_MACVerPerKSNvmRef when the ERRH State is SUM_SSFM_Shutdown_Mode by calling the function @ref Sum_Ssfm_DID_E0BB_SaveData

\R{[$Satisfies $ADS_SSFM 205]} 

\R{[$DDS_SSFM 215]}

The @ref Sum_Ssfm_DID_E0BB_RestoreData is responsible for reading the E0BB buffer from NvM at initialization time. It restores the MAC verification per key slot data, enabling accumulation of status information across ignition cycles. The function validates the restored data and initializes the internal structures used by the E0BB handler.

\R{[$Satisfies $ADS_SSFM 204]} 

\R{[$DDS_SSFM 217]}

The @ref Runnable_SSFM_E0BB_FreezeFrame will be called by the Dem to store the current state of the DID 0xE0BB data in the Freeze Frame buffer.

\R{[$Satisfies $ADS_SSFM 206]} \R{[$Satisfies $ADS_SSFM 207]} \R{[$Satisfies $ADS_SSFM 202]} 

@subsubsection Sum_Ssfm_DID_E0BA_Handler

\R{[$DDS_SSFM 201]}

The DID 0xE0BA is responsible for providing signal status information about Signal Protection Calculation failures. This DID provides a list of up to 16 messages with counters associated with failed protection indications (FPI) and failed continuous operation indications (FOI). The @ref Sum_Ssfm_DID_E0BA_Update_Message_Protection will be informed upon any signal protection calculation failure event, and the list will be updated accordingly.

\R{[$Satisfies $ADS_SSFM 198]}

\R{[$DDS_SSFM 202]}

When a message reports signal protection failures, the following logic is applied:
* Messages with the highest total counters for FPI or FOI are kept in the DID buffer
* If the message is already in the list, the existing entry is updated with new counter values and repositioned based on priority
* If the message is not in the list and there is space (less than 16 messages), the new message is added
* If the message is not in the list and the buffer is full, the message with the lowest total count (FPI + FOI) is replaced if the new message has a higher total count
* The FPI and FOI counters continue to increment until a new message is reported with higher counters.

\R{[$Satisfies $ADS_SSFM 199]}

\R{[$DDS_SSFM 203]}

The DID 0xE0BA uses an automatic sorting mechanism based on total failure count (ascending order). This ensures that:
* Messages with the lowest total failure count are positioned first in the buffer
* When the buffer is full, the message with the lowest priority can be easily identified and replaced
* The sorting maintains both total count priority and FIFO order for equal counts using message ID comparison

\R{[$Satisfies $ADS_SSFM 198]}

\R{[$DDS_SSFM 204]}

The content of the DID shall be stored in NvM using a 96-byte fixed-length buffer. Each message entry consists of:
* Message ID (4 bytes): CAN ID for CAN messages and PDU Header ID for Ethernet messages
* Failed Continuous Operation Indication Counts (1 byte): Range 0-255
* Failed Protection Indication Counts (1 byte): Range 0-255

A proper configuration of one NvM block is needed to guarantee:
* RAM buffer is populated directly by NvM at startup of the ECU
* The content of the RAM buffer is saved at shutdown during transition from SUM_SSFM_Run_Mode to SUM_SSFM_Shutdown_Mode

\R{[$Satisfies $ADS_SSFM 200]}

\R{[$DDS_SSFM 205]}

The @ref Runnable_SSFM_DID_E0BA is responsible for filling the DCM buffer with the current E0BA data. When the DID is requested, this runnable copies the fixed 96-byte response from the RAM buffer to the DCM data buffer as described in the diagnostic specification.

\R{[$Satisfies $ADS_SSFM 198]}

\R{[$DDS_SSFM 206]}

The @ref Sum_Ssfm_DID_E0BA_ResetData will be called upon reception of Clear DTC (UDS 0x14) service. It clears the internal deque structure and resets the RAM buffer, ensuring all counters and message entries are set to zero (default/cleared values).

\R{[$Satisfies $ADS_SSFM 200]}

\R{[$DDS_SSFM 207]}

The @ref Sum_Ssfm_DID_E0BA_SaveData is responsible for updating the RAM buffer during shutdown, so that NvM can store the most updated state of E0BA data. This function is called during the transition to shutdown mode.

\R{[$Satisfies $ADS_SSFM 201]}

\R{[$DDS_SSFM 208]}

The @ref Sum_Ssfm_DID_E0BA_RestoreData is responsible for reading the E0BA buffer from NvM at initialization time. It reconstructs the internal deque structure from the persisted data and validates the restored entries for consistency. Only entries with non-zero message IDs are restored to the active list.

\R{[$Satisfies $SRS_SSFM 200]}

\R{[$DDS_SSFM 209]}

Count capping mechanism ensures that FOI and FPI counters do not exceed the maximum value of 255. When the occurrence count from the FxI framework exceeds this limit, the counter is capped at 255 to prevent overflow and maintain diagnostic data integrity.

\R{[$Satisfies $SRS_SSFM 198]}

@subsubsection Sum_Ssfm_DID_F094_Handler
\R{[$DDS_SSFM 143]}

this DID shall export the result of monitoring SUM_SOH local supervision failures reported by Watchdog
Manager.

* SUM_ERRH_DID_SOHSupFail_Cntr : a 2-byte unsigned integer.  
* SOH_Local_Supervision_Status : a 1-byte enumeration.

\R{[$Satisfies $ADS_SSFM 135]}

Through operation invoked events those values are passed to the DCM. Three Runnables are used :

\R{[$DDS_SSFM 144]}

* Runnable_DID_F094_SUM_ERRH_DID_SOHSupFail_Cntr to pass SUM_ERRH_DID_SOHSupFail_Cntr.

\R{[$Satisfies $ADS_SSFM 135]} \R{[$Satisfies $ADS_SSFM 136]}

\R{[$DDS_SSFM 145]}

* Runnable_DID_F094_SUM_SOHLocalSupervisionStatus to pass SOH_Local_Supervision_Status.

\R{[$Satisfies $ADS_SSFM 135]} \R{[$Satisfies $ADS_SSFM 136]}

\R{[$DDS_SSFM 146]}

* Runnable_ERRH_DID_F094_CheckRead to allow the read.

\R{[$Satisfies $ADS_SSFM 135]} \R{[$Satisfies $ADS_SSFM 136]}

\R{[$DDS_SSFM 147]}

The Detector shall use the threshold during a time window filtering method to monitor for occurrences the SUM_SOH local supervision failures.
To handle the Failed Local Supervision, a Time Window filter is used with the following variables :

* Counter :SUM_ERRH_SOHSupFail_Cntr
* Counter Threshold :SUM_ERRH_SOHSupFail_Thresh
* Time Window :SUM_ERRH_SOHSupFail_TW
* Time Window Threshold :SUM_ERRH_SOHSupFail_TW_Thresh
* DID :SUM_ERRH_DID_SOHSupFail_Cntr

The time window filter is updated every second through the runnable Runnable_SUM_ERRH_Timing.
Such runnable uses the SUM_TCS interface TCS_SWC_Clock which provides the current timestamp with 100 ms resolution.

\R{[$Satisfies $ADS_SSFM 135]} \R{[$Satisfies $ADS_SSFM 136]}

\R{[$DDS_SSFM 148]}

The Runnable_WdgMReliabilityMonitor_ERRH_FailedLocalSupervision which is invoked when one of the following three events occurs:
* The local supervision status of the SUM_SOH transitions from WDGM_LOCAL_STATUS_OK to WDGM_LOCAL_STATUS_FAILED
* The local supervision status of the SUM_SOH transitions from WDGM_LOCAL_STATUS_OK to WDGM_LOCAL_STATUS_EXPIRED
* The local supervision status of the SUM_SOH transitions from WDGM_LOCAL_STATUS_FAILED to WDGM_LOCAL_STATUS_EXPIRED

When the runnable is triggered, it increases the SUM_ERRH_SOHSupFail_Cntr by one unit and calls ERRH_Filter. If the DID has changed, the DID is saved in the NvM.

\R{[$Satisfies $ADS_SSFM 135]} \R{[$Satisfies $ADS_SSFM 136]}


@subsection Sum_Ssfm_DTCTriggeredMessage_Generator_Handler 3.1.22 Sum_Ssfm_DTCTriggeredMessage_Generator_Handler

\R{[$DDS_SSFM 127]}

The DTCTriggered Message Generator Handler runnable is responsible for transmitting the DTC_Triggered message for every DTC configured within the system.

\subsubsection Sum_Ssfm_Errh_CheckDTCStatus_And_Enqueue_id Sum_Ssfm_Errh_CheckDTCStatus_And_Enqueue

The @ref Sum_Ssfm_Errh_CheckDTCStatus_And_Enqueue is responsible for evaluating the status changes of a Diagnostic Trouble Code (DTC) and enqueuing the DTC for further processing if necessary.

This function checks for transitions in the DTC's status by comparing the old status (DTCStatusOld) and the new status (DTCStatusNew). The function enqueues the DTC only when the TestFailed bit (bit 0) changes from 0 to 1. The enqueue operation is protected from interruptions by using RTE functions to enter and exit critical sections.

\subsubsection Sum_Algo_Deque_id Sum_Algo_Deque

The SSFM makes use of Sum_Algo_Deque double ended queue API to keep track of the DTC queue. For more information about the API refer to Design Document of SUM_COMMON.

\R{[$Satisfies $ADS_SSFM 122]} \R{[$Satisfies $ADS_SSFM 123]} \R{[$Satisfies $ADS_SSFM 124]} \R{[$Satisfies $ADS_SSFM 125]}

@subsection Runnable_SSFM_DTCTriggeredMessage_Handler 3.1.18 Runnable_SSFM_DTCTriggeredMessage_Handler
\R{[$DDS_SSFM 149]}

The Runnable_SSFM_DTCTriggeredMessage_Handler is a runnable that manages timing and sending of DTC triggered messages, as well as the dequeuing and updating of DTC + FTB combination.

\subsubsection Runnable_SSFM_DTCTriggeredMessage_Handler_id Runnable_SSFM_DTCTriggeredMessage_Handler
 
The @ref Runnable_SSFM_DTCTriggeredMessage_Handler performs the following operations:

* Checks if the operation cycle has started or if a Service 14 (Clear Diagnostic Information) request has been received.
* Checks if at least one Partial Network Cluster (PNC) is active to ensure communication is active.
* Sends the DTC triggered message if the wait timer exceeds 1000ms or during the initial power-up sequence.
* Dequeues DTCs from the notification queue and updates the DTC triggered message fields if the wait timer exceeds 1500ms.
* Manages internal timers to control the periodicity of message sending and DTC status updates.
* A DTC is queued only when TestFailed bit changes from `0 -> 1` (`Sum_Ssfm_Errh_CheckDTCStatus_And_Enqueue`).
* A healed event (`1 -> 0`) is ignored by enqueue logic (no new DTC item is queued).
* If dequeued DTC is already sent and (`1 -> 0` transition happens or Service 14 is requested), send DTC payload (`Normal` message type).
* If dequeued DTC is already sent and Service 14 is not requested, send reset payload (`Reset` message type).
* When queue is empty, runnable updates output buffer with reset payload (`Reset` message type).
* "Already sent" decision is based on whether the DTC exists in local sent-tracking table.

The runnable ensures that DTC messages are sent and processed in a timely manner, complying with system requirements for operation cycles and diagnostic services.

\subsubsection Sum_Ssfm_IsDTCTriggerMessageAlreadySent_id Sum_Ssfm_IsDTCTriggerMessageAlreadySent

The @ref Sum_Ssfm_IsDTCTriggerMessageAlreadySent checks if a DTC trigger message has already been sent for a given DTC. Beside, if the DTC trigger message has been sent then `DTCTrigger_Item->AlreadySent = TRUE`, `FALSE` otherwise.

\subsubsection Sum_Ssfm_CheckBitStatus_id Sum_Ssfm_CheckBitStatus

The @ref Sum_Ssfm_CheckBitStatus checks if a specific bit is set in an 8-bit variable, return TRUE if the bit at the specified location is set to 1, FALSE otherwise.

\subsubsection Sum_Ssfm_Update_Dtc_Ftb_Combination_id Sum_Ssfm_Update_Dtc_Ftb_Combination

The @ref Sum_Ssfm_Update_Dtc_Ftb_Combination updates the DTC and failure type combination for the DTC triggered message.

\subsubsection Sum_Ssfm_HandleDtcDequeueAndUpdate_id Sum_Ssfm_HandleDtcDequeueAndUpdate

The @ref Sum_Ssfm_HandleDtcDequeueAndUpdate handles dequeuing of DTCs and updates the DTC triggered message accordingly:
* repeated DTC + (`1 -> 0` transition or Service14 requested): send DTC payload (`Normal`)
* repeated DTC + Service14 not requested: send reset payload (`Reset`)
* queue empty: send reset payload (`Reset`)

\subsubsection Sum_Ssfm_UpdateDtcMessageFields_id Sum_Ssfm_UpdateDtcMessageFields

The @ref Sum_Ssfm_UpdateDtcMessageFields updates DTC message fields based on the DTC status.

\R{[$Satisfies $ADS_SSFM 137]} \R{[$Satisfies $ADS_SSFM 138]} \R{[$Satisfies $ADS_SSFM 139]} \R{[$Satisfies $ADS_SSFM 140] [$Satisfies $ADS_SSFM 141]} \R{[$Satisfies $ADS_SSFM 142]} \R{[$Satisfies $ADS_SSFM 143]} \R{[$Satisfies $ADS_SSFM 144] [$Satisfies $ADS_SSFM 145]} \R{[$Satisfies $ADS_SSFM 146]} \R{[$Satisfies $ADS_SSFM 147]}

@subsection Sum_Ssfm_LinResponder_Error_Manager 3.1.23 Sum_Ssfm_LinResponder_Error_Manager

The Sum_Ssfm_LinResponder_Error_Manager is responsible for monitoring the Lin Responder Internal Electronic Fault, Loss of Com and the Node timeout.

@subsubsection Sum_Ssfm_IeF_LINResponder_Manager
\R{[$DDS_SSFM 133]}

The function @ref Sum_Ssfm_IeF_LINResponder_Manager is responsible for managing the internal logic and performing debouncing for Internal Electronic Fault (IeF) for the LIN Responder. It reads the status of the IeF from the Local Interconnect Network Management (LNM) and processes it through a debouncing mechanism. If the debouncing status is conclusive (i.e., not in debouncing), it sets the event status in the Diagnostic Event Manager (DEM).

It checks if the pointer to the function for reading the IeF status from LNM is not NULL.
If the pointer is valid, it reads the IeF status from LNM via `Rte_Read_SUM_SSFM_RP_LNM_SSFM_ERRH_IeF_<LIN_RESPONDER_index>_IntElectFault`.
The function then executes the debouncer logic with the read IeF status. The IeF status is considered failed if the read value is 1 (indicating an invalid signal) and passed otherwise.

The debouncer status is evaluated to determine if it is still in the debouncing phase or if a conclusive status (passed or failed) has been reached.
If a conclusive status is reached, the function converts the evaluation status to a DEM event status (DEM_EVENT_STATUS_PASSED or DEM_EVENT_STATUS_FAILED).

The function then sets the event status in the DEM using the pointer to the `Rte_Call_Dem_SSFM_SetEventStatus` function contained within the err_debouncer structure.
If the debouncer status is still in debouncing or unknown, no action is taken with the DEM.

It implements the following flowchart :
@startuml
start
:Read Internal Electronic Fault status from LNM;
if (Rte_Read_LNM_SSFM_IntElectFault is NULL_PTR) then (no)
  :End;
  stop
else (yes)
  :Check if DTC is set for internal electronic failure fault active;
  if (linslave_IefFA == INVALID_SIGNAL) then (yes)
    :Execute Debouncer with EventSts_Failed;
  else (no)
    :Execute Debouncer with EventSts_Passed;
  endif
  :Get debouncer status;
  if (debouncer_status is InDebouncing or Unknown) then (yes)
    :End;
    stop
  else (no)
    :Convert event status after debouncing;
    if (debouncer_status is Passed) then (yes)
      :Set DEM event status to PASSED;
    else (no)
      :Set DEM event status to FAILED;
    endif
    :Set the event status from Dem_SetEventStatus;
  endif
endif
stop
@enduml

\R{[$Satisfies $ADS_SSFM 129]} \R{[$Satisfies $ADS_SSFM 130]} \R{[$Satisfies $ADS_SSFM 131]} \R{[$Satisfies $ADS_SSFM 132]} 

@subsubsection Sum_Ssfm_IeF_Dem_CB_DTCStatusChanged
\R{[$DDS_SSFM 134]}

This function @ref Sum_Ssfm_IeF_Dem_CB_DTCStatusChanged is a callback that is triggered when the Diagnostic Event Manager (DEM) notifies a change in the status of a Diagnostic Trouble Code (DTC). It is responsible for updating the invalidity of LIN signal interfaces, which inform the Software Components (SWCs) about invalidity of LIN signal whenever getting confirmed events.

The function checks if the new DTC status includes the confirmed event bit by applying a bitmask and comparing it with the bitmask itself.
It then iterates over the array of debouncers linked to the LIN Responder ECUs (Sum_Ssfm_Ief_LNMError_Debouncers).
For each debouncer, it checks if the DTC value associated with the debouncer matches the DTC parameter passed to the function.
If a match is found, the function iterates over the LIN signals associated with the LIN Responder ECU.
For each LIN signal, it calls the `Rte_Write_SSFM_SWC_Signal_Inv` function pointer to update the signal invalidation status based on the confirmed event status.
The loop continues until all relevant LIN signals for the matching DTC have been updated.

It implements the following flowchart :

@startuml
start
:Initialize index_responder to 0;
:Initialize confirmed_evstatus based on DTCStatusNew;

while (index_responder < SSFM_NUM_OF_IEF_LNMERROR)
  :Check if DTC matches with Sum_Ssfm_Ief_LNMError_Debouncers[index_responder].DemEvent->DTCValue;
  if (DTC matches) then (yes)
    :Loop through the Signals associated with this LIN Responder ECU;
    while (index_signal < Sum_Ssfm_Ief_LNMError_Debouncers[index_responder].DebouncerOwner.LINRespInfo->NoOfLINSignals)
      :Sum_Ssfm_Ief_LNMError_Debouncers[index_responder].DebouncerOwner.LINRespInfo->pLINSignalInfo[index_signal]->Rte_Write_SSFM_SWC_Signal_Inv(confirmed_evstatus);
      :Increment index_signal;
    endwhile
  endif
  :Increment index_responder;
endwhile

stop
@enduml

\R{[$Satisfies $ADS_SSFM 129]} \R{[$Satisfies $ADS_SSFM 130]} \R{[$Satisfies $ADS_SSFM 131]} \R{[$Satisfies $ADS_SSFM 132]} 

@subsubsection Runnable_SSFM_LNM_LIN_RESPONDER_index_IntElectFault_Changed
\R{[$DDS_SSFM 135]}

Whenever a change happens to the status of internal electronic fault from LNM, this function will be called to process LIN Responder internal electronic fault manager.

Note: LIN_RESPONDER_index is depending on the user configuration 

It implements the following flowchart :
@startuml
start
:Read BatteryVoltageRangeMinToMax mapping to Internal Electronic Fault <LIN_RESPONDER_index> from SUSD;
if (Rte_Read_SUSD_SSFM_BatteryVoltage returns RTE_E_OK) then (yes)
  if (operating_range == IN_Range) then (yes)
    :Manage internal logic and perform debounce for Internal Electronic Fault for the LIN Responder;
  else (no)
    :Do nothing;
  endif
else (no)
  :Do nothing;
endif
stop
@enduml

\R{[$Satisfies $ADS_SSFM 129]} \R{[$Satisfies $ADS_SSFM 130]} \R{[$Satisfies $ADS_SSFM 131]} \R{[$Satisfies $ADS_SSFM 132]} 

@subsubsection Sum_Ssfm_Errh_FP_SSFM_IntElectFault_CbkWindowMoving
\R{[$DDS_SSFM 136]}

Callback function @ref Sum_Ssfm_Errh_FP_SSFM_IntElectFault_CbkWindowMoving of debouncing for internal electronic fault that notifies to DEM in case of timebased moving window expired.

\R{[$Satisfies $ADS_SSFM 129]} \R{[$Satisfies $ADS_SSFM 130]} \R{[$Satisfies $ADS_SSFM 131]} \R{[$Satisfies $ADS_SSFM 132]} 

@subsubsection Runnable_SSFM_LNM_LIN_RESPONDER_index_MissingMsg_Changed
\R{[$DDS_SSFM 180]}

The Runnable_SSFM_LNM_<LIN_RESPONDER_index>_MissingMsg_Changed function is responsible for handling the status change of the Missing Message event for each of configured LIN responder. The function manages the internal logic for handling the status change of the Missing Message event. It reads the operating range status, processes it, and sets a test enable flag if the status is within range.

The function performs the following steps:

* Reads the operating range status from the RTE interface Rte_Read_RP_SUSD_SSFM_Errh_BatteryVoltage_BatteryVoltageRangeMinToMax_OperatingRange.
* Checks if the return value is E_OK.
* If the operating range status is within range (IN_Range), sets the Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag for the corresponding LIN Network and Responder TRUE.

The function follows this flowchart:
@startuml
start
:Read operating range status from Rte_Read_RP_SUSD_SSFM_Errh_BatteryVoltage_BatteryVoltageRangeMinToMax_OperatingRange;
if (ret_val == E_OK) then (yes)
  if (operating_range == IN_Range) then (yes)
    :Set Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag to TRUE;
  else (no)
    :Do nothing;
  endif
else (no)
  :Do nothing;
endif
stop
@enduml

\R{[$Satisfies $ADS_SSFM 172]} \R{[$Satisfies $ADS_SSFM 173]} \R{[$Satisfies $ADS_SSFM 174]}} 

@subsubsection Sum_Ssfm_Check_MissingMsg_TestEnableCondition
\R{[$DDS_SSFM 181]}

The Sum_Ssfm_Check_MissingMsg_TestEnableCondition function is responsible for checking the test enable condition for the Missing Message (Msg) event and updating the status flag accordingly.The function determines if the test condition for the Missing Message event is met based on the operational status read from the RTE interface. If the conditions are met, it sets the test enable flag for the Missing Message event.
The function performs the following steps:

* Reads the operational status from the RTE interface `Rte_Read_RP_SUSD_SSFM_Errh_BatteryVoltage_BatteryVoltageRangeMinToMax_OperatingRange`.
* Checks if the return value is RTE_E_OK.
* If the operational status is within range (IN_Range), sets the Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag to TRUE.
* If the operational status is not within range, sets the Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag to FALSE.

The function follows this flowchart:
@startuml
start
:Read operational status from Rte_Read_RP_SUSD_SSFM_Errh_BatteryVoltage_BatteryVoltageRangeMinToMax_OperatingRange;
if (ret_val == RTE_E_OK) then (yes)
  if (operational_status == IN_Range) then (yes)
    :Set Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag to TRUE;
  else (no)
    :Set Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag to FALSE;
  endif
else (no)
  :Set Sum_Ssfm_LNMError_MissingMsg_TestEnableFlag to FALSE;
endif
stop
@enduml

\R{[$Satisfies $ADS_SSFM 172]} \R{[$Satisfies $ADS_SSFM 173]} \R{[$Satisfies $ADS_SSFM 174]}} 

@subsubsection Sum_Ssfm_LinResponder_MissingMessage_Debouncer

The function @ref Sum_Ssfm_LinResponder_MissingMessage_Debouncer handles the debouncing process for the LIN responder missing message error. It reads the node status from the Local Network Management (LNM) and updates the debouncer accordingly.

It checks if the function pointer for reading the node status is not NULL. If the pointer is valid, it reads the node status via `Rte_Read_LNM_SSFM_NodeStatusId`. The function then executes the debouncer logic using the read node status. The debouncer status is evaluated to determine if it is still debouncing or if a conclusive status (passed or failed) has been reached. If a conclusive status is reached, the function converts the evaluation status to a DEM event status (DEM_EVENT_STATUS_PASSED or DEM_EVENT_STATUS_FAILED). The function sets the event status in the DEM using the pointer to the `Rte_Call_Dem_SSFM_SetEventStatus `function contained within the err_debouncer structure. If the debouncer status is still debouncing or unknown, no action is taken with the DEM.

The function follows this flowchart:
@startuml
start
:Read node status from LNM;
if (Rte_Read_LNM_SSFM_NodeStatusId is NULL_PTR) then (no)
  :End;
  stop
else (yes)
  :Check if node status indicates missing message error;
  if (node_status == ERROR_EVENT_ACTIVE) then (yes)
    :Execute Debouncer with EventSts_Failed;
  else (no)
    :Execute Debouncer with EventSts_Passed;
  endif
  :Get debouncer status;
  if (debouncer_status is InDebouncing or Unknown) then (yes)
    :End;
    stop
  else (no)
    :Convert event status after debouncing;
    if (debouncer_status is Passed) then (yes)
      :Set DEM event status to PASSED;
    else (no)
      :Set DEM event status to FAILED;
    endif
    :Set the event status using Dem_SetEventStatus;
  endif
endif
stop
@enduml


@subsubsection Sum_Ssfm_LinResponder_MissingMsg_Handler

The function @ref Sum_Ssfm_LinResponder_MissingMsg_Handler is responsible for handling the state of the LIN Responder missing message error. It manages the LIN Responder missing message error state. It reads the node status of the LIN responder and processes it through different states using a switch-case mechanism.

It evaluates the LIN responder state and transitions through normal and lost communication states:

* SUM_SSFM_EcuNodeState_Normal: Processes the normal state by calling Sum_Ssfm_Process_MissingMsg_Normal.
* SUM_SSFM_EcuNodeState_LostComm: Processes the lost communication state by calling Sum_Ssfm_Process_MissingMsg_LossComm.

The function returns the evaluation status based on the processed state.

The function follows this flowchart:

@startuml
start
:Read node status from LNM;
: switch(deb_event->DebouncerOwner.LINRespInfo->LinRespState);
if (SUM_SSFM_EcuNodeState_Normal) then (yes)
  :Execute Sum_Ssfm_Process_MissingMsg_Normal;
else (no)
  if (SUM_SSFM_EcuNodeState_LostComm) then (yes)
    :Execute Sum_Ssfm_Process_MissingMsg_LossComm;
  else (no)
    :Do nothing;
  endif
endif
:Return evaluation status;
stop
@enduml

@subsubsection Sum_Ssfm_Process_MissingMsg_Normal

The function @ref Sum_Ssfm_Process_MissingMsg_Normal handles the normal state of the LIN Responder missing message error. It checks the error status and updates the debouncer state accordingly.

It evaluates the LIN responder error status and processes the state transitions:

* If the error status is active, it starts the timer and sets the state to Lost Communication.
* If the error status is not active, it sets the state to Passed and notifies the DEM of the event status.

The function returns the evaluation status based on the processed state.

The function follows this flowchart:

@startuml
start
:Check LIN responder error status;
if (error_status == ERROR_EVENT_ACTIVE) then (yes)
  :Start timer;
  :Set state to Lost Communication;
  :Return InDebouncing;
else (no)
  :Set state to Passed;
  :Notify DEM of event status PASSED;
  :Return Passed;
endif
stop
@enduml

@subsubsection Sum_Ssfm_Process_MissingMsg_LossComm

The function @ref Sum_Ssfm_Process_MissingMsg_LossComm is responsible for processing the lost communication state of the LIN Responder missing message error.. It handles the lost communication state of the LIN Responder missing message error. It checks the error status and updates the debouncer state accordingly.

It evaluates the LIN responder error status and processes the state transitions:

* If the error status is active, it retrieves the last evaluation status and returns it.
* If the error status is not active, it resets the timer, sets the state to Normal, and notifies the DEM of the event status as Passed.

The function returns the evaluation status based on the processed state.

The function follows this flowchart:

@startuml
start
:Check LIN responder error status;
if (error_status == ERROR_EVENT_ACTIVE) then (yes)
  :Get last evaluation status;
  :Return last evaluation status;
else (no)
  :Reset timer;
  :Set state to Normal;
  :Notify DEM of event status PASSED;
  :Return Passed;
endif
stop
@enduml

@subsubsection Sum_Ssfm_LINResponder_CbkRxTOut_Handler

The function @ref Sum_Ssfm_LINResponder_CbkRxTOut_Handler is responsible for executing the timeout callback for the LIN Responder. It handles the callback when a timeout occurs for the LIN Responder. It indicates a missing message with the LIN Responder to the Local Network Manager (LNM) by setting the NodeTimeout interface to LNM as DTC_ACTIVE.

It updates the status to indicate the timeout event Calls the `Rte_Write_SSFM_LNM_NodeTimeout` function with DTC_ACTIVE to indicate the timeout event.

The function follows this flowchart:
@startuml
start
:Indicate missing message with LIN Responder to LNM;
:Set interface NodeTimeout to LNM as DTC_ACTIVE;
stop
@enduml


@subsubsection Sum_Ssfm_LINResponder_CbkRxRec_Handler

The function @ref Sum_Ssfm_LINResponder_CbkRxRec_Handler is responsible for handling the reception of new data for a given SerialData. It processes the reception of new data for the LIN responder. It checks the supervision status of LIN signals and updates the node timeout status.

It evaluates the LIN signal supervision statuses and processes the recovery flag:
* If any LIN signal supervision status is uninitialized or timed out, the recovery flag is set to FALSE.
* If all LIN signal supervision statuses are valid, the recovery flag is set to TRUE and the interface NodeTimeout is set to DTC_INACTIVE.

The function follows this flowchart:
@startuml
start
:Check LIN signal supervision statuses;
:set recovery_flag to TRUE;
:for each linsignal_idx;
if (SigSts_Supervision != Received) then (yes)
  :Set recovery_flag to FALSE;
else (no)
  :Do nothing;
endif
:endfor;
if (recovery_flag == TRUE) then (yes)
  :Set interface NodeTimeout to DTC_INACTIVE;
else (no)
  :Do nothing;
endif
stop
@enduml

@subsubsection Runnable_SSFM_Execution_LNMError_MissingMessage
\R{[$DDS_SSFM 182]}

The function @ref Runnable_SSFM_Execution_LNMError_MissingMessage function is responsible for monitoring and handling the LIN responder missing message errors periodically.
The function @ref Runnable_SSFM_Execution_LNMError_MissingMessage performs the following steps:

* Checks if the test enable flag for missing message errors is set.
* If the battery voltage is within the specified range, it increments the timer.
* If the time spent in range exceeds the calibratable time, it invokes the missing message check and resets the timer.
* Monitors the debouncing time via NodeStatusTimer from LNM.
* If the timer has expired, it updates the evaluation status to FAILED and notifies the DEM that a loss of communication event has been observed.

The function follows this flowchart:
@startuml
start
:Check test enable flag for missing message error;
if (TestEnableFlag == TRUE) then (yes)
  :Check if battery voltage is within the range;
  if (BatteryVoltageWithinRange == TRUE) then (yes)
    :Increment time spent in range;
    if (TimeInRange >= CalibratableTime) then (yes)
      :Invoke missing message check;
      :Reset time in range timer;
      :Reset TestEnableFlag;
    else (no)
      :Continue incrementing timer;
    endif
  else (no)
    :Reset battery voltage timer;
  endif
endif
:Monitor debouncing time via NodeStatusTimer;
if (TimerElapsed >= NodeStatusTimer) then (yes)
  :Reset TimerElapsed;
  :Update evaluation status to FAILED;
  :Notify DEM of LossOfCommunication event;
else (no)
  :Increment TimerElapsed;
endif
stop
@enduml

\R{[$Satisfies $ADS_SSFM 172]} \R{[$Satisfies $ADS_SSFM 173]} \R{[$Satisfies $ADS_SSFM 174]}} 

@subsection Sum_Ssfm_RID_02C0_Handler 3.1.24 Sum_Ssfm_RID_02C0_Handler
The **Sum_Ssfm_RID_02C0_Handler module** is responsible for clearing the SOH logs connected to the service of RID 0x02C0.
\subsubsection Runnable_SSFM_ERRH_RID_ClearLog_StartClear
\R{[$DDS_SSFM 151]}

The runnable @ref Runnable_SSFM_ERRH_RID_ClearLog_StartClear is responsible for clearing the SOH logs related to DID F094 for the correct service of RID 0x02C0. The SOH demands the logs clear via the client/server interface SOH_ERRH_ClearRID0x02C0 which triggers the above runnable.
The runnable sets the related detectors to their default values then writes the updated values to the NvM through the client/server interface NvMService. In order to achieve this, 
the runnable calls the auxiliary function SSFM_ERRH_ClearItem defined for the purpose in the **Sum_Ssfm_DID_Util** module.

\R{[$Satisfies $ADS_SSFM 150]}

@subsection Sum_Ssfm_LINInfo_Handler 3.1.25 Sum_Ssfm_LINInfo_Handler

The LIN Info Handler is responsible for storing the information needed specifically for LIN SerialDatas.
Moreover, it monitors the status of LNM_SSM_MSCL_LIN and LinSM_CurrentSchedule interfaces required for determining the LIN SerialData availability status.

\R{[$DDS_SSFM 152]}

The LIN Info Handler monitors the LNM_SSM_MSCL_LIN by implementing a runnable for each LIN Channel with the following name pattern: Runnable_SSFM_LIN_<LIN_CHANNEL_ID>_LNM_SSM_MSCL_Changed

Upon any change in the status of MSCL bytes, the runnable will be called by RTE and the LIN Availability Status Manager will be informed through calling the @ref Sum_Ssfm_SigStsAvailability_Manager_LIN.

\R{[$Satisfies $ADS_SSFM 151]} \R{[$Satisfies $ADS_SSFM 153]}

\R{[$DDS_SSFM 153]}

The LIN Info Handler monitors the LinSM_CurrentSchedule by implementing a runnable for each LIN Channel with the following name pattern: Runnable_SSFM_LIN_<LIN_CHANNEL_ID>_ScheduleStatus_Changed

Upon any change in the status of current schedule status, the runnable will be called by RTE and the LIN Availability Status Manager will be informed through calling the @ref Sum_Ssfm_SigStsAvailability_Manager_LIN.

\R{[$Satisfies $ADS_SSFM 151]} \R{[$Satisfies $ADS_SSFM 154]}


@subsection Sum_Ssfm_LIN_Av_Status_Manager 3.1.26 Sum_Ssfm_LIN_Av_Status_Manager

The LIN Availability Status Manager is responsible for handling the state machine of each LIN SerialData's availability status.

\R{[$DDS_SSFM 154]}

The @ref Sum_Ssfm_CheckDisableCondition_LIN implements the logic of disable conditions for LIN SerialDatas.
Reading the latest status of LinSM_CurrentSchedule and LNM_SSM_MSCL_LIN, it checks if the schedule status is OFF or the MSCL bit is set to zero. In either case, the disable condition is met and the availability status will be changed to SUM_SSFM_SigSts_Disabled directly. 

\R{[$Satisfies $ADS_SSFM 152]} \R{[$Satisfies $ADS_SSFM 155]} \R{[$Satisfies $ADS_SSFM 156]}

\R{[$DDS_SSFM 155]}

The @ref Sum_Ssfm_SigStsAvailability_Manager_LIN is responsible for handling the state machine of LIN SerialDatas. First of all, it assesses the disable condition using @ref Sum_Ssfm_CheckDisableCondition_LIN.
If the disable condition is met, the state will be SUM_SSFM_SigSts_Disabled with no other consideration.

Otherwise, if the previous state was SUM_SSFM_SigSts_Disabled, the next state will be always SUM_SSFM_SigSts_Uninitialized.

Starting from being Uninitialized, at any point of time, if the disable conditions are met, the state will change to SUM_SSFM_SigSts_Disabled.

On the other hand, the supervision status determines the state. If the status is SUM_SSFM_SigSts_Supervision_Received the state will be SUM_SSFM_SigSts_Available. And, if the status is SUM_SSFM_SigSts_Supervision_Timeout the state will be SUM_SSFM_SigSts_Unavailable.

\R{[$Satisfies $ADS_SSFM 152]} \R{[$Satisfies $ADS_SSFM 155]} \R{[$Satisfies $ADS_SSFM 156]}

@subsection Sum_Ssfm_Errh_Dtc_Enable_Manager 3.1.27 Sum_Ssfm_Errh_Dtc_Enable_Manager
\R{[$DDS_SSFM 165]}

The Dtc Enable Manager is responsible for receiving the DTCs status, utilizing service ControlDTCSetting 0x85, and transmitting this information to the SWCs.
In the event of a mode switch for the DTCs status value, the @ref Runnable_SSFM_ERRH_ControlDTCSetting_DisableDTC is triggered when the current DTCs status is set to Disabled. Conversely, @ref Runnable_SSFM_ERRH_ControlDTCSetting_EnableDTC is triggered when the status is set to Enabled. Once triggered, the runnable enables the transmission of the DTCs status value to other SWCs.

\R{[$Satisfies $ADS_SSFM 162]} \R{[$Satisfies $ADS_SSFM 161]}

\R{[$DDS_SSFM 189]}
The @ref Runnable_SSFM_RemoteProgrammingOtaPsf_Status_Handler is responsible for handling the changes in the status of RemoteProgrammingOtaPsf and notifying other software components (SWCs) about the status of DTCs.

The function follows this flowchart:
@startuml

start
:Read status from Rte_Read_RP_DIAG_SWC_RmtPrgmOtaPsf_Status_DIAG_RemoteProgrammingOtaPsf;
:retValue == RTE_E_OK?;
if (retValue == RTE_E_OK) then (yes)
    :status == TRUE?;
    if (status == TRUE) then (yes)
        :Notify SWCs that DTCs status is enabled;
        :Rte_Write_PP_SUM_ERRH_SWC_DisableDTCs_TestEnable(TRUE);
        :Set Sum_Ssfm_DTCsDisabled to TRUE;
    else (no)
        :Notify SWCs that DTCs status is disabled;
        :Rte_Write_PP_SUM_ERRH_SWC_DisableDTCs_TestEnable(FALSE);
        :Set Sum_Ssfm_DTCsDisabled to FALSE;
    endif
else (no)
    :Do nothing;
endif
end

@enduml

\R{[$Satisfies $ADS_SSFM 184]} 

@subsection Sum_Ssfm_LinBusOff_Handler 3.1.28 Sum_Ssfm_LinBusOff_Handler
\R{[$DDS_SSFM 172]}

The SSFM receives notifications from the LNM about LIN Bus Off events and counts errors when switching from NO_COMM to FULL_COMM. 
SUM_LNM must then update the Bus Off error event status to SUM_SSFM-ERRH through the event port.

\R{[$Satisfies $ADS_SSFM 166]}

\R{[$DDS_SSFM 173]}

When an event occurs, the Runnable_SSFM_LNM_LIN_Busoff<n> function is activated. The SSFM checks preconditions to set the DTC for Network <n>. 
The conditions for processing the DTC are: if the supply voltage is out of range, the DTC is not processed; 
if the reset or power-up timer is 5 seconds or less, the DTC is also not processed. 
If the OBD ECU detects that the supply voltage is too high, the DTC is processed, and the results are updated.

\R{[$Satisfies $ADS_SSFM 166]} \R{[$Satisfies $ADS_SSFM 167]} \R{[$Satisfies $ADS_SSFM 168]} 

@subsection Sum_Ssfm_QoS_Manager Sum_Ssfm_QoS_Manager

Sum_Ssfm_QoS_Manager is the C module that implements the QoS management. 

\R{[$DDS_SSFM 362]}

The QoS process is triggered by calling the Sum_Ssfm_QoS_Manager_UpdateOnReceivedConsolidatedService_<serialData.name>() at the end of the Com callback that notifies the reception of a PDU
if the paricular Sum_Ssfm_SerialData has some Sum_Ssfm_QoS_Ref reference set. 

\R{[$Satisfies $ADS_SSFM 196]}

\R{[$DDS_SSFM 363]}

The function Sum_Ssfm_QoS_Manager_UpdateOnReceivedConsolidatedService_<SerialData.shortName> is responsible for: 

* Retrieving the QoS payload from the interface SSFM_IF_QoS_PayloadRx_<SerialData.shortName>;
* Compute the QoS status for the SerialData itself based on the metrics;
* Write the QoS status through the interface SSFM_IF_QoS_Indication for the PDU itself
* Extracting the QoS status from the payload read;
* Aggregating the QoS status of each QoS referenced with the PDU's QoS status;
* Writing the aggregated QoS status through the interface SSFM_IF_QoS_Indication.


\R{[$Satisfies $ADS_SSFM 197]}

If the Sum_Ssfm_SerialData has no LcFa associated, its bit in the QoS remains zero.
If the Sum_Ssfm_SerialData hes no safety with FsI source associated, the FsI and FsFa bits remains zero.

@subsection Sum_Ssfm_Initialization 3.1.29 Sum_Ssfm_Initialization

Sum_Ssfm_Initialization is the C module that implements the architectural module Initialization.

\R{[$DDS_SSFM 188]}

There are two ways to initialize the SUM Ssfm module, determined by the configuration of the SsfmGenerateInitRunnable parameter. When the configuration parameter is set to TRUE, the initialization is performed by the Runnable_SSFM_Init, which calls the static functions Sum_Ssfm_ErrhInit and Sum_Ssfm_SsmInit. Otherwise, the initialization is performed by the global functions Sum_Ssfm_ErrhInit and Sum_Ssfm_SsmInit, called by the BswM Init rule.

\R{[$Satisfies $ADS_SSFM 208]}

Furthermore, the Sum_Ssfm_ErrhInit and Sum_Ssfm_SsmInit functions allow for the external calibration of off services by performing the following steps in sequence:

* Call Sum_Ssfm_SsmDisablePdu_Tx function to disable configured PDU in transmittion;
* Call Sum_Ssfm_SetSerialDatasProviders function to check the status of each calibration parameter and records the current provider of each multiple provider SerialData;
* Call Sum_Ssfm_DID_E0B8_RestoreData function to restore DID E0B8 data from NvM;
* Call Sum_Ssfm_DID_E0BB_RestoreData function to restore DID E0BB data from NvM;
* Call Sum_Ssfm_RestoreMessageAvDidE0B9 function to restore message's availability for DID 0xE0B9;

\R{[$DDS_SSFM 367]} 

* Initialize non-discoverable SOME/IP PDUs to Available status: For each SerialData with IsNonDiscoverable flag set to TRUE, set the internal signal status to SUM_SSFM_SigSts_Available and notify RTE consumers via the availability port. These PDUs remain Available throughout the ECU lifecycle;
* Check SdEnableExtendedLearningTableFormat calibration parameter at run time;
* Call Sum_Algo_Deque_Init to initial queue;

\R{[$Satisfies $ADS_SSFM 180]} \R{[$Satisfies $ADS_SSFM 181]} \R{[$Satisfies $ADS_SSFM 182]} \R{[$Satisfies $ADS_SSFM 208]}

@subsection SSFM QoS 
\R{[$DDS_SSFM 200]}

To support consolidated services QoS, SSFM introduces the sender/receiver interfaces SSFM_IF_QoS_PayloadRx and SSFM_IF_QoS_Indication. Generation of the interface, ports, and variable accesses is controlled by the ECUC flag SSFM_QoS_Enabled (default TRUE).

\R{[$Satisfies $ADS_SSFM 194]} \R{[$Satisfies $ADS_SSFM 195]}

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
