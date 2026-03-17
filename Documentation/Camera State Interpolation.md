
`FSCS_CameraStateInterpolation` structure defines interpolation type and speed for every float-based field of `USCS_CameraState` ([Camera State](Camera%20State.md)) structure.

* `ESCS_InterpolationType FOV_InterpolationType` - interpolation type for camera's field of view;
* `float FOV_InterpolationSpeed` - speed of interpolation for camera's field of view;

* `ESCS_InterpolationType BoomArmLength_InterpolationType` - interpolation type for camera state's `BoomArmLength` parameter;
* `float BoomArmLength_InterpolationSpeed` - speed of interpolation for camera state's `BoomArmLength` parameter;

* `ESCS_InterpolationType CameraOffset_InterpolationType` - interpolation type for camera state's `CameraOffset` parameter;
* `float CameraOffset_InterpolationSpeed` - speed of interpolation for camera state's `CameraOffset` parameter;

### Interpolation Type

`ESCS_InterpolationType` is an enum class that contains 3 options:
0. `None` - instant transition;
1. `Linear` - linear interpolation;
2. `Ease` - ease in and ease out interpolation;

### Code

```c++
```