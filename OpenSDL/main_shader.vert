#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;

out vec3 vNormal;
out vec4 vColor;
out vec4 vFragPosLightSpace;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    gl_Position = uProjection * uView * worldPos;
    
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vColor = aColor;
    vFragPosLightSpace = uLightSpaceMatrix * worldPos;
}