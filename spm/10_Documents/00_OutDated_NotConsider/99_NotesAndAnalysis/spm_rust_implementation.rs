//! SPM (Software Profile Manager) - Rust Implementation
//! Based on "Possible Object-Based SPM design.adoc"
//! 
//! This module implements a complete SPM system with the following components:
//! - Power Profile management
//! - Target Device monitoring
//! - Request Card deck system
//! - Software Manager and Power Manager
//! - Network communication with SM frames

use std::collections::{HashMap, VecDeque};
use std::sync::{Arc, Mutex, RwLock};
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};
use std::net::Ipv4Addr;
use serde::{Deserialize, Serialize};
use tokio::sync::mpsc;
use tokio::time;
use thiserror::Error;

/// SPM-specific error types
#[derive(Error, Debug)]
pub enum SpmError {
    #[error("Profile not found: {0}")]
    ProfileNotFound(String),
    #[error("Profile already exists: {0}")]
    ProfileAlreadyExists(String),
    #[error("Target device not found: {0}")]
    TargetDeviceNotFound(Ipv4Addr),
    #[error("Local requests not allowed for profile: {0}")]
    LocalRequestNotAllowed(String),
    #[error("Invalid request: {0}")]
    InvalidRequest(String),
    #[error("Service operation failed: {0}")]
    ServiceOperationFailed(String),
    #[error("Network error: {0}")]
    NetworkError(String),
}

/// SPM operation result type
pub type SpmResult<T> = Result<T, SpmError>;

// ===== ENUMERATIONS =====

/// Profile Status enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum ProfileStatus {
    Inactive = 0,
    Activating = 1,
    Activated = 2,
    Deactivating = 3,
}

impl std::fmt::Display for ProfileStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ProfileStatus::Inactive => write!(f, "INACTIVE"),
            ProfileStatus::Activating => write!(f, "ACTIVATING"),
            ProfileStatus::Activated => write!(f, "ACTIVATED"),
            ProfileStatus::Deactivating => write!(f, "DEACTIVATING"),
        }
    }
}

/// Request Code enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum RequestCode {
    Activation = 0,
    Deactivation = 1,
}

impl std::fmt::Display for RequestCode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            RequestCode::Activation => write!(f, "ACTIVATION"),
            RequestCode::Deactivation => write!(f, "DEACTIVATION"),
        }
    }
}

/// Response Code enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum ResponseCode {
    OkActivation = 0,
    OkDeactivation = 1,
    GenericError = 2,
}

impl std::fmt::Display for ResponseCode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ResponseCode::OkActivation => write!(f, "OK_ACTIVATION"),
            ResponseCode::OkDeactivation => write!(f, "OK_DEACTIVATION"),
            ResponseCode::GenericError => write!(f, "GENERIC_ERROR"),
        }
    }
}

/// Dependency Type enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum DependencyType {
    OnlyActivated = 0,
    ActivatedAndConsumed = 1,
    OnlyConsumed = 2,
}

impl std::fmt::Display for DependencyType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            DependencyType::OnlyActivated => write!(f, "ONLY_ACTIVATED"),
            DependencyType::ActivatedAndConsumed => write!(f, "ACTIVATED_AND_CONSUMED"),
            DependencyType::OnlyConsumed => write!(f, "ONLY_CONSUMED"),
        }
    }
}

/// Device Status enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum DeviceStatus {
    Off = 0,
    On = 1,
    Faulted = 2,
    Reboot = 3,
    Initialized = 4,
}

impl std::fmt::Display for DeviceStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            DeviceStatus::Off => write!(f, "OFF"),
            DeviceStatus::On => write!(f, "ON"),
            DeviceStatus::Faulted => write!(f, "FAULTED"),
            DeviceStatus::Reboot => write!(f, "REBOOT"),
            DeviceStatus::Initialized => write!(f, "INITIALIZED"),
        }
    }
}

/// Power Command enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum PowerCommand {
    None = 0,
    On = 1,
    Off = 2,
}

impl std::fmt::Display for PowerCommand {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PowerCommand::None => write!(f, "NONE"),
            PowerCommand::On => write!(f, "ON"),
            PowerCommand::Off => write!(f, "OFF"),
        }
    }
}

/// Request Role enumeration
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[repr(u8)]
pub enum RequestRole {
    InBound = 0,
    OutBound = 1,
}

impl std::fmt::Display for RequestRole {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            RequestRole::InBound => write!(f, "IN_BOUND"),
            RequestRole::OutBound => write!(f, "OUT_BOUND"),
        }
    }
}

// ===== CORE DATA STRUCTURES =====

/// Power Profile ID structure
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub struct PowerProfileId {
    /// 4-byte profile ID: upper 2 bytes = Service ID, lower 2 bytes = Service Instance ID
    pub profile_id: u32,
}

impl PowerProfileId {
    /// Create a new PowerProfileId from Service ID and Service Instance ID
    pub fn new(service_id: u16, service_instance_id: u16) -> Self {
        let profile_id = ((service_id as u32) << 16) | (service_instance_id as u32);
        Self { profile_id }
    }

    /// Extract the Service ID (upper 2 bytes)
    pub fn service_id(&self) -> u16 {
        (self.profile_id >> 16) as u16
    }

    /// Extract the Service Instance ID (lower 2 bytes)
    pub fn service_instance_id(&self) -> u16 {
        (self.profile_id & 0xFFFF) as u16
    }
}

impl std::fmt::Display for PowerProfileId {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "SvcID:{:04X}_InstID:{:04X}", self.service_id(), self.service_instance_id())
    }
}

/// Profile Dependency structure
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ProfileDependency {
    pub dependency_profile_id: PowerProfileId,
    pub dependency_type: DependencyType,
    pub message_names: Vec<String>, // Only for non-ONLY_ACTIVATED dependencies
}

impl ProfileDependency {
    pub fn new(
        dependency_profile_id: PowerProfileId,
        dependency_type: DependencyType,
        message_names: Vec<String>,
    ) -> Self {
        Self {
            dependency_profile_id,
            dependency_type,
            message_names,
        }
    }
}

/// Power Profile structure
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PowerProfile {
    pub profile_id: PowerProfileId,
    pub profile_number: u8,
    pub is_remote: bool,
    pub local_request_allowed: bool,
    pub application_intf_id: Option<String>, // Only present if local_request_allowed is true
    pub dependencies: Vec<ProfileDependency>,
    pub status: Arc<RwLock<ProfileStatus>>,
    pub last_updated: Arc<RwLock<Instant>>,
}

impl PowerProfile {
    /// Create a new PowerProfile
    pub fn new(
        profile_id: PowerProfileId,
        profile_number: u8,
        is_remote: bool,
        local_request_allowed: bool,
    ) -> Self {
        let application_intf_id = if local_request_allowed {
            Some(format!("APP_INTF_{:08X}_{:02X}", profile_id.profile_id, profile_number))
        } else {
            None
        };

        Self {
            profile_id,
            profile_number,
            is_remote,
            local_request_allowed,
            application_intf_id,
            dependencies: Vec::new(),
            status: Arc::new(RwLock::new(ProfileStatus::Inactive)),
            last_updated: Arc::new(RwLock::new(Instant::now())),
        }
    }

    /// Get the current status (thread-safe)
    pub fn get_status(&self) -> ProfileStatus {
        *self.status.read().unwrap()
    }

    /// Set the status (thread-safe)
    pub fn set_status(&self, status: ProfileStatus) {
        *self.status.write().unwrap() = status;
        *self.last_updated.write().unwrap() = Instant::now();
    }

    /// Add a dependency to the profile
    pub fn add_dependency(&mut self, dependency: ProfileDependency) {
        self.dependencies.push(dependency);
    }

    /// Get unique key for this profile
    pub fn get_key(&self) -> String {
        format!("{:08X}_{:02X}", self.profile_id.profile_id, self.profile_number)
    }
}

/// Target Device structure
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TargetDevice {
    pub ip_address: Ipv4Addr,
    pub monitor_status_enabled: bool,
    pub device_status: Arc<RwLock<DeviceStatus>>,
    pub last_heartbeat_counter_rcv: Arc<RwLock<u32>>,
    pub power_command: Arc<RwLock<PowerCommand>>,
    pub power_command_satisfied: Arc<RwLock<bool>>,
    pub last_updated: Arc<RwLock<Instant>>,
}

impl TargetDevice {
    /// Create a new TargetDevice
    pub fn new(ip_address: Ipv4Addr) -> Self {
        Self {
            ip_address,
            monitor_status_enabled: false,
            device_status: Arc::new(RwLock::new(DeviceStatus::Off)),
            last_heartbeat_counter_rcv: Arc::new(RwLock::new(0)),
            power_command: Arc::new(RwLock::new(PowerCommand::None)),
            power_command_satisfied: Arc::new(RwLock::new(true)),
            last_updated: Arc::new(RwLock::new(Instant::now())),
        }
    }

    /// Get the current device status (thread-safe)
    pub fn get_device_status(&self) -> DeviceStatus {
        *self.device_status.read().unwrap()
    }

    /// Set the device status (thread-safe)
    pub fn set_device_status(&self, status: DeviceStatus) {
        *self.device_status.write().unwrap() = status;
        *self.last_updated.write().unwrap() = Instant::now();
    }

    /// Update heartbeat counter (thread-safe)
    pub fn update_heartbeat(&self, counter: u32) {
        *self.last_heartbeat_counter_rcv.write().unwrap() = counter;
        *self.last_updated.write().unwrap() = Instant::now();
    }

    /// Get heartbeat counter (thread-safe)
    pub fn get_heartbeat_counter(&self) -> u32 {
        *self.last_heartbeat_counter_rcv.read().unwrap()
    }
}

/// Request Card structure
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RequestCard {
    pub target_ip: Ipv4Addr,
    pub profile_id: PowerProfileId,
    pub profile_number: u8,
    pub local_requester: bool,
    pub request_code: RequestCode,
    pub request_role: Option<RequestRole>, // Empty for local requesters
    pub card_timestamp: u32,
    pub created_at: Instant,
}

impl RequestCard {
    /// Create a new RequestCard
    pub fn new(
        target_ip: Ipv4Addr,
        profile_id: PowerProfileId,
        profile_number: u8,
        local_requester: bool,
        request_code: RequestCode,
        request_role: Option<RequestRole>,
    ) -> Self {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs() as u32;

        Self {
            target_ip,
            profile_id,
            profile_number,
            local_requester,
            request_code,
            request_role,
            card_timestamp: timestamp,
            created_at: Instant::now(),
        }
    }

    /// Get unique key for this request card (for Jolly operation)
    pub fn get_unique_key(&self) -> String {
        format!("{}_{:08X}_{:02X}", 
                self.target_ip, 
                self.profile_id.profile_id, 
                self.profile_number)
    }
}

// ===== NETWORK COMMUNICATION STRUCTURES =====

/// SPM Profile Request Item for SM frame
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SpmProfileRequestItem {
    pub profile_id: u32,
    pub profile_number: u8,
    pub request_code: u8, // 0 = ACTIVATION, 1 = DEACTIVATION
}

impl SpmProfileRequestItem {
    pub fn new(profile_id: u32, profile_number: u8, request_code: RequestCode) -> Self {
        Self {
            profile_id,
            profile_number,
            request_code: request_code as u8,
        }
    }
}

/// SPM Profile Response Item for SM frame
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SpmProfileResponseItem {
    pub profile_id: u32,
    pub profile_number: u8,
    pub response_code: u8,  // 0 = OK_ACTIVATION, 1 = OK_DEACTIVATION, 2 = GENERIC_ERROR
    pub profile_status: u8, // 0 = INACTIVE, 1 = ACTIVATING, 2 = ACTIVATED, 3 = DEACTIVATING
}

impl SpmProfileResponseItem {
    pub fn new(
        profile_id: u32,
        profile_number: u8,
        response_code: ResponseCode,
        profile_status: ProfileStatus,
    ) -> Self {
        Self {
            profile_id,
            profile_number,
            response_code: response_code as u8,
            profile_status: profile_status as u8,
        }
    }
}

/// SPM SM Frame structure for network communication
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SpmSmFrame {
    pub src_ip: Ipv4Addr,
    pub dst_ip: Ipv4Addr,
    pub req_msg_type: u8,      // fixed to 0
    pub req_entry_length: u16, // number of request items
    pub requests: Vec<SpmProfileRequestItem>,
    pub res_msg_type: u8,      // fixed to 1
    pub res_entry_length: u16, // number of response items
    pub responses: Vec<SpmProfileResponseItem>,
}

impl SpmSmFrame {
    /// Create a new SM frame
    pub fn new(src_ip: Ipv4Addr, dst_ip: Ipv4Addr) -> Self {
        Self {
            src_ip,
            dst_ip,
            req_msg_type: 0, // fixed value
            req_entry_length: 0,
            requests: Vec::new(),
            res_msg_type: 1, // fixed value
            res_entry_length: 0,
            responses: Vec::new(),
        }
    }

    /// Add a request item to the SM frame
    pub fn add_request(&mut self, profile_id: u32, profile_number: u8, request_code: RequestCode) {
        let request = SpmProfileRequestItem::new(profile_id, profile_number, request_code);
        self.requests.push(request);
        self.req_entry_length = self.requests.len() as u16;
    }

    /// Add a response item to the SM frame
    pub fn add_response(
        &mut self,
        profile_id: u32,
        profile_number: u8,
        response_code: ResponseCode,
        profile_status: ProfileStatus,
    ) {
        let response = SpmProfileResponseItem::new(profile_id, profile_number, response_code, profile_status);
        self.responses.push(response);
        self.res_entry_length = self.responses.len() as u16;
    }
}

// ===== MAIN SPM COMPONENTS =====

/// Software Manager - handles power profiles and activation/deactivation requests
#[derive(Debug)]
pub struct SoftwareManager {
    profiles: Arc<RwLock<HashMap<String, Arc<PowerProfile>>>>,
    request_deck: Arc<Mutex<VecDeque<RequestCard>>>,
    main_func_period: Duration,
    retry_period: Duration,
}

impl SoftwareManager {
    /// Create a new SoftwareManager
    pub fn new() -> Self {
        Self {
            profiles: Arc::new(RwLock::new(HashMap::new())),
            request_deck: Arc::new(Mutex::new(VecDeque::new())),
            main_func_period: Duration::from_millis(10), // 10ms default cycle
            retry_period: Duration::from_secs(1),        // 1s retry period
        }
    }

    /// Register a new power profile
    pub fn register_profile(&self, profile: PowerProfile) -> SpmResult<()> {
        let key = profile.get_key();
        let mut profiles = self.profiles.write().unwrap();
        
        if profiles.contains_key(&key) {
            return Err(SpmError::ProfileAlreadyExists(key));
        }
        
        profiles.insert(key, Arc::new(profile));
        Ok(())
    }

    /// Get a power profile by ID and number
    pub fn get_profile(&self, profile_id: u32, profile_number: u8) -> SpmResult<Arc<PowerProfile>> {
        let key = format!("{:08X}_{:02X}", profile_id, profile_number);
        let profiles = self.profiles.read().unwrap();
        
        profiles.get(&key)
            .cloned()
            .ok_or_else(|| SpmError::ProfileNotFound(key))
    }

    /// Get a profile by application interface ID
    pub fn get_profile_by_app_intf_id(&self, app_intf_id: &str) -> SpmResult<Arc<PowerProfile>> {
        let profiles = self.profiles.read().unwrap();
        
        for profile in profiles.values() {
            if let Some(ref profile_app_id) = profile.application_intf_id {
                if profile_app_id == app_intf_id {
                    return Ok(profile.clone());
                }
            }
        }
        
        Err(SpmError::ProfileNotFound(app_intf_id.to_string()))
    }

    /// Add a request card to the deck (with Jolly operation support)
    pub fn add_request_card(&self, card: RequestCard) {
        let mut deck = self.request_deck.lock().unwrap();
        let card_key = card.get_unique_key();
        
        // Check for existing card with same key (Jolly operation)
        if let Some(pos) = deck.iter().position(|c| c.get_unique_key() == card_key) {
            // Update existing card instead of adding new one
            deck[pos] = card;
        } else {
            // Add new card
            deck.push_back(card);
        }
    }

    /// Remove a request card from the deck
    pub fn remove_request_card(&self, card_key: &str) -> bool {
        let mut deck = self.request_deck.lock().unwrap();
        
        if let Some(pos) = deck.iter().position(|c| c.get_unique_key() == card_key) {
            deck.remove(pos);
            true
        } else {
            false
        }
    }

    /// Process all cards in the request deck
    pub async fn process_request_deck(&self) -> SpmResult<()> {
        let deck_copy = {
            let deck = self.request_deck.lock().unwrap();
            deck.iter().cloned().collect::<Vec<_>>()
        };
        
        for card in deck_copy {
            self.process_request_card(&card).await?;
        }
        
        Ok(())
    }

    /// Process a single request card
    async fn process_request_card(&self, card: &RequestCard) -> SpmResult<()> {
        let profile = self.get_profile(card.profile_id.profile_id, card.profile_number)?;
        
        if profile.is_remote {
            self.process_remote_profile_request(card, &profile).await
        } else {
            self.process_local_profile_request(card, &profile).await
        }
    }

    /// Process a request for a local profile
    async fn process_local_profile_request(&self, card: &RequestCard, profile: &PowerProfile) -> SpmResult<()> {
        let current_status = profile.get_status();
        
        match card.request_code {
            RequestCode::Activation => {
                match current_status {
                    ProfileStatus::Inactive => {
                        profile.set_status(ProfileStatus::Activating);
                        println!("Profile {}_{:02X} transitioning to ACTIVATING", 
                                profile.profile_id, profile.profile_number);
                        
                        // Simulate service activation
                        self.simulate_service_activation(profile).await?;
                    }
                    ProfileStatus::Deactivating => {
                        profile.set_status(ProfileStatus::Activating);
                        println!("Profile {}_{:02X} transitioning from DEACTIVATING to ACTIVATING", 
                                profile.profile_id, profile.profile_number);
                        
                        // Simulate service activation
                        self.simulate_service_activation(profile).await?;
                    }
                    _ => {
                        // Already activating or activated
                    }
                }
            }
            RequestCode::Deactivation => {
                match current_status {
                    ProfileStatus::Activated | ProfileStatus::Activating => {
                        profile.set_status(ProfileStatus::Deactivating);
                        println!("Profile {}_{:02X} transitioning to DEACTIVATING", 
                                profile.profile_id, profile.profile_number);
                        
                        // Simulate service deactivation
                        self.simulate_service_deactivation(profile).await?;
                    }
                    _ => {
                        // Already deactivating or inactive
                    }
                }
            }
        }
        
        Ok(())
    }

    /// Process a request for a remote profile
    async fn process_remote_profile_request(&self, card: &RequestCard, profile: &PowerProfile) -> SpmResult<()> {
        println!("Processing remote profile request for {}_{:02X} on target {}", 
                profile.profile_id, profile.profile_number, card.target_ip);
        
        // This would involve network communication to send SM frame
        // Implementation depends on specific network stack
        
        Ok(())
    }

    /// Simulate service activation (replaces SrvcMgr_ActivateService)
    async fn simulate_service_activation(&self, profile: &PowerProfile) -> SpmResult<()> {
        // Simulate processing time
        tokio::time::sleep(Duration::from_millis(100)).await;
        
        // Assume success for simulation
        if profile.get_status() == ProfileStatus::Activating {
            profile.set_status(ProfileStatus::Activated);
            println!("Profile {}_{:02X} activated successfully", 
                    profile.profile_id, profile.profile_number);
            
            // Remove the card from deck
            let card_key = format!("{}_{:08X}_{:02X}", "127.0.0.1", profile.profile_id.profile_id, profile.profile_number);
            self.remove_request_card(&card_key);
        }
        
        Ok(())
    }

    /// Simulate service deactivation
    async fn simulate_service_deactivation(&self, profile: &PowerProfile) -> SpmResult<()> {
        // Simulate processing time
        tokio::time::sleep(Duration::from_millis(100)).await;
        
        // Assume success for simulation
        if profile.get_status() == ProfileStatus::Deactivating {
            profile.set_status(ProfileStatus::Inactive);
            println!("Profile {}_{:02X} deactivated successfully", 
                    profile.profile_id, profile.profile_number);
            
            // Remove the card from deck
            let card_key = format!("{}_{:08X}_{:02X}", "127.0.0.1", profile.profile_id.profile_id, profile.profile_number);
            self.remove_request_card(&card_key);
        }
        
        Ok(())
    }

    /// Get statistics about the request deck
    pub fn get_deck_stats(&self) -> (usize, Vec<String>) {
        let deck = self.request_deck.lock().unwrap();
        let size = deck.len();
        let cards: Vec<String> = deck.iter()
            .map(|card| format!("{}->{}_{:02X}:{}", 
                               card.target_ip, 
                               card.profile_id, 
                               card.profile_number, 
                               card.request_code))
            .collect();
        (size, cards)
    }
}

/// Power Manager - manages power states and transitions of local and remote devices
#[derive(Debug)]
pub struct PowerManager {
    target_devices: Arc<RwLock<HashMap<Ipv4Addr, Arc<TargetDevice>>>>,
    local_ip: Ipv4Addr,
}

impl PowerManager {
    /// Create a new PowerManager
    pub fn new(local_ip: Ipv4Addr) -> Self {
        Self {
            target_devices: Arc::new(RwLock::new(HashMap::new())),
            local_ip,
        }
    }

    /// Register a new target device
    pub fn register_target_device(&self, device: TargetDevice) {
        let mut devices = self.target_devices.write().unwrap();
        devices.insert(device.ip_address, Arc::new(device));
    }

    /// Get a target device by IP address
    pub fn get_target_device(&self, ip_address: Ipv4Addr) -> SpmResult<Arc<TargetDevice>> {
        let devices = self.target_devices.read().unwrap();
        devices.get(&ip_address)
            .cloned()
            .ok_or_else(|| SpmError::TargetDeviceNotFound(ip_address))
    }

    /// Update device heartbeat
    pub fn update_device_heartbeat(&self, ip_address: Ipv4Addr, counter: u32) -> SpmResult<()> {
        let device = self.get_target_device(ip_address)?;
        device.update_heartbeat(counter);
        Ok(())
    }

    /// Monitor all devices
    pub async fn monitor_devices(&self) {
        let devices = {
            let devices_lock = self.target_devices.read().unwrap();
            devices_lock.values().cloned().collect::<Vec<_>>()
        };
        
        for device in devices {
            if device.monitor_status_enabled {
                println!("Monitoring device {} - Status: {}", 
                        device.ip_address, device.get_device_status());
            }
        }
    }

    /// Get statistics about target devices
    pub fn get_device_stats(&self) -> HashMap<String, String> {
        let devices = self.target_devices.read().unwrap();
        let mut stats = HashMap::new();
        
        for (ip, device) in devices.iter() {
            stats.insert(
                ip.to_string(),
                format!("Status: {}, HB: {}", 
                       device.get_device_status(), 
                       device.get_heartbeat_counter())
            );
        }
        
        stats
    }
}

/// Main SPM System
#[derive(Debug)]
pub struct SpmSystem {
    pub software_manager: Arc<SoftwareManager>,
    pub power_manager: Arc<PowerManager>,
    pub local_ip: Ipv4Addr,
    is_running: Arc<RwLock<bool>>,
    stop_sender: Option<mpsc::Sender<()>>,
}

impl SpmSystem {
    /// Create a new SPM system
    pub fn new(local_ip: Ipv4Addr) -> Self {
        Self {
            software_manager: Arc::new(SoftwareManager::new()),
            power_manager: Arc::new(PowerManager::new(local_ip)),
            local_ip,
            is_running: Arc::new(RwLock::new(false)),
            stop_sender: None,
        }
    }

    /// Start the SPM system
    pub async fn start(&mut self) -> SpmResult<()> {
        let mut is_running = self.is_running.write().unwrap();
        if *is_running {
            return Ok(()); // Already running
        }
        *is_running = true;
        drop(is_running);

        let (stop_tx, mut stop_rx) = mpsc::channel(1);
        self.stop_sender = Some(stop_tx);

        let software_manager = self.software_manager.clone();
        let power_manager = self.power_manager.clone();
        let is_running_clone = self.is_running.clone();

        // Start main function task
        tokio::spawn(async move {
            let mut interval = time::interval(Duration::from_millis(10)); // 10ms cycle
            
            loop {
                tokio::select! {
                    _ = interval.tick() => {
                        // Process request deck
                        if let Err(e) = software_manager.process_request_deck().await {
                            eprintln!("Error processing request deck: {}", e);
                        }
                        
                        // Monitor devices
                        power_manager.monitor_devices().await;
                    }
                    _ = stop_rx.recv() => {
                        println!("SPM System main function stopped");
                        break;
                    }
                }
            }
            
            *is_running_clone.write().unwrap() = false;
        });

        println!("SPM System started with local IP: {}", self.local_ip);
        Ok(())
    }

    /// Stop the SPM system
    pub async fn stop(&mut self) {
        if let Some(sender) = self.stop_sender.take() {
            let _ = sender.send(()).await;
        }
        
        // Wait for system to stop
        while *self.is_running.read().unwrap() {
            tokio::time::sleep(Duration::from_millis(10)).await;
        }
        
        println!("SPM System stopped");
    }

    /// Check if system is running
    pub fn is_running(&self) -> bool {
        *self.is_running.read().unwrap()
    }

    // ===== API FUNCTIONS =====

    /// SPM_ActivateProfile - Main API for local profile activation
    pub async fn spm_activate_profile(&self, application_intf_id: &str) -> SpmResult<()> {
        let profile = self.software_manager.get_profile_by_app_intf_id(application_intf_id)?;
        
        if !profile.local_request_allowed {
            return Err(SpmError::LocalRequestNotAllowed(application_intf_id.to_string()));
        }
        
        let target_ip = if profile.is_remote {
            // In a real implementation, this would be determined from configuration
            Ipv4Addr::new(192, 168, 1, 2) // Example remote IP
        } else {
            self.local_ip
        };
        
        let request_role = if profile.is_remote {
            Some(RequestRole::OutBound)
        } else {
            None
        };
        
        let card = RequestCard::new(
            target_ip,
            profile.profile_id,
            profile.profile_number,
            true, // local requester
            RequestCode::Activation,
            request_role,
        );
        
        self.software_manager.add_request_card(card);
        
        println!("Activation request added for profile {}", application_intf_id);
        Ok(())
    }

    /// SPM_DeactivateProfile - Main API for local profile deactivation
    pub async fn spm_deactivate_profile(&self, application_intf_id: &str) -> SpmResult<()> {
        let profile = self.software_manager.get_profile_by_app_intf_id(application_intf_id)?;
        
        if !profile.local_request_allowed {
            return Err(SpmError::LocalRequestNotAllowed(application_intf_id.to_string()));
        }
        
        let target_ip = if profile.is_remote {
            // In a real implementation, this would be determined from configuration
            Ipv4Addr::new(192, 168, 1, 2) // Example remote IP
        } else {
            self.local_ip
        };
        
        let request_role = if profile.is_remote {
            Some(RequestRole::OutBound)
        } else {
            None
        };
        
        let card = RequestCard::new(
            target_ip,
            profile.profile_id,
            profile.profile_number,
            true, // local requester
            RequestCode::Deactivation,
            request_role,
        );
        
        self.software_manager.add_request_card(card);
        
        println!("Deactivation request added for profile {}", application_intf_id);
        Ok(())
    }

    /// Process incoming SM message from remote devices
    pub async fn process_incoming_message(&self, frame: &SpmSmFrame) -> SpmResult<()> {
        println!("Processing incoming SM frame from {}", frame.src_ip);
        
        // Process incoming requests
        for request in &frame.requests {
            let profile_id = PowerProfileId { profile_id: request.profile_id };
            let request_code = if request.request_code == 0 {
                RequestCode::Activation
            } else {
                RequestCode::Deactivation
            };
            
            // Add to request deck as inbound request
            let card = RequestCard::new(
                frame.src_ip,
                profile_id,
                request.profile_number,
                false, // not local requester
                request_code,
                Some(RequestRole::InBound),
            );
            
            self.software_manager.add_request_card(card);
        }
        
        // Process incoming responses
        for response in &frame.responses {
            println!("Received response: Profile {:08X}_{:02X} Status: {} Response: {}", 
                    response.profile_id, 
                    response.profile_number,
                    match response.profile_status {
                        0 => "INACTIVE",
                        1 => "ACTIVATING", 
                        2 => "ACTIVATED",
                        3 => "DEACTIVATING",
                        _ => "UNKNOWN",
                    },
                    match response.response_code {
                        0 => "OK_ACTIVATION",
                        1 => "OK_DEACTIVATION", 
                        2 => "GENERIC_ERROR",
                        _ => "UNKNOWN",
                    });
        }
        
        Ok(())
    }

    /// Get profile status as string
    pub fn get_profile_status_string(&self, profile_id: u32, profile_number: u8) -> SpmResult<String> {
        let profile = self.software_manager.get_profile(profile_id, profile_number)?;
        Ok(profile.get_status().to_string())
    }

    /// Get system statistics
    pub fn get_system_stats(&self) -> HashMap<String, serde_json::Value> {
        let profiles_count = self.software_manager.profiles.read().unwrap().len();
        let (deck_size, _) = self.software_manager.get_deck_stats();
        let devices_count = self.power_manager.target_devices.read().unwrap().len();
        
        let mut stats = HashMap::new();
        stats.insert("profiles_registered".to_string(), serde_json::Value::Number(profiles_count.into()));
        stats.insert("request_deck_size".to_string(), serde_json::Value::Number(deck_size.into()));
        stats.insert("target_devices".to_string(), serde_json::Value::Number(devices_count.into()));
        stats.insert("system_running".to_string(), serde_json::Value::Bool(self.is_running()));
        stats.insert("local_ip".to_string(), serde_json::Value::String(self.local_ip.to_string()));
        
        stats
    }
}
