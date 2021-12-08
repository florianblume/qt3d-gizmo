# Qt3D Gizmo

3D gizmo to attach to Qt3D entities.

## Preparation

* You need to put a special framegraph part with `QDispatchCompute` and `QBufferCapture` somewhere.

### What's working

**Right now nothing works du to restructuring works**

- Accurate translation with mouse in XYZ
- Accurate rotation around XYZ
- Switching between rotation and translation

### Known issues

- Qt3D only allows to set the picking settings globally and not per object picker. But we would need to set the `pickResultMode` to `All` in order to be able to pick translation or rotation axes (or the switcher sphere) when they are actually behind the object that is to be modified. They are always ontop of everything because they are drawn ontop, but Qt3D's ray intersection procedure checks if the objects are actually behind other objects, when the `pickResultMode` is set to `NearestPick` - which is what you want normally. Setting the result mode to `All` makes the gizmo work properly but now objects that are behind others are also pickable.
- When properly disabling the gizmo by using Qt3D's `setEnabled` function, the gizmo is not usable even after enabling it again. Only e.g. resizing the 3D window then allows the gizmo to work. It seems that there is an internal issue with updating the object pickers to allow to pick the entity after enabling it again. We could investigate this further but I "hacked" the enabling function and am now simply disabling or enabling the materials on the transformation axes. This works but needs to be seen as a workaround.
