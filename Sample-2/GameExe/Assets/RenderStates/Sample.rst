// sample RenderState file (.rst)

// the first tag in the file must be Type "type", where type
// is one of the RenderState types (see RenderState.hpp)

// the text in quotes following the Surface tag represents the file name,
// with extension, of the surface to use

// the text in quotes following the Model tag represents the file name,
// with extension, of the model to use

// the text in quotes following the Vertex Shader tag represents the file name,
// with extension, of the vertex shader object file to use
// if no vertex shader is desired, simply omit this tag

// the text in quotes following the Pixel Shader tag represents the file name,
// with extension, of the pixel shader object file to use
// if no pixel shader is desired, simply omit this tag

@Type@ "Model"
@Surface@ "sample.srf"
@Model@ "Sphere.mdl"
@Effect@ "StaticMesh.fxo"

