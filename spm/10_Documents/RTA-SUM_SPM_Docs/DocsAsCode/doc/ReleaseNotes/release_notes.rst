.. image:: ../logo.png
   :align: right
   :alt: Company Logo
   :width: 200px

=========================
RTA-SUM SPM Release Notes
=========================

************
Introduction
************

This document describes release specific details and notes related to RTA-SUM SPM.

Definitions and Abbreviations
=============================

============================== ===================================================================================================
    Terms and Abbreviations                Definitions
============================== ===================================================================================================
    AUTOSAR                     AUTOmotive Open System ARchitecture
    BSW                         Basic Software
    CAN                         Controlled Area Network
    CDD                         Complex-Device Driver
    CanIf                       Can Interface AUTOSAR BSW Module
    PGTT                        Power Graph Traversal Table
    PM                          Power Manager 
    RTE                         Run-Time environment AUTOSAR middleware between Application Software and Basic Software
    SE                          Software Entity (Assume it as generic software module/function or Software Service)
    SM                          Software Manager
    SPM                         Software/Power manager
    SUM                         Standardized Utility Module
    SWCD                        Software Component Description
    SoAd                        Socket Adaptor AUTOSAR BSW Module
============================== ===================================================================================================

Release Notes Revision History
==============================

=================  =========================================================================  =================================
Version             Description of Changes                                                    Author
=================  =========================================================================  =================================
0.0.1               * Initial Version - First Release                                          RTA-SUM Development Team
        
0.0.2               * Removed Sum_Spm_SmPmIf.                                                 
                    * Added Software for coordination of wake-up on CAN.                       RTA-SUM Development Team
        
0.0.3               * Added support for Can wake-up frames.                                   
                    * First integration in RTA-SUM plugin                                      RTA-SUM Development Team
        
0.0.3_1             * Added header :code:`Cdd_SPM`                                             RTA-SUM Development Team
        
0.0.3_2             * Added scripts                                                           
                        * :code:`Sum_Spm_Requester_Queue_*.xpt`,                                   
                        * :code:`Sum_Spm_Header_h.xpt`,                                           
                        * :code:`Sum_Spm_SM_Req_EthIf_*.xpt`,                                     
                        * :code:`Sum_Spm_SoftwMgr.h`                                           RTA-SUM Development Team
        
0.0.3_3             * Added scripts :code:`Sum_Spm_ComCbk_*.xpt`                               RTA-SUM Development Team
        
0.4.4               * Added scripts :code:`Sum_Spm_Cbk_*.xpt`:                                
                    * Support of Etherent Wake-up Frame and CAN Wake-up frame.                 
                    * Removed                                                                  
                        * :code:`Sum_Spm_WakeUpOnCan_*.xpt`,                                       
                        * :code:`Sum_Spm_CanTransmit_*.xpt`,                                       
                        * :code:`Sum_Spm_CanIf_Cbk_*.xpt` scripts.                                 
                    * Renamed :code:`Cdd_SPM.h` in :code:`Cdd.h`                                
                    * Support of Helper Function as Optional Parameter                         RTA-SUM Development Team
        
0.0.5               * Support of PDU handling: SPM can request to the BswM                    
                    * the switch of the PDU modes in order to activate a Power Profile.        RTA-SUM Development Team
        
0.0.5_1             * RTE supports the metadata by SENDER-RECEIVER interface.                 
                    * Modules :code:`Sum_Spm_ComCbk_*.xpt` not supported                       
                      anymore and replaced by :code:`Sum_Spm_MetaData_*.xpt`                   
                    * Bugfixes                                                                
                    * Fixed Issue in the generation script                                     
                      Sum_Spm_Cfg_h.xpt for macro SPM_REQER_LST_SIZE.                          
                    * Fixed Issue Power profile status is sent several times.                  RTA-SUM Development Team
        
0.0.6               * Support of TC10 wake up frame.                                           
                    * Bugfixes                                                                
                    * Heartbeat and WkUp frame periodicities are calibratable.                 
                    * SPM Incorrect rolling counter for ShutdownPermission is fixed.          
                      It will be rolling from 0 to 3.                                          RTA-SUM Development Team
        
0.0.7               * Support of Metadata handling in Rx through RTE (GMSUM-8365)              RTA-SUM Development Team

0.0.8               * Added Detailed Requirements and update traceabilty (GMSUM-8459)          RTA-SUM Development Team

8.1.0_Pr1           * First prototype release of RTA-SUM SPM 8.1.0 (RTA-SUM 3.1.0) 
                      for SDV 2.0.
                    * Implemented the PermitExecution and ShutdownPermission
                      port interfaces that SUSD employs. (GMSUM-8747)
                    * Checked compatibility with RTA-BSW 12.3.3pr1.   
                    * The tools version have been Updated.                                     RTA-SUM Development Team  

8.1.0_Pr2           * Power Profile (De)Activation re-design. (GMSUM-8774)                    
                    * Add call to C/S SrvcMgr_SPM_ActivateServices to remote
                      power profiles.(GMSUM-8837)                                              
                    * Update signature and logic of C/S operation
                      SPM_ActivateProfile/SPM_DeactivateProfile (GMSUM-8869)                    
                    * Update logic of HearBeat counter (GMSUM-8922)                           
                    * Update PGTT paramdef and related scripts (GMSUM-8870)                   
                    * Update SM and PM Frame (GMSUM-8811)                                     
                    * Support ETH WUF as SOME/IP frame (GMSUM-8928)                                      
                    * Support of WUF A and B (GMSUM-9022)                                     
                    * Update paramdef multiplicity and parameters (GMSUM-8998)                 RTA-SUM Development Team 


8.1.0_Pr3           * Alignment of init function and runnable. (GMSUM-8896)                    RTA-SUM Development Team 


8.1.0_Pr4           * Make use of SpmProfDepType parameter. (GMSUM-9113)                      
                    * Multi VLAN support for SM and WUF frame (GMSUM-8925)                
                    * Multiple TrgDvcID support. (GMSUM-9117)    
                    * Support WakeUp Commands. (GMSUM-9116) 
                    * Fix definition structure SpmConfPowPrf. (GMSUM-9227)                 
                    * Updated traceability for SRS_SPM 0063 requirement. (GMSUM-9109)         
                    * Update logic of transmission of CAN wake-up 
                      frame periodically. (GMSUM-9114)    
                    * Support the configuration and the execution routine for waking
                      an edge node that SPM node can directly hanlde when it is 
                      intermediate	(GMSUM-9143)							 
                    * Update SRS. (GMSUM-9311)             
                    * Multi SpmIntermediateDstAddrs support. (GMSUM-9176)                     
                    * Fix validation check script for spmApplIntfID parameter (GMSUM-9303)    
                    * Support the sending of TC10 command over different VLANs by
                      SPM interfacing with EthSM (GMSUM-9125)                                  RTA-SUM Development Team 

8.1.0_Pr5           * Fixed issue GMSUM-9351                                                   RTA-SUM Development Team

8.1.0_Pr6           * Low Voltage Battery Priority handling. (GMSUM-9078)                      RTA-SUM Development Team
                    * Support SPM error handling to resend (GMSUM-9074)
                      its activate profile request when a remote node
                      is in shutdown pending status

8.1.0_Pr7           * Update SPM-SUSD interaction (GMSUM-9076)
                    * Fix Size_Indicator in SM frame. (GMSUM-9376)                             RTA-SUM Development Team

8.1.0_Pr8           * Fix Component Description for correct XML structure. (GMSUM-9444)
                    * Change the define values, in SPM Types header,
                      without SPM prefix. (GMSUM-9040)
                    * Use AUTOSAR_Platform instead of AUTOSAR_PlatformTypes
                      in SWCD. (GMSUM-9442)                                                     RTA-SUM Development Team 
8.1.0_Pr9           * Support SPM Error Handling - LossOfCom with Requesting SM 
                      (GMSUM-9072)
                    * Implement notification of service activation to SUMs for Remote
                      Power Profiles. (GMSUM-9578)                                             
                    * Support of Memory Mapping using RTA-CAR MemMap Tool (GMSUM-9449)         
                      all status flag before shutting-down (GMSUM-9068)                         RTA-SUM Development Team		
8.1.0_Pr10          * Support debounce of multiple requests from the same requester 
                      via requester list handling (GMSUM-9069)                                 
                    * Complete support of Device Power State (GMSUM-9548)                       RTA-SUM Development Team 
8.1.0_Pr11          * Support of Activation retries (GMSUM-9547)                                  
                    * Add error handling for missing request status message from 
                      requested SM (GMSUM-9067)                                                 RTA-SUM Development Team 
8.1.0_Pr12          * Response to conflicting request for the same profile (GMSUM-9071)
                    * PowerOnDevice Retry: add PowerOff sequence (GMSUM-9556)                                  
                    * PowerOnDevice Retry: add complete support (GMSUM-9555)          
                    * Resuming Activation Requests for Recovered Nodes (GMSUM-9073)             RTA-SUM Development Team 
8.1.0_Pr13          * Implement the interface SPM_GetProfileState (GMSUM-9560)                  RTA-SUM Development Team
8.1.0_Pr14          * Add support to DID 0xE0D7 (GMSUM-9537)                                    RTA-SUM Development Team 
                    * Documentation improvement (GMSUM-9800)                                    
                    * Sending feedback only for the power profile requested
                      without dependencies. (GMSUM-9377)                                        RTA-SUM Development Team
                      
8.1.0_Pr15          * Add support to DID 0xE0DD (GMSUM-9061)     
                    * Add support to Common queue (GMSUM-9079)         
                    * Add support to Population of common queue for DID 0xE0DE/0xE0DF
                      (GMSUM-9063)                            
                    * Paramdef support the PDU nomenclature for Multi-VAL communication        
                      as per SDV-8217 requirements SPM_0053 and SPM_0054                               
                    * Implement the C/S interface SPM_SUSD_ActivityRecordCmd (GMSUM-9060)           
                    * Fix service list is not updated to SSFM when software dependency         
                      type is 3 (consumed only) (GMSUM-9965)   
                    * Add the wake up frame info and bit parameters and corresponding 
                      calibration parameters. (GMSUM-9888)   
                    * Add support to DID 0xE0DE (GMSUM-9062)
                    * Add support to DID 0xE0DF (GMSUM-9063)                                    RTA-SUM Development Team

8.1.0_Pr16          * Add support PDUs nomenclature in Tx (GMSUM-9874)                         
                    * Add support PDUs nomenclature in Rx (GMSUM-9875)                          RTA-SUM Development Team

8.1.0_Pr17          * Implement the calibratable Keep Awake (GMSUM-9858)                       
                    * Removed local Rx CAN frame copy, extended 
                      Sum_Spm_WkupEdgeNode to support type C (GMSUM-9877)                      
                    * Add support for remaining awake if ECU's own WUF bit is set 
                      (GMSUM-9878)                                                              RTA-SUM Development Team

8.1.0_Pr18          * Calibration parameter access generated without dependencies 
                      (GMSUM-10152)                                                             RTA-SUM Development Team

8.1.0_Pr19          * Receive multicast frame on edge and intermediate (GMSUM-9879).           
                    * Send the HB in full scale, without waiting the expiration of the  
                      HB timer (GMSUM-9884).
                    * Update the wrong path for implementation data type (GMSUM-10063).        RTA-SUM Development Team  	 		 
                    * Extend the use-case to support the usage of Sd_GetNodeAddress API
                      in case the parameter `SdEnableExtendedLearningTableFormat` is true             
                      (GMSUM-10078).

8.1.0_Pr20          * ETH WUF Unicast Support for Intermediate Nodes (GMSUM-9880)               RTA-SUM Development Team
                    * Update deactivation requests handling (GMSUM-10217).  
                    * Added support for C Frame and relative command (GMSUM-9898).  
                    * Aligned WakeupStepDelay in PowerOff sequence as described in 
                      (GMSUM-10229)

8.1.0_Pr21          * Added support for getting profile activation permission (GMSUM-10207)    RTA-SUM Development Team
                    * Fixed inclusion issue is Sum_Spm_SoftwMgr.h (GMSUM-10061)

8.1.0_Pr22          * Aligned DID 0xE0DE/0xE0DF to handle multiple entry (GMSUM-10260)         RTA-SUM Development Team

8.1.0_Pr23          * Replacing memset / memcopy with the one in Sum_Common  (GMSUM-10415).    RTA-SUM Development Team
                    * Added NvM block references for DID 0xE0DE/0xE0DF (GMSUM-10387).

8.1.0_Pr24          * Extend pmVehDefActPriorON and SpmVehDefActPriorOFF                       RTA-SUM Development Team
                    * with values MediumLow, MediumHigh and Access (GMSUM-10208).
                    * Added QUEUE length calibration parameters (GMSUM-10378).
                    * Fixed issue in generate_paramdef.bat and add a filter for parameters
                      with multiplicity 0..0 (GMSUM-10496).
                    * Filtered out duplicate ComIpduGroup references to generate univoke
                      port definition for SpmPwrProfile (GMSUM-10253).

8.1.0_Pr25          * Handling a remote requester in FAULTED or REBOOT status                  RTA-SUM Development Team
                      (GMSUM-10221).
                    * Remove variable definitions from header files (GMSUM-10560)
                    * Align multiplicity in paramdef (GMSUM-10595)

8.1.0_Pr26          * Extend size of Requester List and added feedback for each remote         RTA-SUM Development Team 
                      requester (GMSUM-10519).
                    * Removing attender/commander topic so that each target device is          
                      capable to send and receive profile request (GMSUM-10434).
                    * Added Monitor Device functionality, which allows to set a Target         
                      device as Requester, Requested, or both, depending on the requests  
                      made/received (GMSUM-10520).  
                    * Simplified logic for Heartbeat management: now the HB is processed
                      even if the target device is not monitored (GMSUM-10531).
                    * Refactor function Spm_TrgDvcFSMManager (GMSUM-10533).
                    * Refactor PowerOn/OffDevice and PowerOnDevice retry (GMSUM-10532).
                    * Added LowVoltageSoftwareActivationPriorityRequest message in the SPM     
                      System Extract (GMSUM-10431)
                    * Update strategy for SPM HB transmission at startup                       
                      (GMSUM-9889).  
                    * Shutdown coordination for SPM has been updated to include a flag that 
                      determines if target devices are being requested remotely                      
                      (GMSUM-10364).
                    * Fixed Wrong configuration and check of the PduId for 
                      CAN WUF from CanIf (GMSUM-10600)
                    * Extended size of requester list (GMSUM-10718)

8.1.0_Pr27          * Merged Reset and NvMStore operations in SPM_ActivityRecordCmd to         RTA-SUM Development Team
                      ensure DID 0xE0DE/0xE0DF data is saved to NvM when cleared 
                      (RH-14995, GMSUM-10658).
                    * Fixed Wrong the multiplicity of the Rx part of 
                      SpmVLANComPwrMgrWakeupFrames parameter (RH-14897, GMSUM-10537). 
                    * Added support for LowVoltageSoftwareActivationPriorityRequest           
                      service-based information (GMSUM-10432).       
                    * Added activation/deactivation logic for profile 
                      with Application Interface ID: LVOptmzerChk (GMSUM-10433).   
                    * Update WakeUp Frame calibration support (GMSUM-9872).             
                    * Updated SPM Shutdown coordination with last heartbeat (GMSUM-10363).
                    * Added Learning Session status management logic (GMSUM-10551).
                    * Implementation of profile activation response with error code 
                      REJECTED_PROFILE_UNKNOWN (GMSUM-10705). 

8.1.0_Pr28          * Updated HB reception traceability for:                                   RTA-SUM Development Team
                      SDVA-7482, SDVA-7474, SDVA-4221 (GMSUM-9859).
                    * Update RN known issue Table (RH-15354, GMSUM-10723).
                    * Align concepts of Remote and Local Requests for DID E0D7
                      (GMSUM-10514).
                    * Added RID 0x26 for handling DID 0xE0D7 (GMSUM-10365).

8.1.0_Pr29          * Fixed missing SPM_START_SEC_CODE/SPM_STOP_SEC_CODE memory                RTA-SUM Development Team
                      mapping sections in Sum_Spm_SoftwMgr template (GMSUM-10862).
                    * Fixed the Spm_HandleInitAndShutdownState function (GMSUM-10866).
                    * Decreased lower multiplicity of SpmVLANCom child containers
                      and their parameters (GMSUM-10878).
                    * Fixed Rte_CData compliation issue when RteVfbTraceEnabled = true
                      (RH-15649, GMSUM-10916).

8.1.0_Pr30          * Implemented SDVA-7612 of SDV-2017 27.27.196, calibratable delay of 
                      150 ms before switching off PDU in Tx (GMSUM-10907)                       RTA-SUM Development Team
                    * Align LowVoltagePriorityRequest handling (GMSUM-10908) 
                    * Updated Re-try Periodicity (Fast/Slow) with Calibration 
                      for Remote Trgdvc Rejection (GMSUM-10917).

8.1.0_Pr31          * Add WUF rolling buffer support (GMSUM-10926).                            RTA-SUM Development Team
                    * Implemented SDVA-7619 of SDV-2017 27.27.196, immediate SM request
                      transmission if the remote device is in init (GMSUM-10912). 
                    * Add support for receiving a tag as feedback for a request that match
                      with 'None' (GMSUM-10918).
                    * Add clear of REJECTED_LACK_OF_POWER feedback (GMSUM-10910).	    
8.1.0_Pr32          * Updated retry policy for unacknowledged activation requests              RTA-SUM Development Team
                      to remote device (GMSUM-10913).
                    * Updates regarding the deactivation request (GMSUM-10914).
                    * Introduced usage of WUF Rolling Buffer (GMSUM-10928).
                    * BFS algorithm improvement (GMSUM-11054).
8.1.0_Pr33          * Align 0xE0D7 0x26 with the time window (GMSUM-10927).                    RTA-SUM Development Team
                    * Updated Intermediate Node implementation to make use of WUF stored
                      inside the Rolling Buffer (GMSUM-10930).
                    * Align check of own bit w.r.t. WUFs with timing windows (GMSUM-10929).
                    * Add retrieval of Source IP address from TcpIp module under particular
                      conditions (GMSUM-10992).
                    * Variant Handling part 1: restructured SPM to introduce
                      SpmVariant concept and adding provided port for the variant runtime 
                      configuration (GMSUM-10555).  
                    * Variant Handling Part 2: introduced new Sum_Spm_VariantManagement
                      sub module for branded/unbranded, WUF reception logic w.r.t.the active
                      variant, Rte_Call_SPM_RP_SPM_GetDeviceVariantInfo_setVariant
                      periodic calls for runtime configuration (GMSUM-10556).
                    * Extend the SpmVlanCom to receive more HB Pdu (GMSUM-11118).
                    * Deactivation request uses fast periodicity and does not prevent 
                      shutdown (GMSUM-10915).

8.1.0_rc            * Misra violations decreased. (GMSUM-11172)                                 RTA-SUM Development Team
                    * Update SPM timeout for voting for shutdown (GMSUM-11192).
                    * Removed Spm_DevicePowerState_isRead IRV and its usage (GMSUM-11295) 
                    * Remove the vehicle Off condition check for the remote requester
                      (GMSUM-11240)
=================  =========================================================================  =================================


*******************
Release Description
*******************

Intended Use
============

This version of RTA-SUM SPM is recommended for SDV 2.0 programs and is **NOT** qualified for use in
production/series releases.

Functions implemented in this release
=====================================

External References
-------------------

+-----------------------------------------------------------------------------+--------------------------------------------------+
|  Document Version                                                           |        Version                                   |
+=============================================================================+==================================================+
| SDV-2017 2.0 Vehicle Power Management Architecture                          |  3.2 - 27.181.2p0                                |
+-----------------------------------------------------------------------------+--------------------------------------------------+
| SDV-8217 Mechatronic Power Manager Infrastructure Specification             |  27.181.2p1                                      |
+-----------------------------------------------------------------------------+--------------------------------------------------+
| SDV-8201 Service Discovery Extension Specification                          |  27.27.175.2p0                                   |                                         
+-----------------------------------------------------------------------------+--------------------------------------------------+
| SDV-6001 Diagnostic Infrastructure Specification                            |  27.27.184.1p0                                   |                                         
+-----------------------------------------------------------------------------+--------------------------------------------------+

Implementation Description
--------------------------

This version of RTA-SUM SPM implements the following functions:

* Configuration of:
  
    * Power Profile 
    * Power Graph Traversal Table initial values
    * General Parameters (refer to :ref:`module-configuration-overview` in the user Guide)
  
* RTE ports to/from RTE for:

    * Sending a SM control message
    * Sending periodically current instance heartbeat
    * Receiving a SM control message
    * Receiving a Remote Host heartbeat
    * Sending a PM Ethernet wake-up frame message (*according to configuration values*)
    * Receiving a PM Ethernet Wake-up frame message (*according to configuration values*)
    * Sending the Shutdown permission to the RTA-SUM SUSD
    * Receiving the permit execution value from the RTA-SUM SUSD
    * Receiving the Activity Record command from the RTA-SUM SUSD
    * Publish the list of available services
    * Publish the list of Lost Nodes with the DTC status (Lost/Recover)
    * Publish the list of Services to be Activated/Deactivated
    * Receive a local request for activating/deactivating a Power Profile (*Profile locally activated*)
    * Receive a query for current state of a Power Profile
    * Receive a query for activation permission for a Power Profile
    * Provide the DTC number for a Target device
  
* Two Periodic Runnables:

    * To coordinate the Power Manager features as per SDV-8217 and SDV-2017 specifications (*Fixed periodicity to 5ms*)
    * To coordinate the Software Manager features as per SDV-8217 and SDV-2017 specifications (*Configurable periodicity*)

* Per-Instance parameters to calibrate the value of the Heartbeat monitoring timing counter initialization value
* An Initialization function
* Object's attributes and functions for aggregating the request/response to send to remote host
* Object's attributes and functions to build the message to a remote host.
* Functions to coordinate the Transmission and Reception of Wake-up frames on CAN (*According to configuration values*).

ETAS RTA-SUM SPM Related Software
=================================

All the release software is below folder *20_Implementation/*.

============================================================ ===============================================================================================================================
             File                                               Description                                    
============================================================ ===============================================================================================================================
scripts/paramdef/Spm_EcucParamDef.arxml                                 Parameter Definition schema.
scripts/templates/Spm_workflow.properties                               SPM Code generation Properties.
scripts/templates/Sum_Spm_bamf.arxml                                    SPM build action manifest.
scripts/templates/Sum_Spm_Generate.oaw                                  SPM generation action file.
scripts/templates/Sum_Spm_Validate.oaw                                  SPM Validation action file.
scripts/templates/Sum_Spm.chk                                           SPM configuration validation script.
scripts/templates/Spm_Utils.ext                                         SPM extensions scripts.
scripts/templates/Sum_Spm_Cfg_CDD_arxml.xpt                             Generation script for Complex Device Driver Software Component Description.
scripts/templates/Sum_SpmTypes_h.xpt			                              Generation script of Header for SPM Data types
scripts/templates/Sum_Spm_Cbk_c.xpt                                     Generation script of Source for managing Wake-up Frames on CAN and Ethernet.
scripts/templates/Sum_Spm_Cbk_h.xpt                                     Generation script of Header for managing Wake-up Frames on CAN and Ethernet.
scripts/templates/Sum_Spm_Cfg_c.xpt			                                Generation script of Source for Configuration dependent file.
scripts/templates/Sum_Spm_Cfg_h.xpt			                                Generation script of Header for Configuration dependent file.
scripts/templates/Sum_Spm_Diagnostic_c.xpt			                        Generation script of Source for The DID API implementation.
scripts/templates/Sum_Spm_Diagnostic_h.xpt			                        Generation script of Header for The DID API implementation.
scripts/templates/Sum_Spm_DevicePowerState_c.xpt			                  Generation script of Source for The Device Power state API implementation.
scripts/templates/Sum_Spm_DevicePowerState_h.xpt			                  Generation script of Header for The Device Power state API implementation.
scripts/templates/Sum_Spm_MetaData_c.xpt                                Generation script of Source for MetaData handling API implementation.
scripts/templates/Sum_Spm_MetaData_h.xpt                                Generation script of Header for MetaData handling API implementation.
scripts/templates/Sum_Spm_PBCfg_c.xpt			                              Generation script of Source for Post-Build data generated.
scripts/templates/Sum_Spm_PBCfg_h.xpt			                              Generation script of Header for Post-Build data generated.
scripts/templates/Sum_Spm_PowerOnDevice_c.xpt			                      Generation script of Source for PowerOnDevice API implementation.
scripts/templates/Sum_Spm_PowerOnDevice_h.xpt			                      Generation script of Header for PowerOnDevice API implementation.
scripts/templates/Sum_Spm_PowerProfileStateMachine_c.xpt			          Generation script of Source for Power Profile Finite State Machine management.
scripts/templates/Sum_Spm_PowerProfileStateMachine_h.xpt			          Generation script of Header for Power Profile Finite State Machine management.
scripts/templates/Sum_Spm_PwrMgr_c.xpt			                            Generation script of Source for Power Manager functionalities and coordination.
scripts/templates/Sum_Spm_PwrMgr_h.xpt			                            Generation script of Header for Power Manager functionalities and coordination.
scripts/templates/Sum_Spm_RequesterLst_c.xpt			                      Generation script of Source for Requester list for power profile.
scripts/templates/Sum_Spm_RequesterLst_h.xpt			                      Generation script of Header for Requester list for power profile.
scripts/templates/Sum_Spm_Requester_Queue_c.xpt                         Generation script of Source for Requester Queue for power profile.
scripts/templates/Sum_Spm_Requester_Queue_h.xpt                         Generation script of Header for Requester Queue for power profile.
scripts/templates/Sum_Spm_Services_c.xpt			                          Generation script of Source for SPM implementing services.
scripts/templates/Sum_Spm_Services_h.xpt			                          Generation script of Header for SPM implementing services.
scripts/templates/Sum_Spm_SM_Req_EthIf.c                                Generation script of Source for SPM builder of SM frames.
scripts/templates/Sum_Spm_SM_Req_EthIf.h                                Generation script of Header for SPM builder of SM frames.
scripts/templates/Sum_Spm_SoftwMgr_c.xpt			                          Generation script of Source for Software Manager functionalities and coordination.
scripts/templates/Sum_Spm_SoftwMgr_h.xpt			                          Generation script of Header for Software Manager functionalities and coordination.
scripts/templates/Sum_Spm_SrvcMgrIf_c.xpt			                          Generation script of Source for interfacing with Service Manager.
scripts/templates/Sum_Spm_SrvcMgrIf_h.xpt			                          Generation script of Header for interfacing with Service Manager.
scripts/templates/Sum_Spm_WakeUp_c.xpt			                            Generation script of source for Wake-up function.
scripts/templates/Sum_Spm_WakeUp_h.xpt			                            Generation script of Header for Wake-up.
scripts/templates/Sum_Spm_WakeUpHelperFunction_h.xpt			              Generation script of Header for external Helper Function (Project Specific).
scripts/templates/Sum_Spm_h.xpt			                                    Generation script of Header for SPM
scripts/templates/Sum_Spm_WufRollingBuffer_c.xpt			                  Generation script of Source for wake up frames functionalities and coordination.
scripts/templates/Sum_Spm_WufRollingBuffer_h.xpt		                    Generation script of Header for wake up frames functionalities and coordination.
api/Sum_Spm_CanIfDispatchUL.h                                           Header for defining all the function to control the State of CAN bus.
api/Cdd.h                                                               Header Dummy generated by BSW Code (Added only for compilation).
api/Sum_Spm_Integration.h                                               Header for Sum_Spm_Integration. 
api/Sum_Spm_PowerProfileStateMachineEventsCode.h                        Header that contains event codes for the updating of the power profile state machine.
api/Sum_Spm_SrvLstInfoItm.h                                             Header that contains the structure definition for service item.
scr/Sum_Spm_CanIfDispatchUL.c                                           Source file that contains all functions to set the state of the CAN Bus (Integrator scope).
scr/Sum_Spm_CommErrHndlr.c                                              Source file to handle an error on CAN wake-up transmission frame and takes the countermeasures (Integrator scope).
============================================================ ===============================================================================================================================                        

As additional work products, the current release includes some configuration arxml files to configure the involved modules of the CAN stack in the SPM feature.


================================================= =====================================================
 File                                                                   Description
================================================= =====================================================
IO_Aggregator2_Project_Can_EcucValues.arxml             Configuration of the CAN module.
EcuC_DummyPduTx_Rx_SPM_WkonCan.arxml                    Configuration of the PduRef for Pdu Length.
Cdd.arxml                                               Configuration of the Cdd for addressing
                                                        communication between SPM and CanIf.
CanIf.arxml                                             Configuration of the CanIf.
================================================= =====================================================

Configuration Management Items (Documents Only)
===============================================

============================================= ======================== =============================== ===============================
 File name                                              Version                 Description                     Author
============================================= ======================== =============================== ===============================
 user_guide.html                                    8.1.0_Pr33              RTA-SUM SPM User Guide       RTA-SUM Development Team
============================================= ======================== =============================== ===============================

.. note::

  The *user_guide.html* file can be found under the path *RTA-SUM_SPM_Docs/doc/UserGuide*.

Target Environment Description
==============================

N/A

Installation Instruction
========================

*N/A*

Licensing
=========

Please review to the Statement of Work for specific licensing information.

**********************
Change in This version
**********************

Change Requests
===============
=============== ================================================================================== ===============================================================================================================
 Jira Ticket     Description                                                                                    Implementation
=============== ================================================================================== ===============================================================================================================
GMSUM-11172       Misra violations have decreased                                                     The code has been analysed and updated in order to reduce the Misra violations.
GMSUM-11192      Reworked Shutdown Delay Timer after introducing Varians concept                      Shutdown Delay Timer has been moved and sice, while SPM is in Unbranded state, HB is not sent, such timer needs
                                                                                                      to be started to avoid ECU never going to sleep again if left Unbranded. 
=============== ================================================================================== ===============================================================================================================


Fixed in This Version
======================

=============== ================== ================================================================================== ===========================================================================================================================
 Jira Ticket     Hotline ticket      Description                                                                                    Implementation
=============== ================== ================================================================================== ===========================================================================================================================                                                            
GMSUM-11295              -           Spm_DevicePowerState_isRead causes issues                                          Removed Spm_DevicePowerState_isRead  IRV and its usage    
GMSUM-11240          RHGM-11608      Remove the vehicle Off condition check for the remote requester.                   Only the local requester to be deactivated once the vehicle Off timeout expires.           
=============== ================== ================================================================================== ===========================================================================================================================

.. ************
.. Know Issues
.. ************

.. N/A.

***************************************
Contacts, Support and Problem Reporting
***************************************

Contact details for your local sales office and local technical support team (where available) can be found on the ETAS web site:

* ETAS subsidiaries: https://www.etas.com/en/contact.php
* ETAS technical support: https://www.etas.com/en/hotlines.php