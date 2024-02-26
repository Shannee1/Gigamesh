#version 430

// +++ Vertex buffers
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;

// +++ Homogenous matrices for camera orientation and projection:
uniform mat4 modelview;
uniform mat4 projection;

// +++ Direction of the light fixed in relation to the camera:
uniform vec3 LightDirectionFixedCam   = vec3( 0.0, 0.0, 1.0 );
uniform vec3 LightDirectionFixedWorld = vec3( 0.0, 0.0, 1.0 );

// +++ x/y/z stretch and offset e.g. for drawing bounding boxes.
uniform vec3 uStrech = vec3( 1.0, 1.0, 1.0 );
uniform vec3 uOffSet = vec3( 0.0, 0.0, 0.0 );

// +++ Values to be passed on to the fragment shader:
layout(location = 0) out vec4 ec_pos; // eye coordinate position to be used for on-the-fly-computation of a triangles normal within the fragment shader.
layout(location = 1) out vec3 normal_interp;
layout(location = 2) out vec3 FixedCam_halfVector;
layout(location = 3) out vec3 FixedCam_L;
layout(location = 4) out vec3 FixedWorld_halfVector;
layout(location = 5) out vec3 FixedWorld_L;

// +++ Color of the vertex
layout(location = 6) out vec4 vertexColor;

// -----------------------------------------------
void main(void) {
	vec3 posStreched = position * uStrech + uOffSet;
	ec_pos        = modelview * vec4( posStreched, 1.0 );               // Transform the vertex position into the eye coordinate system.
	normal_interp = normalize( modelview * vec4( vNormal, 0.0 ) ).xyz;  // Transform the vertex normal into eye coordinates.
	vec3 E = normalize( -ec_pos.xyz );                                  // "eye" vector, which is the vector from the vertex's eye-space position to the origin.
// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---- Light direction fixed to the camera -----
	FixedCam_L = normalize( LightDirectionFixedCam );
	FixedCam_halfVector = normalize( FixedCam_L + E );                  // "half vector"  which is the normalized vector half-way between the light and eye vectors.
// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---- Light direction fixed to the world/object -----
	FixedWorld_L = normalize( modelview * vec4( LightDirectionFixedWorld, 0.0 ) ).xyz;
	FixedWorld_halfVector = normalize( FixedWorld_L + E );              // "half vector"  which is the normalized vector half-way between the light and eye vectors.

	gl_Position = projection * ec_pos;
	vertexColor = vColor;
}
