This is a sample of model and animation pipeline. It supports two types of models static and skinned with animation. It has two programs.

FBXToBinary: This command line program converts a fbx file into bin file used by the model viewer. It is not a complete example because it always assumes you want to export a mesh, skel, and animation from every file. This kind of tool would be integrated into a game's art build to convert all the assets to a runtime (as in optimized with little work to load) format.

ModelViewer: Displays files exported from the exporter. Drag and drop files to view them. Also not complete for the same reason it expects all files to be the same.

All code is sample code written to get people started. The code needs work to be properly integrated with a game engine but the general class structure is similar to what you would find in most animation systems.


TODO:

1. Modify the exporter by adding command line options for exporting only animations, only meshes, batch processing, etc.

2. Optimize the mesh triangle caching and generate binormals for the mesh before it is exported. You can use Nvidia Mesh Mender, Tri Stripper to do this.

2. Modify the loading code to use an asset system to share assets with model instances. 

3. Add a lot of functions to the animation controller.


Sphere - Simple Sphere
Cube - Simple Cube
Tube - Simple Skinned Model
UFO - High Poly Nivida Sample Model
Up - Simple Model that shows if the models coordinate system is correct
Quad - Simple Quad with two triangles
QuadOnePoly - Simple Quad that needs to be triangulated
Bomber - Model From Base Invaders (Has simple bones and animation)
Tad - Model From Tad Studbody (Has different size polys and animation)


-Chris
