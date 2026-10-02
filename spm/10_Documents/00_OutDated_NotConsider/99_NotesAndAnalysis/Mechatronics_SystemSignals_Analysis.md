# Analisi System Signal Mechatronics - SADM ARXML

## Panoramica

Questo documento contiene l'analisi completa di tutti i System Signal Mechatronics presenti nel file ARXML `SADM_MergedEcuExtract.arxml` e dei loro mapping verso I-Signal e PDU.

## System Signal Mechatronics Identificati

### 1. Mechatronics_SwMgrInfoRx_SystemSignal

**Path ARXML:** `/SVC_DEF_Hierarchy/SVC_SYS_Mechatronics_SwMgrInfoDefRx/Mechatronics_SwMgrInfoRx_SystemSignal`

**Caratteristiche:**
- **UUID:** `b3295816-e785-c462-40de-74ff7a557e57`
- **Dynamic Length:** `true`
- **Funzione:** Ricezione informazioni Software Manager

**Mapping I-Signal:**
- **Nome:** `Mechatronics_SwMgrInfoRx_SADM_v20_ISignal`
- **Path:** `/Topologies/SDV/Ethernet_1/Mechatronics_SwMgrInfoRx_SADM_v20_ISignal`
- **UUID:** `1566970e-b593-4d9d-419c-46b0888dd154`
- **Lunghezza:** 11184 bytes
- **Tipo:** ARRAY
- **Data Type Policy:** TRANSFORMING-I-SIGNAL
- **Protocollo:** SOME/IP (MESSAGE-TYPE: NOTIFICATION)
- **Interface Version:** 1

**Mapping PDU:**
- **PDU:** `SADM_Eth_v20_PDU_SYS_SMUnicastRxMechatronics_Info`
- **Secured PDU:** `SADM_Eth_v20_SPDU_SYS_SMUnicastRxMechatronics_Info`
- **PDU Group:** `Ethernet_1_itsSADM_Mechatronics_SwMgrInfoRx_IN`
- **Tipo comunicazione:** Unicast RX

### 2. Mechatronics_SwMgrInfoTx_SystemSignal

**Path ARXML:** `/SVC_DEF_Hierarchy/SVC_SYS_Mechatronics_SwMgrInfoDef/Mechatronics_SwMgrInfoTx_SystemSignal`

**Caratteristiche:**
- **UUID:** `77520418-cf87-4550-62b5-786e87a80599`
- **Dynamic Length:** `true`
- **Funzione:** Trasmissione informazioni Software Manager

**Mapping I-Signal:**
- **Nome:** `Mechatronics_SwMgrInfoTx_SADM_v20_ISignal`
- **Path:** `/Topologies/SDV/Ethernet_1/Mechatronics_SwMgrInfoTx_SADM_v20_ISignal`
- **Tipo:** ARRAY con Dynamic Length

**Mapping PDU:**
- **PDU:** `SADM_Eth_v20_PDU_SYS_SMUnicastTxMechatronics_Info`
- **Secured PDU:** `SADM_Eth_v20_SPDU_SYS_SMUnicastTxMechatronics_Info`
- **PDU Group:** `Ethernet_1_itsSADM_Mechatronics_SwMgrInfoTx_OUT`
- **Tipo comunicazione:** Unicast TX

### 3. Mechatronics_PwrMgrHeartbeatRx_SystemSignal

**Path ARXML:** `/SVC_DEF_Hierarchy/SVC_SYS_Mechatronics_PwrMgrHeartbeatDefRx/Mechatronics_PwrMgrHeartbeatRx_SystemSignal`

**Caratteristiche:**
- **UUID:** `2f6c1b9b-e694-687e-2b05-f25fd46dfa2c`
- **Dynamic Length:** `true`
- **Funzione:** Ricezione Heartbeat Power Manager

**Mapping I-Signal:**
- **Nome:** `Mechatronics_PwrMgrHeartbeatRx_SADM_v20_ISignal`
- **Path:** `/Topologies/SDV/Ethernet_1/Mechatronics_PwrMgrHeartbeatRx_SADM_v20_ISignal`

**Mapping PDU:**
- **PDU:** `SADM_Eth_v20_PDU_SYS_PMMulticastRxMechatronics_Heartbeat`
- **Secured PDU:** `SADM_Eth_v20_SPDU_SYS_PMMulticastRxMechatronics_Heartbeat`
- **PDU Group:** `Ethernet_1_itsSADM_Mechatronics_PwrMgrHeartbeatRx_IN`
- **Tipo comunicazione:** Multicast RX
- **SecOC Data ID:** 33932

### 4. Mechatronics_PwrMgrHeartbeatTx_SystemSignal

**Path ARXML:** `/SVC_DEF_Hierarchy/SVC_SYS_Mechatronics_PwrMgrHeartbeatDef/Mechatronics_PwrMgrHeartbeatTx_SystemSignal`

**Caratteristiche:**
- **UUID:** `4092b7cc-b5bf-d9f9-0e2f-49769ad04573`
- **Dynamic Length:** `true`
- **Funzione:** Trasmissione Heartbeat Power Manager

**Mapping I-Signal:**
- **Nome:** `Mechatronics_PwrMgrHeartbeatTx_SADM_v20_ISignal`
- **Path:** `/Topologies/SDV/Ethernet_1/Mechatronics_PwrMgrHeartbeatTx_SADM_v20_ISignal`

**Mapping PDU:**
- **PDU:** `SADM_Eth_v20_PDU_SYS_PMMulticastTxMechatronics_Heartbeat`
- **Secured PDU:** `SADM_Eth_v20_SPDU_SYS_PMMulticastTxMechatronics_Heartbeat`
- **PDU Group:** `Ethernet_1_itsSADM_Mechatronics_PwrMgrHeartbeatTx_OUT`
- **Tipo comunicazione:** Multicast TX
- **SecOC Data ID:** 81473
- **PDU Length:** 44 bytes

### 5. Mechatronics_PwrMgrWakeupRx_SystemSignal

**Path ARXML:** `/SVC_DEF_Hierarchy/SVC_SYS_Mechatronics_PwrMgrWakeupDefRx/Mechatronics_PwrMgrWakeupRx_SystemSignal`

**Caratteristiche:**
- **UUID:** `9e5544b1-eac9-da0e-e8d9-90873d4bfd76`
- **Dynamic Length:** `true`
- **Funzione:** Ricezione Wakeup Power Manager

**Mapping I-Signal:**
- **Nome:** `Mechatronics_PwrMgrWakeupRx_SADM_v20_ISignal`
- **Path:** `/Topologies/SDV/Ethernet_1/Mechatronics_PwrMgrWakeupRx_SADM_v20_ISignal`

**Mapping PDU:**
- **PDU:** `SADM_Eth_v20_PDU_SYS_PMMulticastRxMechatronics_Wakeup`
- **PDU Group:** `Ethernet_1_itsSADM_Mechatronics_PwrMgrWakeupRx_IN`
- **Tipo comunicazione:** Multicast RX

### 6. Mechatronics_PwrMgrWakeupTx_SystemSignal

**Path ARXML:** `/SVC_DEF_Hierarchy/SVC_SYS_Mechatronics_PwrMgrWakeupDef/Mechatronics_PwrMgrWakeupTx_SystemSignal`

**Caratteristiche:**
- **UUID:** `2dd9eed0-4ca2-c3ac-71b5-9d6a5af4cc9b`
- **Dynamic Length:** `true`
- **Funzione:** Trasmissione Wakeup Power Manager

**Mapping I-Signal:**
- **Nome:** `Mechatronics_PwrMgrWakeupTx_SADM_v20_ISignal`
- **Path:** `/Topologies/SDV/Ethernet_1/Mechatronics_PwrMgrWakeupTx_SADM_v20_ISignal`

**Mapping PDU:**
- **PDU:** `SADM_Eth_v20_PDU_SYS_PMUnicastTxMechatronics_Wakeup`
- **PDU Group:** `Ethernet_1_itsSADM_Mechatronics_PwrMgrWakeupTx_OUT`
- **Tipo comunicazione:** Unicast TX

## Configurazione di Sicurezza (SecOC)

Tutti i System Signal Mechatronics utilizzano **SecOC (Secure Onboard Communication)** per garantire:

- **Autenticazione:** Verifica dell'integrità e dell'origine dei messaggi
- **Freshness:** Protezione contro replay attacks
- **Authentication Build Attempts:** 2
- **Authentication Retries:** 1
- **Profile:** GMSecOCProfile1

### PDU Sicure (SPDU) Identificate:

1. `SADM_Eth_v20_SPDU_SYS_PMMulticastRxMechatronics_Heartbeat`
2. `SADM_Eth_v20_SPDU_SYS_PMMulticastTxMechatronics_Heartbeat`
3. `SADM_Eth_v20_SPDU_SYS_SMUnicastRxMechatronics_Info`
4. `SADM_Eth_v20_SPDU_SYS_SMUnicastTxMechatronics_Info`

## Configurazione SOME/IP

Tutti gli I-Signal utilizzano la configurazione SOME/IP:

- **Transformer:** `SPM_SOMEIP_Transformer`
- **Data Transformation:** `SPM_SOMEIP_Data_Transformation_PROFILE_GM_01`
- **Message Type:** NOTIFICATION
- **Session Handling:** SESSION-HANDLING-INACTIVE
- **CS Error Reaction:** AUTONOMOUS
- **Interface Version:** 1

## Data Types Associati

### Application Data Types:
- `Mechatronics_SwMgrInfoRxEventAppType`
- `Mechatronics_SwMgrInfoTxEventAppType`
- `Mechatronics_PwrMgrHeartbeatRxEventAppType`
- `Mechatronics_PwrMgrHeartbeatTxEventAppType`
- `Mechatronics_PwrMgrWakeupRxEventAppType`
- `Mechatronics_PwrMgrWakeupTxEventAppType`

### Implementation Data Types:
- `Mechatronics_SwMgrInfoRxEventImpType`
- `Mechatronics_SwMgrInfoTxEventImpType`
- `Mechatronics_PwrMgrHeartbeatRxEventImpType`
- `Mechatronics_PwrMgrHeartbeatTxEventImpType`
- `Mechatronics_PwrMgrWakeupRxEventImpType`
- `Mechatronics_PwrMgrWakeupTxEventImpType`

## Pattern di Comunicazione

### Classificazione per Funzione:

1. **Power Management (PM):**
   - Heartbeat: Comunicazione periodica per monitoraggio stato
   - Wakeup: Gestione risveglio sistema

2. **Software Management (SM):**
   - Info: Scambio informazioni di gestione software

### Classificazione per Modalità:

1. **Multicast:** 
   - Power Manager Heartbeat (RX/TX)
   - Power Manager Wakeup (RX)

2. **Unicast:**
   - Software Manager Info (RX/TX)
   - Power Manager Wakeup (TX)

## Topologia di Rete

- **Rete:** SDV (Software Defined Vehicle)
- **Cluster:** Ethernet_1
- **Versione:** v20
- **Protocollo fisico:** Ethernet
- **Protocollo applicativo:** SOME/IP

## Trigger e Scheduling

I System Signal sono associati ai seguenti PDU-TRIGGERING elementi per la gestione della trasmissione:

- Heartbeat: Trigger periodici per mantenimento stato
- Wakeup: Trigger su eventi di risveglio
- SwMgrInfo: Trigger su richiesta per gestione software

---

**Data analisi:** 14 luglio 2025  
**File ARXML:** `SADM_MergedEcuExtract.arxml`  
**Generato da:** Analisi automatica system signal Mechatronics
