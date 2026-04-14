# Usage Guide

### Setup

**1.** Create a blueprint class, derived from `ScarletCameraControllerBP`: this class will contain you camera configuration. This class will be further referred to as `MyController`.

**2.** In your player character/pawn class use a `GetOrCreateScarletCameraController` to access or create, if necessary, a camera controller. Specify `MyController` as the controller class:
![](Images/Setup.png)

**3.** Make sure your character mesh and capsule do not block Camera trace channel, otherwise camera collision will not work properly.

---
### Adding Profiles

Scarlet Camera System provides 2 general kinds of camera profiles: *Simple* and *Custom*.

*Custom* profiles require you to create a blueprint class, derived from `SCS_CameraProfile`, while *Simple* profiles let you specify there configuration directly in `MyController`'s parameters.

Profiles can be defined in *Class Defaults* section in `MyController` blueprint class:
![](Images/AddingCameraProfiles.png)

**IMPORTANT: Profile name spaces are shared! Simple profile names MUST not overlap with Custom profile names!**

`DefaultCameraProfile` parameter determines which profile will be active on begin-play.

---
### Switching Between Profiles

You can switch between camera profiles using `SetCameraProfile`:
![](Images/SetCameraProfileNode.png)

* `TransitionAnimation` determines whether the transition animation is played or not.
* `Queue` - this flag enables queueing of profile switch requests, if set to true, profile switch requests will play one after the other. Queue transitions are interrupted if  `SetCameraProfile` is called without the `Queue` flag.

For switching between camera profiles based on player input the following pattern is recommended:

![](Images/RecommendedSwitchingPattern_1.png)
![](Images/RecommendedSwitchingPattern_2.png)

Pattern below is NOT recommended:
![](Images/NotRecommendedSwitchingPattern.png)

---
### Profile Configuration

#### Camera State

- `FieldOfView` - camera's field of view;
- `BoomArmLength` - length of the camera arm, that originates in camera controller's location.
- `CameraOffset` - offset of the camera socket at the end of the camera arm; 
- `DoCollisionTest` - whether the camera arm is required to perform collision tests;

- `CameraLocation` - defines location of the camera controller:
	- `LocationType` - defines the way location is resolved: 
		- *Player Attachment* - camera controller is attached to the player's pawn actor;
		- *Actor Attachment* - camera controller is attached to an arbitrary actor, defined by `AttachmentActor`;
		- `World` - camera controller is placed stationary in the world.
	- `AttachmentActor` - acts as an attachment point, when *Actor Attachment* location type is used.
	- `Location` - location/offset of the camera controller in the space, determined by Location Type.

- `CameraArmRotation` - defines rotation of the camera arm (camera controller):
	- `RotationType` - defines the way rotation is resolved:
		* *Player Attachment* - camera arm is attached to the player's pawn actor;
		- *Actor Attachment* - camera arm is attached to an arbitrary actor, defined by `AttachmentActor`;
		- `World` - camera arm is placed stationary in the world.
	- `AttachmentActor` - acts as an attachment point, when *Actor Attachment* rotation type is used.
	- `Rotation` - rotation/offset of the camera arm in space, determined by Rotation Type.

- `EnableSeparateCameraRotation` 
- `SeparateCameraRotation`
	- `RotationType`
	- `AttachmentActor`
	- `Rotation`

#### Camera State Interpolation

#### Blend In Settings

---
### Creating Custom Profiles