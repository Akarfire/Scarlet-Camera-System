
Camera configuration is defined by a `FSCS_CameraState` struct, which contains the following fields:

* `float FOV` - camera's field of view;
* `float BoomArmLength` - how far away is the camera from it's center of rotation;
* `FVector CameraOffset` - offset of the camera, relative to the boom arm's end;
* `bool DoCollisionTest` - whether boom arm's collision should be performed or not.

### Code

```c++
```