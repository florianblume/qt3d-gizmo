# Qt3D Gizmo

3D gizmo to attach to Qt3D entities.

### What's working

NOTHING WORKS. The reason is that picking is problematic in Qt3D. You need to get the picking parameters like the camera matrix from within the shader because we need to compute picking in a special way. No idea when I will find time to finish this project.
