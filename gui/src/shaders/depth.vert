#version 430
uniform mat4 transformMat;

// +++ Vertex buffers -- this corresponds to MeshGL::grVertexElmentBasic
layout(location = 0) in vec3  position;
layout(location = 1) in vec3  vNormal;
layout(location = 2) in vec4  vColor;
layout(location = 3) in float vFuncVal;
// +++ Vertex buffers -- this corresponds to MeshGL::grVertexStripeElment
layout(location = 4) in float vLabelID; // this should be UINT, but thanks to fixed normalization this does not work -- see: http://qt-project.org/forums/viewthread/38929
layout(location = 5) in float vFlags;   // this should be UINT, but thanks to fixed normalization this does not work -- see: http://qt-project.org/forums/viewthread/38929
// -------------------------------------------------------------------------------------------------------------------------------------------------------------

void main() {
    gl_Position = transformMat * vec4(position, 1.0f);
}
