# Qt3D Gizmo

3D gizmo to attach to Qt3D entities.

<p float="left">
  <img src="https://i.imgur.com/qVbwpD1.gif" width="49%" />
  <img src="https://i.imgur.com/LsEwXMM.gif" width="49%" />
</p>

### What's working

- Accurate translation with mouse in XYZ
- Accurate rotation around XYZ
- Switching between rotation and translation
- Translation planes

### What's missing / Caveats

- The options to set some properties like color of the handles, etc.
- To be able to properly pick the gizmo through objects which are infront of it, we need to set the `pickResultMode` of the `QPickingSettings` to `NearestPriorityPick` and assign a higher priority to the gizmo than to any other object in the scene. A bit clumsy but there's no way around it. I tried to compute the rays in the shader but to no avail.
- No proper support for multiple viewports. Might work but not tested. The gizmo will not work properly with multiple viewports if you choose the option to scale the gizmo invariantly of the distance of the object. If you don't need this, you could, theoretically, get the viewport the mouse is in (compute it manually using the mouse position and the viewport rects) and pass the respective camera to the gizmo. It still needs it to compute th movements. It might be easier to create one gizmo per viewport which is then not shown in the others. This will look a bit odd since you would expect the gizmo to be shown in all but that's how it is.
- When properly disabling the gizmo by using Qt3D's `setEnabled` function, the gizmo is not usable even after enabling it again. Only e.g. resizing the 3D window then allows the gizmo to work. It seems that there is an internal issue with updating the object pickers to allow to pick the entity after enabling it again. We could investigate this further but I "hacked" the enabling function and am now simply disabling or enabling the materials on the transformation axes. This works but needs to be seen as a workaround.

### Use it in your code

1. On your `QRenderSettings` get the `QPickingSettings` and set `pickResultMode` to `NearestPriorityPick`
2. To draw the gizmo over all objects, you need the following framegraph structure:

```
                                        rest of your framegraph
                                                  |
            -----------------------------------------------------------------------------
            |                        |                         |                        |
    Clear Buffers (All)     Objects Layer Filter      Clear Buffers (Depth)      Gizmo Layer Filter
            |                                                  |           
         No Draw                                            No Draw
```

And add the `QLayer` you use for the objects layer filter to all objects in the scene, except the gizmo, and the `QLayer` you use for the gizmo layer filter to the gizmo. This way, the gizmo will always be drawn ontop of all objects. Checkout `tests/main.cpp` to see how I constructed the framegraph.

3. Set the window size on the gizmo (udpate it if it changes!)
4. set the camera of your framegraph on the gizmo.
