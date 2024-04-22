#version 430
//#extension GL_ARB_shader_bit_encoding : enable

// +++ Homogenous matrices for camera orientation and projection:
uniform mat4 modelview;
uniform mat4 projection;

// +++ Vertex buffers -- this corresponds to MeshGL::grVertexElmentBasic
layout(location = 0) in vec3  position;
layout(location = 1) in vec3  vNormal;
layout(location = 2) in vec4  vColor;
layout(location = 3) in float vFuncVal;
// +++ Vertex buffers -- this corresponds to MeshGL::grVertexStripeElment
layout(location = 4) in float vLabelID; // this should be UINT, but thanks to fixed normalization this does not work -- see: http://qt-project.org/forums/viewthread/38929
layout(location = 5) in float vFlags;   // this should be UINT, but thanks to fixed normalization this does not work -- see: http://qt-project.org/forums/viewthread/38929
// -------------------------------------------------------------------------------------------------------------------------------------------------------------

// +++ Direction of the light fixed in relation to the camera:
uniform vec3 uLightDirectionFixedCamera = vec3( 0.0, 0.0, 1.0 );
uniform vec3 uLightDirectionFixedWorld  = vec3( 0.0, 0.0, 1.0 );

// +++ Clipping
uniform vec4 uClipPlane0 = vec4( 0.0, -1.0, 0.0, 0.0 ); // Classic clipping plane, provided by the plane of the Mesh.
uniform vec3 uClipBefore = vec3( 0.0, 0.0, 0.0 );       // Point in world coordinates sed when a single primitve is selected, than everything in front of it is clipped.

// +++ Values to be passed on to the geometry shader:
out struct grVertex {
        vec4  ec_pos;        // eye coordinate position to be used for on-the-fly-computation of a triangles normal within the fragment shader.
        vec3  normal_interp; // Normal vector, which will be interpolated
        vec3  FixedCam_halfVector;
        vec3  FixedCam_L;
        vec3  FixedWorld_halfVector;
        vec3  FixedWorld_L;
} oVertex;
// -------------------------------------------------------------------------------------------------------------------------------------------------------------

uniform float uFaceShiftViewZ = 0.0; // offset in view coordinates to prevent z-fighting. e.g. when selected faces are drawn.

void main(void) {
	oVertex.ec_pos        = modelview * vec4( position, 1.0 );                  // Transform the vertex position into the eye coordinate system.
	oVertex.normal_interp = normalize( modelview * vec4( vNormal, 0.0 ) ).xyz;  // Transform the vertex normal into eye coordinates.
	vec3 E = normalize( -oVertex.ec_pos.xyz );                                  // "eye" vector, which is the vector from the vertex's eye-space position to the origin.
	// -----------------------------------------------------------------------------------------------------------------------------------------------------
	// ---- Light direction fixed to the camera -----
	oVertex.FixedCam_L = normalize( uLightDirectionFixedCamera );
	oVertex.FixedCam_halfVector = normalize( oVertex.FixedCam_L + E );                  // "half vector"  which is the normalized vector half-way between the light and eye vectors.
	// -----------------------------------------------------------------------------------------------------------------------------------------------------
	// ---- Light direction fixed to the world/object -----
	oVertex.FixedWorld_L = normalize( modelview * vec4( uLightDirectionFixedWorld, 0.0 ) ).xyz;
	oVertex.FixedWorld_halfVector = normalize( oVertex.FixedWorld_L + E );              // "half vector"  which is the normalized vector half-way between the light and eye vectors.

	// -----------------------------------------------------------------------------------------------------------------------------------------------------
	// ---- Projection to view plane ----
	gl_Position   = projection * oVertex.ec_pos;
	// -----------------------------------------------------------------------------------------------------------------------------------------------------// Clipping
	//  ---- Clipping ----
	gl_ClipDistance[0] = dot( uClipPlane0, vec4( position, 1.0 ) );                     //  using "ec_pos" instead of "vec4( position, 1.0 )" will clip in view space!
	gl_ClipDistance[2] = ( modelview * vec4( uClipBefore, 1.0 ) ).z - oVertex.ec_pos.z;

	// -----------------------------------------------------------------------------------------------------------------------------------------------------
	//  ---- Prevent z-fighting ----
	gl_Position.z += uFaceShiftViewZ;
}
