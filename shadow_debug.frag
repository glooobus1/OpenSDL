#version 120

uniform sampler2D uShadowMap;

void main() {
    float depth = texture2D(uShadowMap, gl_TexCoord[0].xy).r;
    gl_FragColor = vec4(vec3(depth), 1.0);
}