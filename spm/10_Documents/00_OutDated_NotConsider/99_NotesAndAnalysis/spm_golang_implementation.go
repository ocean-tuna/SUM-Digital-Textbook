// Package spm implements the Software Profile Manager (SPM) system
// based on the object-oriented design specification.
// Complete implementation derived from "Possible Object-Based SPM design.md"
package spm

import (
	"fmt"
	"sync"
	"time"
)

// ===== ENUMERATIONS =====

// ProfileStatus represents the current state of a power profile
type ProfileStatus uint8

const (
	ProfileStatusInactive     ProfileStatus = 0
	ProfileStatusActivating   ProfileStatus = 1
	ProfileStatusActivated    ProfileStatus = 2
	ProfileStatusDeactivating ProfileStatus = 3
)

func (ps ProfileStatus) String() string {
	switch ps {
	case ProfileStatusInactive:
		return "INACTIVE"
	case ProfileStatusActivating:
		return "ACTIVATING"
	case ProfileStatusActivated:
		return "ACTIVATED"
	case ProfileStatusDeactivating:
		return "DEACTIVATING"
	default:
		return "UNKNOWN"
	}
}

// RequestCode represents the type of operation requested
type RequestCode uint8

const (
	RequestCodeActivation   RequestCode = 0
	RequestCodeDeactivation RequestCode = 1
)

func (rc RequestCode) String() string {
	switch rc {
	case RequestCodeActivation:
		return "ACTIVATION"
	case RequestCodeDeactivation:
		return "DEACTIVATION"
	default:
		return "UNKNOWN"
	}
}

// ResponseCode represents the response status for a request
type ResponseCode uint8

const (
	ResponseCodeOKActivation   ResponseCode = 0
	ResponseCodeOKDeactivation ResponseCode = 1
	ResponseCodeGenericError   ResponseCode = 2
)

func (rc ResponseCode) String() string {
	switch rc {
	case ResponseCodeOKActivation:
		return "OK_ACTIVATION"
	case ResponseCodeOKDeactivation:
		return "OK_DEACTIVATION"
	case ResponseCodeGenericError:
		return "GENERIC_ERROR"
	default:
		return "UNKNOWN"
	}
}

// DependencyType represents the type of profile dependency
type DependencyType uint8

const (
	DependencyTypeOnlyActivated        DependencyType = 0
	DependencyTypeActivatedAndConsumed DependencyType = 1
	DependencyTypeOnlyConsumed         DependencyType = 2
)

func (dt DependencyType) String() string {
	switch dt {
	case DependencyTypeOnlyActivated:
		return "ONLY_ACTIVATED"
	case DependencyTypeActivatedAndConsumed:
		return "ACTIVATED_AND_CONSUMED"
	case DependencyTypeOnlyConsumed:
		return "ONLY_CONSUMED"
	default:
		return "UNKNOWN"
	}
}

// DeviceStatus represents the power state of a target device
type DeviceStatus uint8

const (
	DeviceStatusOff         DeviceStatus = 0
	DeviceStatusOn          DeviceStatus = 1
	DeviceStatusFaulted     DeviceStatus = 2
	DeviceStatusReboot      DeviceStatus = 3
	DeviceStatusInitialized DeviceStatus = 4
)

func (ds DeviceStatus) String() string {
	switch ds {
	case DeviceStatusOff:
		return "OFF"
	case DeviceStatusOn:
		return "ON"
	case DeviceStatusFaulted:
		return "FAULTED"
	case DeviceStatusReboot:
		return "REBOOT"
	case DeviceStatusInitialized:
		return "INITIALIZED"
	default:
		return "UNKNOWN"
	}
}

// PowerCommand represents power commands for target devices
type PowerCommand uint8

const (
	PowerCommandNone PowerCommand = 0
	PowerCommandOn   PowerCommand = 1
	PowerCommandOff  PowerCommand = 2
)

func (pc PowerCommand) String() string {
	switch pc {
	case PowerCommandNone:
		return "NONE"
	case PowerCommandOn:
		return "ON"
	case PowerCommandOff:
		return "OFF"
	default:
		return "UNKNOWN"
	}
}

// RequestRole represents the direction of a request
type RequestRole uint8

const (
	RequestRoleInbound  RequestRole = 0
	RequestRoleOutbound RequestRole = 1
)

func (rr RequestRole) String() string {
	switch rr {
	case RequestRoleInbound:
		return "IN_BOUND"
	case RequestRoleOutbound:
		return "OUT_BOUND"
	default:
		return "UNKNOWN"
	}
}

// ===== CORE DATA STRUCTURES =====

// PowerProfileID represents the unique identifier for a power profile
type PowerProfileID struct {
	ProfileID uint32 // 4 bytes: 2 bytes Service ID + 2 bytes Service Instance ID
}

// GetServiceID extracts the Service ID from the profile ID (upper 2 bytes)
func (ppid PowerProfileID) GetServiceID() uint16 {
	return uint16(ppid.ProfileID >> 16)
}

// GetServiceInstanceID extracts the Service Instance ID from the profile ID (lower 2 bytes)
func (ppid PowerProfileID) GetServiceInstanceID() uint16 {
	return uint16(ppid.ProfileID & 0xFFFF)
}

// ProfileDependency represents a dependency relationship between profiles
type ProfileDependency struct {
	DependencyProfileID PowerProfileID `json:"dependency_profile_id"`
	DependencyType      DependencyType `json:"dependency_type"`
	MessageNames        []string       `json:"message_names,omitempty"` // Only for non-ONLY_ACTIVATED dependencies
}

// PowerProfile represents a power profile in the SPM system
type PowerProfile struct {
	ProfileID           PowerProfileID      `json:"profile_id"`
	ProfileNumber       uint8               `json:"profile_number"`
	IsRemote            bool                `json:"is_remote"`
	LocalRequestAllowed bool                `json:"local_request_allowed"`
	ApplicationIntfID   *string             `json:"application_intf_id,omitempty"` // Only present if LocalRequestAllowed is true
	Dependencies        []ProfileDependency `json:"dependencies"`
	Status              ProfileStatus       `json:"status"`
	mutex               sync.RWMutex        // For thread-safe operations
}

// NewPowerProfile creates a new PowerProfile instance
func NewPowerProfile(profileID uint32, profileNumber uint8, isRemote, localRequestAllowed bool) *PowerProfile {
	pp := &PowerProfile{
		ProfileID:           PowerProfileID{ProfileID: profileID},
		ProfileNumber:       profileNumber,
		IsRemote:            isRemote,
		LocalRequestAllowed: localRequestAllowed,
		Dependencies:        make([]ProfileDependency, 0),
		Status:              ProfileStatusInactive,
	}

	if localRequestAllowed {
		appIntfID := fmt.Sprintf("APP_INTF_%08X_%02X", profileID, profileNumber)
		pp.ApplicationIntfID = &appIntfID
	}

	return pp
}

// GetStatus returns the current status of the profile (thread-safe)
func (pp *PowerProfile) GetStatus() ProfileStatus {
	pp.mutex.RLock()
	defer pp.mutex.RUnlock()
	return pp.Status
}

// SetStatus sets the status of the profile (thread-safe)
func (pp *PowerProfile) SetStatus(status ProfileStatus) {
	pp.mutex.Lock()
	defer pp.mutex.Unlock()
	pp.Status = status
}

// AddDependency adds a dependency to the profile
func (pp *PowerProfile) AddDependency(dependency ProfileDependency) {
	pp.mutex.Lock()
	defer pp.mutex.Unlock()
	pp.Dependencies = append(pp.Dependencies, dependency)
}

// TargetDevice represents a remote target device managed by the Power Manager
type TargetDevice struct {
	IPAddress               uint32       `json:"ip_address"`
	MonitorStatusEnabled    bool         `json:"monitor_status_enabled"`
	DeviceStatus            DeviceStatus `json:"device_status"`
	LastHeartbeatCounterRcv uint32       `json:"last_heartbeat_counter_rcv"`
	PowerCommand            PowerCommand `json:"power_command"`
	PowerCommandSatisfied   bool         `json:"power_command_satisfied"`
	LastUpdated             time.Time    `json:"last_updated"`
	mutex                   sync.RWMutex // For thread-safe operations
}

// NewTargetDevice creates a new TargetDevice instance
func NewTargetDevice(ipAddress uint32) *TargetDevice {
	return &TargetDevice{
		IPAddress:               ipAddress,
		MonitorStatusEnabled:    false,
		DeviceStatus:            DeviceStatusOff,
		LastHeartbeatCounterRcv: 0,
		PowerCommand:            PowerCommandNone,
		PowerCommandSatisfied:   true,
		LastUpdated:             time.Now(),
	}
}

// UpdateHeartbeat updates the heartbeat counter for the device
func (td *TargetDevice) UpdateHeartbeat(counter uint32) {
	td.mutex.Lock()
	defer td.mutex.Unlock()
	td.LastHeartbeatCounterRcv = counter
	td.LastUpdated = time.Now()
}

// GetDeviceStatus returns the current device status (thread-safe)
func (td *TargetDevice) GetDeviceStatus() DeviceStatus {
	td.mutex.RLock()
	defer td.mutex.RUnlock()
	return td.DeviceStatus
}

// SetDeviceStatus sets the device status (thread-safe)
func (td *TargetDevice) SetDeviceStatus(status DeviceStatus) {
	td.mutex.Lock()
	defer td.mutex.Unlock()
	td.DeviceStatus = status
	td.LastUpdated = time.Now()
}

// RequestCard represents a request in the SPM deck system
type RequestCard struct {
	TargetIP       uint32         `json:"target_ip"`
	ProfileID      PowerProfileID `json:"profile_id"`
	ProfileNumber  uint8          `json:"profile_number"`
	LocalRequester bool           `json:"local_requester"`
	RequestCode    RequestCode    `json:"request_code"`
	RequestRole    *RequestRole   `json:"request_role,omitempty"` // Optional for local requesters
	CardTimestamp  uint32         `json:"card_timestamp"`
	CreatedAt      time.Time      `json:"created_at"`
}

// NewRequestCard creates a new RequestCard instance
func NewRequestCard(targetIP uint32, profileID PowerProfileID, profileNumber uint8,
	localRequester bool, requestCode RequestCode, requestRole *RequestRole) *RequestCard {
	return &RequestCard{
		TargetIP:       targetIP,
		ProfileID:      profileID,
		ProfileNumber:  profileNumber,
		LocalRequester: localRequester,
		RequestCode:    requestCode,
		RequestRole:    requestRole,
		CardTimestamp:  uint32(time.Now().Unix()),
		CreatedAt:      time.Now(),
	}
}

// GetUniqueKey returns a unique key for the request card
func (rc *RequestCard) GetUniqueKey() string {
	return fmt.Sprintf("%08X_%08X_%02X", rc.TargetIP, rc.ProfileID.ProfileID, rc.ProfileNumber)
}

// ===== NETWORK COMMUNICATION STRUCTURES =====

// SPM_ProfileRequestItm represents a profile request item in SM frame
type SPM_ProfileRequestItm struct {
	ProfileID     uint32 `json:"profile_id"`
	ProfileNumber uint8  `json:"profile_number"`
	RequestCode   uint8  `json:"request_code"` // 0 = ACTIVATION, 1 = DEACTIVATION
}

// SPM_ProfileResponseItm represents a profile response item in SM frame
type SPM_ProfileResponseItm struct {
	ProfileID     uint32 `json:"profile_id"`
	ProfileNumber uint8  `json:"profile_number"`
	ResponseCode  uint8  `json:"response_code"`  // 0 = OK_ACTIVATION, 1 = OK_DEACTIVATION, 2 = GENERIC_ERROR
	ProfileStatus uint8  `json:"profile_status"` // 0 = INACTIVE, 1 = ACTIVATING, 2 = ACTIVATED, 3 = DEACTIVATING
}

// SPM_SMFrame represents the Software Manager frame for network communication
type SPM_SMFrame struct {
	SrcIP          uint32                   `json:"src_ip"`
	DstIP          uint32                   `json:"dst_ip"`
	ReqMsgType     uint8                    `json:"req_msg_type"`     // fixed to 0
	ReqEntryLength uint16                   `json:"req_entry_length"` // number of request items
	Requests       []SPM_ProfileRequestItm  `json:"requests"`
	ResMsgType     uint8                    `json:"res_msg_type"`     // fixed to 1
	ResEntryLength uint16                   `json:"res_entry_length"` // number of response items
	Responses      []SPM_ProfileResponseItm `json:"responses"`
}

// NewSPM_SMFrame creates a new SM frame
func NewSPM_SMFrame(srcIP, dstIP uint32) *SPM_SMFrame {
	return &SPM_SMFrame{
		SrcIP:          srcIP,
		DstIP:          dstIP,
		ReqMsgType:     0, // fixed value
		ReqEntryLength: 0,
		Requests:       make([]SPM_ProfileRequestItm, 0),
		ResMsgType:     1, // fixed value
		ResEntryLength: 0,
		Responses:      make([]SPM_ProfileResponseItm, 0),
	}
}

// AddRequest adds a request item to the SM frame
func (frame *SPM_SMFrame) AddRequest(profileID uint32, profileNumber uint8, requestCode RequestCode) {
	request := SPM_ProfileRequestItm{
		ProfileID:     profileID,
		ProfileNumber: profileNumber,
		RequestCode:   uint8(requestCode),
	}
	frame.Requests = append(frame.Requests, request)
	frame.ReqEntryLength = uint16(len(frame.Requests))
}

// AddResponse adds a response item to the SM frame
func (frame *SPM_SMFrame) AddResponse(profileID uint32, profileNumber uint8, responseCode ResponseCode, profileStatus ProfileStatus) {
	response := SPM_ProfileResponseItm{
		ProfileID:     profileID,
		ProfileNumber: profileNumber,
		ResponseCode:  uint8(responseCode),
		ProfileStatus: uint8(profileStatus),
	}
	frame.Responses = append(frame.Responses, response)
	frame.ResEntryLength = uint16(len(frame.Responses))
}

// ===== MAIN SPM COMPONENTS =====

// SoftwareManager manages power profiles and activation/deactivation requests
type SoftwareManager struct {
	profiles       map[string]*PowerProfile // Key: profile_id_number
	requestDeck    []*RequestCard           // Deck of request cards
	mutex          sync.RWMutex             // For thread-safe operations
	mainFuncPeriod time.Duration            // Main function execution period
	retryPeriod    time.Duration            // Retry period for failed requests
}

// NewSoftwareManager creates a new SoftwareManager instance
func NewSoftwareManager() *SoftwareManager {
	return &SoftwareManager{
		profiles:       make(map[string]*PowerProfile),
		requestDeck:    make([]*RequestCard, 0),
		mainFuncPeriod: 10 * time.Millisecond, // Default 10ms cycle
		retryPeriod:    1 * time.Second,       // Default 1s retry
	}
}

// RegisterProfile registers a new power profile
func (sm *SoftwareManager) RegisterProfile(profile *PowerProfile) error {
	sm.mutex.Lock()
	defer sm.mutex.Unlock()

	key := fmt.Sprintf("%08X_%02X", profile.ProfileID.ProfileID, profile.ProfileNumber)
	if _, exists := sm.profiles[key]; exists {
		return fmt.Errorf("profile %s already exists", key)
	}

	sm.profiles[key] = profile
	return nil
}

// GetProfile retrieves a power profile by ID and number
func (sm *SoftwareManager) GetProfile(profileID uint32, profileNumber uint8) (*PowerProfile, error) {
	sm.mutex.RLock()
	defer sm.mutex.RUnlock()

	key := fmt.Sprintf("%08X_%02X", profileID, profileNumber)
	profile, exists := sm.profiles[key]
	if !exists {
		return nil, fmt.Errorf("profile %s not found", key)
	}

	return profile, nil
}

// AddRequestCard adds a new request card to the deck
func (sm *SoftwareManager) AddRequestCard(card *RequestCard) {
	sm.mutex.Lock()
	defer sm.mutex.Unlock()

	// Check for existing card with same key and update instead of adding new
	cardKey := card.GetUniqueKey()
	for i, existingCard := range sm.requestDeck {
		if existingCard.GetUniqueKey() == cardKey {
			// "Jolly" operation - update existing card
			sm.requestDeck[i] = card
			return
		}
	}

	// Add new card
	sm.requestDeck = append(sm.requestDeck, card)
}

// RemoveRequestCard removes a request card from the deck
func (sm *SoftwareManager) RemoveRequestCard(cardKey string) bool {
	sm.mutex.Lock()
	defer sm.mutex.Unlock()

	for i, card := range sm.requestDeck {
		if card.GetUniqueKey() == cardKey {
			// Remove card from slice
			sm.requestDeck = append(sm.requestDeck[:i], sm.requestDeck[i+1:]...)
			return true
		}
	}
	return false
}

// ProcessRequestDeck processes all cards in the request deck
func (sm *SoftwareManager) ProcessRequestDeck() {
	sm.mutex.RLock()
	deckCopy := make([]*RequestCard, len(sm.requestDeck))
	copy(deckCopy, sm.requestDeck)
	sm.mutex.RUnlock()

	for _, card := range deckCopy {
		sm.processRequestCard(card)
	}
}

// processRequestCard processes a single request card
func (sm *SoftwareManager) processRequestCard(card *RequestCard) {
	profile, err := sm.GetProfile(card.ProfileID.ProfileID, card.ProfileNumber)
	if err != nil {
		fmt.Printf("Error processing card: %v\n", err)
		return
	}

	if profile.IsRemote {
		sm.processRemoteProfileRequest(card, profile)
	} else {
		sm.processLocalProfileRequest(card, profile)
	}
}

// processLocalProfileRequest processes a request for a local profile
func (sm *SoftwareManager) processLocalProfileRequest(card *RequestCard, profile *PowerProfile) {
	currentStatus := profile.GetStatus()

	switch card.RequestCode {
	case RequestCodeActivation:
		switch currentStatus {
		case ProfileStatusInactive:
			profile.SetStatus(ProfileStatusActivating)
			// Send response with OK_ACTIVATION and ACTIVATING status
			fmt.Printf("Profile %08X_%02X transitioning to ACTIVATING\n",
				card.ProfileID.ProfileID, card.ProfileNumber)

		case ProfileStatusDeactivating:
			profile.SetStatus(ProfileStatusActivating)
			// Send response with OK_ACTIVATION and ACTIVATING status
			fmt.Printf("Profile %08X_%02X transitioning from DEACTIVATING to ACTIVATING\n",
				card.ProfileID.ProfileID, card.ProfileNumber)
		}

	case RequestCodeDeactivation:
		switch currentStatus {
		case ProfileStatusActivated, ProfileStatusActivating:
			profile.SetStatus(ProfileStatusDeactivating)
			// Send response with OK_DEACTIVATION and DEACTIVATING status
			fmt.Printf("Profile %08X_%02X transitioning to DEACTIVATING\n",
				card.ProfileID.ProfileID, card.ProfileNumber)
		}
	}
}

// processRemoteProfileRequest processes a request for a remote profile
func (sm *SoftwareManager) processRemoteProfileRequest(card *RequestCard, profile *PowerProfile) {
	// For remote profiles, send request to target device and wait for response
	fmt.Printf("Processing remote profile request for %08X_%02X on target %08X\n",
		card.ProfileID.ProfileID, card.ProfileNumber, card.TargetIP)

	// This would involve network communication to send SM frame
	// Implementation depends on specific network stack
}

// PowerManager manages power states and transitions of local and remote devices
type PowerManager struct {
	targetDevices map[uint32]*TargetDevice // Key: IP address
	localIP       uint32
	mutex         sync.RWMutex
}

// NewPowerManager creates a new PowerManager instance
func NewPowerManager(localIP uint32) *PowerManager {
	return &PowerManager{
		targetDevices: make(map[uint32]*TargetDevice),
		localIP:       localIP,
	}
}

// RegisterTargetDevice registers a new target device
func (pm *PowerManager) RegisterTargetDevice(device *TargetDevice) {
	pm.mutex.Lock()
	defer pm.mutex.Unlock()
	pm.targetDevices[device.IPAddress] = device
}

// GetTargetDevice retrieves a target device by IP address
func (pm *PowerManager) GetTargetDevice(ipAddress uint32) (*TargetDevice, error) {
	pm.mutex.RLock()
	defer pm.mutex.RUnlock()

	device, exists := pm.targetDevices[ipAddress]
	if !exists {
		return nil, fmt.Errorf("target device %08X not found", ipAddress)
	}

	return device, nil
}

// UpdateDeviceHeartbeat updates the heartbeat for a target device
func (pm *PowerManager) UpdateDeviceHeartbeat(ipAddress uint32, counter uint32) error {
	device, err := pm.GetTargetDevice(ipAddress)
	if err != nil {
		return err
	}

	device.UpdateHeartbeat(counter)
	return nil
}

// ===== MAIN SPM SYSTEM =====

// SPMSystem represents the complete SPM system
type SPMSystem struct {
	SoftwareManager *SoftwareManager
	PowerManager    *PowerManager
	LocalIP         uint32
	IsRunning       bool
	stopChan        chan struct{}
}

// NewSPMSystem creates a new SPM system instance
func NewSPMSystem(localIP uint32) *SPMSystem {
	return &SPMSystem{
		SoftwareManager: NewSoftwareManager(),
		PowerManager:    NewPowerManager(localIP),
		LocalIP:         localIP,
		IsRunning:       false,
		stopChan:        make(chan struct{}),
	}
}

// Start starts the SPM system main functions
func (spm *SPMSystem) Start() {
	if spm.IsRunning {
		return
	}

	spm.IsRunning = true

	// Start main function goroutine
	go spm.mainFunction()

	fmt.Printf("SPM System started with local IP: %08X\n", spm.LocalIP)
}

// Stop stops the SPM system
func (spm *SPMSystem) Stop() {
	if !spm.IsRunning {
		return
	}

	spm.IsRunning = false
	close(spm.stopChan)

	fmt.Println("SPM System stopped")
}

// mainFunction implements the main periodic function of SPM
func (spm *SPMSystem) mainFunction() {
	ticker := time.NewTicker(spm.SoftwareManager.mainFuncPeriod)
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			// Process request deck
			spm.SoftwareManager.ProcessRequestDeck()

			// Process network messages (placeholder)
			spm.processNetworkMessages()

			// Update device states (placeholder)
			spm.updateDeviceStates()

		case <-spm.stopChan:
			return
		}
	}
}

// processNetworkMessages processes incoming network messages
func (spm *SPMSystem) processNetworkMessages() {
	// Placeholder for network message processing
	// This would read SM frames from network and process them
}

// updateDeviceStates updates the states of target devices
func (spm *SPMSystem) updateDeviceStates() {
	// Placeholder for device state updates
	// This would check heartbeats and update device statuses
}

// ===== API FUNCTIONS =====

// SPM_ActivateProfile is the main API for local profile activation
func (spm *SPMSystem) SPM_ActivateProfile(applicationIntfID string) error {
	// Find profile by application interface ID
	var targetProfile *PowerProfile
	spm.SoftwareManager.mutex.RLock()
	for _, profile := range spm.SoftwareManager.profiles {
		if profile.ApplicationIntfID != nil && *profile.ApplicationIntfID == applicationIntfID {
			targetProfile = profile
			break
		}
	}
	spm.SoftwareManager.mutex.RUnlock()

	if targetProfile == nil {
		return fmt.Errorf("profile with application interface ID %s not found", applicationIntfID)
	}

	if !targetProfile.LocalRequestAllowed {
		return fmt.Errorf("local requests not allowed for profile %s", applicationIntfID)
	}

	// Create request card
	var requestRole *RequestRole
	if targetProfile.IsRemote {
		role := RequestRoleOutbound
		requestRole = &role
	}

	targetIP := spm.LocalIP
	if targetProfile.IsRemote {
		// In a real implementation, this would be determined from configuration
		targetIP = 0xC0A80102 // Example: 192.168.1.2
	}

	card := NewRequestCard(
		targetIP,
		targetProfile.ProfileID,
		targetProfile.ProfileNumber,
		true, // local requester
		RequestCodeActivation,
		requestRole,
	)

	spm.SoftwareManager.AddRequestCard(card)

	fmt.Printf("Activation request added for profile %s\n", applicationIntfID)
	return nil
}

// SPM_DeactivateProfile is the main API for local profile deactivation
func (spm *SPMSystem) SPM_DeactivateProfile(applicationIntfID string) error {
	// Find profile by application interface ID
	var targetProfile *PowerProfile
	spm.SoftwareManager.mutex.RLock()
	for _, profile := range spm.SoftwareManager.profiles {
		if profile.ApplicationIntfID != nil && *profile.ApplicationIntfID == applicationIntfID {
			targetProfile = profile
			break
		}
	}
	spm.SoftwareManager.mutex.RUnlock()

	if targetProfile == nil {
		return fmt.Errorf("profile with application interface ID %s not found", applicationIntfID)
	}

	if !targetProfile.LocalRequestAllowed {
		return fmt.Errorf("local requests not allowed for profile %s", applicationIntfID)
	}

	// Create request card
	var requestRole *RequestRole
	if targetProfile.IsRemote {
		role := RequestRoleOutbound
		requestRole = &role
	}

	targetIP := spm.LocalIP
	if targetProfile.IsRemote {
		// In a real implementation, this would be determined from configuration
		targetIP = 0xC0A80102 // Example: 192.168.1.2
	}

	card := NewRequestCard(
		targetIP,
		targetProfile.ProfileID,
		targetProfile.ProfileNumber,
		true, // local requester
		RequestCodeDeactivation,
		requestRole,
	)

	spm.SoftwareManager.AddRequestCard(card)

	fmt.Printf("Deactivation request added for profile %s\n", applicationIntfID)
	return nil
}

// ===== UTILITY FUNCTIONS =====

// GetProfileStatusString returns a human-readable status for a profile
func (spm *SPMSystem) GetProfileStatusString(profileID uint32, profileNumber uint8) (string, error) {
	profile, err := spm.SoftwareManager.GetProfile(profileID, profileNumber)
	if err != nil {
		return "", err
	}

	return profile.GetStatus().String(), nil
}

// GetSystemStats returns statistics about the SPM system
func (spm *SPMSystem) GetSystemStats() map[string]interface{} {
	spm.SoftwareManager.mutex.RLock()
	profileCount := len(spm.SoftwareManager.profiles)
	deckSize := len(spm.SoftwareManager.requestDeck)
	spm.SoftwareManager.mutex.RUnlock()

	spm.PowerManager.mutex.RLock()
	deviceCount := len(spm.PowerManager.targetDevices)
	spm.PowerManager.mutex.RUnlock()

	return map[string]interface{}{
		"profiles_registered": profileCount,
		"request_deck_size":   deckSize,
		"target_devices":      deviceCount,
		"system_running":      spm.IsRunning,
		"local_ip":            fmt.Sprintf("%08X", spm.LocalIP),
	}
}
