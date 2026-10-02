# Confronto tra Flussi di Attivazione Locale e Remota - SPM

## Overview

Questo documento presenta un'analisi comparativa dettagliata tra i due principali flussi di attivazione nel sistema SPM (Software Profile Manager):
1. **Flusso di Attivazione Locale** - Richieste provenienti da moduli software locali
2. **Flusso di Attivazione Remota** - Richieste provenienti da messaggi di rete

## Tabella di Confronto Funzioni Principali

| Aspetto | Flusso Locale | Flusso Remoto |
|---------|---------------|---------------|
| **Punto di Ingresso** | `SPM_ActivateProfile()` (API) | `Spm_ReadIncoming_SM_Message()` (Network) |
| **Trigger** | Chiamata diretta da modulo esterno | Messaggio ricevuto dalla rete |
| **Validazione Iniziale** | Application Interface ID | IP Address + Software ID |
| **Tipo di Requestor** | Modulo software locale | Dispositivo remoto via rete |
| **Meccanismo di Input** | RTE API Call | Network Message Reception |

## Confronto dei Call Tree

### 1. Punto di Ingresso e Inizializzazione

#### **Flusso Locale:**
```
External Module
?
??? SPM_ActivateProfile()
    ?? Entry point API for local activation requests
    ?? Validates input parameters and handles local requests
```

#### **Flusso Remoto:**
```
Network Message Reception
?
??? Operating System Periodic Cycle
    ?
    ??? SPM_SoftMgr_MainFunction()
        ??? Spm_ReadIncoming_SM_Message()
            ?? Reads and processes incoming SM messages from network
            ?? Entry point for remote activation requests
```

**Differenze Chiave:**
- **Locale**: Chiamata diretta e sincrona dall'API
- **Remoto**: Processamento periodico asincrono dei messaggi di rete

### 2. Validazione e Preprocessing

#### **Flusso Locale:**
- Validazione immediata del `Application Interface ID`
- Verifica dei parametri di input dell'API
- Controllo diretto dell'autorizzazione locale

#### **Flusso Remoto:**
```
??? Rte_Receive_RP_SPM_RemSMInfo_SPM_dt()
?   ??? Spm_GetIndexTrgDvcFromIP()
??? Spm_GetPowProfIndexFromSwIDProfNumber()
??? Spm_prv_SM_RemoteActDeactRequestDispatch()
```

**Differenze Chiave:**
- **Locale**: Validazione più semplice e diretta
- **Remoto**: Richiede mapping IP?Device, SW ID?Profile, e controllo conflitti nel frame

### 3. Gestione dei Conflitti

#### **Flusso Locale:**
- Controllo conflitti con richieste locali in corso
- Utilizzo di variabili inter-runnable per tracking

#### **Flusso Remoto:**
```
Spm_prv_SM_RemoteActDeactRequestDispatch()
?? Checks for conflicting requests in the same frame
?? Ensures no activation/deactivation conflicts for same profile
```

**Differenze Chiave:**
- **Locale**: Conflitti tra richieste sequenziali nel tempo
- **Remoto**: Conflitti all'interno dello stesso frame di rete

### 4. Validazione dei Permessi

#### **Flusso Locale:**
- Validazione permessi locali diretta
- Controllo configurazione locale del profilo

#### **Flusso Remoto:**
```
Spm_Handler_prfReqDispatcherForActivationFromRemote()
??? Spm_Get_ActivationAvailabilityForRemoteRequest()
```

**Differenze Chiave:**
- **Locale**: Permessi basati su configurazione locale
- **Remoto**: Validazione specifica per richieste remote con controlli aggiuntivi

## Convergenza dei Flussi

### Punto di Unificazione

Entrambi i flussi convergono al livello del dispatcher comune:

```
Spm_prv_prfReqDispatcher()
?   ?? Handles profile dependency resolution
?   ?? Common logic for both local and remote requests
?
??? SPM_EnqueueRequest()
    ?   ?? Unified request queuing mechanism
    ?
    ??? [Common Processing Pipeline]
```

### Processamento Comune Post-Convergenza

Una volta che le richieste entrano nella coda comune, il processamento è identico:

1. **Queue Management**: `SPM_DequeueRequest()`
2. **Requester List Management**: `Spm_RequesterList_ActivationRequestHandler()`
3. **State Machine Coordination**: `Spm_FSMManger()`
4. **Service Activation**: `SrvcMgr_ActivateServices()`
5. **Network Setup**: `Spm_SwitchNetworkPdu()`

## Differenze nelle State Machine

### State Machine Locali vs Remote

#### **Flusso Locale:**
```
Spm_LocalPowerProfile_FSM()
?? Handles local power profile state machine logic
?? Manages specific activation logic for local power profiles
```

#### **Flusso Remoto:**
```
Spm_RemotePowerProfile_FSM()
?? Manages remote power profile states
?? Handles network communication and retries
```

**Differenze Principali:**
- **Locale**: Focus su attivazione diretta dei servizi
- **Remoto**: Gestione aggiuntiva di comunicazione di rete, retry, e feedback

## Meccanismi di Feedback

### Feedback Locale
- Valore di ritorno diretto dell'API
- Notifiche RTE per aggiornamenti di stato
- Meccanismi sincroni

### Feedback Remoto
```
Spm_CheckFeedbackReceivedByRequested()
Spm_ProvideInitialFeedbackToRemoteRequester()
```
- Messaggi di rete per conferma
- Meccanismi di retry periodici
- Gestione asincrona degli acknowledgment

## Funzioni Periodiche del Sistema Operativo

### Funzioni "MainFunction"

Entrambi i flussi utilizzano le stesse funzioni periodiche del sistema operativo:

1. **`SPM_SoftMgr_MainFunction()`**
   - Coordinatore principale delle attività SPM
   - Invocata periodicamente dal sistema operativo
   - Gestisce sia richieste locali che remote

2. **Ciclo di Processamento Periodico:**
   ```
   OS Scheduler
   ?
   ??? SPM_SoftMgr_MainFunction()
       ??? Spm_ReadIncoming_SM_Message() [Solo per remote]
       ??? Spm_DequeueReqActionHandler() [Comune]
       ??? Spm_FSMManger() [Comune]
       ??? Spm_SoftwMgr_PopulateSMFrame() [Solo per remote]
   ```

## Implicazioni Architetturali

### Vantaggi del Design Unificato

1. **Riusabilità del Codice**: La logica di processamento comune riduce la duplicazione
2. **Consistenza**: Comportamento uniforme indipendentemente dalla sorgente della richiesta  
3. **Manutenibilità**: Modifiche alla logica core impattano entrambi i flussi
4. **Testabilità**: Possibilità di testare la logica comune una sola volta

### Complessità Gestita

1. **Separazione delle Responsabilità**: Pre-processamento specifico per tipo di richiesta
2. **Astrazione**: Interfaccia comune nasconde i dettagli implementativi
3. **Scalabilità**: Facilità di aggiungere nuovi tipi di richieste

## Raccomandazioni per Sviluppatori

### Debugging
- **Flusso Locale**: Focus su validazione API e gestione coda
- **Flusso Remoto**: Attenzione a processamento messaggi e mappatura ID

### Ottimizzazioni
- **Comune**: Ottimizzare la logica di `Spm_prv_prfReqDispatcher()`
- **Locale**: Ridurre latenza dell'API call
- **Remoto**: Ottimizzare processamento messaggi di rete

### Estensibilità
- Nuove validazioni possono essere aggiunte nei rispettivi preprocessor
- La logica comune può essere estesa senza impatti sui punti di ingresso
- Meccanismi di feedback possono essere personalizzati per tipo di richiesta

## Conclusioni

Il design SPM presenta un'architettura elegante che:

1. **Separa le Preoccupazioni**: Gestione specifica per input locale vs remoto
2. **Unifica il Processamento**: Logica comune per elaborazione delle richieste
3. **Mantiene la Flessibilità**: Possibilità di personalizzare comportamenti specifici
4. **Garantisce Robustezza**: Gestione consistente degli errori e degli stati

Questa architettura permette un'evoluzione indipendente delle interfacce di input mantenendo una logica di business centralizzata e testabile.
