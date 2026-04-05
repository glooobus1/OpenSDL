#version 120

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModel;
varying vec4 vFragPosLightSpace;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec4 fragPos = uModel * gl_Vertex;
    vFragPosLightSpace = uLightSpaceMatrix * fragPos;
}