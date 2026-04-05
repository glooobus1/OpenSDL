// vertex_color.vert
#version 120
attribute vec3 aColor;
varying vec3 vColor;
void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vColor = aColor;
}