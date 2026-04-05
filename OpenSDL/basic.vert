#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;

out vec3 vColor;
out vec2 vUV;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(aPosition, 1.0);
    vColor = aColor;
    vUV = aUV;
}