# Scarlet Camera System

An Unreal Engine 5.4 plugin that implements a dynamic & modular camera control system. Scarlet Camera System (SCS) provides means to define static or dynamic Camera Profiles and dynamically switch between them.

**Status: Early Access / In Development**

---
### Features

#### Dynamic Camera Profiles

The profile below changes it's characteristics based on how fast the camera is moving!
![](Documentation/Images/GIFs/SpeedCameraProfile.gif)
![](Documentation/Images/CustomProfileExample.png)

By creating custom camera profiles you can easily define dynamic camera behavior, which can be as complex as you want it to be! 

*For example the profile above uses camera speed to blend between parameters of OTHER camera profiles.*

---
#### Profile Transition Animation

Switching between camera profiles is handled with a customizable transition animation!
![](Documentation/Images/GIFs/CameraProfileSwitching.gif)
![](Documentation/Images/BlendingSettings.png)

*NOTE: Ease-In-Out interpolation is used whenever you leave curves unspecified.*

---
#### "Simple" Camera Profile Customization

In addition to class-defined custom camera profiles, this system provides Simple Camera Profiles, that can be easily customized using the following set of parameters:
![](Documentation/Images/ProfileSettings.png)

*NOTE: This same set of parameters can be dynamically controlled using custom profiles, simple profiles just provide a parameter-only way of customizing them.*

*Simple profiles can be used for many purposes, including providing configuration for custom profiles, that blend blend between parameters of OTHER camera profiles, as shown in the example above.*

---
### User Guide

#### Setup

**1.** Create a blueprint class, derived from `ScarletCameraControllerBP`: this class will contain you camera configuration. This class will be further referred to as `MyController`.

**2.** In your player character/pawn class use a `GetOrCreateScarletCameraController` to access or create, if necessary, a camera controller. Specify `MyController` as the controller class:
![](Images/Setup.png)

**3.** Make sure your character mesh and capsule do not block Camera trace channel, otherwise camera collision will not work properly.

---
#### Adding Profiles

Scarlet Camera System provides 2 general kinds of camera profiles: *Simple* and *Custom*.

*Custom* profiles require you to create a blueprint class, derived from `SCS_CameraProfile`, while *Simple* profiles let you specify there configuration directly in `MyController`'s parameters.

Profiles can be defined in *Class Defaults* section in `MyController` blueprint class:
![](Images/AddingCameraProfiles.png)

**IMPORTANT: Profile name spaces are shared! Simple profile names MUST not overlap with Custom profile names!**

`DefaultCameraProfile` parameter determines which profile will be active on begin-play.

---
#### Switching Between Profiles

You can switch between camera profiles using `SetCameraProfile`:
![](Images/SetCameraProfileNode.png)

* `TransitionAnimation` determines whether the transition animation is played or not.
* `Queue` - this flag enables queueing of profile switch requests, if set to true, profile switch requests will play one after the other. Queue transitions are interrupted if  `SetCameraProfile` is called without the `Queue` flag.
#### ...
#### For complete guide see: [Guide](Documentation/Guide.md)

---
### Installation

1. Download the latest release;
2. Extract the archive and move plugin folder to the `./Plugins` directory in your project (if your project has no `./Plugin` directory, then create it).

---
### Documentation

