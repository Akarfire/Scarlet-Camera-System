# Core Features

### General

* [x] Adding Simple Profiles in Default Settings
* [x] Adding Custom Profiles in Default Settings
* [x] Specifying Default Profile
* [x] Adding Profiles in Runtime
	* [x] Add Simple Profile
	* [x] Add Custom Profile
	* [x] Add Custom Profile Existing
* [x] Camera Collision
* [x] Profile Switching Queue
* [x] Option to disable Profile Switching Queue (the Default behavior)
* [ ] #Missing User-Friendly interface for modifying Simple Camera Profiles from Blueprints

---
### Simple Camera Profiles

* [x] Static Properties:
	* [x] Field Of View
	* [x] Boom Arm Length
	* [x] Camera Offset
	* [x] Do Collision Test
* [x] Runtime Property Syncing
* [x] Camera Location
	* [x] Player Attachment
	* [x] Actor Attachment
	* [x] World
* [x] Camera Arm Rotation
	* [x] Player Attachment
	* [x] Actor Attachment
	* [x] World
* [x] Separate Camera Rotation
	* [x] Player Attachment
	* [x] Actor Attachment
	* [x] World
* [x] Interpolation Modes & Speeds
	* [x] Field Of View
	* [x] Boom Arm Length
	* [x] Camera Offset
	* [x] Location
	* [x] Rotation
	* [x] Separate Camera Rotation

---
### Profile Switching

* [x] Can Switch Profiles with no animation
* [x] Default transition animation works
* [x] Interrupt transition (in no Queue Mode)
* [x] Properties
	* [x] Blend Animation Duration
	* [x] Always Freeze Previous State
	* [x] FOV Curve
	* [x] Boom Arm Length Curve
	* [x] Camera Offset Curve
	* [x] Location Curve
	* [x] Rotation Curve
	* [x] Separate Camera Rotation Curve

---
### Custom Camera Profiles

* [x] Activation
* [x] Deactivation
* [x] Updating
* [x] Dynamic Camera State
* [x] Dynamic Camera State Interpolation
* [x] Dynamic Blend In Settings

---

# Integration

### Scarlet Character Movement

- [x] Linking Scarlet Movement
- [x] Mapping movement states to profiles from the Camera Controller
- [x] Default profile for unmapped states
- [x] Customizing movement state mapping
- [ ] #Missing Example camera profiles for movement states