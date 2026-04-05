// texture.vert
#version 120
attribute vec2 aUV;
varying vec2 vUV;
void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vUV = aUV;
}