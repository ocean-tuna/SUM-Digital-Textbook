//! SPM Rust Implementation - Usage Example
//! This file demonstrates how to use the SPM system derived from the AsciiDoc design document

use std::net::Ipv4Addr;
use std::time::Duration;
use tokio::time;

// Import the SPM module
// In a real scenario, this would be: use spm_crate::*;
// For this example, we assume the module is available as a local file
mod spm_rust_implementation;
use spm_rust_implementation::*;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("=== SPM Rust Implementation Example ===");
    
    // 1. Create SPM System
    let local_ip = Ipv4Addr::new(192, 168, 1, 1);
    let mut spm_system = SpmSystem::new(local_ip);
    
    // 2. Setup profiles
    setup_profiles(&spm_system).await?;
    
    // 3. Setup target devices
    setup_target_devices(&spm_system).await;
    
    // 4. Start the SPM system
    spm_system.start().await?;
    
    // 5. Demonstrate API usage
    demonstrate_api(&spm_system).await?;
    
    // 6. Demonstrate state machine operations
    demonstrate_state_machine(&spm_system).await?;
    
    // 7. Demonstrate SM frame optimization
    demonstrate_sm_frame_optimization().await;
    
    // 8. Show system statistics
    show_system_stats(&spm_system).await;
    
    // 9. Demonstrate error handling
    demonstrate_error_handling(&spm_system).await;
    
    // Keep the system running for a while to observe periodic operations
    println!("\nSystem running for 3 seconds...");
    time::sleep(Duration::from_secs(3)).await;
    
    // 10. Stop the system
    spm_system.stop().await;
    
    Ok(())
}

/// Setup and register various power profiles
async fn setup_profiles(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- Setting up Power Profiles ---");
    
    // Profile 1: Local profile with local requests allowed
    let profile_id_1 = PowerProfileId::new(0x0001, 0x0001); // Service ID: 1, Instance ID: 1
    let mut profile1 = PowerProfile::new(profile_id_1, 1, false, true);
    
    // Add dependency
    let dependency_profile_id = PowerProfileId::new(0x0001, 0x0002);
    let dependency1 = ProfileDependency::new(
        dependency_profile_id,
        DependencyType::OnlyActivated,
        vec![]
    );
    profile1.add_dependency(dependency1);
    
    let app_intf_id_1 = profile1.application_intf_id.clone().unwrap();
    spm_system.software_manager.register_profile(profile1)?;
    println!("? Registered local profile: {} (App ID: {})", 
             profile_id_1, app_intf_id_1);
    
    // Profile 2: Remote profile
    let profile_id_2 = PowerProfileId::new(0x0002, 0x0001); // Service ID: 2, Instance ID: 1
    let profile2 = PowerProfile::new(profile_id_2, 1, true, false);
    
    spm_system.software_manager.register_profile(profile2)?;
    println!("? Registered remote profile: {}", profile_id_2);
    
    // Profile 3: Local profile with dependencies and message handling
    let profile_id_3 = PowerProfileId::new(0x0003, 0x0001); // Service ID: 3, Instance ID: 1
    let mut profile3 = PowerProfile::new(profile_id_3, 2, false, true);
    
    // Add dependency with message consumption
    let dependency3 = ProfileDependency::new(
        PowerProfileId::new(0x0002, 0x0001),
        DependencyType::ActivatedAndConsumed,
        vec!["HeartbeatMsg".to_string(), "StatusUpdateMsg".to_string()]
    );
    profile3.add_dependency(dependency3);
    
    let app_intf_id_3 = profile3.application_intf_id.clone().unwrap();
    smp_system.software_manager.register_profile(profile3)?;
    println!("? Registered local profile with dependencies: {} (App ID: {})", 
             profile_id_3, app_intf_id_3);
    
    // Profile 4: Test profile for demonstration
    let profile_id_test = PowerProfileId::new(0x00FF, 0x0001); // Test Service ID
    let test_profile = PowerProfile::new(profile_id_test, 99, false, true);
    
    let app_intf_id_test = test_profile.application_intf_id.clone().unwrap();
    spm_system.software_manager.register_profile(test_profile)?;
    println!("? Registered test profile: {} (App ID: {})", 
             profile_id_test, app_intf_id_test);
    
    Ok(())
}

/// Setup and register target devices
async fn setup_target_devices(spm_system: &SpmSystem) {
    println!("\n--- Setting up Target Devices ---");
    
    // Target Device 1
    let mut device1 = TargetDevice::new(Ipv4Addr::new(192, 168, 1, 2));
    device1.monitor_status_enabled = true;
    device1.set_device_status(DeviceStatus::Initialized);
    
    let device1_ip = device1.ip_address;
    spm_system.power_manager.register_target_device(device1);
    println!("? Registered target device: {} (INITIALIZED)", device1_ip);
    
    // Target Device 2
    let mut device2 = TargetDevice::new(Ipv4Addr::new(192, 168, 1, 3));
    device2.monitor_status_enabled = true;
    device2.set_device_status(DeviceStatus::On);
    
    let device2_ip = device2.ip_address;
    spm_system.power_manager.register_target_device(device2);
    println!("? Registered target device: {} (ON)", device2_ip);
}

/// Demonstrate the main SPM APIs
async fn demonstrate_api(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- Demonstrating API Usage ---");
    
    // Activate a local profile
    let app_intf_id_1 = "APP_INTF_00010001_01";
    println!("Activating profile with App Interface ID: {}", app_intf_id_1);
    spm_system.spm_activate_profile(app_intf_id_1).await?;
    println!("? Activation request submitted successfully");
    
    // Wait a bit and check status
    time::sleep(Duration::from_millis(200)).await;
    let status = spm_system.get_profile_status_string(0x00010001, 1)?;
    println!("Current profile status: {}", status);
    
    // Activate another profile
    let app_intf_id_3 = "APP_INTF_00030001_02";
    println!("Activating profile with App Interface ID: {}", app_intf_id_3);
    spm_system.spm_activate_profile(app_intf_id_3).await?;
    println!("? Activation request submitted successfully");
    
    // Wait and then deactivate
    time::sleep(Duration::from_millis(300)).await;
    println!("Deactivating profile with App Interface ID: {}", app_intf_id_1);
    spm_system.spm_deactivate_profile(app_intf_id_1).await?;
    println!("? Deactivation request submitted successfully");
    
    Ok(())
}

/// Demonstrate the state machine behavior
async fn demonstrate_state_machine(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- Demonstrating State Machine ---");
    
    let app_intf_id_test = "APP_INTF_00FF0001_63"; // Test profile (99 = 0x63)
    println!("Using test profile: {}", app_intf_id_test);
    
    // Demonstrate state transitions
    for cycle in 1..=2 {
        println!("\n--- Cycle {} ---", cycle);
        
        // INACTIVE -> ACTIVATING -> ACTIVATED
        println!("Requesting activation...");
        spm_system.spm_activate_profile(app_intf_id_test).await?;
        
        time::sleep(Duration::from_millis(150)).await;
        let status = spm_system.get_profile_status_string(0x00FF0001, 99)?;
        println!("Status after activation: {}", status);
        
        // ACTIVATED -> DEACTIVATING -> INACTIVE
        println!("Requesting deactivation...");
        spm_system.spm_deactivate_profile(app_intf_id_test).await?;
        
        time::sleep(Duration::from_millis(150)).await;
        let status = spm_system.get_profile_status_string(0x00FF0001, 99)?;
        println!("Status after deactivation: {}", status);
    }
    
    Ok(())
}

/// Demonstrate SM frame optimization
async fn demonstrate_sm_frame_optimization() {
    println!("\n--- Demonstrating SM Frame Optimization ---");
    
    // Create SM frame for optimization demonstration
    let src_ip = Ipv4Addr::new(192, 168, 1, 1);
    let dst_ip = Ipv4Addr::new(192, 168, 1, 2);
    
    let mut frame = SpmSmFrame::new(src_ip, dst_ip);
    
    // Add multiple requests for the same target
    println!("Adding multiple requests to single SM frame:");
    
    frame.add_request(0x00010001, 1, RequestCode::Activation);
    println!("? Added activation request for profile 00010001_01");
    
    frame.add_request(0x00020001, 1, RequestCode::Activation);
    println!("? Added activation request for profile 00020001_01");
    
    frame.add_request(0x00030001, 2, RequestCode::Deactivation);
    println!("? Added deactivation request for profile 00030001_02");
    
    // Add responses
    frame.add_response(0x00010001, 1, ResponseCode::OkActivation, ProfileStatus::Activated);
    println!("? Added response for profile 00010001_01");
    
    frame.add_response(0x00020001, 1, ResponseCode::GenericError, ProfileStatus::Inactive);
    println!("? Added error response for profile 00020001_01");
    
    println!("\nOptimized SM Frame Summary:");
    println!("  Source IP: {}", frame.src_ip);
    println!("  Destination IP: {}", frame.dst_ip);
    println!("  Number of Requests: {}", frame.req_entry_length);
    println!("  Number of Responses: {}", frame.res_entry_length);
    println!("  Total message size optimized for single transmission");
}

/// Show current system statistics
async fn show_system_stats(spm_system: &SpmSystem) {
    println!("\n--- System Statistics ---");
    
    let stats = spm_system.get_system_stats();
    
    for (key, value) in &stats {
        println!("  {}: {}", key, value);
    }
    
    // Show individual profile statuses
    println!("\n--- Profile Status Summary ---");
    let profiles = vec![
        (0x00010001, 1, "Local Service Profile 1"),
        (0x00020001, 1, "Remote Service Profile 1"),
        (0x00030001, 2, "Local Service Profile 3 with Dependencies"),
        (0x00FF0001, 99, "Test Profile"),
    ];
    
    for (id, number, name) in profiles {
        match spm_system.get_profile_status_string(id, number) {
            Ok(status) => println!("  {}: {}", name, status),
            Err(e) => println!("  {}: ERROR - {}", name, e),
        }
    }
    
    // Show request deck statistics
    let (deck_size, cards) = spm_system.software_manager.get_deck_stats();
    println!("\n--- Request Deck Summary ---");
    println!("  Deck size: {}", deck_size);
    for (i, card) in cards.iter().enumerate() {
        println!("  Card {}: {}", i + 1, card);
    }
    
    // Show device statistics
    println!("\n--- Target Device Summary ---");
    let device_stats = spm_system.power_manager.get_device_stats();
    for (ip, info) in device_stats {
        println!("  Device {}: {}", ip, info);
    }
}

/// Demonstrate error handling scenarios
async fn demonstrate_error_handling(spm_system: &SpmSystem) {
    println!("\n--- Demonstrating Error Handling ---");
    
    // Try to activate non-existent profile
    match spm_system.spm_activate_profile("NONEXISTENT_APP_ID").await {
        Err(e) => println!("? Expected error for non-existent profile: {}", e),
        Ok(_) => println!("? Unexpected success for non-existent profile"),
    }
    
    // Try to get status of non-existent profile
    match spm_system.get_profile_status_string(0xDEADBEEF, 255) {
        Err(e) => println!("? Expected error for non-existent profile status: {}", e),
        Ok(_) => println!("? Unexpected success for non-existent profile status"),
    }
    
    // Try to get non-existent target device
    match spm_system.power_manager.get_target_device(Ipv4Addr::new(10, 0, 0, 1)) {
        Err(e) => println!("? Expected error for non-existent target device: {}", e),
        Ok(_) => println!("? Unexpected success for non-existent target device"),
    }
}

/// Simulate processing an incoming SM message
async fn simulate_remote_message(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- Simulating Remote Message Processing ---");
    
    // Create incoming SM frame
    let remote_ip = Ipv4Addr::new(192, 168, 1, 2);
    let local_ip = Ipv4Addr::new(192, 168, 1, 1);
    
    let mut incoming_frame = SpmSmFrame::new(remote_ip, local_ip);
    
    // Add request from remote device
    incoming_frame.add_request(0x00010001, 1, RequestCode::Activation);
    
    // Add response from remote device
    incoming_frame.add_response(0x00020001, 1, ResponseCode::OkActivation, ProfileStatus::Activated);
    
    println!("Processing incoming frame from {}", remote_ip);
    println!("  Requests: {}", incoming_frame.requests.len());
    println!("  Responses: {}", incoming_frame.responses.len());
    
    // Process the incoming message
    spm_system.process_incoming_message(&incoming_frame).await?;
    
    Ok(())
}

/// Demonstrate system health monitoring
async fn monitor_system_health(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- System Health Monitoring ---");
    
    // Update device heartbeats
    let device_ip = Ipv4Addr::new(192, 168, 1, 2);
    spm_system.power_manager.update_device_heartbeat(device_ip, 12345)?;
    println!("? Updated device heartbeat");
    
    // Check device status
    let device = spm_system.power_manager.get_target_device(device_ip)?;
    println!("Device {} status: {} (Last HB: {})", 
            device.ip_address, 
            device.get_device_status(), 
            device.get_heartbeat_counter());
    
    Ok(())
}

/// Advanced usage demonstration
async fn demonstrate_advanced_features(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- Advanced Features Demonstration ---");
    
    // 1. Jolly operation demonstration
    println!("1. Demonstrating Jolly operation:");
    let profile_id = PowerProfileId::new(0x1234, 0x5678);
    
    // Add activation request
    let card1 = RequestCard::new(
        Ipv4Addr::new(192, 168, 1, 2),
        profile_id,
        1,
        true,
        RequestCode::Activation,
        Some(RequestRole::OutBound),
    );
    
    println!("   Adding activation request...");
    spm_system.software_manager.add_request_card(card1);
    let (size1, _) = spm_system.software_manager.get_deck_stats();
    println!("   Deck size: {}", size1);
    
    // Add deactivation request for same profile (Jolly operation)
    let card2 = RequestCard::new(
        Ipv4Addr::new(192, 168, 1, 2),
        profile_id,
        1,
        true,
        RequestCode::Deactivation,
        Some(RequestRole::OutBound),
    );
    
    println!("   Adding deactivation request (Jolly operation)...");
    spm_system.software_manager.add_request_card(card2);
    let (size2, cards) = spm_system.software_manager.get_deck_stats();
    println!("   Deck size after Jolly operation: {} (should be same)", size2);
    println!("   Last card: {}", cards.last().unwrap_or(&"None".to_string()));
    
    // 2. Dependency handling demonstration
    println!("\n2. Demonstrating dependency handling:");
    // This would involve more complex logic in a real implementation
    println!("   Dependencies are checked during profile activation");
    println!("   Message reception is activated/deactivated based on dependency type");
    
    Ok(())
}

/// Performance testing
async fn performance_test(spm_system: &SpmSystem) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n--- Performance Test ---");
    
    let start = std::time::Instant::now();
    
    // Add many request cards
    for i in 0..1000 {
        let profile_id = PowerProfileId::new(0x1000, i as u16);
        let card = RequestCard::new(
            Ipv4Addr::new(192, 168, 1, 2),
            profile_id,
            1,
            true,
            if i % 2 == 0 { RequestCode::Activation } else { RequestCode::Deactivation },
            Some(RequestRole::OutBound),
        );
        spm_system.software_manager.add_request_card(card);
    }
    
    let elapsed = start.elapsed();
    let (deck_size, _) = spm_system.software_manager.get_deck_stats();
    
    println!("Added 1000 request cards in {:?}", elapsed);
    println!("Final deck size: {} (Jolly operations reduced duplicates)", deck_size);
    
    Ok(())
}
