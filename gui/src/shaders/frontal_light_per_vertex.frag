#version 430
layout(location = 0) in float brightness;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(brightness, brightness, brightness, 1.0);
}
