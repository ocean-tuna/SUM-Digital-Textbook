# SPM Rust Implementation

**Software Profile Manager (SPM) - Complete Rust Implementation**

Implementazione completa del sistema SPM (Software Profile Manager) in Rust, derivata dal documento di design AsciiDoc "Possible Object-Based SPM design.adoc".

## ?? Caratteristiche Principali

- **Gestione Profili di Potenza**: Sistema completo per gestire profili locali e remoti
- **Sistema di Carte di Richiesta**: Implementa il deck di carte con operazione "Jolly"
- **Comunicazione di Rete**: Frame SM ottimizzati per comunicazione point-to-point
- **State Machine**: Macchina a stati completa per profili locali
- **Thread Safety**: Implementazione thread-safe con Arc, RwLock e Mutex
- **Async/Await**: Sistema asincrono basato su Tokio
- **Gestione Errori**: Sistema robusto di gestione errori con thiserror

## ?? Struttura del Progetto

```
??? spm_rust_implementation.rs  # Implementazione principale
??? spm_rust_example.rs         # Esempi di utilizzo
??? Cargo.toml                  # Configurazione dipendenze
??? README.md                   # Questa documentazione
```

## ??? Architettura

### Componenti Principali

1. **Software Manager**: Gestisce profili di potenza e richieste di attivazione/disattivazione
2. **Power Manager**: Gestisce stati di potenza e transizioni di dispositivi locali/remoti
3. **SPM System**: Sistema principale che coordina tutti i componenti

### Enumerazioni Principali

```rust
pub enum ProfileStatus {
    Inactive = 0,
    Activating = 1,
    Activated = 2,
    Deactivating = 3,
}

pub enum RequestCode {
    Activation = 0,
    Deactivation = 1,
}

pub enum DeviceStatus {
    Off = 0,
    On = 1,
    Faulted = 2,
    Reboot = 3,
    Initialized = 4,
}
```

### Strutture Dati Principali

```rust
pub struct PowerProfile {
    pub profile_id: PowerProfileId,
    pub profile_number: u8,
    pub is_remote: bool,
    pub local_request_allowed: bool,
    pub application_intf_id: Option<String>,
    pub dependencies: Vec<ProfileDependency>,
    pub status: Arc<RwLock<ProfileStatus>>,
    // ...
}

pub struct RequestCard {
    pub target_ip: Ipv4Addr,
    pub profile_id: PowerProfileId,
    pub profile_number: u8,
    pub local_requester: bool,
    pub request_code: RequestCode,
    pub request_role: Option<RequestRole>,
    // ...
}
```

## ??? Installazione e Utilizzo

### Prerequisiti

- Rust 1.70+
- Cargo

### Dipendenze

```toml
[dependencies]
tokio = { version = "1.0", features = ["full"] }
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
thiserror = "1.0"
```

### Esempio di Utilizzo Base

```rust
use std::net::Ipv4Addr;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // 1. Crea il sistema SPM
    let local_ip = Ipv4Addr::new(192, 168, 1, 1);
    let mut spm_system = SpmSystem::new(local_ip);
    
    // 2. Registra un profilo di potenza
    let profile_id = PowerProfileId::new(0x0001, 0x0001);
    let profile = PowerProfile::new(profile_id, 1, false, true);
    spm_system.software_manager.register_profile(profile)?;
    
    // 3. Avvia il sistema
    spm_system.start().await?;
    
    // 4. Attiva un profilo
    spm_system.spm_activate_profile("APP_INTF_00010001_01").await?;
    
    // 5. Controlla lo stato
    let status = spm_system.get_profile_status_string(0x00010001, 1)?;
    println!("Profile status: {}", status);
    
    // 6. Ferma il sistema
    spm_system.stop().await;
    
    Ok(())
}
```

## ?? API Principali

### Attivazione/Disattivazione Profili

```rust
// Attiva un profilo locale
spm_system.spm_activate_profile("APP_INTF_ID").await?;

// Disattiva un profilo locale
spm_system.spm_deactivate_profile("APP_INTF_ID").await?;

// Ottieni lo stato di un profilo
let status = spm_system.get_profile_status_string(profile_id, profile_number)?;
```

### Gestione Dispositivi Target

```rust
// Registra un dispositivo target
let device = TargetDevice::new(Ipv4Addr::new(192, 168, 1, 2));
spm_system.power_manager.register_target_device(device);

// Aggiorna heartbeat
spm_system.power_manager.update_device_heartbeat(ip_address, counter)?;
```

### Frame SM per Comunicazione di Rete

```rust
// Crea frame SM
let mut frame = SpmSmFrame::new(src_ip, dst_ip);

// Aggiungi richieste
frame.add_request(profile_id, profile_number, RequestCode::Activation);

// Aggiungi risposte
frame.add_response(profile_id, profile_number, ResponseCode::OkActivation, ProfileStatus::Activated);

// Processa frame in arrivo
spm_system.process_incoming_message(&frame).await?;
```

## ?? Caratteristiche del Design

### 1. Sistema di Carte di Richiesta (Request Deck)

Il sistema implementa un deck di carte per gestire richieste multiple:

- **Operazione Jolly**: Le carte esistenti vengono aggiornate invece di aggiungerne di nuove
- **Priorità**: Le operazioni iniziano sempre dall'indice 0
- **Persistenza**: Le carte rimangono nel deck finché non sono soddisfatte

### 2. Macchina a Stati per Profili Locali

| Stato | Evento | Transizione | Operazione |
|-------|--------|-------------|------------|
| INACTIVE | Richiesta ACTIVATE | ACTIVATING | Invia risposta OK_ACTIVATION |
| ACTIVATING | SrvcMgr_ActivateService() OK | ACTIVATED | Attiva messaggi, rimuovi carta |
| ACTIVATED | Nessuna carta ACTIVATE | DEACTIVATING | Invia risposta OK_DEACTIVATION |
| DEACTIVATING | SrvcMgr_ActivateService() NOT_OK | INACTIVE | Disattiva messaggi, rimuovi carta |

### 3. Ottimizzazione Frame SM

Il sistema ottimizza la trasmissione dei frame SM:
- Un singolo messaggio per dispositivo target per ciclo
- Raggruppa tutte le richieste e risposte per lo stesso target
- Riduce il traffico di rete

### 4. Thread Safety

- **Arc<RwLock<T>>**: Per dati condivisi in lettura/scrittura
- **Arc<Mutex<T>>**: Per accesso esclusivo a strutture dati
- **Async/Await**: Operazioni non bloccanti

## ?? Testing ed Esempi

### Esegui l'Esempio

```bash
cargo run --example spm_basic_usage
```

### Test delle Funzionalità

L'esempio include dimostrazioni di:
- Setup di profili e dispositivi
- Utilizzo delle API
- Macchina a stati
- Ottimizzazione frame SM
- Gestione errori
- Monitoraggio del sistema

### Output Esempio

```
=== SPM Rust Implementation Example ===

--- Setting up Power Profiles ---
? Registered local profile: SvcID:0001_InstID:0001 (App ID: APP_INTF_00010001_01)
? Registered remote profile: SvcID:0002_InstID:0001

--- Demonstrating API Usage ---
Activating profile with App Interface ID: APP_INTF_00010001_01
Activation request added for profile APP_INTF_00010001_01
? Activation request submitted successfully
Current profile status: ACTIVATED
```

## ?? Monitoraggio e Debug

### Statistiche del Sistema

```rust
let stats = spm_system.get_system_stats();
println!("Profiles registered: {}", stats["profiles_registered"]);
println!("Request deck size: {}", stats["request_deck_size"]);
```

### Statistiche del Deck

```rust
let (deck_size, cards) = spm_system.software_manager.get_deck_stats();
println!("Deck contains {} cards", deck_size);
```

### Stato dei Dispositivi

```rust
let device_stats = spm_system.power_manager.get_device_stats();
for (ip, info) in device_stats {
    println!("Device {}: {}", ip, info);
}
```

## ?? Gestione Errori

Il sistema include tipi di errore specifici:

```rust
pub enum SpmError {
    ProfileNotFound(String),
    ProfileAlreadyExists(String),
    TargetDeviceNotFound(Ipv4Addr),
    LocalRequestNotAllowed(String),
    InvalidRequest(String),
    ServiceOperationFailed(String),
    NetworkError(String),
}
```

Esempio di gestione:

```rust
match spm_system.spm_activate_profile("INVALID_ID").await {
    Ok(_) => println!("Success"),
    Err(SpmError::ProfileNotFound(id)) => {
        eprintln!("Profile {} not found", id);
    }
    Err(e) => eprintln!("Other error: {}", e),
}
```

## ?? Performance

- **Async**: Operazioni non bloccanti con Tokio
- **Memory Efficient**: Uso di Arc per condivisione senza clonazione
- **Scalable**: Gestione di migliaia di profili e dispositivi
- **Low Latency**: Ciclo principale a 10ms

## ?? Mapping dal Design Document

| Elemento Design | Implementazione Rust |
|----------------|---------------------|
| PowerProfileID | `PowerProfileId` struct |
| Request Cards | `RequestCard` struct con deck `VecDeque` |
| SM Frame | `SpmSmFrame` struct |
| State Machine | Implementata in `process_local_profile_request` |
| Jolly Operation | Logica in `add_request_card` |
| Software Manager | `SoftwareManager` struct |
| Power Manager | `PowerManager` struct |

## ?? Contributi

1. Fork del repository
2. Crea feature branch (`git checkout -b feature/amazing-feature`)
3. Commit delle modifiche (`git commit -m 'Add amazing feature'`)
4. Push al branch (`git push origin feature/amazing-feature`)
5. Apri Pull Request

## ?? Licenza

Questo progetto è licenziato sotto MIT OR Apache-2.0 - vedi i file LICENSE per dettagli.

## ?? Supporto

Per domande o supporto:
- Crea un issue su GitHub
- Contatta il team SPM
- Consulta la documentazione API

---

**Implementazione derivata dal documento "Possible Object-Based SPM design.adoc"**
