---------------------------------------
CS 460 - Project #1

By: Kevin Neece
---------------------------------------

--------
OVERVIEW
--------
This project demonstrates the rendering of a hierarchical model
using Direct3D Ver.9.0b.

-----------
DESCRIPTION
-----------
The four models come from our project for game class, "Scavenger Hunt".

The models have been exported from 3DS MAX using a freeware program
called "Flexporter".

Flexporter exports the bone hierarchy and vertex dependencies per bone
in the *.animesh file.

The *.ani files contain the translations and rotations per bone per frame
for a particular animation.

---------
INTERFACE
---------
Use menu to open a model (*.animesh).

The following camera controls are in place:

' 0 ': Rotate camera left about camera center;
' . ': Rotate camera right about camera center;

' 5 ': Reset camera to default position;
' 4 ': Move camera AND camera center along the camera's left vector;
' 6 ': Move camera AND camera center along the camera's right vector;
' 8 ': Move camera but NOT center along the camera's up vector;
' 2 ': Move camera but NOT center along the camera's down vector;

' + ': Zoom camera in towards camera center;
' - ': Zoom camera out from camera center;

The following model controls are in place:

' SPACE ': Cycle to the next animation;
(Each model has 11 total animations)

' * ': Increase the animation rate;
' / ': Decrease the animation rate;
(The default speed is 30)

' B ': Toggle the bone drawing on/off.
' M ': Toggle the mesh drawing on/off.
