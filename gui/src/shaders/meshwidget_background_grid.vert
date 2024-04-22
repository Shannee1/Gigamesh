#version 430

uniform float uScaleX   = 1.0;    // Scale for x-axis -- typically mm/pixel.
uniform float uScaleY   = 1.0;    // Scale for y-axis -- should always be the same value as uScaleX.
uniform float uDepthPos = 0.0;    // Offset for the default range for the frustum z: +1.0-Epsilon (Far) to -1.0 (Near)

uniform vec2 uCenterOffset = vec2(0.0,0.0);

// +++ Vertex buffers
layout(location = 0) in vec2 vertPosition;        // Positions of the veritces defining the background - their typicall position is equal to the four corners of the viewport

layout(location = 0) out vec2 gridPos;            // Interpolated position of the grid i.e. pixel coordinates converted to world coordinates
layout(location = 1) out vec4 ecPosition;         // Interpolated position of the fragment in eye coordinates.

void main(void) {
	gridPos = (vertPosition + uCenterOffset) * vec2( uScaleX, uScaleY );
	ecPosition = vec4( vertPosition, uDepthPos+0.9999, 1.0 );
	gl_Position = ecPosition;
}
