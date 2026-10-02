# Call Tree: Remote Activation Profile Request Flow

## Overview

Questo documento descrive il flusso completo delle funzioni coinvolte nelle richieste di attivazione del profilo di potenza provenienti da richieste remote. Analizza il percorso dal ricevimento iniziale del messaggio di rete fino all'attivazione effettiva del servizio, includendo le funzioni periodiche del sistema operativo.

## Call Tree Diagram

```
Network Message Reception
?
??? Operating System Periodic Cycle
    ?
    ??? SPM_SoftMgr_MainFunction()
        ?   ?? Main periodic function invoked by OS  
        ?   ?? Coordinates all SPM periodic activities
        ?
        ??? Spm_ReadIncoming_SM_Message()
            ?   ?? Reads and processes incoming SM messages from network
            ?   ?? Entry point for remote activation requests
            ?   ?? Validates destination IP and extracts source device ID
            ?
            ??? Rte_Receive_RP_SPM_RemSMInfo_SPM_dt()
            ?   ?   ?? Retrieves incoming message from remote SM via RTE
            ?   ?   ?? Network layer interface for SM frame reception
            ?   ?
            ?   ??? Spm_GetIndexTrgDvcFromIP()
            ?       ?? Finds target device index using IP address
            ?       ?? Maps IP address to internal device table
            ?
            ??? Spm_GetPowProfIndexFromSwIDProfNumber()
            ?   ?   ?? Retrieves power profile index from SW ID and profile number
            ?   ?   ?? Maps external IDs to internal profile configuration
            ?   ?
            ?   ??? Spm_prv_SM_RemoteActDeactRequestDispatch()
            ?       ?   ?? Checks for conflicting requests in the same frame
            ?       ?   ?? Ensures no activation/deactivation conflicts for same profile
            ?       ?   ?? Prevents inconsistent state requests
            ?       ?
            ?       ??? Spm_Handler_prfReqDispatcherForActivationFromRemote()
            ?           ?   ?? Validates remote activation request permissions
            ?           ?   ?? Checks activation availability for remote requests
            ?           ?
            ?           ??? Spm_Get_ActivationAvailabilityForRemoteRequest()
            ?           ?   ?? Determines if profile activation is permitted
            ?           ?   ?? Validates vehicle conditions and permissions
            ?           ?
            ?           ??? Spm_prv_prfReqDispatcher()
            ?               ?   ?? Handles profile dependency resolution
            ?               ?   ?? Same dispatcher used for local requests
            ?               ?   ?? Manages recursive dependency activation
            ?               ?
            ?               ??? SPM_EnqueueRequest()
            ?                   ?   ?? Adds request to internal processing queue
            ?                   ?   ?? Ensures thread-safe request handling
            ?                   ?
            ?                   ???????????????????????????????????????????????????????????
            ?                   ?           SUBSEQUENT OS PERIODIC CYCLES                 ?
            ?                   ?                                                         ?
            ?                   ??? SPM_SoftMgr_MainFunction()                           ?
            ?                       ?   ?? Continues periodic processing                 ?
            ?                       ?                                                     ?
            ?                       ??? Spm_DequeueReqActionHandler()                    ?
            ?                           ?   ?? Processes queued requests                 ?
            ?                           ?   ?? Manages request processing state machine ?
            ?                           ?                                                 ?
            ?                           ??? SPM_DequeueRequest()                         ?
            ?                               ?   ?? Removes request from queue           ?
            ?                               ?   ?? Returns next request to process      ?
            ?                               ?                                             ?
            ?                               ??? Spm_RequesterList_ActivationRequestHandler() ?
            ?                                   ?   ?? Manages remote requester lists       ?
            ?                                   ?   ?? Updates activation request status    ?
            ?                                   ?                                             ?
            ?                                   ??? Spm_FSMManger()                          ?
            ?                                       ?   ?? Manages all power profile FSMs   ?
            ?                                       ?   ?? Coordinates state transitions     ?
            ?                                       ?                                         ?
            ?                                       ??? Spm_RemotePowerProfile_FSM()          ?
            ?                                           ?   ?? Handles remote profile FSM     ?
            ?                                           ?   ?? Manages remote activation logic ?
            ?                                           ?                                       ?
            ?                                           ??? Spm_ActivateServiceSrvcMgrHandlerFunction() ?
            ?                                           ?   ?   ?? Interfaces with external service mgr ?
            ?                                           ?   ?   ?? Triggers actual service activation  ?
            ?                                           ?   ?                                           ?
            ?                                           ?   ??? SrvcMgr_ActivateServices()             ?
            ?                                           ?       ?? External service manager interface  ?
            ?                                           ?       ?? Performs actual service activation  ?
            ?                                           ?                                               ?
            ?                                           ??? Spm_SwitchNetworkPdu()                     ?
            ?                                               ?? Enables/disables network PDU groups    ?
            ?                                               ?? Manages network communication state    ?
            ?
            ??? Spm_CheckFeedbackReceivedByRequested()
                ?   ?? Processes feedback from previously requested profiles
                ?   ?? Updates request status based on remote responses
                ?   ?? Handles acknowledgments from target devices
                ?
                ??? Spm_ProvideInitialFeedbackToRemoteRequester()
                    ?? Sends feedback to remote requester about request status
                    ?? Provides immediate response for request validation
```

## Function Descriptions

### Network Message Reception Functions

#### `Spm_ReadIncoming_SM_Message()`
- **Called by**: `SPM_SoftMgr_MainFunction()` (periodically)
- **Purpose**: Main entry point for processing remote activation requests
- **Why called**: Checks for and processes incoming SM frames from remote devices
- **Key actions**: Message validation, source device identification, request parsing
- **Parameters**: None (reads from RTE queue)
- **Return**: void
- **Critical for**: Initial remote request reception and validation

#### `Rte_Receive_RP_SPM_RemSMInfo_SPM_dt()`
- **Called by**: `Spm_ReadIncoming_SM_Message()`
- **Purpose**: Retrieves incoming messages from remote SM via RTE interface
- **Why called**: Provides network layer interface for SM frame reception
- **Key actions**: Message extraction from RTE queue, metadata handling
- **Parameters**: Message pointer, metadata array
- **Return**: Std_ReturnType (E_OK if message available)

### Request Validation Functions

#### `Spm_GetIndexTrgDvcFromIP()`
- **Called by**: `Spm_ReadIncoming_SM_Message()`
- **Purpose**: Maps remote device IP address to internal device index
- **Why called**: Validates source device and enables device monitoring
- **Key actions**: IP address lookup, device table validation
- **Parameters**: uint32 IP address
- **Return**: sint16 device index (SPM_INVALID if not found)

#### `Spm_GetPowProfIndexFromSwIDProfNumber()`
- **Called by**: `Spm_ReadIncoming_SM_Message()`
- **Purpose**: Maps external SW ID and profile number to internal profile index
- **Why called**: Translates remote request identifiers to local configuration
- **Key actions**: Configuration table lookup, profile validation
- **Parameters**: uint32 SW ID, uint8 profile number
- **Return**: sint16 profile index (SPM_INVALID if not found)

### Conflict Resolution Functions

#### `Spm_prv_SM_RemoteActDeactRequestDispatch()`
- **Called by**: `Spm_ReadIncoming_SM_Message()`
- **Purpose**: Validates frame integrity and prevents conflicting requests
- **Why called**: Ensures no activation/deactivation conflicts in same frame
- **Key actions**: Frame traversal, conflict detection, error feedback generation
- **Parameters**: Frame index, profile index, SM frame pointer, return status
- **Return**: void
- **Critical for**: Preventing inconsistent state requests from same source

#### `Spm_Handler_prfReqDispatcherForActivationFromRemote()`
- **Called by**: `Spm_prv_SM_RemoteActDeactRequestDispatch()`
- **Purpose**: Validates remote activation request permissions
- **Why called**: Checks if remote activation is allowed for the profile
- **Key actions**: Permission checking, activation availability validation
- **Parameters**: Profile index, source IP requester
- **Return**: void

### Permission Validation Functions

#### `Spm_Get_ActivationAvailabilityForRemoteRequest()`
- **Called by**: `Spm_Handler_prfReqDispatcherForActivationFromRemote()`
- **Purpose**: Determines activation permission for remote requests
- **Why called**: Validates vehicle conditions and profile-specific permissions
- **Key actions**: Vehicle power mode check, battery voltage validation, profile permission check
- **Parameters**: Profile index
- **Return**: SpmRejectionReasonType (acceptance or rejection reason)

### Request Processing Functions

#### `Spm_prv_prfReqDispatcher()`
- **Called by**: `Spm_Handler_prfReqDispatcherForActivationFromRemote()`
- **Purpose**: Handles profile dependency resolution and request queuing
- **Why called**: Manages recursive dependency activation (same as local requests)
- **Key actions**: Dependency tree traversal, queue management, recursive activation
- **Parameters**: Profile index, requester IP, request code, local/remote flag, return status
- **Return**: void
- **Note**: Same function used for local requests, ensuring consistent processing

### Queue Management Functions

#### `SPM_EnqueueRequest()`
- **Called by**: `Spm_prv_prfReqDispatcher()`
- **Purpose**: Adds activation request to internal processing queue
- **Why called**: Ensures thread-safe request handling and deferred processing
- **Key actions**: Queue insertion, request serialization, thread safety
- **Parameters**: Queue pointer, request item
- **Return**: boolean (success/failure)

### Periodic Processing Functions

#### `Spm_DequeueReqActionHandler()`
- **Called by**: `SPM_SoftMgr_MainFunction()` (subsequent cycles)
- **Purpose**: Processes queued activation requests during OS cycles
- **Why called**: Provides controlled, periodic processing of remote requests
- **Key actions**: Queue management, processing state machine, error handling

#### `SPM_DequeueRequest()`
- **Called by**: `Spm_DequeueReqActionHandler()`
- **Purpose**: Removes next request from queue for processing
- **Why called**: Retrieves next queued request in FIFO order
- **Key actions**: Queue removal, request validation, thread safety

### State Machine Functions

#### `Spm_RequesterList_ActivationRequestHandler()`
- **Called by**: `Spm_DequeueReqActionHandler()`
- **Purpose**: Manages remote requester lists and activation states
- **Why called**: Tracks remote requesters and manages their activation states
- **Key actions**: Requester list updates, state tracking, conflict resolution

#### `Spm_FSMManger()`
- **Called by**: `Spm_RequesterList_ActivationRequestHandler()`
- **Purpose**: Manages all power profile finite state machines
- **Why called**: Coordinates state transitions across multiple profiles
- **Key actions**: FSM scheduling, state synchronization, transition management

#### `Spm_RemotePowerProfile_FSM()`
- **Called by**: `Spm_FSMManger()`
- **Purpose**: Handles remote power profile state machine logic
- **Why called**: Manages specific activation logic for remote power profiles
- **Key actions**: State transitions, remote communication, activation conditions

### Service Activation Functions

#### `Spm_ActivateServiceSrvcMgrHandlerFunction()`
- **Called by**: `Spm_RemotePowerProfile_FSM()`
- **Purpose**: Interfaces with external service manager for service activation
- **Why called**: Bridges SPM with external service management system
- **Key actions**: Service manager interface, activation triggering

#### `SrvcMgr_ActivateServices()`
- **Called by**: `Spm_ActivateServiceSrvcMgrHandlerFunction()`
- **Purpose**: External service manager interface for actual service activation
- **Why called**: Performs the actual service activation outside SPM
- **Key actions**: Service activation, dependency management, status reporting

### Network Management Functions

#### `Spm_SwitchNetworkPdu()`
- **Called by**: `Spm_RemotePowerProfile_FSM()`
- **Purpose**: Enables/disables network PDU groups for activated services
- **Why called**: Manages network communication state for activated services
- **Key actions**: PDU group management, network interface control

### Feedback Management Functions

#### `Spm_CheckFeedbackReceivedByRequested()`
- **Called by**: `Spm_ReadIncoming_SM_Message()`
- **Purpose**: Processes feedback from previously requested profiles
- **Why called**: Updates request status based on remote responses
- **Key actions**: Feedback validation, status updates, acknowledgment processing
- **Parameters**: Profile index, status request, profile state
- **Return**: void

#### `Spm_ProvideInitialFeedbackToRemoteRequester()`
- **Called by**: Various validation functions
- **Purpose**: Sends immediate feedback to remote requester
- **Why called**: Provides quick response for request validation results
- **Key actions**: Feedback message generation, error reporting
- **Parameters**: Profile index, requester IP, status code, etc.
- **Return**: void

## Key Differences from Local Request Flow

### 1. Network Message Processing
- **Remote**: Starts with `Spm_ReadIncoming_SM_Message()` reading from the network
- **Local**: Starts with `SPM_ActivateProfile()` API call

### 2. Source Validation
- **Remote**: Requires IP address validation and device monitoring setup
- **Local**: Uses application interface ID validation

### 3. Conflict Detection
- **Remote**: Must check for conflicts within the same network frame
- **Local**: Checks for conflicts with ongoing local requests

### 4. Permission Validation
- **Remote**: Uses `Spm_Get_ActivationAvailabilityForRemoteRequest()`
- **Local**: Uses different local permission validation logic

### 5. Feedback Mechanism
- **Remote**: Must send network feedback to the requester via SM frames
- **Local**: Uses RTE return codes and local notifications

### 6. State Machine
- **Remote**: Uses `Spm_RemotePowerProfile_FSM()`
- **Local**: Uses `Spm_LocalPowerProfile_FSM()`

## Integration Points

Both remote and local flows converge at:
- **`Spm_prv_prfReqDispatcher()`**: Common dependency resolution logic
- **`SPM_EnqueueRequest()`**: Unified request queuing mechanism
- **`Spm_FSMManger()`**: Central state machine coordinator
- **`SrvcMgr_ActivateServices()`**: External service activation interface

This design ensures consistent behavior regardless of request origin while maintaining appropriate validation and feedback mechanisms for each request type.
