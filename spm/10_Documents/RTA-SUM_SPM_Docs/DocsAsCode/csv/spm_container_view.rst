
SPM Configuration
------------------

.. csv-table:: The description of configuration parameters are the following:
    :header: "Container Name", "Description"
    :file: containers.csv
    :widths: 10, 25
    
    
SpmGeneral parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmSMMainFuncPeriodicity", "Periodicity in milliseconds for the execution of SPM Software Manager functionalities.", "FLOAT", "1 .. 1"
    "SpmPMMainFuncPeriodicity", "(CURRENTLY NOT SUPPORTED. VALUE Fixed to 5ms) Periodicity in milliseconds for the execution of the SPM Power manager functionalities.", "FLOAT", "1 .. 1"
    "SpmBurstPhsPeriodicity", "BurstPhsPeriodicity is a period less than the usual one, that happens as soon as SPM starts counting and sharing its heartbeat and the wake-up frame. This period is maintained for the first 3 counts. The parameter configures the message periodicity in milliseconds for this phase before switching to the normal one.", "INTEGER", "1 .. 1"
    "SpmPermitExecutionMask", "The parameter is a bitmap (1 Byte length) that defines the DON'T CARE wake-up sources that trigger the SPM execution. The DON'T CARE wake-up sources needs to be configured as 1.", "INTEGER", "1 .. 1"
    "SpmWkUpHelperFunction", "The parameter defines the name of the wake-up helper function to be used by SPM, in case the ECU support the SDV-8217 defined Custom wake-up mechanism. The support of this function is valid only for intermediate nodes as CHM MCU.", "STRING", "0 .. 1"
    "SpmInitFuncPeriodicity", "Period in milliseconds that configures the runnable init function periodicity. This parameter is required when SpmGenerateInitRunnable is set to TRUE.", "FLOAT", "0 .. 1"
    "SpmGenerateInitRunnable", "The parameter shall generate SPM init runnable if it is configured as TRUE. Otherwise the SPM_Init() function shall be called by a BswM action list during startup", "BOOLEAN", "1 .. 1"
    "Spm_SmFrame_QueueLength", "Maximum queue length of SM Frame Rx messages. If the parameter is not configured it takes the number of target devices * number of local profiles as default value.", "INTEGER", "0 .. 1"
    "Spm_PmHeartbeat_QueueLength", "Maximum queue length of PM heartbeat Rx messages. If the parameter is not configured it takes the number of target devices as default value.", "INTEGER", "0 .. 1"
    "Spm_PmETHwakeUpUNIC_QueueLength", "Maximum queue length of PM eth wakeup Unicast Rx messages. If the parameter is not configured it takes the number of target devices as default value.", "INTEGER", "0 .. 1"
    "Spm_PmETHwakeUpMULTIC_QueueLength", "Maximum queue length of PM eth wakeup Multicast Rx messages. If the parameter is not configured it takes the following as a default value ((SM main function periodicity / 5) + 1) SM main function periodicity.", "INTEGER", "0 .. 1"
    "Spm_LocalRequests_QueueLength", "Maximum queue length of local activation/deactivatino requests as well as diagnostic requests. If the parameter is not configured it take the number of active profiles * 2 as default value.", "INTEGER", "0 .. 1"
    "SpmHBShutdownDelayMs", "Initial value for the calibration parameter indicating the time (in ms) the PM has to wait, after the last burst HeartBeat has been sent, before allowing the shutdown (Keep-Awake Timer)", "INTEGER", "1 .. 1"
    "SpmSMStayAwakeTimeMs", "Calibration parameter value indicating the time (in ms) during which the SM should stay awake if the only requester is in fault or restart state, before allowing shutdown.", "INTEGER", "1 .. 1"
    
SpmDiagnostic parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "spm_DID_E0DD_ActivationProfileActivationStatus_DCM_Ref", "Reference to the DcmDspDid for supporting DID 0xE0DD Activation Profile Activation Status", "REFERENCE", "0 .. 1"
    "SpmDID0xE0DEDcmRef", "Reference to the DcmDspDid for supporting DID 0xE0DE Locally Activated Activation Profile Off Power Mode Activity Records", "REFERENCE", "0 .. 1"
    "SpmDID0xE0DFDcmRef", "Reference to the DcmDspDid for supporting DID 0xE0DF Remotely Activated Activation Profile Off Power Mode Activity Records", "REFERENCE", "0 .. 1"
    "SpmDID0xE0DENvmRef", "Reference to the NvM block for DID 0xE0DE Locally Activated Activation Profile Off Power Mode Activity Records", "REFERENCE", "0 .. 1"
    "SpmDID0xE0DFNvmRef", "Reference to the NvM block for DID 0xE0DF Remotely Activated Activation Profile Off Power Mode Activity Records", "REFERENCE", "0 .. 1"
    
SpmETHWkUpFrmRef parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmETHSwitchNetwork", "Reference to the network handle ID", "REFERENCE", "0 .. 0"
    "SpmETHWkUpFrmTxRef", "Reference to the SOAd Pdu in Tx", "REFERENCE", "0 .. 0"
    "SpmETHWkUpFrmRxRef", "Reference to the SOAd Pdu in Rx", "REFERENCE", "0 .. 0"
    
SpmCANWkUpFrmRef parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmCANWkUpFrmTxRef", "parameters defines the references to CanIfPdu in Tx side for SPM CAN wake-up frames", "REFERENCE", "0 .. 3"
    "SpmCANWkUpFrmRxRef", "parameters defines the references to CanIfPdu in Rx side for SPM CAN wake-up frames", "REFERENCE", "0 .. Inf"
    
SpmPowPrf parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmSrvcAndInstID", "The parameter defines the Profile associated software service identifier and the service instance number. Software service represents the 16 most significant bits of the configured number. The instance number the 16 less significant ones. It is assumed at least one profile is managed by the current node.", "INTEGER", "1 .. 1"
    "SpmProfNumber", "The parameter defines the Profile number.", "INTEGER", "1 .. 1"
    "SpmProfileIsLocal", "The parameter addresses if the current Power Profile is Local to the ECU or it is a remote service. For Local profile the value is TRUE.", "BOOLEAN", "1 .. 1"
    "SpmLocActivationAllowed", "The parameter defines if the profile can be locally activated. This means that the activation request can be done by an ECU-local SWC using CLIENT/SERVER operation. Only local profile can be activated from a local SWC. So the default value is FALSE.", "BOOLEAN", "1 .. 1"
    "SpmApplIntfID", "The parameter defines the profile enumeration value to be used for asking the profile activation locally. This parameter is configured only in case SpmLocActivationAllowed is TRUE. Lower multiplicity is 0 because it is valid only for local profiles", "INTEGER", "0 .. 1"
    "SpmProfBitMapIndex", "The parameter maps the profile status as a specific bit of value. This value is reported by the DID E0DD and specified in SDV-6001.", "INTEGER", "0 .. 1"
    "SpmProfActivationTimeOut", "OBSOLETE - NAMING MISMATCH", "INTEGER", "0 .. 0"
    "SpmProfActivationTimeOutVehOff", "The parameter defines the duration in Seconds of a valid timing-window before deactivating the profile automatically. Valid only for local profiles. The default value is 0, meaning that the profile is automatically deactivated when VehiclePowerMode is OFF.", "INTEGER", "0 .. 1"
    "SpmVehONDefActPriorON", "OBSOLETE - NAMING MISMATCH", "ENUMERATION", "0 .. 0"
    "SpmVehDefActPriorON", "The parameter defines the profile activation priority when vehicle is going to ON. The Lower multiplicity  is 0 because it is valid only for local profiles, while the Upper multiplicity is 1 because it is unique for each profile. Default value is SPM_PRF_AP_NOTAPP.", "ENUMERATION", "0 .. 1"
    "SpmVehDefActPriorOFF", "The parameter defines the profile activation priority when vehicle is going to OFF. The Lower multiplicity is 0 because it is valid only for local profiles, while the Upper multiplicity is 1 because it is unique for each profile. The default value is SPM_PRF_AP_NOTAPP.", "ENUMERATION", "0 .. 1"
    "SpmComIPduGroupEvnt_Tx", "The parameter defines the reference to data PDU to enable/disable in Tx once a profile request has been satisfied if the configuration requires profile to be consumed.", "REFERENCE", "0 .. Inf"
    "SpmComIPduGroupEvnt_Rx", "The parameter defines the references to data PDU to enable/disable in Rx once a profile request has been satisfied if the configuration requires profile to be consumed.", "REFERENCE", "0 .. Inf"
    
SpmProfileSwDep parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmProfDepType", "The parameter defines the type of dependency for the power profile. If SPM_PRF_DEP_ACT_AND_CONS then the profile is activated as services and services-data PDU are switched ON in RX by the current ECU instance. If SPM_PRF_ACTIVATED only service is activated. If SPM_PRF_CONSUMED only service-data pdu are switched ON in RX for the current ECU instance. All the dependencies need to be identified by this parameter (mandatory).", "ENUMERATION", "1 .. 1"
    "SpmProfRef", "The parameter defines the reference to configured profile that acts as current profile dependency.", "REFERENCE", "1 .. 1"
    
    
SpmTrgDvc parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmTrgDvcID", "The parameter defines all the IPv4 address for the target device. Each Target device could have more IP addresses since it can be reached from different VLAN.", "STRING", "1 .. Inf"
    "SpmTrgDvcDtcBaseID", "The parameter defines the base DTC (2 bytes) to set when the target communication is lost.", "INTEGER", "1 .. 1"
    "SpmTrgDvcDtcTimeOut", "The parameter defines the maturation time to set when the target communication is lost.", "INTEGER", "1 .. 1"
    "SpmTrgDvcPowerOnDeviceRetryPeriodicity", "This parameter defines the periodicity of the PowerOnDevice retry. Please, configure a value that it is greater than the sum of the SpmSeqStepDly of the wake-up sequence steps PLUS the value of SpmTrgDvcPowerOnDeviceWaitTimeBetweenOffandOnSequence PLUS the sum of the delay values between steps in the sequence PLUS the SpmTrgDvcDtcTimeOut of this TargetDevice. The value is in milliseconds.", "INTEGER", "0 .. 1"
    "SpmTrgDvcPowerOnDeviceWaitTimeBetweenOffandOnSequence", "The parameter defines the time in milliseconds to wait between the OFF and ON sequence of the target device during the PowerOnDevice retry. Please, configure a value that it is greater than 250 ms to see changes in the WUF. The value it is in milliseconds.", "INTEGER", "0 .. 1"
    "SpmTrgDvcDiagAddr", "The parameter defines the diagnostic address associated to the target for the diagnostic sessions.", "INTEGER", "0 .. 1"
    
SpmTrgDvcVariantHndlng parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmTrgAssocWkUpFrm", "Wake-up frame for this variant/target combination. Present only when Directly Managed by the Waking Device is True.", "ENUMERATION", "0 .. 1"
    "SpmTrgAssocWkupBit", "Wake-up bit for this variant/target combination. Present only when Directly Managed by the Waking Device is True.", "INTEGER", "0 .. 1"
    "SpmTrgDvcVariantRef", "Reference to the SpmVariantCfg container identifying which variant this entry applies to.", "REFERENCE", "1 .. 1"
    "SpmWkUpSeqRef", "The parameter defines the reference to the wake-up sequence to use. In case this value is not configured, then it means the target device is a requester for the current SPM.", "REFERENCE", "0 .. 1"
    
SpmWkngDvc parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmNodeType", "The parameter defines the type of dependency for the power profile. The possibilities are SPM_NODE_EDGE and SPM_NODE_INTERMEDIATE. The default value is considering the node as an intermediate. Mandatory parameter.", "ENUMERATION", "1 .. 1"
    
SpmVariantCfg parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmVariantWkupFrame", "Wake-up frame for this variant. From Wakeup Frame Information per variant.", "ENUMERATION", "1 .. 1"
    "SpmVariantWkupBit", "Wake-up bit position (0..63) for this variant. From Wakeup Frame Information per variant.", "INTEGER", "1 .. 1"
    "SpmVariantIntermediateDstAddr", "IP address(es) of intermediate nodes for this variant. Mirrors SpmWkngDvc.SpmIntermediateDstAddrs for the no-variant path.", "STRING", "0 .. Inf"
    
SpmVariantHBVLANCfg parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmVLANId", "VLAN identifier for this variant heartbeat entry.", "INTEGER", "1 .. 1"
    "SpmSrcIp", "Source IP address on this VLAN for this variant.", "STRING", "1 .. 1"
    "SpmDstIp", "Multicast destination IP address on this VLAN for this variant.", "STRING", "1 .. 1"
    
SpmWkUpSeq parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmComStepSeparatPeridoForShutDownInit", "(OBSOLETE - NOT SUPPORTED ANYMORE)parameter defines the type of command in the step", "INTEGER", "0 .. 0"
    
SpmWkUpStep parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmStepID", "parameter defines the type of command in the step", "INTEGER", "1 .. 1"
    "SpmCommandType", "The parameter defines the type of command in the step. Each command type will address a different way to wake-up a target device.", "ENUMERATION", "1 .. 1"
    "SpmCommandValue", "parameter defines the value for the command step", "INTEGER", "1 .. 1"
    "SpmSeqStepDly", "The parameter defines the delay in milliseconds to spend before executing the next step in the sequence. The value is mandatory but it is assumed 0 as default value.", "INTEGER", "1 .. 1"
    
SpmVLANCom parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmVLANId", "The parameter defines the VLAN identifier for the configured interface.", "INTEGER", "1 .. 1"
    
SpmVLANComPwrMgrHeartbeat parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmHbTxAbsPth", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface is defined.", "STRING", "0 .. 1"
    "SpmHbRxAbsPth", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface Implementation Data Type is defined.", "STRING", "0 .. 1"
    "SpmPwrMgrHeartbeatTxImpDataTypeRef", "parameters defines the references to PwrMgrHeartbeat Tx Implementation Data Type", "REFERENCE", "0 .. 1"
    "SpmPwrMgrHeartbeatTxSendRecvIntRef", "parameters defines the references to PwrMgrHeartbeat Tx Sender Receiver Interface", "REFERENCE", "0 .. 1"
    "SpmPwrMgrHeartbeatRxImpDataTypeRef", "parameters defines the references to PwrMgrHeartbeat Rx Implementation Data Type", "REFERENCE", "0 .. Inf"
    "SpmPwrMgrHeartbeatRxSendRecvIntRef", "parameters defines the references to PwrMgrHeartbeat Rx Sender Receiver Interface", "REFERENCE", "0 .. Inf"
    
SpmVLANComPwrMgrWakeupFrames parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmWkupTxAbsPth", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface is defined.", "STRING", "0 .. 1"
    "SpmWkupRxAbsPth", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface Implementation Data Type is defined.", "STRING", "0 .. 1"
    "SpmPwrMgrWakeupTxImpDataTypeRef", "parameters defines the references to PwrMgrWakeup Tx Implementation Data Type", "REFERENCE", "0 .. 1"
    "SpmPwrMgrWakeupRxImpDataTypeRef", "parameters defines the references to PwrMgrWakeup Rx Implementation Data Type", "REFERENCE", "0 .. 1"
    "SpmPwrMgrWakeupTxSendRecvIntRef", "parameters defines the references to PwrMgrWakeup Tx Sender Receiver Interface", "REFERENCE", "0 .. 1"
    "SpmPwrMgrWakeupRxSendRecvIntRef", "parameters defines the references to PwrMgrWakeup Rx Sender Receiver Interface", "REFERENCE", "0 .. 1"
    
SpmVLANComPwrMgrWakeupFramesMulticast parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmWkupRxAbsPthMulticast", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface Implementation Data Type is defined.", "STRING", "0 .. 1"
    "SpmPwrMgrWakeupRxImpDataTypeMulticastRef", "parameters defines the references to PwrMgrWakeup Rx Multicast Implementation Data Type", "REFERENCE", "0 .. 1"
    "SpmPwrMgrWakeupRxSendRecvIntMulticastRef", "parameters defines the references to PwrMgrWakeup Rx Multicast Sender Receiver Interface", "REFERENCE", "0 .. 1"
    
SpmVLANComSwrMgrInfo parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmSmTxAbsPth", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface is defined.", "STRING", "0 .. 1"
    "SpmSmRxAbsPth", "The parameter defines absolute XML path in the extract in which the VLAN sender-receiver interface Implementation Data Type is defined.", "STRING", "0 .. 1"
    "SpmSwMgrInfoTxImpDataTypeRef", "parameters defines the references to SwMgrInfo Tx Implementation Data Type", "REFERENCE", "0 .. 1"
    "SpmSwMgrInfoRxImpDataTypeRef", "parameters defines the references to SwMgrInfo Rx Implementation Data Type", "REFERENCE", "0 .. 1"
    "SpmSwMgrInfoTxSendRecvIntRef", "parameters defines the references to SwMgrInfo Tx Sender Receiver Interface", "REFERENCE", "0 .. 1"
    "SpmSwMgrInfoRxSendRecvIntRef", "parameters defines the references to SwMgrInfo Rx Sender Receiver Interface", "REFERENCE", "0 .. 1"
    
SpmLowVoltagePowerInfo parameter definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table:: The description of configuration parameters for :
    :header: "Parameter", "Description", "Type", "Multiplicity"

    "SpmLowVoltSwActPrioReqSignal", "Reference to the ComSignal (Service Based) that carries the LowVoltageSoftwareActivationPriorityRequest information.", "REFERENCE", "0 .. 1"
    "SpmLowVoltSwActPrioReqDataServiceBased", "Reference to the implementation data type element for service based LowVoltageSoftwareActivationPriorityRequest.", "REFERENCE", "0 .. 1"
    "SpmLowVoltSwActPrioReqDataTypeMappingSet", "Reference to the DATA-TYPE-MAPPING-SET in the system extract for service-based operations.", "REFERENCE", "0 .. 1"
    "SpmLowVoltSwActPrioNvmRef", "Reference to NvM LowVoltageSoftwareActivationPriorityRequest NvMBlockDescriptor.", "REFERENCE", "0 .. 1"
    "SpmLowVoltLVOptmzerChkApplicationInterfaceID", "LVOptmzerChk Application Interface ID of the power profile associated with the LV Energy Optimizer.", "INTEGER", "0 .. 1"
