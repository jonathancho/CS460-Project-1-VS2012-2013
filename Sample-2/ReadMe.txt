All content © 2004 DigiPen (USA) Corporation, all rights reserved.

================================================================
===============================
========
Articuluated Model and Animation Viewer for CS460
Date: June 17, 2004
Author: Max Wagner, mwagner@digipen.edu
========
===============================
================================================================


================================================================
Camera Controls
================================================================
Zoom 	(+/-): q/e
Pitch 	(+/-): w/x
Yaw 	(+/-): a/d
Roll	(+/-): z/c
Translate    : arrow keys
Reset	     : s


================================================================
Application Commands
================================================================
Toggle Full Screen	: F1
Quit			: ESC


================================================================
Overview
================================================================

This project demonstrates skinned mesh animation, using an
articulated mesh modeled and animated in 3dsMax 6 using Character
Studio Biped (the exporter also supports regular 3dsMax Bones).

The 3dsMax plugin exporter was written using the 3dsMax SDK, and 
exports to a custom text file format (.mdl).  The exporter computes 
model data including the AABB, the Bounding Sphere, all vertex 
normals, before compressing the vertices into an indexed triangle 
list.  Data extracted directly from 3dsMax include vertex 
weights, bone indices, and rotation and position keys for each
bone.  

The .mdl file format uses simple tags to denote the beginning 
of major sections, where tags are enclosed by the '@' symbol.
It includes sections for Indices, Vertices, AABB, Sphere, and
animation data, including the Bone Hierarchy, followed by the
actual animation data, consisting of Translation and Rotation
keyframes stored as 3-d and 4-d vectors, respectively.

The viewer implements a custom animation system (not using any
ID3DXMesh or other animation/mesh library) which supports multiple
quaternion interpolation methods and speed control.  The actual
rendering is implemented in the vertex shader using DirectX's
vertex assembly shading language, version 1.1.  A single vertex
shader implements skinning/vertex blending and gouraud shading for 
multiple light sources.

All math, interpolation, model/animation export, and model/animation 
rendering code is original and written by the author. 
The D3DX utility library is used exclusively to allocate textures 
and for font/text rendering.


================================================================
Relevant Files
================================================================

// solution project file
[root]\MonteCarlo.sln

// the executable (must be run from this location!)
[root]\EditorExe\EditorExe.exe

// the model file
[root]\GameExe\Assets\Models\Avery.mdl

// user interface and shell
[root]\EditorExe\GameDefDoc.hpp/.cpp

// shader files
[root]\GameExe\Assets\Effects\SkinAndBones.fx
[root]\GameExe\Assets\Effects\StaticMesh.fx
[root]\GameExe\Assets\Effects\ModelEffectHeader.hpp
[root]\GameExe\Assets\Effects\ModelEffectLighting.hpp
[root]\GameExe\Assets\Effects\Model1TexturePixelShader.hpp

// model files
[root]\GraphicsLib\Model.hpp/.cpp
[root]\GraphicsLib\ModelTemplatizedImp.hpp
[root]\GraphicsLib\StaticModel.hpp/.cpp
[root]\GraphicsLib\SkinnedModel.hpp/.cpp

// animation files
[root]\GraphicsLib\Animation.hpp/.cpp
[root]\CoreLib\QuatSpline.hpp/.cpp
[root]\CoreLib\Interpolation.hpp/.cpp
[root]\CoreLib\QuatLerp.hpp/.cpp
[root]\CoreLib\Slerp.hpp/.cpp
[root]\CoreLib\Squad.hpp/.cpp

// exporter files
[root]\ModelExporter\ModelExporter.hpp/.cpp

// rendering and Direct3D files
[root]\GraphicsLib\Renderer.hpp/.cpp
[root]\GraphicsLib\DXTypes.hpp
[root]\GraphicsLib\DXUtil.hpp/.cpp
[root]\GraphicsLib\Camera.hpp/.cpp

// math and geometry files
[root]\CoreLib\Vector.hpp
[root]\CoreLib\VectorImp.hpp
[root]\CoreLib\Matrix.hpp
[root]\CoreLib\MatrixImp.hpp
[root]\CoreLib\Quat.hpp
[root]\CoreLib\QuatImp.hpp
[root]\CoreLib\Spline.hpp/.cpp
[root]\CoreLib\Plane.hpp/.cpp
[root]\CoreLib\Frustum.hpp/.cpp
[root]\CoreLib\Sphere.hpp/.cpp


================================================================
Thanks to...
================================================================
Thanks to Ben Hopper for the use of his model, Avery.
(Don't blame him for the animation, however; that part's mine.)

