#version 430

layout(location = 0) in vec4 vertexColor;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 halfWay;
layout(location = 0) out vec4 frag_color;

void main()
{
    //compute fake lighting...
    vec3 vN = normalize(vertexNormal);

    vec3 lightDir = normalize(vec3(1.0,1.0,1.0));

    float diffuse = max(dot(vN, lightDir), 0.5) + 0.1 * clamp(dot(vN, -lightDir),0.0, 1.0);

    vec3 specular = vec3(pow(max(dot( vN, halfWay), 0.0), 10.0));

    frag_color.rgb = (vertexColor.rgb * diffuse) + specular;
    frag_color.a = 1.0;
}
