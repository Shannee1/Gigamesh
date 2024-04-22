#version 430
layout(location = 0) in float data;
layout(location = 1) in float selected;

uniform sampler2D uFuncValTexture;
uniform float uColorMapIndex = 0.0f;

uniform bool invertFuncVal = false;

uniform float uUpperQuantil = 1.0f;

layout(location = 0) out vec4 fragColor;

void main(void)
{
	float funcVal = data;
	if(invertFuncVal)
		funcVal = 1.0 - funcVal;

	funcVal = clamp(funcVal / uUpperQuantil, 0.0, 1.0);

	vec2 funcvalTexCoord = vec2(funcVal, (uColorMapIndex * 10.0f + 5.0f)  / 512.0);

	fragColor = texture(uFuncValTexture, funcvalTexCoord);
	fragColor.rgb = mix(fragColor.rgb, vec3(0,1,0), selected);
}
